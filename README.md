<h1 align="center">
  <br>
  <a href="https://github.com/relativty/Relativ"><img src="https://avatars0.githubusercontent.com/u/31791204?s=200&v=4" width="180"></a>
</h1>

<h4 align="center">We can't afford an Oculus so we built one
</h4>

<p align="center">
  <img src="https://img.shields.io/github/license/relativty/Relativ.svg">
  <img src="https://img.shields.io/github/stars/relativty/Relativ.svg">
  <img src="https://img.shields.io/github/issues/relativty/Relativ.svg">
</p>

<img src="https://github.com/relativty/fastVR-sdk/blob/master/img/fastVR_demo.gif" width="100%"  height="500">
<h4 align="center">Build your own VR headset for 100$ 💸
</h4>

# What is Relativ ? 📖

<p> My name is <a href="https://twitter.com/maxcoutte">Maxime Coutté</a>. I’m 16 years old, I live in a small village in France where I’m the only teenager. With my best friends, <a href="https://medium.com/@jonasceccon">Jonas Ceccon</a> and <a href="https://medium.com/@gabrielcombe">Gabriel Combe</a>, and my math teacher (we call him Sensei) we
built our own VR headset for 100$. </p>

<p> I started programming when I was 13, thanks to Sensei, he created a robotic club. The first day we were 12 students, the next week we were 3, you guess the two other were Gabriel and Jonas. Because of an anime called SAO, I fell in love with VR, but the problem was that the Oculus Rift was way too expensive for me. I talked about this to Gabriel and we both agree that we should build our own VR headset. That got us into the
math and physics behind VR (quaternions, proper acceleration, antiderivatives…). Then we bought the cheapper component we could and we re invent VR. Now using <a href="https://github.com/relativty/fastVR-sdk">FastVR</a> and Relativ you can
build your VR headset too. </p>

# Who we are ?

<p align="center"> <img src="/img/gabriel.png" width="370"> </p>
<p align="center"> <strong> Gabriel: </strong>I work on optics for <a href="https://github.com/relativty/fastVR-sdk">FastVR</a>, 3D conception with Maxime and cheap tracking for the next big update. </p>

<p align="center"> <img src="/img/maxime.png" width="370"> </p>
<p align="center"> <strong> Maxime: </strong> I created <a href="https://github.com/relativty/wrmhl">WRMHL</a> and with Gabriel we created <a href="https://github.com/relativty/fastVR-sdk">FastVR</a>. I'm also behind the code of the hardware. </p>
<p align="center"> <img src="/img/jonas.png" width="370"> </p>
<p align="center"> <strong> Jonas: </strong> I conviced chinese factory to sell us component with premium prices and I'm working on a Master Guide for Relativ. </p>
<p align="center"> <img src="/img/sensei.png" width="370"> </p>
<p align="center"> <strong> Sensei: </strong> I'm the theorician of the team, I teach them math and I help them solving algorithme issues </p>

# Why open source ? ❤️

<p> Thanks to this project I have had the incredible chance to meet amazing people, including the chief architect at Oculus, Atman Brinstock. He gave me a precious advice "open source-it". I deleted all the code I had, and started rewriting it all from scratch, better. And I convinced my two friends and our math teacher to put the project into open source. </p>

<img src="/img/headset.JPG" width="100%">

# Getting Started ⚡️

## What you'll need

### What component ?
<p> First will need to the following componant:
* Arduino Due, 34$ for the offical one and 10$ for a Chinese clone
* GY-521 MPU-6050, 1$
* 5.5 inch 2560*1440 2K LCD Screen HDMI to MIPI, from 45$ on Alie Express (recommended) ⚠️ Price may vary a lot
* Fresnel lens Focal Length 50mm (recommended)
</p>

<img src="/img/component.JPG" width="100%">

### How to Install ?

you can either install it using [Git](https://git-scm.com/) or direct [Download](https://github.com/relativty/Relativ/archive/master.zip). Or from the <strong>command line</strong>:

```bash
# Clone this repository
$ git clone https://github.com/relativty/Relativ
```
Copy all folders in **Relativ/src/libraries** and past them in your Arduino Librairies folder: **Documents\Arduino\libraries**. You're now good to Build the headset !

# Building the hardware

Now what you need is to 3D print the hardware, if needed you can change any models with the Source Files provided. You will need to print the following:
* Relativ_headset_structure.STL x1
* box_Arduino_DUE.stl x1
* box_lens_1.STL x2 ⚠️
* box_lens_2.STL x2 ⚠️
* box_mpu6050.STL x1
* box_screen_adaptor.STL x1
* box_screen_controller.STL x1
* box_screen_spherical.STL x1 OR box_screen.STL x1 ⚠️
* support_lens.STL x1

# Building the software

Make sure you have all the librairies needed installed and upload to the Arduino the following program:
#### Path: Relativ/src/main/main.ino
<img src="/img/arduino-upload.gif">

# Play some demos

<p align="center"> <strong> <> with ❤️, maxime@relativty.com</strong> </p>
