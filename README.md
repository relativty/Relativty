<h1 align="center">
	<img
		width="400"
		alt="Relativty"
		src="ressources/img/title.png">
</h1>

<h3 align="center">
	Open-source VR headset with SteamVR support.
</h3>
<p align="center">
	<strong>
		<a href="https://relativty.com">Website</a>
		•
		<a href="https://discord.gg/jARCsVb">Discord</a>
	</strong>
</p>

I’m <a href="https://twitter.com/maxim_xyz?lang=en">Maxim xyz</a> and when my best friend Gabriel Combe and I were 15 years old we built our own VR headset because we couldn't afford to buy one.

5 years later : this headset became Relativty.

* Fully Open-source - **hardware**, **software**, **firmware**.
* **Steam VR** support.
* Natively displays **2K** resolution at **120FPS**.
* Compatible with **Arduino**.
* Experimental **Body-Tracking**


**This repo serves as a Build guide, to learn more about the headset see you at <a href="https://relativty.com">Relativty.com</a>**

Made for Hackers
-
Relativty is **NOT A CONSUMER PRODUCT**. We made Relativty in my bedroom with a soldering iron and a 3D printer and we expect you to do the same: **build it yourself**.

Relativty at its basics is a 3 Degrees of Freedom (3-DoF) VR headset with no built-in controller support, designed to work with SteamVR.
This may limit its practical usability for games that demand knuckle-style VR controllers.
If you are looking for a more full-featured DIY VR project 6-Dof tracking and controller support, we recommend you check out <a href="https://github.com/HadesVR/HadesVR">HadesVR</a>.

Whether or not you are building either Relativty or HadesVR, or if you just want to join our community and hang out, we invite you to <a href="https://discord.gg/F8GNKjy6RF">Relativty's Guild on Discord</a>.

It is a place for the DIY/Open-Source VR community to learn, grow, share knowledge, and ask for help. 

# Start Building Relativty

<p align="center"> <img src="ressources/img/open.jpg"> </p>

# Building The Hardware
## 1. CURRENT RECOMMENDED BUILD - As of May 2023
### 1.1 Introduction
This is an updated build guide to help people who recently discovered Relativty. It aims to explain how to build the minimum functional product, which can be further improved upon and modified to the individual's liking. The original build guide can still be found further below.


### 1.2 ELECTRONICS

The following electronic components are required:

- ATmega32U4 Microcontroller (MCU) with USB HID support, such as an <a href="https://www.amazon.co.uk/diymore-Atmega32U4-Development-Microcontroller-Header/dp/B0BKGSVX2X">Arduino Pro Micro</a>
- Inertial Measurement Unit (IMU) that is supported by the <a href="https://github.com/LiquidCGS/FastIMU">FastIMU Library</a>
- A small, high resolution and high refresh rate PC display + driver board, such as <a href="https://www.aliexpress.com/item/32975198897.html">this model</a>

            NOTES on VR DISPLAYS:
            - The display does not connect to the Microcontroller, it only connects to the Computer running your VR Apps.
            - Technically, any PC display/monitor can be configured as the display used by Relativty. Therefore, you can simply test your build on your PC monitor
            first to make sure it works, before you decide to spend a significant amount of money on lenses, display(s) and other parts.
            - High performance, small form factor displays are expensive, and often very delicate. Handle them with care! :)

### 1.2.1 Wiring the IMU and MCU
The IMU needs to be connected to the MCU for power and communication.
The MCU connects to your computer via USB to send the IMU readings to SteamVR.

In case of an Arduino Pro Micro, you need to connect the following pins:
```
Pro Micro   IMU
VCC     ->  VCC  
GND     ->  GND  
SDA(2)  ->  SDA  
SCL(3)  ->  SCL  
```  
If you use a different MCU, the SDA and SCL pins might be mapped to different pin numbers.

Also, make sure that the VCC of your MCU is compatible with the rated operating voltage of your IMU.

Supplying incorrect voltage to electronic components may cause damage to them.

The MCU itself simply connets to your computer via the USB port.

### 1.2.2 Connecting the display
As mentioned previously, any display that is compatible with a personal computer should be able to function as your VR display.

In case you are using the recommended one or a similar component that is powered via micro-USB or other USB standard, it is possible that you will encounter situations where the board does not power up. This could be either because the micro-USB cable you are using is too long (so the board cannot power up because the voltage dropped too much), or the board is unable to turn on because the USB port it connects to on the computer side is simplay unable to supply enough power. In this case, you might need to try different ports on your computer or a powered USB hub.


# 2. LEGACY BUILD
The hardware is based on the Relativty Motherboard which includes an Atmel SAM3X8E ARM Cortex-M3 processor and uses an MPU-6050 as it’s IMU.
Alternatively, any processor that supports the ArduinoCore and is connected to an MPU-6050/MPU-9250 can be used as the hardware for Relativty. Both methods are explained below.

## Building The Relativty Motherboard
### PCB Manufacturing.

We first start with the naked PCB.

it can be manufactured and bought for around $4 on websites like <a href="https://jlcpcb.com/">jlcpcb</a>.

You’ll need to provide the Gerber file folder `Relativty_Electronics_build/GerberFiles.zip` which describes the shape of the board.


### Assembling

Soldering the components onto the naked PCB. 

You’ll have to buy the components listed in one of the two BOMs, depending on availability:
- `Relativty_Electronics_build/Assembly/jlcpcb.com_motherboard_BOM.csv` 
- `Relativty_Electronics_build/Assembly/motherboard_BOM.xlsx`

Where to position those components on the PCB is described in the file `Relativty_Electronics_source/motherboard.brd` which can be opened in Eagle.

<p align="center"> <img src="ressources/img/motherboard.jpg"> </p>

#### Using an Arduino

An alternative to the Relativty Motherboard is to use an Arduino Due and to connect it to an MPU-6050.
```
5V      -> VCC  
GND     -> GND  
SDA(20) -> SDA  
SCL(21) -> SCL  
PIN 2   -> INT  
```

If you are using an MPU-9250 with the alternative firmware provided the pinout is:

```
5V      -> VCC  
GND     -> GND  
SDA(20) -> SDA  
SCL(21) -> SCL   
```

Then push the `ERASE` and the `RESET` button on the Arduino Due and you’ll be able to install the Relativty Firmware.

#### Installing The Relativty Firmware

<p align="center"> <img src="ressources/img/cards.jpg"> </p>
You’ll now need to install the Relativty board in the Arduino IDE. 

To do that copy that JSON URL: https://raw.githubusercontent.com/relativty/Relativty/master/Relativty_Firmware/package_Relativty_board_index.json and open the Arduino IDE

In Arduino, Click File and then Preferences:

- If you are using the Relativty PCB, aad the JSON URL to the `Additional Boards Manager` text box.
- Go to `Tools > Board > Board Manager` and you should see the Relativty Board, click install.
- Reboot the Arduino IDE and under Tools > Boards, select Relativty.
- You can now open `Relativty_Firmware/firmware/firmware.ino` and upload it to your board.

If you are using a different board e.g. Arduino Due:

- install contents of `Relativty_Firmware\Relativty_board\` to your Arduino IDE
- if you are using MPU-6050, use `Relativty_Firmware/firmware/firmware.ino`
- if you are using MPU-9250, use `Relativty_Firmware\MP9250-HID\MP9250-HID\MP9250-HID.ino`



# Building The Mechanical Parts

## Assembling the HMD

All the files needed for the 3D printing can be found in the `Relativty_Mechanical_build` folder and the screws needed to assemble the Headset are listed in `screws_BOM.xlsx`.

We've used parts from Aliexpress:

- <a href="https://www.aliexpress.com/item/33058848848.html">The Strap</a>, 
- <a href="https://www.aliexpress.com/item/4000199486058.html">The Foam</a>,
- <a href="https://www.aliexpress.com/item/33029909783.html">The Lenses</a> (40mm diameter/50mm focal length).

### The screen for the HMD
<p align="center"> <img src="ressources/img/display.jpg"> </p>

Relativty Headset runs a dual-screen at 120FPS 2K, however, because of the open nature of Relativty you can equip it with any screen.

Our model can be found on Aliexpress, but depending on the vendor similar screens can cost from $150 to $190. You'll have to hunt and maybe wait for the right vendor at the right price to get the display for cheap (or buy in bulk). 

This is [the model we used](https://www.aliexpress.com/item/32975198897.html).

### Setting Up the Software

<p align="center"> <img src="ressources/img/front.jpg"> </p>

#### Installing Relativty Driver for SteamVR

The Relativty Driver is contained within `Relativty_Driver/Relativty` folder. 

⚠️ You’ll need to set it up by editing the JSON file `Relativty_Driver/Relativty/resources/settings/default.vrsettings`

If you are not using a Relativty PCB, you will need to change these:

      "hmdPid" : 9,
      "hmdVid": 4617,
	  
These are the USB HID device's unique Vendor and Producit Identifieres (pid/vid)

If you are using and Arduino Due, the correct values will be:

      "hmdPid" : 62,
      "hmdVid" : 9025,
	  
In case you are using a different board, the process to get the right values is as below:

1.	Plug your board in

2.	Select your board in Arduino IDE and click Tools/Get Board info. you will see something like this:

```
	BN: Arduino Due (Native USB Port)
	VID: 2341
	PID: 003e
	SN: HIDHB
```
3.	Make note of the VID and PID numbers. These are hexadecimal values.

	To apply them to the config, they need to be converted to int.
	
	If you are unsure how to do that, there is plenty online converters available.
	
	Such as: https://www.rapidtables.com/convert/number/hex-to-decimal.html

4.	Change your hmdPid and hmdVid values to the converted values.

Next, you need to set up the display coordinates and resolution.

At first, you should have the HMD's display set up as a secondary screen extending your desktop,

aligned onto the top right corner of your primary display.


In the config file's "Relativty_extendedDisplay" segment, find and set these:
```
      "windowX" : *whatever your primary screen resolution's width is*,
      "windowY" : 0,
      "windowWidth" : *HMD's native resolution width*,
      "windowHeight" : *HMD's native resolution height*,
      "renderWidth" : *HMD's native resolution width*,
      "renderHeight" : *HMD's native resolution height*,
	  
	  And at the bottom of this segment:
	  
      "IsDisplayRealDisplay" : true,
      "IsDisplayOnDesktop" : true
```  
	  
Make sure not to delete any "," symbols as that will break the config.

Only the last item in the config should not have a "," symbol.


If for whatever reason the above settings do not work out for you try:


Set your HMD display as a mirrored display of your primary display.

Change config as follows:
```
      "windowX" : 0,
      "windowY" : 0,
      "windowWidth" : *HMD's native resolution width*,
      "windowHeight" : *HMD's native resolution height*,
      "renderWidth" : *HMD's native resolution width*,
      "renderHeight" : *HMD's native resolution height*,
	
      "IsDisplayRealDisplay" : false,
      "IsDisplayOnDesktop" : true
	
```	
⚠️ Please note that this may result in keyboard/mouse input not being captured by the VR window, should your game require it, it might become unplayable.

You can also make IPD (Interpupillary Disance) adjustments within the configuration file:

In the "Relativty_hmd" segment find and adjust:

```
      "IPDmeters" : 0.063,
```

You can also change the lens distortion correction by changing these:

```
      "DistortionK1" : 0.4,
      "DistortionK2" : 0.5,
```

You can now install Relativty Driver:
- Locate your `vrpathreg.exe` program, usually located at `C:/Steam/steamapps/common/SteamVR/bin/win64/vrpathreg.exe`
- Then open the Windows Command Prompt and run the following commands:
`cd C:/Steam/steamapps/common/SteamVR/bin/win64
vrpathreg.exe`

And then assuming your `Relativty_Driver/Relativty` driver folder is located at:
`C:/code/Relativty_Driver/Relativty`
- run `vrpathreg adddriver C:/code/Relativty_Driver/Relativty`

Relativty Driver is now installed. You can uninstall it any time by running:  
- `vrpathreg removedriver C:/code/Relativty_Driver/Relativty`

#### Setting up the Experimental 3D Tracking

The tracking is still very experimental and can only be run on NVIDIA GPU due to the usage of CUDA. The tracking makes uses of only a video input and an Artificial Neural Network AI trained to estimate a 3D body position. 

This method is nothing close to the precision or freedom of movements of a dedicated sensor however we believe that the model can be trained and improved by orders of magnitude.

You first need to connect a webcam to your computer and install Python 3.8.4 and select the option to add it to the path. Then install `PyTorch`, you can do so by running the following commands:

`python -m pip install torch===1.6.0 torchvision===0.7.0 -f https://download.pytorch.org/whl/torch_stable.html`

Then proceed to install `CUDA Toolkit 11.0.`

You’ll then need to download the Folder `PYTHONPATH` at https://github.com/relativty/Relativty/releases and add its location to `PyPath` in the `JSON Relativty_Driver/Relativty/resources/settings/default.vrsettings`

Set `tracking` to 1. The tracking is now turned on and can be turned off anytime by setting `tracking` to 0. The tracking can also be calibrated depending on your camera, this can be done by tweeking `scalesCoordinateMeter` and `offsetCoordinate`. `scalesCoordinateMeter` correspond to the delta of coordinates in a given axis calculated by the AI after normalization when you move 1 meter in that given axis.

**Note:** If you plan on using mpu9250 firmware do not forget to switch to it in the driver by setting `hmdIMUdmpPackets` to `false`:
`"hmdIMUdmpPackets":  false,`

## Final Steps

Everything is now set up to start playing. 

To start using Relativty:
- Lay the headset on a flat surface with the front panel facing the ground
- Plug in the Headset.
It’ll automatically calibrate after a few seconds.

You can now run any SteamVR game!

