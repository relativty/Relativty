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

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "User32.lib")

#include <atomic>
#include <algorithm>
#include <WinSock2.h>
#include <Windows.h>
#include "hidapi/hidapi.h"
#include "openvr_driver.h"

#include "driverlog.h"

#include "Relativty_HMDDriver.hpp"
#include "Relativty_ServerDriver.hpp"
#include "Relativty_EmbeddedPython.h"
#include "Relativty_components.h"
#include "Relativty_base_device.h"

#include <string>

inline void Normalize(std::array<float, 3> &norma, const std::array<float, 3> &v, const std::array<float, 3> &max, const std::array<float, 3> &min, const int &up, const int &down, const std::array<float, 3> &scale, const std::array<float, 3> offset) {
	for (int i = 0; i < 3; i++) {
		norma[i] = (((up - down) * ((v[i] - min[i]) / (max[i] - min[i])) + down) / scale[i]) + offset[i];
	}
}

vr::EVRInitError Relativty::HMDDriver::Activate(uint32_t unObjectId) {
	RelativtyDevice::Activate(unObjectId);
	this->setProperties();

	if (hid_init()) {
		Relativty::ServerDriver::Log("USB: HID API initialization failed. \n");
		return vr::VRInitError_Driver_TrackedDeviceInterfaceUnknown;
	}

	this->handle = hid_open((unsigned short)m_iVid, (unsigned short)m_iPid, nullptr);
	if (!this->handle) {
#ifdef DRIVERLOG_H
		DriverLog("USB: Unable to open HMD device with pid=%d and vid=%d.\n", m_iPid, m_iVid);
#else
		Relativty::ServerDriver::Log("USB: Unable to open HMD device with pid=" + std::to_string(m_iPid) + " and vid=" + std::to_string(m_iVid) + ".\n");
#endif
		return vr::VRInitError_Init_InterfaceNotFound;
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
	hid_close(this->handle);
	hid_exit();

	if (this->start_tracking_server) {
		this->retrieve_vector_isOn = false;
		closesocket(this->sock);
		this->retrieve_vector_thread_worker.join();
		WSACleanup();
	}
	RelativtyDevice::Deactivate();
	this->update_pose_thread_worker.join();

	Relativty::ServerDriver::Log("Thread0: all threads exit correctly \n");
}

void Relativty::HMDDriver::update_pose_threaded() {
	Relativty::ServerDriver::Log("Thread2: successfully started\n");
	while (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid) {
		if (this->new_quaternion_available && this->new_vector_available) {
			std::shared_lock quatLock(quatMutex);
			std::shared_lock vectorLock(vectorMutex);
			std::copy(this->quat.begin(), this->quat.end(), reinterpret_cast<float *>(&m_Pose.qRotation));
			std::copy(this->vector_xyz.begin(), this->vector_xyz.end(), std::begin(m_Pose.vecPosition));

			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, m_Pose, sizeof(vr::DriverPose_t));
			this->new_quaternion_available = false;
			this->new_vector_available = false;
		} else if (this->new_quaternion_available) {
			std::shared_lock quatLock(quatMutex);
			std::copy(this->quat.begin(), this->quat.end(), reinterpret_cast<float *>(&m_Pose.qRotation));

			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, m_Pose, sizeof(vr::DriverPose_t));
			this->new_quaternion_available = false;
		} else if (this->new_vector_available) {
			std::shared_lock vectorLock(vectorMutex);
			std::copy(this->vector_xyz.begin(), this->vector_xyz.end(), std::begin(m_Pose.vecPosition));

			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, m_Pose, sizeof(vr::DriverPose_t));
			this->new_vector_available = false;
		}
	}
	Relativty::ServerDriver::Log("Thread2: successfully stopped\n");
}

void Relativty::HMDDriver::calibrate_quaternion() {
	std::lock_guard writeLock(quatMutex);
	if ((0x01 & GetAsyncKeyState(0x52)) != 0) {
		qconj[0] = this->quat[0];
		qconj[1] = -1 * this->quat[1];
		qconj[2] = -1 * this->quat[2];
		qconj[3] = -1 * this->quat[3];
	}

	this->quat[0] = qconj[0] * this->quat[0] - qconj[1] * this->quat[1] - qconj[2] * this->quat[2] - qconj[3] * this->quat[3];
	this->quat[1] = qconj[0] * this->quat[1] + qconj[1] * this->quat[0] + qconj[2] * this->quat[3] - qconj[3] * this->quat[2];
	this->quat[2] = qconj[0] * this->quat[2] - qconj[1] * this->quat[3] + qconj[2] * this->quat[0] + qconj[3] * this->quat[1];
	this->quat[3] = qconj[0] * this->quat[3] + qconj[1] * this->quat[2] - qconj[2] * this->quat[1] + qconj[3] * this->quat[0];
}

void Relativty::HMDDriver::retrieve_device_quaternion_packet_threaded() {
	std::array<uint8_t, 64> packet_buffer;
	std::array<int16_t, 4> quaternion_packet;
//this struct is for mpu9250 support
#pragma pack(push, 1)
	struct pak {
		uint8_t id;
		std::array<float, 4> quat;
	};
#pragma pack(pop)
	Relativty::ServerDriver::Log("Thread1: successfully started\n");
	while (this->retrieve_quaternion_isOn) {
		if (hid_read(this->handle, packet_buffer.data(), packet_buffer.size()) > 0) {
			if (m_bIMUpktIsDMP) {
				quaternion_packet[0] = ((packet_buffer[1] << 8) | packet_buffer[2]);
				quaternion_packet[1] = ((packet_buffer[5] << 8) | packet_buffer[6]);
				quaternion_packet[2] = ((packet_buffer[9] << 8) | packet_buffer[10]);
				quaternion_packet[3] = ((packet_buffer[13] << 8) | packet_buffer[14]);
				std::lock_guard writeLock(quatMutex);
				std::transform(quaternion_packet.begin(), quaternion_packet.end(), this->quat.begin(), [](const int16_t &item) { return static_cast<float>(item) / 16384.0F; });

				this->quat[0] = quat[0];
				this->quat[1] = quat[1];
				this->quat[2] = -1 * quat[2];
				this->quat[3] = -1 * quat[3];
			} else {
				pak *recv = reinterpret_cast<pak *>(packet_buffer.data());
				std::lock_guard writeLock(quatMutex);
				std::copy(recv->quat.begin(), recv->quat.end(), this->quat.begin());
			}
			this->calibrate_quaternion();
			this->new_quaternion_available = true;
		} else {
			Relativty::ServerDriver::Log("Thread1: Issue while trying to read USB\n");
		}
	}
	Relativty::ServerDriver::Log("Thread1: successfully stopped\n");
}

void Relativty::HMDDriver::retrieve_client_vector_packet_threaded() {
	WSADATA wsaData;
	struct sockaddr_in server, client;
	int addressLen;
	constexpr int receiveBufferLen = 12;
	char receiveBuffer[receiveBufferLen];
	int resultReceiveLen;

	std::array<float, 3> normalize_min = {this->normalizeMinX, this->normalizeMinY, this->normalizeMinZ};
	std::array<float, 3> normalize_max = {this->normalizeMaxX, this->normalizeMaxY, this->normalizeMaxZ};
	std::array<float, 3> scales_coordinate_meter = {this->scalesCoordinateMeterX, this->scalesCoordinateMeterY, this->scalesCoordinateMeterZ};
	std::array<float, 3> offset_coordinate = {this->offsetCoordinateX, this->offsetCoordinateY, this->offsetCoordinateZ};

	std::array<float, 3> coordinate = {0, 0, 0};
	std::array<float, 3> coordinate_normalized;

	Relativty::ServerDriver::Log("Thread3: Initialising Socket.\n");
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		Relativty::ServerDriver::Log("Thread3: Failed. Error Code: " + WSAGetLastError());
		return;
	}
	Relativty::ServerDriver::Log("Thread3: Socket successfully initialised.\n");

	if ((this->sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		Relativty::ServerDriver::Log("Thread3: could not create socket: " + WSAGetLastError());
		return;
	}
	Relativty::ServerDriver::Log("Thread3: Socket created.\n");

	server.sin_family = AF_INET;
	server.sin_port = htons(50000);
	server.sin_addr.s_addr = INADDR_ANY;

	if (bind(this->sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
		Relativty::ServerDriver::Log("Thread3: Bind failed with error code: " + WSAGetLastError());
		return;
	}
	Relativty::ServerDriver::Log("Thread3: Bind done \n");

	listen(this->sock, 1);

	this->serverNotReady = false;

	Relativty::ServerDriver::Log("Thread3: Waiting for incoming connections...\n");
	addressLen = sizeof(struct sockaddr_in);
	this->sock_receive = accept(this->sock, (struct sockaddr *)&client, &addressLen);
	if (this->sock_receive == INVALID_SOCKET) {
		Relativty::ServerDriver::Log("Thread3: accept failed with error code: " + WSAGetLastError());
		return;
	}
	Relativty::ServerDriver::Log("Thread3: Connection accepted");

	Relativty::ServerDriver::Log("Thread3: successfully started\n");
	while (this->retrieve_vector_isOn) {
		resultReceiveLen = recv(this->sock_receive, receiveBuffer, receiveBufferLen, 0);
		if (resultReceiveLen > 0) {
			coordinate[0] = *(float *)(receiveBuffer);
			coordinate[1] = *(float *)(receiveBuffer + 4);
			coordinate[2] = *(float *)(receiveBuffer + 8);

			Normalize(coordinate_normalized, coordinate, normalize_max, normalize_min, static_cast<int>(this->upperBound), static_cast<int>(this->lowerBound), scales_coordinate_meter, offset_coordinate);

			this->vector_xyz[0] = coordinate_normalized[1];
			this->vector_xyz[1] = coordinate_normalized[2];
			this->vector_xyz[2] = coordinate_normalized[0];
			this->new_vector_available = true;
		}
	}
	Relativty::ServerDriver::Log("Thread3: successfully stopped\n");
}

Relativty::HMDDriver::HMDDriver(const std::string &myserial) : RelativtyDevice(myserial, "akira_") {
	// keys for use with the settings API
	static const char *const Relativty_hmd_section = "Relativty_hmd";

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

	this->m_iPid = vr::VRSettings()->GetInt32(Relativty_hmd_section, "hmdPid");
	this->m_iVid = vr::VRSettings()->GetInt32(Relativty_hmd_section, "hmdVid");

	this->m_bIMUpktIsDMP = vr::VRSettings()->GetBool(Relativty_hmd_section, "hmdIMUdmpPackets");

	char buffer[1024];
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
