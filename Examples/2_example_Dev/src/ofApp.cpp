#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	w.setup(&w);

	// customize
	//w.setFrameRate(120);

	// force debug
	w.bShowDebug = true;
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// debug feedback when changes
	if (w.isChanged()) ofClear(ofColor::yellow);
	else ofClear(64);
}

//--------------------------------------------------------------
void ofApp::exit()
{
}
