// Copyright (C) 2020  Max Coutte, Gabriel Combe
// Copyright (C) 2020  Relativty.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <cstring>

#include <atomic>

#ifdef __unix__
	#include <unistd.h>
	#include <stdlib.h>
	#include <X11/Xlib.h>
	#define XK_LATIN1
	#include <X11/keysymdef.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/ip.h>
#else
	#pragma comment(lib, "Ws2_32.lib")
	#pragma comment (lib, "Setupapi.lib")
	#pragma comment(lib, "User32.lib")
	#include <WinSock2.h>
	#include <Windows.h>
	#include <ws2tcpip.h>
#endif
#include <libserialport.h>

#include "../hidapi/hidapi/hidapi.h"

#include "../include/openvr_driver.hpp"

#include "../include/driverlog.hpp"

#include "../include/Relativty_HMDDriver.hpp"
#include "../include/Relativty_ServerDriver.hpp"
#include "../include/Relativty_EmbeddedPython.hpp"
#include "../include/Relativty_components.hpp"
#include "../include/Relativty_base_device.hpp"


#include <string>

#ifdef __unix__
	Display * g_pDisplay = nullptr;

	static bool GetKeyState(KeySym keySym){
		if(g_pDisplay == nullptr)
		{
			DefaultScreen(&g_pDisplay);
			if(g_pDisplay == nullptr)
				return false;
		}

		char szKey[32];
		int iKeyCodeToFind = XKeysymToKeycode(g_pDisplay, keySym);

		XQueryKeymap(g_pDisplay, szKey);

		return szKey[iKeyCodeToFind / 8] & (1 << (iKeyCodeToFind % 8));
	}

#endif

struct sp_port * open_serial(const char * desired_port, unsigned int baudrate) {
    struct sp_port *port;

	enum sp_return error = sp_get_port_by_name(desired_port,&port);
	if (error == SP_OK) {
		error = sp_open(port,SP_MODE_READ);
		if (error == SP_OK) {
			sp_set_baudrate(port, baudrate);
            return port;
		} else {
			Relativty::ServerDriver::Log("Error opening serial device\n");
		}
	} else {
		Relativty::ServerDriver::Log("Error finding serial device\n");
	}
	return 0;
}

inline vr::HmdQuaternion_t HmdQuaternion_Init(double w, double x, double y, double z) {
	vr::HmdQuaternion_t quat;
	quat.w = w;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
}

inline void Normalize(float norma[3], float v[3], float max[3], float min[3], int up, int down, float scale[3], float offset[3]) {
	for (int i = 0; i < 3; i++) {
		norma[i] = (((up - down) * ((v[i] - min[i]) / (max[i] - min[i])) + down) / scale[i])+ offset[i];
	}
}

vr::EVRInitError Relativty::HMDDriver::Activate(uint32_t unObjectId) {
	RelativtyDevice::Activate(unObjectId);
	this->setProperties();

	if(this->isSerial) {
		Relativty::ServerDriver::Log("Starting serial\n");
		this->serialPort = open_serial(this->serialDevice.c_str(), this->baudrate);
		if(this->serialPort == nullptr) {
			return vr::VRInitError_Init_InterfaceNotFound;
		}
	} else {
		Relativty::ServerDriver::Log("Starting hid \n");
		int result = hid_init(); //Result should be 0.
		if (result) {
			Relativty::ServerDriver::Log("USB: HID API initialization failed. \n");
			return vr::VRInitError_Driver_TrackedDeviceInterfaceUnknown;
		}
		this->handle = hid_open((unsigned short)m_iVid, (unsigned short)m_iPid, NULL);
		if (!this->handle) {
			#ifdef DRIVERLOG_H
			DriverLog("USB: Unable to open HMD device with pid=%d and vid=%d.\n", m_iPid, m_iVid);
			#else
			Relativty::ServerDriver::Log("USB: Unable to open HMD device with pid="+ std::to_string(m_iPid) +" and vid="+ std::to_string(m_iVid) +".\n");
			#endif
			return vr::VRInitError_Init_InterfaceNotFound;
		}
	}


	this->retrieve_quaternion_isOn = true;
	this->retrieve_quaternion_thread_worker = std::thread(&Relativty::HMDDriver::retrieve_device_quaternion_packet_threaded, this);

	if (this->start_tracking_server) {
		this->retrieve_vector_isOn = true;
		this->retrieve_vector_thread_worker = std::thread(&Relativty::HMDDriver::retrieve_client_vector_packet_threaded, this);
		while (this->serverNotReady) {
			// do nothing
		}
		this->startPythonTrackingClient_worker = std::thread(startPythonTrackingClient_threaded, this->PyPath);
	}

	this->update_pose_thread_worker = std::thread(&Relativty::HMDDriver::update_pose_threaded, this);

	return vr::VRInitError_None;
}

void Relativty::HMDDriver::Deactivate() {
	this->retrieve_quaternion_isOn = false;
	this->retrieve_quaternion_thread_worker.join();

	if(this->isSerial) {
		sp_close(this->serialPort);
	} else {
		hid_close(this->handle);
		hid_exit();
	}

	if (this->start_tracking_server) {
		this->retrieve_vector_isOn = false;
		#ifdef __unix__
			close(this->sock);
		#else
			closesocket(this->sock);
			this->retrieve_vector_thread_worker.join();
			WSACleanup();
		#endif
	}
	RelativtyDevice::Deactivate();
	this->update_pose_thread_worker.join();

	Relativty::ServerDriver::Log("Thread0: all threads exit correctly \n");
}

void Relativty::HMDDriver::update_pose_threaded() {
	Relativty::ServerDriver::Log("Thread2: successfully started\n");
	while (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid) {
		if (this->new_quaternion_avaiable && this->new_vector_avaiable) {
			m_Pose.qRotation.w = this->quat[0];
			m_Pose.qRotation.x = this->quat[1];
			m_Pose.qRotation.y = this->quat[2];
			m_Pose.qRotation.z = this->quat[3];

			m_Pose.vecPosition[0] = this->vector_xyz[0];
			m_Pose.vecPosition[1] = this->vector_xyz[1];
			m_Pose.vecPosition[2] = this->vector_xyz[2];

			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, m_Pose, sizeof(vr::DriverPose_t));
			this->new_quaternion_avaiable = false;
			this->new_vector_avaiable = false;

		} else if (this->new_quaternion_avaiable) {
			m_Pose.qRotation.w = this->quat[0];
			m_Pose.qRotation.x = this->quat[1];
			m_Pose.qRotation.y = this->quat[2];
			m_Pose.qRotation.z = this->quat[3];

			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, m_Pose, sizeof(vr::DriverPose_t));
			this->new_quaternion_avaiable = false;

		} else if (this->new_vector_avaiable) {

			m_Pose.vecPosition[0] = this->vector_xyz[0];
			m_Pose.vecPosition[1] = this->vector_xyz[1];
			m_Pose.vecPosition[2] = this->vector_xyz[2];

			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, m_Pose, sizeof(vr::DriverPose_t));
			this->new_vector_avaiable = false;

		}
	}
	Relativty::ServerDriver::Log("Thread2: successfully stopped\n");
}

void Relativty::HMDDriver::calibrate_quaternion() {
	#ifdef __unix__

	if(GetKeyState(XK_R))
	#else
	if ((0x01 & GetAsyncKeyState(0x52)) != 0)
	#endif
	{
		qconj[0].store(quat[0]);
		qconj[1].store(-1 * quat[1]);
		qconj[2].store(-1 * quat[2]);
		qconj[3].store(-1 * quat[3]);
	}
	float qres[4];

	qres[0] = qconj[0] * quat[0] - qconj[1] * quat[1] - qconj[2] * quat[2] - qconj[3] * quat[3];
	qres[1] = qconj[0] * quat[1] + qconj[1] * quat[0] + qconj[2] * quat[3] - qconj[3] * quat[2];
	qres[2] = qconj[0] * quat[2] - qconj[1] * quat[3] + qconj[2] * quat[0] + qconj[3] * quat[1];
	qres[3] = qconj[0] * quat[3] + qconj[1] * quat[2] - qconj[2] * quat[1] + qconj[3] * quat[0];

	this->quat[0] = qres[0];
	this->quat[1] = qres[1];
	this->quat[2] = qres[2];
	this->quat[3] = qres[3];
}

void Relativty::HMDDriver::retrieve_device_quaternion_packet_threaded() {
	if(this->isSerial) {
		this->retrieve_device_quaternion_packet_serial();
	} else {
		this->retrieve_device_quaternion_packet_hid();
	}
}

void Relativty::HMDDriver::retrieve_device_quaternion_packet_serial() {
	struct RelativtySerial {
		float w;
		float x;
		float y;
		float z;
	} payload;

	while(this->retrieve_quaternion_isOn) {
		int bytes_waiting = sp_input_waiting(this->serialPort);
		if (bytes_waiting >= (int)sizeof(payload)) {
			sp_nonblocking_read(this->serialPort, &payload, sizeof(payload));
			this->quat[0] = payload.w;
			this->quat[1] = payload.x;
			this->quat[2] = payload.y;
			this->quat[3] = payload.z;

			this->calibrate_quaternion();

			this->new_quaternion_avaiable = true;
		}
	}
}

void Relativty::HMDDriver::retrieve_device_quaternion_packet_hid() {
	uint8_t packet_buffer[64];
	int16_t quaternion_packet[4];
	//this struct is for mpu9250 support
	#pragma pack(push, 1)
	struct pak {
		uint8_t id;
		float quat[4];
		uint8_t rest[47];
	};
	#pragma pack(pop)
	int result;
	Relativty::ServerDriver::Log("Thread1: successfully started\n");
	while (this->retrieve_quaternion_isOn) {
		result = hid_read(this->handle, packet_buffer, 64); //Result should be greater than 0.
		if (result > 0) {


			if (m_bIMUpktIsDMP) {

				quaternion_packet[0] = ((packet_buffer[1] << 8) | packet_buffer[2]);
				quaternion_packet[1] = ((packet_buffer[5] << 8) | packet_buffer[6]);
				quaternion_packet[2] = ((packet_buffer[9] << 8) | packet_buffer[10]);
				quaternion_packet[3] = ((packet_buffer[13] << 8) | packet_buffer[14]);
				this->quat[0] = static_cast<float>(quaternion_packet[0]) / 16384.0f;
				this->quat[1] = static_cast<float>(quaternion_packet[1]) / 16384.0f;
				this->quat[2] = static_cast<float>(quaternion_packet[2]) / 16384.0f;
				this->quat[3] = static_cast<float>(quaternion_packet[3]) / 16384.0f;

				float qres[4];
				qres[0] = quat[0];
				qres[1] = quat[1];
				qres[2] = -1 * quat[2];
				qres[3] = -1 * quat[3];

				this->quat[0] = qres[0];
				this->quat[1] = qres[1];
				this->quat[2] = qres[2];
				this->quat[3] = qres[3];

				this->calibrate_quaternion();

				this->new_quaternion_avaiable = true;

			}
			else {

				pak* recv = (pak*)packet_buffer;
				this->quat[0] = recv->quat[0];
				this->quat[1] = recv->quat[1];
				this->quat[2] = recv->quat[2];
				this->quat[3] = recv->quat[3];

				this->calibrate_quaternion();

				this->new_quaternion_avaiable = true;

			}


		}
		else {
			Relativty::ServerDriver::Log("Thread1: Issue while trying to read USB\n");
		}
	}
	Relativty::ServerDriver::Log("Thread1: successfully stopped\n");
}

void Relativty::HMDDriver::retrieve_client_vector_packet_threaded() {
	socklen_t addressLen;
	struct sockaddr_in server, client;
	int receiveBufferLen = 12;
	char receiveBuffer[12];
	int resultReceiveLen;

	float normalize_min[3]{ this->normalizeMinX, this->normalizeMinY, this->normalizeMinZ};
	float normalize_max[3]{ this->normalizeMaxX, this->normalizeMaxY, this->normalizeMaxZ};
	float scales_coordinate_meter[3]{ this->scalesCoordinateMeterX, this->scalesCoordinateMeterY, this->scalesCoordinateMeterZ};
	float offset_coordinate[3] = { this->offsetCoordinateX, this->offsetCoordinateY, this->offsetCoordinateZ};

	float coordinate[3]{ 0, 0, 0 };
	float coordinate_normalized[3];

	#ifndef __unix__
	WSADATA wsaData;
	Relativty::ServerDriver::Log("Thread3: Initialising Socket.\n");
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		Relativty::ServerDriver::Log("Thread3: Failed. Error Code: " + WSAGetLastError());
		return;
	}
	Relativty::ServerDriver::Log("Thread3: Socket successfully initialised.\n");
	if ((this->sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		Relativty::ServerDriver::Log("Thread3: could not create socket: " + WSAGetLastError());
	#else
	if ((this->sock = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
		std::string errorlog = "Thread3: could not create socket: ";
		errorlog += strerror(errno);
		Relativty::ServerDriver::Log(errorlog);
	}
	#endif

	Relativty::ServerDriver::Log("Thread3: Socket created.\n");

	server.sin_family = AF_INET;
	server.sin_port = htons(50000);
	server.sin_addr.s_addr = INADDR_ANY;

	#ifndef __unix__
	if (bind(this->sock, (struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR)
		Relativty::ServerDriver::Log("Thread3: Bind failed with error code: " + WSAGetLastError());
	#else
	if (bind(this->sock, (struct sockaddr*) & server, sizeof(server)) > 0) {
		std::string errorlog = "Thread3: Bind failed with error code: ";
		errorlog += strerror(errno);
		Relativty::ServerDriver::Log(errorlog);
	}

	#endif
	Relativty::ServerDriver::Log("Thread3: Bind done \n");

	listen(this->sock, 1);

	this->serverNotReady = false;

	Relativty::ServerDriver::Log("Thread3: Waiting for incoming connections...\n");
	addressLen = sizeof(struct sockaddr_in);
	this->sock_receive = accept(this->sock, (struct sockaddr*) & client, &addressLen);
	if (this->sock_receive < 0) {
		std::string errorlog = "Thread3: accept failed with error code: ";
		errorlog += strerror(errno);
		Relativty::ServerDriver::Log(errorlog);
	}
	#ifdef __unix__

	#else
	if (this->sock_receive == INVALID_SOCKET)
		Relativty::ServerDriver::Log("Thread3: accept failed with error code: " + WSAGetLastError());

	#endif
	Relativty::ServerDriver::Log("Thread3: Connection accepted");

	Relativty::ServerDriver::Log("Thread3: successfully started\n");
	while (this->retrieve_vector_isOn) {
		#ifdef __unix__
		resultReceiveLen = recv(this->sock_receive, receiveBuffer, receiveBufferLen, 0);
		#else
		resultReceiveLen = recv(this->sock_receive, receiveBuffer, receiveBufferLen, NULL);
		#endif
		if (resultReceiveLen > 0) {
			coordinate[0] = *(float*)(receiveBuffer);
			coordinate[1] = *(float*)(receiveBuffer + 4);
			coordinate[2] = *(float*)(receiveBuffer + 8);

			Normalize(coordinate_normalized, coordinate, normalize_max, normalize_min, this->upperBound, this->lowerBound, scales_coordinate_meter, offset_coordinate);

			this->vector_xyz[0] = coordinate_normalized[1];
			this->vector_xyz[1] = coordinate_normalized[2];
			this->vector_xyz[2] = coordinate_normalized[0];
			this->new_vector_avaiable = true;
		}
	}
	Relativty::ServerDriver::Log("Thread3: successfully stopped\n");
}

Relativty::HMDDriver::HMDDriver(std::string myserial):RelativtyDevice(myserial, "akira_") {
	// keys for use with the settings API
	static const char* const Relativty_hmd_section = "Relativty_hmd";

	// openvr api stuff
	m_sRenderModelPath = "{Relativty}/rendermodels/generic_hmd";
	m_sBindPath = "{Relativty}/input/relativty_hmd_profile.json";

	m_spExtDisplayComp = std::make_shared<Relativty::RelativtyExtendedDisplayComponent>();

	// not openvr api stuff
	Relativty::ServerDriver::Log("Loading Settings\n");
	this->IPD = vr::VRSettings()->GetFloat(Relativty_hmd_section, "IPDmeters");
	this->SecondsFromVsyncToPhotons = vr::VRSettings()->GetFloat(Relativty_hmd_section, "secondsFromVsyncToPhotons");
	this->DisplayFrequency = vr::VRSettings()->GetFloat(Relativty_hmd_section, "displayFrequency");

	this->start_tracking_server = vr::VRSettings()->GetBool(Relativty_hmd_section, "startTrackingServer");
	this->upperBound = vr::VRSettings()->GetFloat(Relativty_hmd_section, "upperBound");
	this->lowerBound = vr::VRSettings()->GetFloat(Relativty_hmd_section, "lowerBound");
	this->normalizeMinX = vr::VRSettings()->GetFloat(Relativty_hmd_section, "normalizeMinX");
	this->normalizeMinY = vr::VRSettings()->GetFloat(Relativty_hmd_section, "normalizeMinY");
	this->normalizeMinZ = vr::VRSettings()->GetFloat(Relativty_hmd_section, "normalizeMinZ");
	this->normalizeMaxX = vr::VRSettings()->GetFloat(Relativty_hmd_section, "normalizeMaxX");
	this->normalizeMaxY = vr::VRSettings()->GetFloat(Relativty_hmd_section, "normalizeMaxY");
	this->normalizeMaxZ = vr::VRSettings()->GetFloat(Relativty_hmd_section, "normalizeMaxZ");
	this->scalesCoordinateMeterX = vr::VRSettings()->GetFloat(Relativty_hmd_section, "scalesCoordinateMeterX");
	this->scalesCoordinateMeterY = vr::VRSettings()->GetFloat(Relativty_hmd_section, "scalesCoordinateMeterY");
	this->scalesCoordinateMeterZ = vr::VRSettings()->GetFloat(Relativty_hmd_section, "scalesCoordinateMeterZ");
	this->offsetCoordinateX = vr::VRSettings()->GetFloat(Relativty_hmd_section, "offsetCoordinateX");
	this->offsetCoordinateY = vr::VRSettings()->GetFloat(Relativty_hmd_section, "offsetCoordinateY");
	this->offsetCoordinateZ = vr::VRSettings()->GetFloat(Relativty_hmd_section, "offsetCoordinateZ");


	this->isSerial = vr::VRSettings()->GetBool(Relativty_hmd_section, "isSerial");
	this->baudrate = vr::VRSettings()->GetInt32(Relativty_hmd_section, "baudrate");

	char buffer[1024];
	vr::VRSettings()->GetString(Relativty_hmd_section, "serialDevice", buffer, sizeof(buffer));
	this->serialDevice = std::string(buffer);

	this->m_iPid = vr::VRSettings()->GetInt32(Relativty_hmd_section, "hmdPid");
	this->m_iVid = vr::VRSettings()->GetInt32(Relativty_hmd_section, "hmdVid");

	this->m_bIMUpktIsDMP = vr::VRSettings()->GetBool(Relativty_hmd_section, "hmdIMUdmpPackets");

	vr::VRSettings()->GetString(Relativty_hmd_section, "PyPath", buffer, sizeof(buffer));
	this->PyPath = buffer;

	// this is a bad idea, this should be set by the tracking loop
	m_Pose.result = vr::TrackingResult_Running_OK;
}

inline void Relativty::HMDDriver::setProperties() {
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, vr::Prop_UserIpdMeters_Float, this->IPD);
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, vr::Prop_UserHeadToEyeDepthMeters_Float, 0.16f);
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, vr::Prop_DisplayFrequency_Float, this->DisplayFrequency);
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, vr::Prop_SecondsFromVsyncToPhotons_Float, this->SecondsFromVsyncToPhotons);

	// avoid "not fullscreen" warnings from vrmonitor
	vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, vr::Prop_IsOnDesktop_Bool, false);
}
