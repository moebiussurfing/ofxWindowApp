# ofxWindowApp

## Screencast

![screenshot](/ofxWindowApp.gif?raw=true "MoebiusSurfing")


## Screenshots

![screenshot](/screenshot2.JPG?raw=true "MoebiusSurfing")



## Overview

This openFrameworks addon debugs, auto stores and recalls the basic app window state/settings:

* window position (x, y) 
* window size (w, h) 
* window mode (window/full screen)
* app target fps settings, real current framerate, and vsync state
* "trigs alert" and shows a performance bar when fps drops under the expected frame rate


This openFrameworks addon debugs, stores and recalls the basic app window state/settings.


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

The JSON file will look like:

AppWindow.json
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

Usually when you start a clean OF project you want "to focus on your code", but you need to compile many times and to move the app window out your IDE window several times...
Using this addon you can forget about this annoying behavior, as the app window will be open in the same place and with the same size and settings.  (Apart from this, it can be useful to maintain window settings between your app sessions.)



## Dependencies

ofxSerialize from @bakercp (Thanks!)  
https://github.com/bakercp/ofxSerializer



## Usage

### ofApp.h
```cpp
    #include "ofxWindowApp.h"

    ofxWindowApp WindowApp;
```

### ofApp.cpp
Nothing more is required.

```cpp 

setup()
    //required if no previous session json file settings 
    //windowApp.setSettingsFps(30);
    //windowApp.setSettingsVsync(false);
    //show or hide
    //windowApp.setShowDebug(true);

    //customize: 
    //default folder is "ofxWindowApp/". 
    //(Check to create the folder if it's not created or required)
    WindowApp.setPathFolder("settings/");
    WindowApp.setPathFilename("AppWindow.json");
    WindowApp.setAutoSaveLoad(false);//to disable auto

update()
    //nothing to do.

exit()
    //nothing to do.
       
everywhere()       
    WindowApp.saveWindow();//save manually
    WindowApp.loadWindow();//load manually
```



## Tested on systems

- Windows 10 / VS2017 / OF 0.11.0
- macOS / High Sierra / Xcode / OF 0.11.0



### TODO

+ mix with another addon to handle more window settings borderless, put in-front, half screen... etc



## Author

MoebiusSurfing  
GitHub: https://github.com/moebiussurfing  
link: http://https://www.instagram.com/moebiussurfing/
