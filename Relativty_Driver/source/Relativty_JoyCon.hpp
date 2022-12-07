#include "../include/Relativty_base_device.hpp"

namespace Relativty {
	class JoyConDriver :  public RelativtyDevice<false> {
		explicit JoyConDriver(const std::string &myserial);
		~JoyConDriver();

		// Inherited from RelativtyDevice, to be overridden
		vr::EVRInitError Activate(uint32_t unObjectId) override;
		void Deactivate() override;
	};
}
