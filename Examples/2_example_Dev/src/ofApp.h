#pragma once
#include "ofMain.h"

#include "ofxWindowApp.h"
#include "ofxGui.h"

// NOTE: Press "k" to toggle enable key commands!

// NOTE: Enable full Debug:
// In ofxWindowApp.h enable directive.
// #define OFX_WINDOW_APP__DEVELOP_DEBUG

class ofApp : public ofBaseApp {
public:
	void setup();
	void draw();
	void keyPressed(int key);
	void exit();

	ofxWindowApp w;

	ofxPanel gui;
};
