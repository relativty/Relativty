template<bool UseHaptics>
void Relativty::RelativtyDevice<UseHaptics>::ProcessEvent(const vr::VREvent_t &vrEvent) {
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

template<bool UseHaptics>
void * Relativty::RelativtyDevice<UseHaptics>::GetComponent(const char *pchComponentNameAndVersion) {
	// don't touch this
	DriverLog("device serial \"%s\", got request for \"%s\" component\n", m_sSerialNumber.c_str(), pchComponentNameAndVersion);
	if (!imp_stricmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version) && m_spExtDisplayComp != nullptr){
		DriverLog("component found, responding...\n");
		return m_spExtDisplayComp.get();
	}
	DriverLog("component not found, request ignored\n");

	return nullptr;
}

template<bool UseHaptics>
void Relativty::RelativtyDevice<UseHaptics>::Deactivate() {
	// even more boilerplate
	DriverLog("device with serial %s deactivated\n", m_sSerialNumber.c_str());
	m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
}

template<bool UseHaptics>
void Relativty::RelativtyDevice<UseHaptics>::EnterStandby() {}

template<bool UseHaptics>
void Relativty::RelativtyDevice<UseHaptics>::PowerOff() {}

template<bool UseHaptics>
void Relativty::RelativtyDevice<UseHaptics>::DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) {
	DriverLog("device serial \"%s\", got debug request: \"%s\"", m_sSerialNumber.c_str(), pchRequest);
	if (unResponseBufferSize >= 1)
		pchResponseBuffer[0] = 0;
}

template<bool UseHaptics>
vr::EVRInitError Relativty::RelativtyDevice<UseHaptics>::Activate(vr::TrackedDeviceIndex_t unObjectId) {
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
		vr::VRDriverInput()->CreateHapticComponent(m_ulPropertyContainer, "/output/haptic", &m_compHaptic);
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
