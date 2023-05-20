# ofxWindowApp

## Screencast

![screenshot](readme_images/ofxWindowApp.gif?raw=true "MoebiusSurfing")


## Screenshot

![screenshot](readme_images/screenshot2.JPG?raw=true "MoebiusSurfing")



## OVERVIEW

**openFrameworks** addon to **auto store** and **recall** the basic app **window state/settings**:


## FEATURES

* Window **Position** (x, y). 
* Window **Size** (w, h). 
* Window **Mode** (window/full screen).
* App target **FPS** settings, real current **framerate**, and **vSync** state. 
* "**Alert**" with a **performance** red bar when **low FPS drops** under the expected target frame rate.
* Easy change the app settings just by editing the **JSON** file**, not the app code.
* **Lock mode** to avoid future changes after modifying the window. (WIP)
* Reset the command to restore **1080p Full HD** settings.
* Custom TTF font.
* On top mode, easy console window disabler. (WIN 32 only)

~~**Dual settings**: **Big** and **Mini** settings to **switch** between both presets. (WIP)~~  
  Currently, Mini mode is disabled. 


## KEY COMMANDS

```
ALT  
( caps enabled or SHIFT )  

+  

W: Show debug info.  
F: Full screen / Windowed.  
V: vSync state (On/Off).  
L: Lock mode.  
T: Always on top (Window only). 
R: Reset Full HD size on the main monitor.  
BACKSAPCE: Reset.  

```

## WHY?

It can be useful to **auto-maintain** window settings between your **app sessions**.  

_Not only for the final user but also for the coder. Usually when you start a clean ```OF project``` you want "to focus on your code", but you need to compile many times and move the app window out of your IDE window several times..._  

_Using this add-on you can forget about this "annoying behavior", as the **App Window will be open in the same place** and with the **same size and settings**._  

## POWERED BY

**ofxSerialize** from **@bakercp** (Thanks!)  
https://github.com/bakercp/ofxSerializer  
Now it's included into ```\ofxWindowApp\libs\ofxSerializer``` to simplify.  
You don't need to add with **PROJECT GENERATOR**!

## USAGE

#### ofApp.h
```.c++
    #include "ofxWindowApp.h"
    ofxWindowApp w;
```

#### ofApp.cpp
Nothing more is required on ```update()``` or ```draw()```!  
Some settings can be configured using key commands.  
```.c++ 
// Optional
ofApp::setup()
{
    // Default FPS is 60 fps, vSync = off
    //w.setFrameRate(30);
    //w.setVerticalSync(false);
}
```

The add-on will **auto-load** the settings when your **app starts** and **auto stores** too on **App exit**.  

**Nothing more it's required!**  

The **JSON file** (bin/data/ofxWindowApp/ofxWindowApp.json) will look like this:  
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

* Join with another addon to handle more window settings borderless, put in-front, half screen, send to 2nd monitor... etc

## TESTED SYSTEMS
- **Windows10** / **VS2022** / **OF ~0.11**
- **macOS High Sierra** / **Xcode 9/10** / **OF ~0.11**

## AUTHOR
Addon by **@moebiusSurfing**  
*( ManuMolina ). 2020-2023.*

## LICENSE
*MIT License.*