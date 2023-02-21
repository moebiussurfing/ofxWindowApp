# ofxWindowApp

## Screencast

![screenshot](readme_images/ofxWindowApp.gif?raw=true "MoebiusSurfing")


## Screenshot

![screenshot](readme_images/screenshot2.JPG?raw=true "MoebiusSurfing")



## OVERVIEW

**openFrameworks** add-on to **auto store** and **recall** the basic app **window state/settings**:

* Window **Position** (x, y). 
* Window **Size** (w, h). 
* Window **Mode** (window/full screen).
* App target **FPS** settings, real current **framerate**, and **vSync** state. 
* "Trigs **Alert**" and shows a **performance** bar when **low FPS drops** under the target (expected) frame rate.
* Easy change the app settings just editing the **JSON file** not the app code.
* ~~**Dual settings**: **Big** and **Mini** settings to **switch** between both presets. (WIP)~~  
    * Currently mini mode is disabled. 
* **Lock mode** to avoid future changes after modifiying window. (WIP)
* Reset command to restore **1080p Full HD** settings.
* Custom TTF font.

## SHORTCUTS

```
W: Show debug info  
F: Fullscreen  
V: vSync state (on/off)  
L: Lock mode  
R: Set Full HD size on the main monitor  
M: Switch Big/Mini modes (WIP)  
```

## WHY?

It can be useful to **auto-maintain** window settings between your **app sessions**.  

(_Usually when you start a clean ```OF project``` you want "to focus on your code", but you need to compile many times and to move the app window out your IDE window several times..._
_Using this addon you can forget about this "annoying behavior", as the **app window will be open in the same place** and with the **same size and settings**._)  

## DEPENDENCIES

**ofxSerialize** from **@bakercp** (Thanks!)  
https://github.com/bakercp/ofxSerializer  
Now it's included into ```\ofxWindowApp\libs\ofxSerializer``` to simplify.  
You don't need to add with **Project Generator**.

## USAGE

## ofApp.h
```.c++
    #include "ofxWindowApp.h"
    ofxWindowApp windowApp;
```

## ofApp.cpp
Nothing more is required on ```update()``` or ```draw()```!  
Some settings can be configured using key commands.  
```.c++ 
ofApp::setup(){
    // default FPS is 60 fps, vSync = off
    //windowApp.setFrameRate(30);
    //windowApp.setVerticalSync(false);
}
```

The add-on will **auto-load** the settings when your **app starts** and **auto-stores** too on **app exit**.  

**Nothing more it's required!**  

The **JSON file** (bin/data/ofxWindowApp/ofxWindowApp.json) will looks like:  
```.json
[
    {
        "Preset": "Big",
        "position": {
            "x": 2.0,
            "y": 25.0
        },
        "size": {
            "height": 1053,
            "width": 1916
        },
        "window_mode": "OF_WINDOW"
    },
    {
        "extra_settings": {
            "debugPerformance": "1",
            "fps": "60",
            "lockMode": "0",
            "miniPreset": "0",
            "showInfo": "0",
            "vsync": "0"
        }
    }
]
```

### TODO

* Join with another add-on to handle more window settings borderless, put in-front, half screen, send to 2nd monitor... etc

## TESTED SYSTEMS
- **Windows10** / **VS2017** / **OF ~0.11**
- **macOS High Sierra** / **Xcode 9/10** / **OF ~0.11**

## AUTHOR
Add-on by **@moebiusSurfing**  
*(ManuMolina). 2020.*

## LICENSE
*MIT License.*