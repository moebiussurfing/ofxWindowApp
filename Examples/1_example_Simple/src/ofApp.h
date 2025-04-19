#pragma once
#include "ofMain.h"

#include "ofxWindowApp.h"

// NOTE:
// Press Alt + D
// (uppercased!) to toggle show debug info and key commands

class ofApp : public ofBaseApp {
public:
	void setup();

	ofxWindowApp w;
};
