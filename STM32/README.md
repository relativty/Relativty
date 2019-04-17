# STM32-Tracker
An HMD VR Tracker based on an STM32 board

This repo is based on the original Relativty Arduino Due code, which was then re-developed by Saraghz for the STM32 boards. I tested it with Saraghz's help, tweaked it a bit more, cleaned it up a bit, wrote the instructions and did my best to make it an easily approachable build.

Detailed instructions and parts lists can be found in the Relativ Wiki at:

https://wiki.relativty.net/index.php/STM32

Basically, copy the contents of "library" folder into your Arduino libraries folder, then copy the main STM32-Tracker folder (which contains the STM32_BlackPill and STM32_BluePill folders) into your main Arduino folder. However, all the critical information that you need to build this tracker is in the Wiki, so you really do need to read it before you start this build.

Because we had to edit some of the main library files so that they would compile on an STM32 board, we have included them in each sketch folder (so that the changes don't mess up any other sketches that use those particular libraries). However, bacause other parts of the code rely on the un-patched libraries, we have to make the original libraries available in the main arduino libraries folder as well.

These really should work straignt out of the box - however, if they don't then please raise a GitHub issue or message me on the Relativ Discord
Justin
