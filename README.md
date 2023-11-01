# CyanogenMod 11 for P-01D
## Working
*  Boot
*  Display
*  Hardware Keys
*  TouchScreen
*  FrontLED
*  Flashlight
*  Wi-Fi
*  IMEI
## Working(incomplete)
*  GApps
## Not Working
*  Audio
*  Bluetooth
*  AOSP Webview
*  Camera
*  Felica
*  OneSeg DTV
*  SELinux
## Unchecked
*  3G
*  Most others...

## Force Software VSYNC Patch(Fix slow rendering)
	cd /yourbuilddir/
	patch -p1 < '/yourbuilddir/device/panasonic/p01d/vsync.diff'
	
## WLAN Build Fix Patch
	cd /yourbuilddir/
	patch -p1 < '/yourbuilddir/device/panasonic/p01d/wlan.diff'

Thanks! [@hiikezoe](https://github.com/hiikezoe)
