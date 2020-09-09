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

#pragma once
#include <thread>
#include <atomic>
#include <WinSock2.h>
#include <hidapi/hidapi.h>
#include <OpenVR/openvr_driver.h>

namespace Relativty {
	class HMDDriver : public vr::ITrackedDeviceServerDriver, public vr::IVRDisplayComponent
	{
	public:
		HMDDriver();
		~HMDDriver() = default;

		void frameUpdate();
		std::string GetSerialNumber();
		inline void setProperties(uint32_t unObjectId);

		// Inherited from ITrackedDeviceServerDriver.
		virtual vr::EVRInitError Activate(uint32_t unObjectId) override;
		virtual void Deactivate() override;
		virtual void EnterStandby() override;
		virtual void* GetComponent(const char* pchComponentNameAndVersion) override;
		virtual void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) override;
		virtual vr::DriverPose_t GetPose() override;

		// Inherited from IVRDisplayComponent.
		virtual void GetWindowBounds(int32_t* pnX, int32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) override;
		virtual bool IsDisplayOnDesktop() override;
		virtual bool IsDisplayRealDisplay() override;
		virtual void GetRecommendedRenderTargetSize(uint32_t* pnWidth, uint32_t* pnHeight) override;
		virtual void GetEyeOutputViewport(vr::EVREye eEye, uint32_t* pnX, uint32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) override;
		virtual void GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom) override;
		virtual vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eEye, float fU, float fV) override;
	private:
		vr::TrackedDeviceIndex_t ObjectId;
		vr::PropertyContainerHandle_t PropertyContainer;
		std::string SerialNumber;
		std::string ModelNumber;

		int32_t WindowX;
		int32_t WindowY;
		int32_t WindowWidth;
		int32_t WindowHeight;
		int32_t RenderWidth;
		int32_t RenderHeight;

		float SecondsFromVsyncToPhotons;
		float DisplayFrequency;
		float IPD;
		float HeadToEyeDepth;

		vr::DriverPose_t lastPose = {0};
		hid_device* handle;

		std::atomic<float> quat[4];
		std::atomic<bool> retrieve_quaternion_isOn = false;
		std::atomic<bool> new_quaternion_avaiable = false;

		std::atomic<float> qconj[4] = {1, 0, 0, 0};
		void calibrate_quaternion();

		std::thread retrieve_quaternion_thread_worker;
		void retrieve_device_quaternion_packet_threaded();

		std::atomic<float> vector_xyz[3];
		std::atomic<bool> retrieve_vector_isOn = false;
		std::atomic<bool> new_vector_avaiable = false;
		bool start_tracking_server = false;
		SOCKET sock, sock_receive;
		float upperBound;
		float lowerBound;

		float normalizeMinX;
		float normalizeMinY;
		float normalizeMinZ;

		float normalizeMaxX;
		float normalizeMaxY;
		float normalizeMaxZ;

		float scalesCoordinateMeterX;
		float scalesCoordinateMeterY;
		float scalesCoordinateMeterZ;

		float offsetCoordinateX;
		float offsetCoordinateY;
		float offsetCoordinateZ;

		std::atomic<bool> serverNotReady = true;
		std::thread retrieve_vector_thread_worker;
		void retrieve_client_vector_packet_threaded();

		std::thread update_pose_thread_worker;
		void update_pose_threaded();

		std::string PyPath;
		std::thread startPythonTrackingClient_worker;
	};
}