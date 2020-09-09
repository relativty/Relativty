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

#include <atomic>
#include <WinSock2.h>
#include <Windows.h>
#include <hidapi/hidapi.h>
#include <OpenVR/openvr_driver.h>
#include "Relativty_HMDDriver.hpp"
#include "Relativty_ServerDriver.hpp"
#include "Relativty_EmbeddedPython.h"

inline vr::HmdQuaternion_t HmdQuaternion_Init(double w, double x, double y, double z) {
	vr::HmdQuaternion_t quat;
	quat.w = w;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
}

inline void Normalize(float norma[3], float v[3], float max[3], float min[3], int up, int down, float scale[3], float offset[3]) {
	for (int i = 0; i < 4; i++) {
		norma[i] = (((up - down) * ((v[i] - min[i]) / (max[i] - min[i])) + down) / scale[i])+ offset[i];
	}
}

vr::EVRInitError Relativty::HMDDriver::Activate(uint32_t unObjectId) {
	this->setProperties(unObjectId);

	int result;
	result = hid_init(); //Result should be 0.
	if (result) {
		Relativty::ServerDriver::Log("USB: HID API initialization failed. \n");
		return vr::VRInitError_Unknown;
	}
	this->handle = hid_open(0x1209, 0x0009, NULL);
	if (!this->handle) {
		Relativty::ServerDriver::Log("USB: Unable to open HMD device. \n");
		return vr::VRInitError_Unknown;
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

	this->ObjectId = vr::k_unTrackedDeviceIndexInvalid;
	this->update_pose_thread_worker.join();

	Relativty::ServerDriver::Log("Thread0: all threads exit correctly \n");
}

void Relativty::HMDDriver::update_pose_threaded() {
	Relativty::ServerDriver::Log("Thread2: successfully started\n");
	while (this->ObjectId != vr::k_unTrackedDeviceIndexInvalid) {
		if (this->new_quaternion_avaiable || this->new_vector_avaiable) {
			this->lastPose.qRotation.w = this->quat[0];
			this->lastPose.qRotation.x = this->quat[1];
			this->lastPose.qRotation.y = this->quat[2];
			this->lastPose.qRotation.z = this->quat[3];

			this->lastPose.vecPosition[0] = this->vector_xyz[0];
			this->lastPose.vecPosition[1] = this->vector_xyz[1];
			this->lastPose.vecPosition[2] = this->vector_xyz[2];

			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(this->ObjectId, this->lastPose, sizeof(vr::DriverPose_t));
			this->new_quaternion_avaiable = false;
			this->new_vector_avaiable = false;
		}
	}
	Relativty::ServerDriver::Log("Thread2: successfully stopped\n");
}

void Relativty::HMDDriver::calibrate_quaternion() {
	if ((0x01 & GetAsyncKeyState(0x52)) != 0) {
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
	uint8_t packet_buffer[64];
	int16_t quaternion_packet[4];
	int result;
	Relativty::ServerDriver::Log("Thread1: successfully started\n");
	while (this->retrieve_quaternion_isOn) {
		result = hid_read(this->handle, packet_buffer, 64); //Result should be greater than 0.
		if (result > 0) {
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
			Relativty::ServerDriver::Log("Thread1: Issue while trying to read USB\n");
		}
	}
	Relativty::ServerDriver::Log("Thread1: successfully stopped\n");
}

void Relativty::HMDDriver::retrieve_client_vector_packet_threaded() {
	WSADATA wsaData;
	struct sockaddr_in server, client;
	int addressLen;
	int receiveBufferLen = 12;
	char receiveBuffer[12];
	int resultReceiveLen;

	float normalize_min[3]{ this->normalizeMinX, this->normalizeMinY, this->normalizeMinZ};
	float normalize_max[3]{ this->normalizeMaxX, this->normalizeMaxY, this->normalizeMaxZ};
	float scales_coordinate_meter[3]{ this->scalesCoordinateMeterX, this->scalesCoordinateMeterY, this->scalesCoordinateMeterZ};
	float offset_coordinate[3] = { this->offsetCoordinateX, this->offsetCoordinateY, this->offsetCoordinateZ};

	float coordinate[3]{ 0, 0, 0 };
	float coordinate_normalized[3];

	Relativty::ServerDriver::Log("Thread3: Initialising Socket.\n");
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		Relativty::ServerDriver::Log("Thread3: Failed. Error Code: " + WSAGetLastError());
		return;
	}
	Relativty::ServerDriver::Log("Thread3: Socket successfully initialised.\n");

	if ((this->sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		Relativty::ServerDriver::Log("Thread3: could not create socket: " + WSAGetLastError());
	Relativty::ServerDriver::Log("Thread3: Socket created.\n");

	server.sin_family = AF_INET;
	server.sin_port = htons(50000);
	server.sin_addr.s_addr = INADDR_ANY;

	if (bind(this->sock, (struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR)
		Relativty::ServerDriver::Log("Thread3: Bind failed with error code: " + WSAGetLastError());
	Relativty::ServerDriver::Log("Thread3: Bind done \n");

	listen(this->sock, 1);

	this->serverNotReady = false;

	Relativty::ServerDriver::Log("Thread3: Waiting for incoming connections...\n");
	addressLen = sizeof(struct sockaddr_in);
	this->sock_receive = accept(this->sock, (struct sockaddr*) & client, &addressLen);
	if (this->sock_receive == INVALID_SOCKET)
		Relativty::ServerDriver::Log("Thread3: accept failed with error code: " + WSAGetLastError());
	Relativty::ServerDriver::Log("Thread3: Connection accepted");

	Relativty::ServerDriver::Log("Thread3: successfully started\n");
	while (this->retrieve_vector_isOn) {
		resultReceiveLen = recv(this->sock_receive, receiveBuffer, receiveBufferLen, NULL);
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

vr::DriverPose_t Relativty::HMDDriver::GetPose() {
	return lastPose;
}

void Relativty::HMDDriver::frameUpdate() {
}

void Relativty::HMDDriver::EnterStandby() {
}

void* Relativty::HMDDriver::GetComponent(const char* pchComponentNameAndVersion) {
	if (!_stricmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version)) {
		return static_cast<vr::IVRDisplayComponent*>(this);
	}
	return nullptr;
}

std::string Relativty::HMDDriver::GetSerialNumber() {
	return this->SerialNumber.c_str();
}

void Relativty::HMDDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) {
	if (unResponseBufferSize >= 1)
		pchResponseBuffer[0] = 0;
}

void Relativty::HMDDriver::GetWindowBounds(int32_t* pnX, int32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) {
	*pnX = this->WindowX;
	*pnY = this->WindowY;
	*pnWidth = this->WindowWidth;
	*pnHeight = this->WindowHeight;
}

bool Relativty::HMDDriver::IsDisplayOnDesktop() {
	return true;
}

bool Relativty::HMDDriver::IsDisplayRealDisplay() {
	return false;
}

void Relativty::HMDDriver::GetRecommendedRenderTargetSize(uint32_t* pnWidth, uint32_t* pnHeight) {
	*pnWidth = this->RenderWidth;
	*pnHeight = this->RenderHeight;
}

void Relativty::HMDDriver::GetEyeOutputViewport(vr::EVREye eEye, uint32_t* pnX, uint32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) {
	*pnY = 0;
	*pnWidth = this->WindowWidth / 2;
	*pnHeight = this->WindowHeight;

	if (eEye == vr::EVREye::Eye_Left) {
		*pnX = 0;
	}
	else {
		*pnX = this->WindowWidth / 2;
	}
}

void Relativty::HMDDriver::GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom) {
	*pfLeft = -1.0;
	*pfRight = 1.0;
	*pfTop = -1.0;
	*pfBottom = 1.0;
}

vr::DistortionCoordinates_t Relativty::HMDDriver::ComputeDistortion(vr::EVREye eEye, float fU, float fV) {
	vr::DistortionCoordinates_t coordinates;
	coordinates.rfBlue[0] = fU;
	coordinates.rfBlue[1] = fV;
	coordinates.rfGreen[0] = fU;
	coordinates.rfGreen[1] = fV;
	coordinates.rfRed[0] = fU;
	coordinates.rfRed[1] = fV;
	return coordinates;
}

Relativty::HMDDriver::HMDDriver() {
	// keys for use with the settings API
	static const char* const Relativty_Section = "driver_Relativty";

	this->ObjectId = vr::k_unTrackedDeviceIndexInvalid;
	this->PropertyContainer = vr::k_ulInvalidPropertyContainer;

	Relativty::ServerDriver::Log("Loading Settings\n");
	this->IPD = vr::VRSettings()->GetFloat(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_IPD_Float);

	char buffer[1024];
	vr::VRSettings()->GetString(Relativty_Section, "serialNumber", buffer, sizeof(buffer));
	this->SerialNumber = buffer;
	vr::VRSettings()->GetString(Relativty_Section, "modelNumber", buffer, sizeof(buffer));
	this->ModelNumber = buffer;
	this->WindowX = vr::VRSettings()->GetInt32(Relativty_Section, "windowX");
	this->WindowY = vr::VRSettings()->GetInt32(Relativty_Section, "windowY");
	this->WindowWidth = vr::VRSettings()->GetInt32(Relativty_Section, "windowWidth");
	this->WindowHeight = vr::VRSettings()->GetInt32(Relativty_Section, "windowHeight");
	this->RenderWidth = vr::VRSettings()->GetInt32(Relativty_Section, "renderWidth");
	this->RenderHeight = vr::VRSettings()->GetInt32(Relativty_Section, "renderHeight");
	this->SecondsFromVsyncToPhotons = vr::VRSettings()->GetFloat(Relativty_Section, "secondsFromVsyncToPhotons");
	this->DisplayFrequency = vr::VRSettings()->GetFloat(Relativty_Section, "displayFrequency");

	this->start_tracking_server = vr::VRSettings()->GetBool(Relativty_Section, "startTrackingServer");
	this->upperBound = vr::VRSettings()->GetFloat(Relativty_Section, "upperBound");
	this->lowerBound = vr::VRSettings()->GetFloat(Relativty_Section, "lowerBound");
	this->normalizeMinX = vr::VRSettings()->GetFloat(Relativty_Section, "normalizeMinX");
	this->normalizeMinY = vr::VRSettings()->GetFloat(Relativty_Section, "normalizeMinY");
	this->normalizeMinZ = vr::VRSettings()->GetFloat(Relativty_Section, "normalizeMinZ");
	this->normalizeMaxX = vr::VRSettings()->GetFloat(Relativty_Section, "normalizeMaxX");
	this->normalizeMaxY = vr::VRSettings()->GetFloat(Relativty_Section, "normalizeMaxY");
	this->normalizeMaxZ = vr::VRSettings()->GetFloat(Relativty_Section, "normalizeMaxZ");
	this->scalesCoordinateMeterX = vr::VRSettings()->GetFloat(Relativty_Section, "scalesCoordinateMeterX");
	this->scalesCoordinateMeterY = vr::VRSettings()->GetFloat(Relativty_Section, "scalesCoordinateMeterY");
	this->scalesCoordinateMeterZ = vr::VRSettings()->GetFloat(Relativty_Section, "scalesCoordinateMeterZ");
	this->offsetCoordinateX = vr::VRSettings()->GetFloat(Relativty_Section, "offsetCoordinateX");
	this->offsetCoordinateY = vr::VRSettings()->GetFloat(Relativty_Section, "offsetCoordinateY");
	this->offsetCoordinateZ = vr::VRSettings()->GetFloat(Relativty_Section, "offsetCoordinateZ");

	vr::VRSettings()->GetString(Relativty_Section, "PyPath", buffer, sizeof(buffer));
	this->PyPath = buffer;

	this->lastPose.poseIsValid = true;
	this->lastPose.result = vr::TrackingResult_Running_OK;
	this->lastPose.deviceIsConnected = true;
	this->lastPose.willDriftInYaw = false;

	this->lastPose.qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
	this->lastPose.qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);
	this->lastPose.vecPosition[0] = 0;
	this->lastPose.vecPosition[1] = 0;
	this->lastPose.vecPosition[2] = 0;
}

inline void Relativty::HMDDriver::setProperties(uint32_t unObjectId) {
	this->ObjectId = unObjectId;
	this->PropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(this->ObjectId);

	vr::VRProperties()->SetStringProperty(this->PropertyContainer, vr::Prop_ModelNumber_String, this->ModelNumber.c_str());
	vr::VRProperties()->SetStringProperty(this->PropertyContainer, vr::Prop_RenderModelName_String, this->ModelNumber.c_str());
	vr::VRProperties()->SetFloatProperty(this->PropertyContainer, vr::Prop_UserIpdMeters_Float, this->IPD);
	vr::VRProperties()->SetFloatProperty(this->PropertyContainer, vr::Prop_UserHeadToEyeDepthMeters_Float, 0.16f);
	vr::VRProperties()->SetFloatProperty(this->PropertyContainer, vr::Prop_DisplayFrequency_Float, this->DisplayFrequency);
	vr::VRProperties()->SetFloatProperty(this->PropertyContainer, vr::Prop_SecondsFromVsyncToPhotons_Float, this->SecondsFromVsyncToPhotons);
	// return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
	vr::VRProperties()->SetUint64Property(this->PropertyContainer, vr::Prop_CurrentUniverseId_Uint64, 2);
	// avoid "not fullscreen" warnings from vrmonitor
	vr::VRProperties()->SetBoolProperty(this->PropertyContainer, vr::Prop_IsOnDesktop_Bool, false);
}