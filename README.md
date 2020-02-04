# ofxWindowApp;

## Overview

The addon stores and recall the basic app window states:

-Window position (x, y), window size (w, h) and window mode (window/full screen).
-the app fps and vsync state.

When you starts for first time and no json file present, you need to set the extra settings:

windowApp.setSettingsFps(25);
windowApp.setSettingsVsync(false);

To show printed the window info you must call:
windowApp.setShowDebug(true);
Also shortcut CMD+w toggles the show debug info.

The addon auto loads when app starts and stores on app exit. Nothing more it's required.

The json file will looks like:

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
            "debug": "0",
            "fps": "60",
            "vsync": "0"
        }
    }
]
```


## Dependencies

ofxSerialize from @bakercp 
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
//must be defined to allow that the addon handles this right. 
//    windowApp.setSettingsFps(25);
//    windowApp.setSettingsVsync(false);

//    windowApp.setShowDebug(true);

    //all is optional: 

    //default folder is "ofxWindowApp/". 
    //(Check to create the folder if it's not created or required)
    WindowApp.setPathFolder("settings/");
    WindowApp.setPathFilename("AppWindow.json");
    WindowApp.setAutoSaveLoad(false);//to disable auto
    //WindowApp.setup();

update()
    //nothing to do.

exit()
    //nothing to do.
       
everywhere()       
    WindowApp.saveWindow();//save manually
    WindowApp.loadWindow();//load manually
```


## Tested system

- Windows 10 / VS2017 / OF 0.11.0
- macOS / High Sierra / Xcode / OF 0.11.0

### TODO

+ mix with another addon to handle borderless, put in-front, half screen... etc


## Author

MoebiusSurfing

github: https://github.com/moebiussurfing

link: http://https://www.instagram.com/moebiussurfing/
