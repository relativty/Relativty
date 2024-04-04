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
#include "Relativty_ServerDriver.hpp"


static std::shared_ptr<Relativty::ServerDriver> Relativty_Driver;

extern "C" __declspec(dllexport) void* HmdDriverFactory(const char* InterfaceName, int* ReturnCode) {
	if (std::strcmp(InterfaceName, vr::IServerTrackedDeviceProvider_Version) == 0) {
		if (!Relativty_Driver) {
			Relativty_Driver = std::make_shared<Relativty::ServerDriver>();
		}
		return Relativty_Driver.get();
	}
	if (ReturnCode)
		*ReturnCode = vr::VRInitError_Init_InterfaceNotFound;
	return nullptr;
}