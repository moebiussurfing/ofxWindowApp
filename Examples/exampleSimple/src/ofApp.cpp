#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	// To modify default settings, (60fps, FHD, windowed, v-sync disabled...)
	// required when addon started for first time on your project
	// or when no json file settings is present into /bin/data/ofxWindowApp/ofxWindowApp.json
	// or if you want to hard-code the settings.
	//w.setFrameRate(60);
	//w.setVerticalSync(true);
	//w.setPositionDebugInfo(ofxWindowApp::DEBUG_POSITION_TOP);
}

//--------------------------------------------------------------
void ofApp::draw() 
{
	// Help info
	w.drawInfo();
}
