#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	w.setup(&w);

	// custom
	//w.setFrameRate(120);

	// force debug
	w.bShowDebug = true;
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
