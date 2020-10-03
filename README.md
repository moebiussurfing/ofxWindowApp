# ofxWindowApp

## Screencast

![screenshot](readme_images/ofxWindowApp.gif?raw=true "MoebiusSurfing")


## Screenshot

![screenshot](readme_images/screenshot2.JPG?raw=true "MoebiusSurfing")



## Overview

This **openFrameworks** addon **auto stores** and recalls the basic app **window state/settings** and **debugs**:

* Window **position** (x, y) 
* Window **size** (w, h) 
* Window **mode** (window/full screen)
* App target **FPS** settings, real current **framerate**, and **vSync** state
* "Trigs **Alert**" and shows a **performance** bar when **low FPS drops** under the target (expected) frame rate.
* Easy change the app settings just editing the **JSON file** not the app code.
* **Dual** settings: **big** and **mini** settings to **switch** between both modes. (WIP)
* Lock mode to avoid changes. (WIP)
* Command to restore **standard Full HD** settings.

When you use the addon for the first time into a project and no JSON file is present, you need to set the initial settings:  
```.cpp
windowApp.setSettingsFps(30);
windowApp.setSettingsVsync(false);
```

## Shortcuts

```
W: Show debug info  
F: Fullscreen  
V: vSync state  
L: Lock mode  
R: Set Full HD size on main monitor  
```

## Why?

It can be useful to **auto-maintain** window settings between your **app sessions**.  

Usually when you start a clean OF project you want "to focus on your code", but you need to compile many times and to move the app window out your IDE window several times...
Using this addon you can forget about this "annoying behavior", as the **app window will be open in the same place** and with the **same size and settings**.  

## Dependencies

**ofxSerialize** from @bakercp (Thanks!)  
https://github.com/bakercp/ofxSerializer  
Now it's included into \ofxWindowApp\libs\ofxSerializer to simplify.  
You don't need to add with Project Generator.

## Usage

## ofApp.h
```.cpp
    #include "ofxWindowApp.h"

    ofxWindowApp WindowApp;
```

## ofApp.cpp
Nothing more is required on update() or draw().

```.cpp 

//setup()

//only required if no previous session json file settings present
WindowApp.setSettingsFps(30);
WindowApp.setSettingsVsync(false);

//optional customization:  
//WindowApp.setPathFolder("settings/");//default folder is "data/ofxWindowApp/"
//WindowApp.setPathFilename("AppWindow.json");
//WindowApp.setAutoSaveLoad(false);//to disable auto
//WindowApp.setShowPerformanceAllways(true)//show performance alert also when debug is hidden
       
//available methods:         
//WindowApp.saveWindow();//save  
//WindowApp.loadWindow();//load  
//WindowApp.setShowDebug(true);//show/hide
```

The addon **auto loads** the settings when your **app starts** and **stores** too on **app exit**.  
Nothing more it's required!  
The **JSON file** (bin/data/ofxWindowApp/ofxWindowApp.json) will looks like:  
```.json
[
    {
        "position": {
            "x": 150.0,
            "y": 142.0
        },
        "size": {
            "height": 713,
            "width": 1399
        },
        "window_mode": "OF_WINDOW"
    },
    {
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
            "lock": "1",
            "miniMode": "0",
            "showInfo": "1",
            "vsync": "1"
        }
    }
]
```

## Tested on systems

- Windows 10 / VS2017 / OF 0.11.0
- macOS / High Sierra / Xcode / OF 0.11.0

### TODO

* Mix with another addon to handle more window settings borderless, put in-front, half screen, send to 2nd monitor... etc

## Author
Addon by **@moebiusSurfing**  
*(ManuMolina). 2020.*

## License
*MIT License.*