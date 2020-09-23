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

#include "openvr_driver.h"

#include "driverlog.h"

#include "Relativty_ServerDriver.hpp"
#include "Relativty_HMDDriver.hpp"

vr::EVRInitError Relativty::ServerDriver::Init(vr::IVRDriverContext* DriverContext) {

	vr::EVRInitError eError = vr::InitServerDriverContext(DriverContext);
		if (eError != vr::VRInitError_None) {
			return eError;
	}
	#ifdef DRIVERLOG_H
	InitDriverLog(vr::VRDriverLog());
	DriverLog("Relativty driver version 0.1.1"); // report driver version
	DriverLog("Thread1: hid quaternion packet listener loop");
	DriverLog("Thread2: update driver pose loop");
	DriverLog("Thread3: receive positional data from python loop");
	#endif

	this->Log("Relativty Init successful.\n");
	
	this->HMDDriver = new Relativty::HMDDriver("zero");
	vr::VRServerDriverHost()->TrackedDeviceAdded(HMDDriver->GetSerialNumber().c_str(), vr::ETrackedDeviceClass::TrackedDeviceClass_HMD, this->HMDDriver);
	// GetSerialNumber() is there for a reason!

	return vr::VRInitError_None;
}

void Relativty::ServerDriver::Cleanup() {
	delete this->HMDDriver;
	this->HMDDriver = NULL;

	#ifdef DRIVERLOG_H
	CleanupDriverLog();
	#endif

	VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

const char* const* Relativty::ServerDriver::GetInterfaceVersions() {
	return vr::k_InterfaceVersions;
}

void Relativty::ServerDriver::RunFrame() {} // if ur not using it don't populate it with garbage!

bool Relativty::ServerDriver::ShouldBlockStandbyMode() {
	return false;
}

void Relativty::ServerDriver::EnterStandby() {

}

void Relativty::ServerDriver::LeaveStandby() {

}

void Relativty::ServerDriver::Log(std::string log) {
	vr::VRDriverLog()->Log(log.c_str());
}