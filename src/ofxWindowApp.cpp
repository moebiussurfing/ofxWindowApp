#include "ofxWindowApp.h"

//--------------------------------------------------------------
ofxWindowApp::ofxWindowApp()
{
	ofSetLogLevel("ofxWindowApp", OF_LOG_NOTICE);
	//ofSetLogLevel("ofxWindowApp", OF_LOG_VERBOSE);

	//auto call setup
	setup();
}

//--------------------------------------------------------------
ofxWindowApp::~ofxWindowApp()
{
	if (autoSaveLoad)
		saveWindow();
}

//--------------------------------------------------------------
void ofxWindowApp::setup()
{
	ofLogVerbose("ofxWindowApp") << "called setup";

	ofAddListener(ofEvents().update, this, &ofxWindowApp::update);
	ofAddListener(ofEvents().draw, this, &ofxWindowApp::draw);
	ofAddListener(ofEvents().keyPressed, this, &ofxWindowApp::keyPressed);


	//extra settings
	params_Settings.add(vSync);
	params_Settings.add(fps);
	params_Settings.add(ENABLE_Debug);

	//load
	if (autoSaveLoad)
		loadWindow();

	windowResized(ofGetWindowSize().x, ofGetWindowSize().y);
}

//--------------------------------------------------------------
void ofxWindowApp::update(ofEventArgs & args)
{
	ofLogVerbose("ofxWindowApp") << "called update";
}

//--------------------------------------------------------------
void ofxWindowApp::draw(ofEventArgs & args)
{
	ofLogVerbose("ofxWindowApp") << "called draw";

	if (ENABLE_Debug)
		drawDEBUG();
}

//--------------------------------------------------------------
void ofxWindowApp::saveWindow()
{
	ofLogNotice("ofxWindowApp") << "saveWindow: " << path_folder + path_filename;

	//save window settings
	ofWindowSettings AppWindow;
	AppWindow.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
	AppWindow.setSize(ofGetWindowSize().x, ofGetWindowSize().y);
	AppWindow.windowMode = ofGetCurrentWindow()->getWindowMode();

	ofJson j;
	to_json(j, AppWindow);
	ofJson j2;
	ofSerialize(j2, params_Settings);

	//A. using 2 files
	//ofSavePrettyJson(path_folder + path_filename, j);
	//TODO:
	//we can't get framerate and vsync mode from window app.
	//should be setted by hand
	//extra settings could be mixed in one json only for both
	//TEST:
	//ofSavePrettyJson(path_folder + path_filename2, j2);

	//B. settings in one file
	ofJson data;
	data.push_back(j);
	data.push_back(j2);

	std::cout << data.dump(4) << std::endl;
	ofSavePrettyJson(path_folder + path_filename, data);
}

//--------------------------------------------------------------
void ofxWindowApp::loadWindow()
{
	//load window settings

	//A. using 2 files
	//ofJson j = ofLoadJson(path_folder + path_filename);
	//ofx::Serializer::ApplyWindowSettings(j);
	////extra settings could be mixed in one json only for both
	////TEST:
	//ofJson j2;
	//j2 = ofLoadJson(path_folder + path_filename2);
	//ofLogNotice("ofxWindowApp") << "json: " << j2;
	//ofDeserialize(j2, params_Settings);

	//B. settings in one file
	ofJson data;
	data = ofLoadJson(path_folder + path_filename);
	ofLogNotice("ofxWindowApp") << "all json: " << data;
	ofJson j = data[0];
	ofJson j2 = data[1];
	//recall both params groups
	ofDeserialize(j2, params_Settings);
	ofx::Serializer::ApplyWindowSettings(j);

	//-

	applySettings();

	ofLogNotice("ofxWindowApp") << "loadWindow: " << path_folder + path_filename;
}

//--------------------------------------------------------------
void ofxWindowApp::drawDEBUG()
{
	float realFps = ofGetFrameRate();

	string vSyncStr;
	string fpsRealStr;
	string fpsTargetStr;

	//----

	//debug overlay screen modes
	string strPad = "    ";//add spaces
	string str = "ofxWindowApp" + strPad;
	string screenStr = "";
	string screenPosStr = "";
	string screenMode = "";

	screenStr = ofToString(window_W) + "x" + ofToString(window_H);
	vSyncStr = ofToString((vSync ? "ON" : "OFF"));
	fpsRealStr = ofToString(realFps, 1);
	fpsTargetStr = ofToString(fps);
	//screenPosStr = " [" + ofToString(ofGetWindowPositionX()) + ", " + ofToString(ofGetWindowPositionY()) + "]";
	screenPosStr = " " + ofToString(ofGetWindowPositionX()) + "," + ofToString(ofGetWindowPositionY());

	bool bMode;
	if (ofGetWindowMode() == OF_WINDOW)//go full screen
	{
		bMode = false;
	}
	else if (ofGetWindowMode() == OF_FULLSCREEN)//go window mode
	{
		bMode = true;
	}
	screenMode += bMode ? "FULL SCREEN MODE" : "WINDOW MODE";

	str += "FPS " + fpsRealStr;
	str += " [" + fpsTargetStr + "]";
	str += strPad + "VSYNC-" + vSyncStr;
	str += strPad + "SIZE " + screenStr;
	str += strPad + "POSITION" + screenPosStr;
	str += strPad + screenMode;

	////WORKAROUND until windowResize implemented
	//window_X = ofGetWindowPositionX();
	//window_Y = ofGetWindowPositionY();
	//window_W = ofGetWindowSize().x;
	window_H = ofGetWindowSize().y;

	int xx, yy;

	if (layout_DEBUG_Position == DEBUG_BOTTOM)
	{
		xx = 0;
		yy = window_H - 2;
		//ofDrawBitmapStringHighlight(str, 0, window_H - 2);
	}
	else if (layout_DEBUG_Position == DEBUG_TOP)
	{
		xx = 0;
		yy = 15;
		//ofDrawBitmapStringHighlight(str, 0, 15);
	}
	ofDrawBitmapStringHighlight(str, xx, yy);

	//-

	//monitor fps performance

	////thresholds by factor
	//float fpsThreshold = 0.9f;//below this trigs alert red state
	//bool bPreShow = (realFps < fps*0.999);//by ratio

	//thresholds by factor
	//monitor fps performance
	float fpsThreshold = 10;//num frames below this trigs alert red state
	bool bPreShow = (realFps < fps - 5); //below 5 fps starts showing bar

	//bool bAlert = (realFps < fps*fpsThreshold);//by ratio
	bool bAlert = (realFps < (fps - fpsThreshold));//by absolute fps

	//-

	if (bPreShow)//to draw only under pre threshold
	{
		float fx, fy, fw, fh, fwMax;
		fwMax = 100;//max width
		fh = 10;
		fx = window_W - fwMax - 25;//pad to border
		fy = yy - fh;
		fw = ofMap(realFps, 0.0f*fps, fps, 0, fwMax, true);
		int fa = 200;

		ofPushStyle();
		ofFill();
		ofSetColor(bAlert ? (ofColor(ofColor::red, fa)) : (ofColor(ofColor::black, fa)));
		ofDrawRectangle(fx, fy, fw, fh);
		ofNoFill();
		ofSetLineWidth(1.0f);
		ofSetColor(ofColor(ofColor::white, fa - 100));
		ofDrawRectangle(fx, fy, fwMax, fh);
		ofPopStyle();
	}
}


//--------------------------------------------------------------
void ofxWindowApp::windowResized(int w, int h)
{
	window_X = ofGetWindowPositionX();
	window_Y = ofGetWindowPositionY();

	//window_W = ofGetWindowSize().x;
	//window_H = ofGetWindowSize().y;

	window_W = w;
	window_H = h;

	bChanged = true;
}

//--------------------------------------------------------------
void ofxWindowApp::keyPressed(ofKeyEventArgs &eventArgs)
{
	const int &key = eventArgs.key;

	//modifier
	bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);//macOS
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);//Windows. not working

	if (false)
		ofLogNotice("ofxWindowApp") << "keyPressed: '" << (char)key << "' [" << key << "]";

	//disable draw debug
	if (mod_COMMAND && key == 'w' || mod_CONTROL && key == 'w' ||
		key == 'W')
	{
		ENABLE_Debug = !ENABLE_Debug;
		ofLogVerbose("ofxWindowApp") << "changed draw debug: " << (ENABLE_Debug ? "ON" : "OFF");
	}

	//switch window mode
	if (key == 'F')
	{
		//WORKAROUND: to fit window and his bar visible into the screen
		float windowBar_h = 25;

		if (ofGetWindowMode() == OF_WINDOW)//go full screen
		{
			ofSetFullscreen(true);

			////WORKAROUND
			//window_X = ofGetWindowPositionX();
			//window_Y = 0;//align to top border
			//ofSetWindowPosition(window_X, window_Y);
		}
		else if (ofGetWindowMode() == OF_FULLSCREEN)//go window mode
		{
			ofSetFullscreen(false);

			//TODO:
			//rare behaviour with black screen in secondary monitors..

			//WORKAROUND
			//kick a little down to avoid hidden window title bar
			window_Y = MAX(ofGetWindowPositionY(), windowBar_h);//avoid negative out of screen. minimal h is 25
			window_X = ofGetWindowPositionX();
			ofSetWindowPosition(window_X, window_Y);

			////window_W = ofGetWindowWidth();
			//window_H = ofGetWindowHeight();

			//if (window_Y + window_H + windowBar_h > 1080)//bottom border goes out of v screen
			//{
			//	//float hMax = ofGetScreenHeight() - window_Y;// -windowBar_h;
			//	float hMax = 1080 - window_Y;// -windowBar_h;
			//	
			//	window_H = hMax;
			//	//ofSetWindowPosition(window_X, window_Y);
			//	ofSetWindowShape(window_W, window_H);
			//}
		}

		//update
		windowResized(ofGetWidth(), ofGetHeight());
	}
	else if (key == 'V')//switch Vsync mode
	{
		vSync = !vSync;
		ofSetVerticalSync(vSync);
	}
}
