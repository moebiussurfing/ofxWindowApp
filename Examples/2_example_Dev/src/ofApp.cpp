#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	w.setup(&w);

	// customize
	w.setFrameRate(120);
	
	// force show debug
	w.bShowDebug = true;
	
	// force show info
//	w.bShowInfo = true;

	// log
	ofSetLogLevel("ofxWindowApp", OF_LOG_VERBOSE);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// debug feedback when changes
	if (w.isChanged()) ofClear(ofColor::green);
	else ofClear(64);
}

//--------------------------------------------------------------
void ofApp::exit()
{
}
