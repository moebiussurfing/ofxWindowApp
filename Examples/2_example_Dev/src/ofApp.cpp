#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	w.setup(&w);

	// Customize
	w.setFrameRate(144);
	//w.setFrameRate(0); // Max possible or unlocked if VSYNC_OFF, or up to display refresh freq if VSYNC_ON.

	//--
	
	// Force show Info
	w.bShowInfo = true;

	// Debug gui
	gui.setup(w.params);
	gui.setPosition(250, 10);
}

//--------------------------------------------------------------
void ofApp::draw() {
	// Debug feedback when window settings changes and JSON file is saved:
	// Flash bg green during 100ms
	{
		static bool bFlash = 0;
		static uint64_t tLast = 0;
		{
			if (w.isChanged()) {
				bFlash = 1;
				tLast = ofGetElapsedTimeMillis();
			}
			if (bFlash && ofGetElapsedTimeMillis() - tLast > 100) {
				bFlash = 0;
			}
		}
		if (bFlash)
			ofClear(ofColor::green);
		else
			ofClear(64, 255);
	}

	if (bGui) gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'k') w.setToggleEnableKeys();
	if (key == 'g') bGui = !bGui;
}

//--------------------------------------------------------------
void ofApp::exit() {
}
