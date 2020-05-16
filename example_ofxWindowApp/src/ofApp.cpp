#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
#ifdef USE_ofxWindowApp
	windowApp.setSettingsFps(60);
	windowApp.setSettingsVsync(true);
#else
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
#endif
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
#ifdef USE_ofxWindowApp
	if (key == 'p')
		windowApp.togglePosition();
#endif
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	//TODO:
	//there's some rare behaviors maybe when multiple monitors and switching to fullscreen/window modes... 
}

