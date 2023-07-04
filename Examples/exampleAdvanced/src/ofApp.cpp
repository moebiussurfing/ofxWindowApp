#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//// Set the instance before calling setup()
	//ofxWindowApp::setInstance(&w);
	w.setup();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	w.drawInfo();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}

//--------------------------------------------------------------
void ofApp::exit()
{
	//w.save();
}