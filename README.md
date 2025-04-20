# ofxWindowApp

## Screencast

![screenshot](readme_images/ofxWindowApp.gif?raw=true "MoebiusSurfing")


## Screenshot

![screenshot](readme_images/screenshot2.JPG?raw=true "MoebiusSurfing")


## OVERVIEW

**openFrameworks** addon to **auto store** and **recall** the basic app **window state/settings**:


> [!CAUTION] 
> I have noticed many problems when reloading the window size and position settings when applying settings to the window application. This seems to be due to errors in `GLFW` or `OF` related to the size of the window bar. These problems also occur when using multiple monitors with different resolutions and repositioning. Usually the main monitor works fine, and other monitors may differ between sessions...


## FEATURES

* Window **Position** (x, y). 
* Window **Size** (w, h). 
* Window **Mode** (window/full screen).
* App target **FPS** settings (vs real current **framerate**), and **vSync** state. 
* "**Alert**" with a **performance** red bar when **low FPS drops** too much under the expected target frame rate.
* Easy change the app settings just by editing the **JSON file**, instead of the app code.
* Optional custom TTF font for debug display.
* `Stay on top` mode, easy console window disabler. (**WIN32** only)


## KEY COMMANDS

```
ALT +  

(Uppercased, caps enabled or using SHIFT)  
W: Show debug info.  
D: Show debug key commands.  
F: Full screen / Windowed.  
V: vSync state (On/Off).  
T: Stay-on-top (Window only). 

BACKSAPCE: Reset.  
```

## WHY?

It can be useful to **auto-maintain** window settings between your **app sessions**.  

_Not only for the final user but also for the coder. Usually when you start a clean ```OF project``` you want "to focus on your code", but you need to compile many times and move the app window out of your IDE window several times..._  

_Using this add-on you can forget about this "annoying behaviour", as the **App Window will be opened in the same position** and with the **same size and settings**._  

### POWERED BY

**ofxSerializer** from **@bakercp** (Thanks!)  
https://github.com/bakercp/ofxSerializer  
Now it's included into ```\ofxWindowApp\libs\ofxSerializer``` to simplify.  
Included into `OFX_ADDON/libs`.  
You don't need to add `ofxSerializer` with the **PROJECT GENERATOR**!

## USAGE

#### ofApp.h
```.c++
    #include "ofxWindowApp.h"
    ofxWindowApp w;
```

#### ofApp.cpp
Nothing more is required on ```update()``` or ```draw()```!  
Some settings can be configured using key commands.  
```.cpp 
ofApp::setup()
{
    w.setup(&w);

    // First time opening app:
    // Default FPS is 60 fps
    // Default vSync is disabled

    // Optional customized init:
    w.setFrameRate(120);
    w.setVerticalSync(true);
    // Can be forced to overwrite JSON settings too.
}
```

The add-on will **auto-load** the settings when your **app starts** and **auto stores** too on **App exit**.  

**Nothing more it's required!**  

The **JSON file** (`bin/data/ofxWindowApp/ofxWindowApp.json`) will look like this:  
```.json
[
    {
        "position": {
            "x": -1569.0,
            "y": 1415.0
        },
        "size": {
            "height": 375,
            "width": 1709
        },
        "window_mode": "OF_WINDOW"
    },
    {
        "Extra": {
            "Session": {
                "DebugPerformance": "1",
                "DisableAutosave": "0",
                "ShowWindowInfo": "1",
                "WindowOnTop": "0"
            },
            "Window": {
                "Fps": "60",
                "vSync": "1"
            }
        }
    }
]
```

### TODO

* Merge with another addon to handle more window settings borderless, put in-front, half screen, send to 2nd monitor... etc

## TESTED SYSTEMS
- **Windows 10/11** / **VS 2022** / **OF ~0.11** /  **OF 0.12+**
- **macOS High Sierra** / **Xcode 9/10** / **OF ~0.11** /  **OF 0.12+**

## AUTHOR
Addon by **@moebiusSurfing**  
*( ManuMolina ). 2020-2025.*

## LICENSE
*MIT License.*