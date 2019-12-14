#include "ofxWindowApp.h"

//--------------------------------------------------------------
ofxWindowApp::ofxWindowApp()
{
    if (autoSaveLoad)
        loadWindow();
}

//--------------------------------------------------------------
ofxWindowApp::~ofxWindowApp()
{
    if (autoSaveLoad)
        saveWindow();
}

//--------------------------------------------------------------
void ofxWindowApp::saveWindow()
{
    //save window settings
    ofWindowSettings AppWindow;
    AppWindow.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
    AppWindow.setSize(ofGetWindowSize().x, ofGetWindowSize().y);
    AppWindow.windowMode = ofGetCurrentWindow()->getWindowMode();

    ofJson j;
    to_json(j, AppWindow);
    ofSavePrettyJson(path_folder + path_filename, j);
}

//--------------------------------------------------------------
void ofxWindowApp::loadWindow()
{
    //load window settings
    ofJson j = ofLoadJson(path_folder + path_filename);
    ofx::Serializer::ApplyWindowSettings(j);
}


