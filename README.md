
<h1 align="center">
  <br>
  <a href="https://github.com/relativty/Relativ"><img src="https://avatars0.githubusercontent.com/u/31791204?s=200&v=4" width="180"></a>
</h1>

# Why Relativty LM? 📖

<p> I love the Relativty headset as it is, but I wanted to do something more with it, just like hand tracking. The first and best way to do this, is using a Leap Motion controller. After some research, and testing, I got it working! So I want to share this with you. Here you can find what you will need, and a small documentation.

# Getting Started ⚡️

Please NOTE, you will need to build the Relativty headset first! Wich you can find here; 

## What you'll need

### What components ?
First you will need the following components:
* Relativty headset
* Leap Motion Controller

### Building and installing the software

First we will install a Leap Motion SteamVR driver from here; https://github.com/cbuchner1/driver_leap/releases

After installation, we will do the following steps;
* Get the Leap Motion Orion Beta runtimes https://developer.leapmotion.com/get-started
* Install the Visual C++ 2015 Update 2 redistributables (32 and 64 bits) https://www.microsoft.com/en-us/download/details.aspx?id=51682
* Download the most recent zip file from the Releases section of this project and run the contained exe setup program https://github.com/cbuchner1/driver_leap/releases

Start SteamVR to see if two additional controllers show up (they should be blinking if your hands are not in the field of view of the Leap Motion, solid otherwise).

If this works, we will add the following driver to the Leap Motion driver; https://github.com/SDraw/driver_leap/releases

After installing, we will do the following steps;
First, extract the files to this folder; Extract files to "<your_drive_letter>:/SteamVR Leap Motion driver/leap/bin/<your_platform>"

# Building the hardware

Simply mount the Leap Motion controller on the front site of your headset!

# ENJOY!
