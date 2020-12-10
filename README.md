# ofxWindowApp

## Screencast

![screenshot](readme_images/ofxWindowApp.gif?raw=true "MoebiusSurfing")


## Screenshot

![screenshot](readme_images/screenshot2.JPG?raw=true "MoebiusSurfing")



## Overview

This **openFrameworks** addon **auto stores** and **recalls** the basic app **window state/settings**:

* Window **Position** (x, y). 
* Window **Size** (w, h). 
* Window **Mode** (window/full screen).
* App target **FPS** settings, real current **framerate**, and **vSync** state. 
* "Trigs **Alert**" and shows a **performance** bar when **low FPS drops** under the target (expected) frame rate.
* Easy change the app settings just editing the **JSON file** not the app code.
* **Dual settings**: **Big** and **Mini** settings to **switch** between both presets. (WIP)
* **Lock mode** to avoid future changes after modifiying window. (WIP)
* Command to restore **1080p Full HD** settings.

When you use the addon for the first time into a project and no JSON file is present, you need to set the initial settings:  
```.c++
windowApp.setFrameRate(30);
windowApp.setVerticalSync(false);
```

## Shortcuts

```
W: Show debug info  
F: Fullscreen  
V: vSync state  
L: Lock mode  
R: Set Full HD size on the main monitor  
M: Switch Big/Mini modes (WIP)  
```

## Why?

It can be useful to **auto-maintain** window settings between your **app sessions**.  

Usually when you start a clean ```OF project``` you want "to focus on your code", but you need to compile many times and to move the app window out your IDE window several times...
Using this addon you can forget about this "annoying behavior", as the **app window will be open in the same place** and with the **same size and settings**.  

## Dependencies

**ofxSerialize** from @bakercp (Thanks!)  
https://github.com/bakercp/ofxSerializer  
Now it's included into ```\ofxWindowApp\libs\ofxSerializer``` to simplify.  
You don't need to add with Project Generator.

## Usage

## ofApp.h
```.c++
    #include "ofxWindowApp.h"

    ofxWindowApp windowApp;
```

## ofApp.cpp
Nothing more is required on ```update()``` or ```draw()```!

```.c++ 

//setup()

// only required if no previous session JSON file settings present
windowApp.setFrameRate(30);
windowApp.setVerticalSync(false);

// Nothing more!
```

The addon **auto-loads** the settings when your **app starts** and **auto-stores** too on **app exit**.  

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
        "Preset": "Mini",
        "position": {
            "x": 20.0,
            "y": 20.0
        },
        "size": {
            "height": 200,
            "width": 200
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

## Tested systems
- **Windows10** / **VS2017** / **OF ~0.11**
- **macOS High Sierra** / **Xcode 9/10** / **OF ~0.11**

## Author
Addon by **@moebiusSurfing**  
*(ManuMolina). 2020.*

## License
*MIT License.*