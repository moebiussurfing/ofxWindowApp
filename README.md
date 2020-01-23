# ofxWindowApp;

## Overview

The addon stores and recall the basic app window states:

Window position (x, y), window size (w, h) and window mode (window/full screen).

The addon auto loads when app starts and stores on app exit. Nothing more it's required.

The json file will looks like:

AppWindow.json
```
{
    "position": {
        "x": -1920.0,
        "y": 23.0
    },
    "size": {
        "height": 1057,
        "width": 1920
    },
    "window_mode": "OF_WINDOW"
}
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

- Windows 10 + VS2017 + of0.10.1.
- macOS HighSierra + of0.10.1.

### TODO

+ mix with another addon to handle borderless, put in-front, half screen... etc


## Author

MoebiusSurfing

github: https://github.com/moebiussurfing

link: http://https://www.instagram.com/moebiussurfing/
