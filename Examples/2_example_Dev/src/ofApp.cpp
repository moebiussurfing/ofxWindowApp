#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	w.setup(&w);

	// Customize
	w.setFrameRate(144);
	//w.setFrameRate(0); // Max possible or unlocked if VSYNC_OFF, or up to display refresh freq if VSYNC_ON. 

	// Force show info
	w.bShowInfo = true;
}

//--------------------------------------------------------------
void ofApp::draw() {
	// Debug feedback when window settings changes:
	// Flash bg green during 200ms
	static bool bFlash = 0;
	if (w.isChanged()) {
		bFlash = 1;
		tLast = ofGetElapsedTimeMillis();
	}
	if (bFlash && ofGetElapsedTimeMillis() - tLast > 200) {
		bFlash = 0;
	}
	if (bFlash)
		ofClear(ofColor::green);
	else
		ofClear(64, 255);
}

//--------------------------------------------------------------
void ofApp::exit() {
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'k') w.setToggleEnableKeys();
}
