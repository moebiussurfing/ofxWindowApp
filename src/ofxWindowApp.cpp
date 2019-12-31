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
void ofxWindowApp::setup()
{
    if (autoSaveLoad)
        loadWindow();
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

    ofLogNotice("ofxWindowApp") << "saveWindow: " << path_folder + path_filename;
}

//--------------------------------------------------------------
void ofxWindowApp::loadWindow()
{
    //load window settings
    ofJson j = ofLoadJson(path_folder + path_filename);
    ofx::Serializer::ApplyWindowSettings(j);

    ofLogNotice("ofxWindowApp") << "loadWindow: " << path_folder + path_filename;
}


