#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	// required when addon started for first time on your project
	// or when no json file settings is present into /bin/data/ofxWindowApp/ofxWindowApp.json
	// or if you want hardcode the settings
	windowApp.setFrameRate(60);
	//windowApp.setVerticalSync(true);
	//WindowApp.setPositionDebugInfo(ofxWindowApp::DEBUG_POSITION_TOP);
}

//--------------------------------------------------------------
void ofApp::draw() {
}