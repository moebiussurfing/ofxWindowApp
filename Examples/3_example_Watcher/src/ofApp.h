#pragma once
#include "ofMain.h"

#include "ofxWindowApp.h"

// NOTE: Enable file watcher to reload JSON file when changed:
// Uses https://github.com/nariakiiwatani/ofxWatcher and requires enable the directive:
// #define OFX_WINDOW_APP__USE_OFX_WATCHER

class ofApp : public ofBaseApp {
public:
	void setup();

	ofxWindowApp w;
};
