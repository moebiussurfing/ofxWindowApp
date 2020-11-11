#pragma once

#include "ofMain.h"

#include "ofxWindowApp.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void draw();

	ofxWindowApp windowApp;
};
