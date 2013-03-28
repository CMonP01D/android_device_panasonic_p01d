# B2G for Panasonic P-01D

## Build

    $ git clone git://github.com/mozilla-b2g/B2G.git
    $ cd B2G
    $ hg clone http://hg.mozilla.org/l10n-central/ja gecko-l10n/ja
    $ wget https://raw.github.com/hiikezoe/android_device_panasonic_p01d/b2g/p01d.xml
    $ DEVICE=p01d ./config.sh p01d p01d.xml (*1)

extract Qualcomm Adreno driver[1] into vendor/qcom/adreno200/proprietary/.

    $ ./build.sh


[1] https://developer.qualcomm.com/download/adreno200-aulinuxandroidicschococs04000306001.zip

(*1) Need https://github.com/hiikezoe/B2G/commit/1a6103a459bc9ca6b4a26a927776b36fe0e188d0

## Install
    $ ./flash.sh (*1)

(*1) Need https://github.com/hiikezoe/B2G/commit/1dc08277170e7c1aadd2105c9143dc384d82f1a2
