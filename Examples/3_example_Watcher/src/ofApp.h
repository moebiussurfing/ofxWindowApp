#pragma once
#include "ofMain.h"

#include "ofxWindowApp.h"

// NOTE: Enable file watcher to reload JSON file when changed:
// Requires https://github.com/nariakiiwatani/ofxWatcher and to enable the directive in ofxWindowApp.h:
// #define OFX_WINDOW_APP__USE_OFX_WATCHER

class ofApp : public ofBaseApp {
public:
	void setup();

	ofxWindowApp w;
};
