#pragma once

#include "ofMain.h"

#include "ofxWindowApp.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void draw();
	void exit();
	void windowResized(int w, int h);

	ofxWindowApp w;
};
