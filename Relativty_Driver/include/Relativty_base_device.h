#pragma once

#ifndef VR_DEVICE_BASE_H
#define VR_DEVICE_BASE_H

#include "driverlog.h"


#include "Relativty_components.h"

namespace Relativty {
  inline vr::HmdQuaternion_t HmdQuaternion_Init(double w, double x, double y,
                                            double z) {
    vr::HmdQuaternion_t quat;
    quat.w = w;
    quat.x = x;
    quat.y = y;
    quat.z = z;
    return quat;
  }

  static const char *const k_pch_Driver_Section = "driver_Relativty";
  static const char *const k_pch_Driver_PoseTimeOffset_Float = "PoseTimeOffset";
  static const char *const k_pch_Driver_UpdateUrl_String = "ManualUpdateURL";

   // for now this will never signal for updates, this same function will be executed for all derived device classes on Activate
  // you can implement your own version/update check here
  inline bool _checkForDeviceUpdates(std::string deviceSerial) {
    return false; // true steamvr will signal an update, false not
  }

  // should be publicly inherited
  template<bool UseHaptics>
  class RelativtyDevice: public vr::ITrackedDeviceServerDriver {
  public:
    RelativtyDevice(std::string myserial, std::string deviceBreed): m_sSerialNumber(myserial) {
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
      m_Pose.qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
      m_Pose.qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);
      m_Pose.qRotation = HmdQuaternion_Init(1, 0, 0, 0);
      m_Pose.vecPosition[0] = 0.;
      m_Pose.vecPosition[1] = 0.;
      m_Pose.vecPosition[2] = 0.;
      m_Pose.willDriftInYaw = true;
    }

    ~RelativtyDevice(){
      DriverLog("device with serial %s yeeted out of existence\n", m_sSerialNumber.c_str());

    }

    virtual vr::EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId) {
      // more boilerplate
      m_unObjectId = unObjectId;
      m_ulPropertyContainer =
          vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

      vr::VRProperties()->SetStringProperty(
          m_ulPropertyContainer, vr::Prop_ModelNumber_String, m_sModelNumber.c_str());
      vr::VRProperties()->SetStringProperty(
          m_ulPropertyContainer, vr::Prop_RenderModelName_String, m_sRenderModelPath.c_str());

      // return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
      vr::VRProperties()->SetUint64Property(m_ulPropertyContainer,
                                            vr::Prop_CurrentUniverseId_Uint64, 2);

      vr::VRProperties()->SetStringProperty(
          m_ulPropertyContainer, vr::Prop_InputProfilePath_String,
          m_sBindPath.c_str());

      DriverLog("device activated\n");
      DriverLog("device serial: %s\n", m_sSerialNumber.c_str());
      DriverLog("device render model: \"%s\"\n", m_sRenderModelPath.c_str());
      DriverLog("device input binding: \"%s\"\n", m_sBindPath.c_str());

      if constexpr(UseHaptics) {
        DriverLog("device haptics added\n");
        vr::VRDriverInput()->CreateHapticComponent(m_ulPropertyContainer,
                                                       "/output/haptic", &m_compHaptic);
      }

      vr::VRProperties()->SetBoolProperty(
          m_ulPropertyContainer, vr::Prop_Identifiable_Bool, UseHaptics);

      vr::VRProperties()->SetStringProperty(
        m_ulPropertyContainer, vr::Prop_Firmware_ManualUpdateURL_String,
        m_sUpdateUrl.c_str());

      bool shouldUpdate = _checkForDeviceUpdates(m_sSerialNumber);

      if (shouldUpdate)
        DriverLog("device update available!\n");

      vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer,
                                          vr::Prop_Firmware_UpdateAvailable_Bool, shouldUpdate);
      vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer,
                                          vr::Prop_Firmware_ManualUpdate_Bool, shouldUpdate);


      return vr::VRInitError_None;
    }

    virtual void Deactivate() {
      // even more boilerplate
      DriverLog("device with serial %s deactivated\n", m_sSerialNumber.c_str());
      m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
    }

    virtual void EnterStandby() {}

    virtual void PowerOff() {}

    // debug request from the client, TODO: uh... actually implement this? 
    virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer,
                              uint32_t unResponseBufferSize) {
      DriverLog("device serial \"%s\", got debug request: \"%s\"", m_sSerialNumber.c_str(), pchRequest);
      if (unResponseBufferSize >= 1)
        pchResponseBuffer[0] = 0;
    }

    virtual vr::DriverPose_t GetPose() { return m_Pose; }

    void *GetComponent(const char *pchComponentNameAndVersion) {
      // don't touch this
      DriverLog("device serial \"%s\", got request for \"%s\" component\n", m_sSerialNumber.c_str(), pchComponentNameAndVersion);
      if (!_stricmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version) && m_spExtDisplayComp != nullptr){
        DriverLog("component found, responding...\n");
        return m_spExtDisplayComp.get();
      }
      DriverLog("component not found, request ignored\n");

      return nullptr;
    }

    std::string GetSerialNumber() const { return m_sSerialNumber; }

    // processes events, reacts to haptics only
    void ProcessEvent(const vr::VREvent_t &vrEvent) {
      if constexpr(UseHaptics)
      {
        switch (vrEvent.eventType) {
          case vr::VREvent_Input_HapticVibration: {
            if (vrEvent.data.hapticVibration.componentHandle == m_compHaptic) {
                // haptic!
                DriverLog("%s haptic event: %f, %f, %f\n", m_sSerialNumber, vrEvent.data.hapticVibration.fDurationSeconds,
                          vrEvent.data.hapticVibration.fFrequency, vrEvent.data.hapticVibration.fAmplitude);
            }
          } break;
        }
      }
    }

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

#endif // VR_DEVICE_BASE_H