#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	w.setup(&w);

	// Customize
	w.setFrameRate(144);
	//w.setFrameRate(0); // Max possible or unlocked if VSYNC_OFF, or up to display refresh freq if VSYNC_ON. 

	// Force show info
	w.bShowInfo = true;

	gui.setup(w.params);
	gui.setPosition(250, 10);
}

//--------------------------------------------------------------
void ofApp::draw() {
	// Debug feedback when window settings changes:
	// Flash bg green during 200ms
	static uint64_t tLast = 0;
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

	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'k') w.setToggleEnableKeys();
}

//--------------------------------------------------------------
void ofApp::exit() {
}
