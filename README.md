# CyanogenMod 11 for P-01D
## Working
*  Boot
*  Display
*  Hardware Keys
*  TouchScreen
*  FrontLED
*  Flashlight
## Not Working
*  Wi-Fi
*  RIL
*  Audio
*  Most others...

## Force Software VSYNC Patch(Fix slow rendering)
	cd /yourbuilddir/
	patch -p1 < '/yourbuilddir/device/panasonic/p01d/vsync.diff'

Thanks! [@hiikezoe](https://github.com/hiikezoe)
