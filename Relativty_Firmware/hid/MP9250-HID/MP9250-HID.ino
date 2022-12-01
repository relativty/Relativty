/*
 * Alternative Relativty firmware using mpu9250 for 3dof head tracking
 * created by 7ep3s with invaluable help from okawo and Seneral
 * you can get in touch on Relativty's Discord server
 */
#include <SparkFunMPU9250-DMP.h>
#include "Wire.h"
#include "I2Cdev.h"
#include "HID.h"
#if !defined(_USING_HID)
#error "Using legacy HID core (non pluggable)"
#endif

static const uint8_t _hidReportDescriptor[] PROGMEM = {
 0x06, 0x00, 0xff,         // USAGE_PAGE (vendor defined)
    0x09, 0x00,         // USAGE (Undefined)
    0xa1, 0x01,         // COLLECTION (Application)
    0x15, 0x00,         //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,   //   LOGICAL_MAXIMUM (255)
    0x85, 0x01,         //   REPORT_ID (1)
    0x75, 0x08,         //   REPORT_SIZE (16)
    
    0x95, 0x3f,         //   REPORT_COUNT (1)
    
    0x09, 0x00,         //   USAGE (Undefined)
    0x81, 0x02,         //   INPUT (Data,Var,Abs) - to the host
    0xc0           
};

MPU9250_DMP imu;
float quat[4];


void setup() {
  static HIDSubDescriptor node (_hidReportDescriptor, sizeof(_hidReportDescriptor));
  HID().AppendDescriptor(&node);

  
  Wire.begin();
  Wire.setClock(400000);
  delay(1000);
  
  // Call imu.begin() to verify communication and initialize
  if (imu.begin() != INV_SUCCESS)
  {
    while (1)
    {
      Serial.println("Unable to communicate with MPU-9250");
      Serial.println("Check connections, and try again.");
      Serial.println();
      delay(5000);
    }
  }
  
  imu.dmpBegin(DMP_FEATURE_6X_LP_QUAT | // Enable 6-axis quat
               DMP_FEATURE_GYRO_CAL, // Use gyro calibration
              60); // Set DMP FIFO rate to 10 Hz
  // DMP_FEATURE_LP_QUAT can also be used. It uses the 
  // accelerometer in low-power mode to estimate quat's.
  // DMP_FEATURE_LP_QUAT and 6X_LP_QUAT are mutually exclusive

}

void loop() {
  // Check for new data in the FIFO
  if ( imu.fifoAvailable() )
  {
    // Use dmpUpdateFifo to update the ax, gx, mx, etc. values
    if ( imu.dmpUpdateFifo() == INV_SUCCESS)
    {
      // computeEulerAngles can be used -- after updating the
      // quaternion values -- to estimate roll, pitch, and yaw
      imu.computeEulerAngles();
      printIMUData();
    }
  }

}
void printIMUData(void)
{  
  // After calling dmpUpdateFifo() the ax, gx, mx, etc. values
  // are all updated.
  // Quaternion values are, by default, stored in Q30 long
  // format. calcQuat turns them into a float between -1 and 1

  quat[0] = imu.calcQuat(imu.qw);
  quat[1] = imu.calcQuat(imu.qy);
  quat[2] = imu.calcQuat(imu.qz);
  quat[3] = imu.calcQuat(imu.qx);
  
  HID().SendReport(1,quat,63);
}
