#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	w.setup(&w);

	// Customize
//	w.setFrameRate(240);
//	w.setFrameRate(0); // Max possible, to display refresh freq if VSYNC_ON, or unlocked if VSYNC_OFF.
	
	// Force show debug
	w.bShowDebug = true;
	
	// Force show info
	w.bShowInfo = true;

	// Log
	ofSetLogLevel("ofxWindowApp", OF_LOG_VERBOSE);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// Flash bg 200ms when window changed
	static bool bFlash=0;
	if(w.isChanged()) {
		bFlash=1;
		tLast=ofGetElapsedTimeMillis();}
	if(bFlash&&ofGetElapsedTimeMillis()-tLast>200){
		bFlash=0;
	}
	
	// debug feedback when changes
	if (bFlash) ofClear(ofColor::green);
	else ofClear(64, 255);
}

//--------------------------------------------------------------
void ofApp::exit()
{
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'k') w.setToggleEnableKeys();
}
