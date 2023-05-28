#pragma once

#define RELATIVTY_BASE_DEVICE

#include "../include/driverlog.hpp"
#include "../include/stricmp.hpp"
#include "Relativty_components.hpp"
#include <memory>

#ifdef __unix__
	#include <string.h>
#endif

namespace Relativty {
	static const char *const k_pch_Driver_Section = "driver_Relativty";
	static const char *const k_pch_Driver_PoseTimeOffset_Float = "PoseTimeOffset";
	static const char *const k_pch_Driver_UpdateUrl_String = "ManualUpdateURL";

	// for now this will never signal for updates, this same function will be executed for all derived device classes on Activate
	// you can implement your own version/update check here
	inline bool _checkForDeviceUpdates([[maybe_unused]] const std::string& deviceSerial) {
		return false; // true steamvr will signal an update, false not
	}

	// should be publicly inherited
	template<bool UseHaptics>
	class RelativtyDevice: public vr::ITrackedDeviceServerDriver {
	public:
		RelativtyDevice(const std::string& myserial, const std::string& deviceBreed): m_sSerialNumber(myserial) {
		// boilerplate

			m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
			m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;

			m_sModelNumber = deviceBreed + m_sSerialNumber;

			m_fPoseTimeOffset = vr::VRSettings()->GetFloat(k_pch_Driver_Section, k_pch_Driver_PoseTimeOffset_Float);
			char buff[1024];
			vr::VRSettings()->GetString(k_pch_Driver_Section, k_pch_Driver_UpdateUrl_String, buff, sizeof(buff));
			m_sUpdateUrl = buff;

			DriverLog("device created\n");
			DriverLog("device breed: %s\n", deviceBreed.c_str());
			DriverLog("device serial: %s\n", m_sSerialNumber.c_str());
			DriverLog("device model: %s\n", m_sModelNumber.c_str());
			DriverLog("device pose time offset: %f\n", m_fPoseTimeOffset);

			m_Pose.poseTimeOffset = (double)m_fPoseTimeOffset;
			m_Pose.poseIsValid = true;
			m_Pose.deviceIsConnected = true;
			m_Pose.qWorldFromDriverRotation = vr::HmdQuaternion_t{1, 0, 0, 0};
			m_Pose.qDriverFromHeadRotation = vr::HmdQuaternion_t{1, 0, 0, 0};
			m_Pose.qRotation = vr::HmdQuaternion_t{1, 0, 0, 0};
			m_Pose.vecPosition[0] = 0.;
			m_Pose.vecPosition[1] = 0.;
			m_Pose.vecPosition[2] = 0.;
			m_Pose.willDriftInYaw = true;
		}

		~RelativtyDevice(){
			DriverLog("device with serial %s yeeted out of existence\n", m_sSerialNumber.c_str());
		}

		virtual vr::EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId);
		virtual void Deactivate();
		virtual void EnterStandby();
		virtual void PowerOff();

		// debug request from the client, TODO: uh... actually implement this?
		virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize);

		virtual vr::DriverPose_t GetPose() { return m_Pose; }

		void *GetComponent(const char *pchComponentNameAndVersion);

		inline std::string GetSerialNumber() const { return m_sSerialNumber; }

		// processes events, reacts to haptics only
		void ProcessEvent(const vr::VREvent_t &vrEvent);

	protected:
		// openvr api stuff
		vr::TrackedDeviceIndex_t m_unObjectId; // DO NOT TOUCH THIS, parent will handle this, use it as read only!
		vr::PropertyContainerHandle_t m_ulPropertyContainer; // THIS EITHER, use it as read only!


		std::string m_sRenderModelPath; // path to the device's render model, should be populated in the constructor of the derived class
		std::string m_sBindPath; // path to the device's input bindings, should be populated in the constructor of the derived class

		vr::DriverPose_t m_Pose; // device's pose, use this at runtime

		std::shared_ptr<RelativtyExtendedDisplayComponent> m_spExtDisplayComp;

	private:
		// openvr api stuff that i don't trust you with
		float m_fPoseTimeOffset; // time offset of the pose, set trough the config
		vr::VRInputComponentHandle_t m_compHaptic; // haptics, used if UseHaptics is true

		std::string m_sUpdateUrl; // url to which steamvr will redirect if checkForDeviceUpdates returns true on Activate, set trough the config
		std::string m_sSerialNumber; // steamvr uses this to identify devices, no need for you to touch this after init
		std::string m_sModelNumber; // steamvr uses this to identify devices, no need for you to touch this after init
	};
}

#include "Relativty_base_device.ipp"
