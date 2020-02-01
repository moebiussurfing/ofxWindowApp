#include "ofxWindowApp.h"

//--------------------------------------------------------------
ofxWindowApp::ofxWindowApp()
{
    ofSetLogLevel("ofxWindowApp", OF_LOG_NOTICE);
    //ofSetLogLevel("ofxWindowApp", OF_LOG_VERBOSE);

//    if (autoSaveLoad)
//        loadWindow();

    //auto call setup
    setup();
}

//--------------------------------------------------------------
ofxWindowApp::~ofxWindowApp()
{
    if (autoSaveLoad)
        saveWindow();
}

//--------------------------------------------------------------
void ofxWindowApp::update(ofEventArgs & args)
{
    ofLogVerbose("ofxWindowApp") << "called update";
}

//--------------------------------------------------------------
void ofxWindowApp::draw(ofEventArgs & args)
{
    ofLogVerbose("ofxWindowApp") << "called draw";

    if (ENABLE_Debug)
        drawDEBUG();
}

//--------------------------------------------------------------
void ofxWindowApp::setup()
{
    ofLogVerbose("ofxWindowApp") << "called setup";

    ofAddListener(ofEvents().update, this, &ofxWindowApp::update);
    ofAddListener(ofEvents().draw, this, &ofxWindowApp::draw);
    ofAddListener(ofEvents().keyPressed, this, &ofxWindowApp::keyPressed);


    //extra settings
    params_Settings.add(vSync);
    params_Settings.add(fps);
    params_Settings.add(ENABLE_Debug);

    //load
    if (autoSaveLoad)
        loadWindow();

    windowResized(ofGetWindowSize().x, ofGetWindowSize().y);
}

//--------------------------------------------------------------
void ofxWindowApp::saveWindow()
{
    ofLogNotice("ofxWindowApp") << "saveWindow: " << path_folder + path_filename;

    //save window settings
    ofWindowSettings AppWindow;
    AppWindow.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
    AppWindow.setSize(ofGetWindowSize().x, ofGetWindowSize().y);
    AppWindow.windowMode = ofGetCurrentWindow()->getWindowMode();

    ofJson j;
    to_json(j, AppWindow);
    ofJson j2;
    ofSerialize(j2, params_Settings);

    //A. using 2 files
    //ofSavePrettyJson(path_folder + path_filename, j);
    //TODO:
    //we can't get framerate and vsync mode from window app.
    //should be setted by hand
    //extra settings could be mixed in one json only for both
    //TEST:
    //ofSavePrettyJson(path_folder + path_filename2, j2);

    //B. settings in one file
    ofJson data;
    data.push_back(j);
    data.push_back(j2);

    std::cout << data.dump(4) << std::endl;
    ofSavePrettyJson(path_folder + path_filename, data);
}

//--------------------------------------------------------------
void ofxWindowApp::loadWindow()
{
    //load window settings

    //A. using 2 files
    //ofJson j = ofLoadJson(path_folder + path_filename);
    //ofx::Serializer::ApplyWindowSettings(j);
    ////extra settings could be mixed in one json only for both
    ////TEST:
    //ofJson j2;
    //j2 = ofLoadJson(path_folder + path_filename2);
    //ofLogNotice("ofxWindowApp") << "json: " << j2;
    //ofDeserialize(j2, params_Settings);

    //B. settings in one file
    ofJson data;
    data = ofLoadJson(path_folder + path_filename);
    ofLogNotice("ofxWindowApp") << "all json: " << data;
    ofJson j = data[0];
    ofJson j2 = data[1];
    //recall both params groups
    ofDeserialize(j2, params_Settings);
    ofx::Serializer::ApplyWindowSettings(j);

    //-

    applySettings();

    ofLogNotice("ofxWindowApp") << "loadWindow: " << path_folder + path_filename;
}

//--------------------------------------------------------------
void ofxWindowApp::drawDEBUG()
{
    string vSyncStr;
    string fpsRealStr;
    string fpsTargetStr;

    //----

    //debug overlay screen modes
    string str = "";
    string screenStr = "";
    string screenPosStr = "";
    string screenMode = "";
    string strPad = "    ";//add spaces

    screenStr = ofToString(window_W) + "x" + ofToString(window_H);
    vSyncStr = ofToString((vSync ? "ON" : "OFF"));
    fpsRealStr = ofToString(ofGetFrameRate(), 1);
    fpsTargetStr = ofToString(fps);
    screenPosStr = " [" + ofToString(ofGetWindowPositionX()) + ", " + ofToString(ofGetWindowPositionY()) + "]";

    bool bMode;
    if (ofGetWindowMode() == OF_WINDOW)//go full screen
    {
        bMode = false;
    }
    else if (ofGetWindowMode() == OF_FULLSCREEN)//go window mode
    {
        bMode = true;
    }
    screenMode += bMode ? "FULL SCREEN MODE" : "WINDOW MODE";

    str += "FPS " + fpsRealStr;
    str += " [" + fpsTargetStr +"]";
    str += strPad + "VSYNC-" + vSyncStr;
    str += strPad + "SIZE:" + screenStr;
    str += strPad + "POSITION" + screenPosStr;
    str += strPad + screenMode;

    //WORKAROUND until windowResize implemented
    window_X = ofGetWindowPositionX();
    window_Y = ofGetWindowPositionY();
    window_W = ofGetWindowSize().x;
    window_H = ofGetWindowSize().y;

    ofDrawBitmapStringHighlight(str, 0, window_H - 2);
}


//--------------------------------------------------------------
void ofxWindowApp::windowResized(int w, int h)
{
    window_X = ofGetWindowPositionX();
    window_Y = ofGetWindowPositionY();

    //window_W = ofGetWindowSize().x;
    //window_H= ofGetWindowSize().y;

    window_W = w;
    window_H = h;
}

//--------------------------------------------------------------
void ofxWindowApp::keyPressed(ofKeyEventArgs &eventArgs)
{
    const int &key = eventArgs.key;

    //modifier
    bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);

    //disable draw debug
    if (mod_COMMAND && key == 'w')
    {
        ofLogNotice("ofxWindowApp") << "keyPressed: " << (char)key << " [" << key << "]";

        ENABLE_Debug = !ENABLE_Debug;
    }

}
