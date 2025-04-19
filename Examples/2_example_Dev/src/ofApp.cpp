#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	w.setup(&w);

	// custom
	//w.setEnableKeys(false);

	// force debug
	w.bShowDebugKeysInfo = true;
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// debug feedback
	if (w.isChanged()) ofClear(ofColor::yellow);
	else ofClear(64);
}

//--------------------------------------------------------------
void ofApp::exit()
{
}
