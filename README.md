# ofxWindowApp

## Screenshot
![screenshot](Capture.png)

## OVERVIEW
**openFrameworks** addon to **auto store** and **recall** the basic **App Window state/settings**:

## FEATURES

* Window **Position** (x, y). 
* Window **Size** (w, h). 
* Window **Mode** (Windowed / Fullscreen).
* Window **Target FPS** (vs real current **FPS**).
* Window **vSync** state. 
* Easy change the app settings just by editing the **JSON file** (and restart app), instead of requiring edit the app code.
* **Stay on Top** mode, easy **Console Window** disabler. (**Windows** only)
* Window shape **Presets** with **Key commands** for squared, centered, default and common **Instagram** sizes.
* **Alert display**" HUD with a **performance** red bar when **FPS drops** too low under the expected **Target Framerate**.
* Optional **custom TTF font** for debug display.

## KEY COMMANDS

```
d : Show debug & monitors.  
i : Show info & performance.  
f : Full screen / Windowed.  
v : vSync state (On/Off).  
t : Stay-on-top (Windows only). 
l : Lock / Disable autosave.

PRESETS
c : Center Window.  
q : Squared 800 x 800
w : Squared w x w
1 : IGTV Cover Photo
2 : IG Landscape Photo
3 : IG Portrait
4 : IG Story
5 : IG Square
BACKSPACE : Reset Default
```

## WHY?

_Not only for the `final user` but also for the `coder`. Usually when you work on an `OF project` you want "to focus on your code updates", but you need to compile many times and move manually the app window out of your IDE window several times..._  
_Using this **addon** you can forget about this "annoying behaviour", as the **App Window will be opened in the same position, size and some settings too**._  

## USAGE

#### ofApp.h
```.c++
    #include "ofxWindowApp.h"
    ofxWindowApp w;
```

#### ofApp.cpp

```.cpp 
ofApp::setup()
{
    w.setup(&w);

    //----

    // NOTE:
    // First time opening an app:
    // Default FPS is 60 fps.
    // Default vSync is disabled.

    // OPTIONAL: 
    // Custom init:
    w.setFrameRate(120);
    w.setVerticalSync(true);
}
```
Nothing more is required on `update()`,`draw()` or `exit()`!  
All settings can be configured also using `Key Commands`.  
The addon will **auto load** the settings when your **App starts** and **auto stores** too on **any changes**.  

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
                "ShowInfo": "1",
                "ShowInfoPerformanceAlways": "1"
            },
            "Window": {
                "FpsTarget": "60",
                "vSync": "1"
            }
        }
    }
]
```

## TESTED SYSTEMS
- **Windows 11** / **VS 2022** / **OF 0.12.1**
- **macOS Sequoia 15.4 (Silicon ARM)** / **Xcode 16.2** / **OF 0.12.1**

### AUTHOR
Addon by **@moebiusSurfing**  
*( ManuMolina ). 2020-2025.*

### LICENSE
*MIT License.*