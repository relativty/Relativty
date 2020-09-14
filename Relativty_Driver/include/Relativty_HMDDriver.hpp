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
#include "hidapi/hidapi.h"
#include "openvr_driver.h"
#include "Relativty_components.h"

namespace Relativty {
	class HMDDriver : public vr::ITrackedDeviceServerDriver
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

	private:
		vr::TrackedDeviceIndex_t ObjectId;
		vr::PropertyContainerHandle_t PropertyContainer;
		std::string SerialNumber;
		std::string ModelNumber;

		std::shared_ptr<RelativtyExtendedDisplayComponent> m_pExtDisplayComp;

		int32_t m_iPid;
		int32_t m_iVid;

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