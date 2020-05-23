#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

#ifdef USE_ofxWindowApp
	//windowApp.setFrameRate(60);//required when started for first time or no file settings present
	//windowApp.setSetVerticalSync(true);

	//can get some addon settings if required
	if (windowApp.isModeMini()) ofSetBackgroundColor(ofColor::blue);
	else ofSetBackgroundColor(ofColor::green);
#else
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
#endif
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
#ifdef USE_ofxWindowApp
	if (key == 'p')
		windowApp.togglePositionDebugInfo();

	if (key == 'm') {
		windowApp.toggleMode();

		if (windowApp.isModeMini()) ofSetBackgroundColor(ofColor::blue);
		else ofSetBackgroundColor(ofColor::green);
	}
#endif
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	//TODO:
	//there's some rare behaviors maybe when multiple monitors and switching to fullscreen/window modes... 
}

