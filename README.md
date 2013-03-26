# B2G for Panasonic P-01D

## Build

    $ git clone git://github.com/mozilla-b2g/B2G.git
    $ cd B2G
    $ wget https://raw.github.com/hiikezoe/android_device_panasonic_p01d/b2g/p01d.xml
    $ ./config.sh p01d p01d.xml

extract Qualcomm Adreno driver[1] into vendor/qcom/adreno200/proprietary/.

    $ pushd device/panasonic/p01d
    $ ./extract-files.sh && ./setup-makefiles.sh
    $ popd
    $ ./build.sh


[1] https://developer.qualcomm.com/download/adreno200-aulinuxandroidicschococs04000306001.zip
