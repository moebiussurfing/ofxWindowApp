#include "ofxWindowApp.h"

//--------------------------------------------------------------
ofxWindowApp::ofxWindowApp()
{
	ofSetLogLevel("ofxWindowApp", OF_LOG_NOTICE);

	if (autoSaveLoad)
		loadWindow();

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
}

//--------------------------------------------------------------
void ofxWindowApp::setup()
{
	//ofAddListener(ofEvent<whateverType>,//This is the event you are going to listen to. When a new event is notified it must pass a reference to an object of type whateverType, which can also be void. This object can be understood as a message to whomever is listening.
	//	pointerToSomeClassInstance,// this is a pointer to the object that will receive the event message. If you place this function inside the class that will be listening you would use "this" as argument
	//	pointerToSomeClassMethod);//This method must be part of the class that passed in the previous argument. It's argument must be a reference of class whateverType.
	ofLogVerbose("ofxWindowApp") << "called setup";

	ofAddListener(ofEvents().update, this, &ofxWindowApp::update);
	ofAddListener(ofEvents().draw, this, &ofxWindowApp::draw);

	if (autoSaveLoad)
		loadWindow();

	//more window settings
	fps = 60;
	bVSync = true;
	//ofSetFrameRate(fps);
	//ofSetVerticalSync(bVSync);
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

//--------------------------------------------------------------
void ofxWindowApp::drawDEBUG()
{
	//----

	//debug overlay screen modes
	string str = "";
	string screenStr = "";
	string screenPosStr = "";
	string screenMode = "";
	string strPad = "    ";

	screenStr = ofToString(window_W) + "x" + ofToString(window_H);
	vSyncStr = ofToString((bVSync ? "ON" : "OFF"));
	fpsStr = ofToString(ofGetFrameRate(), 1);
	screenPosStr = "(" + ofToString(ofGetWindowPositionX()) + ", " + ofToString(ofGetWindowPositionY()) + ")";

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

	str += "FPS " + fpsStr;
	str += strPad + "VSYNC " + vSyncStr;
	str += strPad + "SIZE " + screenStr;
	str += strPad + "POSITION " + screenPosStr;
	str += strPad + screenMode;

	{
		ofDrawBitmapStringHighlight(str, 0, window_H - 5);
	}
}