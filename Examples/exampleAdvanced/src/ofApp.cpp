#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
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
	w.save();
}