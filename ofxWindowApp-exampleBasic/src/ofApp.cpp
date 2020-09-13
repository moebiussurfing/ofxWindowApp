#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	windowApp.setFrameRate(60);//required when started for first time or when no json file settings is present into /bin/data/ofxWindowApp/ofxWindowApp.json
	windowApp.setVerticalSync(true);
}

//--------------------------------------------------------------
void ofApp::draw() {
	string ss;
	ss += "KEYS:";
	ss += "F: FULL SCREEN\n";
	ss += "W: SHOW INFO\n";
	ss += "V: VERTICAL SYNC\n";
	ss += "M: MINI MODE";
	ofDrawBitmapStringHighlight(ss, 10, 20);
}