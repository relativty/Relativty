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

<p> My name is <a href="https://twitter.com/maximecoutte">Maxime Coutte</a>.  I’m 16 years old and I live in a small village in the countryside where I’m the only teenager. With my best friends, <a href="https://medium.com/@jonasceccon">Jonas Ceccon</a> and <a href="https://medium.com/@gabrielcombe">Gabriel Combe</a>, and my math teacher <a href="https://www.linkedin.com/in/jérôme-dieudonne-696a18b5"> Jerome Dieudonne </a> (we call him Sensei) we built our own VR headset for $100.

<p> I started programming when I was 13, thanks to Sensei when he created a robotics club. On the first day we were 12 students, the next week we were 3 - yep, you guessed it, the other two were Gabriel and Jonas. I fell in love with VR because of an anime called SAO, but the problem was that the Oculus Rift was way too expensive for me. I talked about this with Gabriel and we both agreed that we should build our own VR headset. That got us into the math and physics behind VR (quaternions, proper acceleration, antiderivatives…). Then we bought the cheapest components we could and we reinvented VR. Now using <a href="https://github.com/relativty/fastVR-sdk">FastVR</a> and Relativ you can
build your VR headset too. </p>
<p> Here's a schematic by TheYxxy to give you an overview of the project: </p>
  <a href="https://github.com/TheYxxy/Relativ"><img src="https://raw.githubusercontent.com/TheYxxy/Relativ/master/img/Schematic.png"  width=100%></a>



# Meet the team ❤️

<img align="right" width="100" height="100" src="https://github.com/relativty/Relativ/blob/master/img/gabriel.png"> </p>
<p align="left"> <strong> Gabriel: </strong>I work on optics for <a href="https://github.com/relativty/fastVR-sdk">FastVR</a>, 3D conception with Maxime and cheap tracking for the next big update. </p>
<br>
<br>
<img align="right" width="100" height="100" src="https://github.com/relativty/Relativ/blob/master/img/maxime.png"> </p>
<p align="left"> <strong> Maxime: </strong> I created <a href="https://github.com/relativty/wrmhl">WRMHL</a> and with Gabriel we created <a href="https://github.com/relativty/fastVR-sdk">FastVR</a>. I'm also behind the code for the hardware. </p>
<br>
<br>
<img align="right" width="100" height="100" src="https://github.com/relativty/Relativ/blob/master/img/jonas.png"> </p>
<p align="left"> <strong> Jonas: </strong>  I convinced a Chinese factory to sell us premium components at low prices and I'm working on a Master Guide for Relativ. </p>
<br>
<br>
<img align="right" width="100" height="100" src="https://github.com/relativty/Relativ/blob/master/img/sensei.png"> </p>
<p align="left"> <strong> Sensei: </strong>  I'm the theoretician of the team. I teach them math and I help them to solve algorithm issues. </p>

# Open-Source it FTW

<p> Thanks to this project I’ve had the incredible fortune to meet amazing people, including the chief architect at Oculus, Atman Binstock. He gave me a precious piece of advice: "open source it". I deleted all the code I had, and started rewriting it all from scratch, better. And I convinced my two friends and our math teacher to open source the project </p>

<img src="/img/headset.JPG" width="70%">

# Getting Started ⚡️

**PLEASE NOTE**; We highly recommend that you use the STM32 tracker because it has proved to be more stable than the Due based one. The Due code has recently been reported to throw compiler errors, and the Due trackers have exhibited stability issues. Because of this, we will be phasing out support for the Arduino DUE code in the next couple of months. *(NOTE added on Apr 17, 2019)*

## What you'll need

### What components ?
There are a few variants of Relativ. The original design by the founders **(deprecated)** and two new version by TheYXXY and Vang1 based on a on a model from <a href="http://horizonlab.co/2017/05/28/vr-headset-fully-3d-printed/">Horizon Lab</a>. Shoutout to them for making this possible and allowing their design to be modified! </p>

##### 3D models
All our 3D models can be found on our Thingiverse account; https://www.thingiverse.com/Relativty/designs. We have decided to move our 3D models to Thingiverse, because of the amount of different designs, and remakes. This to keep the GitHub file zise low, and people don't have to downlaod *all* the 3D models with the code together. Some 3D designs require additional parts, they can be found on the Thingiverse page itself.

##### Parts needed for all the designs:
* STM32, $3,50 [recommended](https://robotdyn.com/stm32-arm-arduino-mini-system-dev-board-blue-pill-with-arduino-bootloader.html "recommended")
* ST-link V2 USB ~$3,00 [recommended](https://www.amazon.com/ "recommended") ⚠️ Price may vary a lot
* GY-521 MPU-6050, $1
* 5.5 inch 25601440 2K LCD Screen with an HDMI to MIPI board, between €65 and €95 on AliExpress (<a href="https://www.aliexpress.com/item/5-5-inch-1440x2560-2K-IPS-LCD-screen-display-with-HDMI-top-MIPI-controller-board-for/32817672501.html">recommended</a>) ⚠️ Price may vary a lot
* Face foam (10mm wide), about $4, see [here](https://www.ebay.com/sch/i.html?_from=R40&_trksid=m570.l1313&_nkw=face+foam+vr+headset+10mm&_sacat=0 "here")

<img src="/img/component.JPG" width="100%">

### How to Install?

You can either install it using [Git](https://git-scm.com/) or direct [Download](https://github.com/relativty/Relativ/archive/master.zip). Or from the <strong>command line</strong>:

```bash
# Clone this repository
$ git clone https://github.com/relativty/Relativ
```

# Building the headset

Detailed instructions and parts lists can be found in the Relativ Wiki at:

https://wiki.relativty.net/index.php/STM32

Basically, copy the contents of **"library"** folder into your Arduino libraries folder, then copy the main STM32-Tracker folder (which contains the **STM32_BlackPill** and **STM32_BluePill** folders) into your main Arduino folder. However, all the critical information that you need to build this tracker is in the Wiki, so you really do need to read it before you start this build.

Because we had to edit some of the main library files so that they would compile on an STM32 board, we have included them in each sketch folder (so that the changes don't mess up any other sketches that use those particular libraries). However, bacause other parts of the code rely on the un-patched libraries, we have to make the original libraries available in the main arduino libraries folder as well.

These really should work straignt out of the box - however, if they don't then please raise a GitHub issue or message us on the Relativ Discord.

# Start playing your favorite games!

<p> You can now play with your headset by downloading SteamVR, and OSVR.
<p> I would love to hear about what you’ve experienced building the headset or help if you have any questions. Ping me at maxime@relativty.com or <a href="https://twitter.com/maximecoutte">@maximecoutte</a>. </p>
<p align="center">🤗You can now chat with me and the Fellowship https://discord.gg/W9VKbjU</p>
<p align="center"> <strong> <> with ❤️, maxime@relativty.com</strong> </p>
