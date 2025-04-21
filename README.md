# ofxWindowApp

> [!CAUTION] 
> WIP. Currently partially broken!

## Screencast

![screenshot](readme_images/ofxWindowApp.gif?raw=true "MoebiusSurfing")

## Screenshot

![screenshot](readme_images/screenshot2.JPG?raw=true "MoebiusSurfing")

## OVERVIEW

**openFrameworks** addon to **auto store** and **recall** the basic app **window state/settings**:

> [!CAUTION] 
> I have noticed many problems when reloading the window size and position settings to the window application. This problem often occurs when using multiple monitors and resolutions. Usually using the application on the main monitor works fine, but on other monitors the window settings may differ from session to session. This seems to be due to problems in `GLFW` or `OF`, maybe related to the size of the window bar (?)

## FEATURES

* Window **Position** (x, y). 
* Window **Size** (w, h). 
* Window **Mode** (window/full screen).
* Window target **FPS** settings (vs real current **framerate**).
* Window **vSync** state. 
* "**Alert**" with a **performance** red bar when **low FPS drops** too much under the expected target frame rate.
* Easy change the app settings just by editing the **JSON file**, instead of the app code.
* Optional `custom TTF font` for debug display.
* `Stay on top` mode, easy console window disabler. (**WIN32** only)
* Window shape presets with `key commands` for squared and common `Instagram` sizes.

## KEY COMMANDS

```
ALT +  

D: Show debug & monitors.  
W: Show info & performance.  
F: Full screen / Windowed.  
V: vSync state (On/Off).  
T: Stay-on-top (Windows only). 
L: Lock / Disable autosave.
(All uppercased, caps enabled or using SHIFT too)  

PRESETS
q : Squared 800 x 800
Q : Squared W x W
1 : IGTV Cover Photo
2 : IG Landscape Photo
3 : IG Portrait
4 : IG Story
5 : IG Square
BACKSPACE : Reset default
```

## WHY?

It can be useful to **auto-maintain** window settings between your **app sessions**.  

_Not only for the final user but also for the coder. Usually when you start a clean ```OF project``` you want "to focus on your code", but you need to compile many times and move the app window out of your IDE window several times..._  

_Using this add-on you can forget about this "annoying behaviour", as the **App Window will be opened in the same position** and with the **same size and settings**._  

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

    //----

    // NOTE:
    // First time opening app:
    // Default FPS is 60 fps
    // Default vSync is disabled

    // OPTIONAL: 
    // Custom init
    // Forced to overwrite JSON default settings.
    // Will be aplied the next time the app starts.
    w.setFrameRate(120);
    w.setVerticalSync(true);
}
```

The add-on will **auto-load** the settings when your **app starts** and **auto stores** too on **App exit**.  

**Nothing more it's required!**  

The **JSON file** (`bin/data/ofxWindowApp/ofxWindowApp.json`) will look like this:  
```.json
[
    {
        "position": {
            "x": -2469.0,
            "y": 1150.0
        },
        "size": {
            "height": 800,
            "width": 1539
        },
        "window_mode": 0
    },
    {
        "Extra": {
            "Session": {
                "DisableAutosave": "0",
                "ShowDebugInfo": "1",
                "ShowDebugPerformance": "1"
            },
            "Window": {
                "FpsTarget": "60",
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

### AUTHOR
Addon by **@moebiusSurfing**  
*( ManuMolina ). 2020-2025.*

### LICENSE
*MIT License.*