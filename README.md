# ofxWindowApp

## Screencast

![screenshot](/ofxWindowApp.gif?raw=true "MoebiusSurfing")


## Screenshot

![screenshot](/screenshot2.JPG?raw=true "MoebiusSurfing")



## Overview

This openFrameworks addon debugs, auto stores and recalls the basic app window state/settings:

* Window position (x, y) 
* Window size (w, h) 
* Window mode (window/full screen)
* App target fps settings, real current framerate, and vsync state
* "Trigs Alert" and shows a performance bar when fps drops under the expected frame rate
* Easy change the app settings just editing the JSON file.

When you use the addon for the first time into a project and no JSON file is present, you need to set the initial settings:  
```
windowApp.setSettingsFps(30);
windowApp.setSettingsVsync(false);
```

To show or hide the window info you must call:  
```
windowApp.setShowDebug(true);
```

The addon auto loads the settings when your app starts and stores too on app exit. Nothing more it's required.  
The JSON file (AppWindow.json) will look like:  
```
[
    {
        "position": {
            "x": 0.0,
            "y": 22.0
        },
        "size": {
            "height": 878,
            "width": 1436
        },
        "window_mode": "OF_WINDOW"
    },
    {
        "extra_settings": {
            "debug": "1",
            "fps": "30",
            "vsync": "0"
        }
    }
]
```



## Shortcuts

```
W: toggle show debug info.  
F: toggle fullscreen  
V: toggle vsync state
```



## Why?

It can be useful to auto-maintain window settings between your app sessions.  

Usually when you start a clean OF project you want "to focus on your code", but you need to compile many times and to move the app window out your IDE window several times...
Using this addon you can forget about this annoying behavior, as the app window will be open in the same place and with the same size and settings.  


## Dependencies

ofxSerialize from @bakercp (Thanks!)  
https://github.com/bakercp/ofxSerializer

(Check and create the /data/ofxWindowApp folder if it's required to avoid startup crash!)


## Usage

## ofApp.h
```cpp
    #include "ofxWindowApp.h"

    ofxWindowApp WindowApp;
```

## ofApp.cpp
Nothing more is required.

```cpp 

setup()

//required only if no previous session json file settings present
WindowApp.setSettingsFps(30);
WindowApp.setSettingsVsync(false);

//optional customize: 
//default folder is "data/ofxWindowApp/". 
//WindowApp.setPathFolder("settings/");
//WindowApp.setPathFilename("AppWindow.json");
//WindowApp.setAutoSaveLoad(false);//to disable auto
//WindowApp.setShowPerformanceAllways(true)//show performance alert also when debug is hidden

update()//nothing to do.
draw()//nothing to do.
exit()//nothing to do.
       
everywhere()       
WindowApp.saveWindow();//manually save
WindowApp.loadWindow();//manually load
WindowApp.setShowDebug(true);//manually show/hide
```



## Tested on systems

- Windows 10 / VS2017 / OF 0.11.0
- macOS / High Sierra / Xcode / OF 0.11.0



### TODO

* Mix with another addon to handle more window settings borderless, put in-front, half screen, send to 2nd monitor... etc



## Author

MoebiusSurfing, 2020.  
GitHub:  
https://github.com/moebiussurfing  
Link:  
https://www.instagram.com/moebiussurfing/
