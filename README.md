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

<img src="/img/headset.JPG" width="100%">

# Getting Started ⚡️

Relativ is based on <a href="https://github.com/relativty/wrmhl">WRMHL</a> and <a href="https://github.com/relativty/fastVR-sdk">FastVR</a>.

## What you'll need

### What components ?
There are now three variants of Relativ. The original design by the founders **(deprecated)** and two new version by TheYXXY and Vang1 based on a on a model from <a href="http://horizonlab.co/2017/05/28/vr-headset-fully-3d-printed/">Horizon Lab</a>. Shoutout to them for making this possible and allowing their design to be modified! </p>

##### Parts needed for all the designs:
* Arduino Due, $34 for the official one or $10 for a Chinese clone
* GY-521 MPU-6050, $1
* 5.5 inch 2560*1440 2K LCD Screen HDMI to MIPI, up to 100€ on AliExpress (<a href="https://www.aliexpress.com/item/5-5-inch-1440x2560-2K-IPS-LCD-screen-display-with-HDMI-top-MIPI-controller-board-for/32817672501.html">recommended</a>) ⚠️ Price may vary a lot
* Face foam, about $4, see [here](https://www.ebay.com/itm/Face-Foam-Replacement-Eye-Masks-Pads-Cover-Fit-For-HTC-Vive-VR-Goggles-Headphone/152798949280?_mwBanner=1&_rdt=1 "here")

###### Extra needed for the TheYXXY design:
* Biconvex lens, Focal length 37mm, 45mm diameter <a href="https://www.ebay.com/">recommended</a>
* 4 M3 Screws and 2 M2 screws, both with nuts, $3 - alternatively you can use hot glue if M2 screws are not available
* TPU 95 filament is recommended in addition to the normal PLA, and a printer which supports printing it
* A screen which has a cable with a 90 degree bend just like the <a href="https://www.aliexpress.com/item/5-5-inch-1440x2560-2K-IPS-LCD-screen-display-with-HDMI-top-MIPI-controller-board-for/32817672501.html">recommended</a> one. For the direction: it should go up behind the screen when the small non-screen part is on the right while looking at the screen.

###### Extra needed for the Vang1 design:
* Biconvex lens, Focal length 37mm, 45mm diameter <a href="https://www.ebay.com/">recommended</a>
* Headstrap <a href="https://www.ebay.com/itm/SJCAM-SJ4000-SJ5000-Xiaomi-Yi-Head-Strap-Mount-Elastic-Adjustable-Head-Band/272024772003?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2060353.m2749.l2649">recommended</a> (needs a bit modification)
* ABS or PETG filament with 20% infill is recommended, but PLA also works fine. All the parts can be printed without infill.
* M3 screws (More info will be added soon!!)

###### Extra needed for the old (deprecated) design:
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

Now what you need to do is 3D-print the hardware; if needed you can change any models with provided Source Files. 
You will need to print the following :

* (1x): Part 1, Part 2a, Part 2b, Part 3, Part 3a, Part 4, Part 5
* (2x): Part 2c, Part 2d, Part 3b
* (1x) Part 6: Use of Flexible filament with 10% to 50% infill density
* (3x) Part 7: Use of Flexible filament with 10% to 50% infill density
* (1x) Part 8: Use of Flexible filament with 5% infill density (it makes it like a sponge)
* If you uses the the short straps (190mm, because the large ones don't fit in your printer for example), there are extensions (part 9) that should be printed aswell (2x).

⚠️ **PAY EXTRA ATTENTION TO THE SCREEN, IT'S VERY FRAGILE** ⚠️

Additional build information can be found in the build manuals in the 3D-Models folders.

# Building the software

Make sure you have all the libraries needed installed and upload the following program to the Arduino:
#### Path: Relativ/src/main/main.ino
<img src="/img/arduino-upload.gif">

# Play some demos

<p> You can now play with your headset by downloading some cool demos by <a href="https://github.com/relativty/Relativ/releases/download/v0.1-beta/Unity_build.zip">clicking here<a/>. Or build your own game in a minute using the Relativty sdk, learn more by <a href="https://github.com/relativty/fastVR-sdk"> clicking here </a>. </p>
<p> I would love to hear about what you’ve experienced building the headset or help if you have any questions. Ping me at maxime@relativty.com or <a href="https://twitter.com/maximecoutte">@maximecoutte</a>. </p>
<p align="center">🤗You can now chat with me and the Fellowship https://discord.gg/W9VKbjU</p>
<p align="center"> <strong> <> with ❤️, maxime@relativty.com</strong> </p>
