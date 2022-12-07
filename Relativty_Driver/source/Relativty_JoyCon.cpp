#include "Relativty_JoyCon.hpp"

using namespace Relativty;

JoyConDriver::JoyConDriver(const std::string &serial):RelativtyDevice(serial, "akira_") {

}

vr::EVRInitError JoyConDriver::Activate(uint32_t unObjectId) {
    RelativtyDevice::Activate(unObjectId);


    return vr::VRInitError_Init_InterfaceNotFound;
    return vr::VRInitError_None;
}
