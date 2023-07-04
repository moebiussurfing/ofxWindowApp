#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
}

//--------------------------------------------------------------
void ofApp::draw()
{
	if (w.isChanged()) ofClear(ofColor::yellow);
	else ofClear(64);

	w.drawDebugInfo();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}

//--------------------------------------------------------------
void ofApp::exit()
{
	w.save();
}