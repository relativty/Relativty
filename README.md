<h1 align="center">
  <br>
  <a href="https://github.com/relativty/Relativ"><img src="https://avatars0.githubusercontent.com/u/31791204?s=200&v=4" width="180"></a>
</h1>

<h4 align="center">We couldn't afford an Oculus so we built one
</h4>

<p align="center">
  <img src="https://img.shields.io/github/license/relativty/Relativ.svg">
  <img src="https://img.shields.io/github/stars/relativty/Relativ.svg">
  <img src="https://img.shields.io/github/issues/relativty/Relativ.svg">
</p>

<img src="https://github.com/relativty/fastVR-sdk/blob/master/img/fastVR_demo.gif" width="100%">
<h4 align="center">Build your own VR headset for $100 💸
</h4>

# Why Relativ? 📖

<p> My name is <a href="https://twitter.com/maximecoutte">Maxime Coutté</a>.  I’m 16 years old and I live in a small village in France where I’m the only teenager. With my best friends, <a href="https://medium.com/@jonasceccon">Jonas Ceccon</a> and <a href="https://medium.com/@gabrielcombe">Gabriel Combe</a>, and my math teacher <a href="https://www.linkedin.com/in/jérôme-dieudonne-696a18b5"> Jerome Dieudonne </a> (we call him Sensei) we built our own VR headset for $100.

<p> I started programming when I was 13, thanks to Sensei when he created a robotics club. On the first day we were 12 students, the next week we were 3 - yep, you guessed it, the other two were Gabriel and Jonas. I fell in love with VR because of an anime called SAO, but the problem was that the Oculus Rift was way too expensive for me. I talked about this with Gabriel and we both agreed that we should build our own VR headset. That got us into the math and physics behind VR (quaternions, proper acceleration, antiderivatives…). Then we bought the cheapest components we could and we reinvented VR. Now using <a href="https://github.com/relativty/fastVR-sdk">FastVR</a> and Relativ you can
build your VR headset too. </p>

# Meet the team ❤️

<p align="center"> <img src="/img/gabriel.png" width="370"> </p>
<p align="center"> <strong> Gabriel: </strong>I work on optics for <a href="https://github.com/relativty/fastVR-sdk">FastVR</a>, 3D conception with Maxime and cheap tracking for the next big update. </p>

<p align="center"> <img src="/img/maxime.png" width="370"> </p>
<p align="center"> <strong> Maxime: </strong> I created <a href="https://github.com/relativty/wrmhl">WRMHL</a> and with Gabriel we created <a href="https://github.com/relativty/fastVR-sdk">FastVR</a>. I'm also behind the code for the hardware. </p>
<p align="center"> <img src="/img/jonas.png" width="370"> </p>
<p align="center"> <strong> Jonas: </strong>  I convinced a Chinese factory to sell us premium components at low prices and I'm working on a Master Guide for Relativ. </p>
<p align="center"> <img src="/img/sensei.png" width="370"> </p>
<p align="center"> <strong> Sensei: </strong>  I'm the theoretician of the team. I teach them math and I help them solving algorithm issues. </p>

# Open-Source it FTW

<p> Thanks to this project I’ve had the incredible fortune to meet amazing people, including the chief architect at Oculus, Atman Binstock. He gave me a precious piece of advice: "open source it". I deleted all the code I had, and started rewriting it all from scratch, better. And I convinced my two friends and our math teacher to put the project into open source. </p>

<img src="/img/headset.JPG" width="100%">

# Getting Started ⚡️

Relativ is based on <a href="https://github.com/relativty/wrmhl">WRMHL</a> and <a href="https://github.com/relativty/fastVR-sdk">FastVR</a>.

## What you'll need

### What components ?
First you will need the following components:
* Arduino Due, $34 for the official one or $10 for a Chinese clone
* GY-521 MPU-6050, $1
* 5.5 inch 2560*1440 2K LCD Screen HDMI to MIPI, up to 100€ on AliExpress (<a href="https://www.aliexpress.com/item/5-5-inch-1440x2560-2K-IPS-LCD-screen-display-with-HDMI-top-MIPI-controller-board-for/32817672501.html">recommended</a>) ⚠️ Price may vary a lot
* Fresnel lens Focal Length 80mm, $3 (<a href="https://www.ebay.com/itm/2Pcs-Fresnel-lens-Focal-Length-40-50-55-60-70-80mm-for-Google-Cardboard-3D-VR/201984369021">recommended</a>)

<img src="/img/component.JPG" width="100%">

### How to Install?

You can either install it using [Git](https://git-scm.com/) or direct [Download](https://github.com/relativty/Relativ/archive/master.zip). Or from the <strong>command line</strong>:

```bash
# Clone this repository
$ git clone https://github.com/relativty/Relativ
```
Copy all folders in **Relativ/src/libraries** and past them in your Arduino Libraries folder: **Documents\Arduino\libraries**. You're now ready to build the headset!

# Building the hardware

Now what you need to do is 3D-print the hardware; if needed you can change any models with provided Source Files. You will need to print the following:
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

Make sure you have all the libraries needed installed and upload the following program to the Arduino:
#### Path: Relativ/src/main/main.ino
<img src="/img/arduino-upload.gif">

# Play some demos

<p> You can now play with your headset by downloading some cool demos by <a href="https://github.com/relativty/Relativ/releases/download/v0.1-beta/Unity_build.zip">clicking here<a/>. Or build your own game in a minute using the Relativty sdk, learn more by <a href="https://github.com/relativty/fastVR-sdk"> clicking here </a>. </p>
<p> I would love to hear about what you’ve experienced building the headset or help if you have any questions. Ping me at maxime@relativty.com or <a href="https://twitter.com/maximecoutte">@maximecoutte</a>. </p>
<p align="center">😍🤙 You can now chat with me and the Fellowship https://discord.gg/W9VKbjU</p>
<p align="center"> <strong> <> with ❤️, maxime@relativty.com</strong> </p>
