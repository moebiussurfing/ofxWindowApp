#pragma once

#include "ofMain.h"
#include "ofxSerializer.h"

class ofxWindowApp
{

public:

    ofxWindowApp();
    ~ofxWindowApp();

    string path_folder = "ofxWindowApp/";//this is to folder all files to avoid mixing with other addons data
    string path_filename = "AppWindow.json";

    void setup();

    void saveWindow();
    void loadWindow();

    void setPathFolder(string s)
    {
        path_folder = s;
    }

    void setPathFilename(string s)
    {
        path_filename = s;
    }

    bool autoSaveLoad = true;
    void setAutoSaveLoad(bool b)
    {
        autoSaveLoad = b;
    }
};


