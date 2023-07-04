#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
#ifdef SURFING_WINDOW_APP__USE_STATIC
	w.setup(&w);
#endif
}

//--------------------------------------------------------------
void ofApp::draw()
{
	if (w.isChanged()) ofClear(ofColor::yellow);
	else ofClear(64);

	w.drawDebugInfo();
}

//--------------------------------------------------------------
void ofApp::exit()
{
	//w.save();
}