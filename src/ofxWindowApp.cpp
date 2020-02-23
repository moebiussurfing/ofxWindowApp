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

	ofRemoveListener(ofEvents().update, this, &ofxWindowApp::update);
	ofRemoveListener(ofEvents().draw, this, &ofxWindowApp::draw);
	ofRemoveListener(ofEvents().keyPressed, this, &ofxWindowApp::keyPressed);
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
	params_Settings.add(targetFps);
	params_Settings.add(SHOW_Debug);

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

	if (SHOW_Debug)
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

	//----

	//debug overlay screen modes

	string vSyncStr;
	string fpsRealStr;
	string fpsTargetStr;
	string strPad = "    ";//add spaces
	string str = "ofxWindowApp" + strPad;
	string screenStr = "";
	string screenPosStr = "";
	string screenMode = "";

	screenStr = ofToString(window_W) + "x" + ofToString(window_H);
	vSyncStr = ofToString((vSync ? "ON" : "OFF"));
	fpsRealStr = ofToString(realFps, 1);
	fpsTargetStr = ofToString(targetFps);
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

	////WORKAROUND:
	//until windowResize well implemented
	//window_X = ofGetWindowPositionX();
	//window_Y = ofGetWindowPositionY();
	window_W = ofGetWindowSize().x;
	window_H = ofGetWindowSize().y;

	int xx = 0;
	int yy = 0;
	if (positionLayout == DEBUG_POSITION_BOTTOM)
	{
		yy = window_H - 6;
	}
	else if (positionLayout == DEBUG_POSITION_TOP)
	{
		yy = 15;
	}
	ofDrawBitmapStringHighlight(str, xx, yy);

	//-

	//monitor fps performance alert

	bool bPreShow;//starts draw black
	bool bAlert;//draws red
	float fpsThreshold;

	////A. thresholds by factor fps between target and real fps
	//fpsThreshold = 0.9f;//below this trigs alert red state
	//bPreShow = (realFps < targetFps*0.999);//by ratio
	//bAlert = (realFps < targetFps*fpsThreshold);//A. by ratio

	//B. thresholds by absolute fps difference between desired target and real fps
	//monitor fps performance
	fpsThreshold = 10.f;//num frames below this trigs alert red state
	bPreShow = (realFps < targetFps - 5); //below 5 fps starts showing bar
	bAlert = (realFps < (targetFps - fpsThreshold));//B. by absolute fps diff

	//-

	if (bPreShow)//to draw only under pre threshold
	{
		float fx, fy, fw, fh, fwMax, fPad;
		fwMax = 100.f;//max width
		fh = 10.f;
		fPad = 5.f;//pad to border
		fx = window_W - fwMax - fPad;
		fy = yy - fh + 1.0f;
		fw = ofMap(realFps, 0.0f*targetFps, targetFps, 0, fwMax, true);
		int fa = 150;//alpha
		int iDiff = (int)targetFps - realFps;
		string diff = ofToString(iDiff, 0);
		if (iDiff < 10) diff = " " + diff;
		diff += " FPS";

		ofPushStyle();

		ofFill();
		ofColor cAlert(bAlert ? (ofColor(ofColor::red, fa)) : (ofColor(ofColor::black, fa)));
		ofSetColor(cAlert);
		ofDrawRectangle(fx, fy, fw, fh);//out-performance bar
		ofDrawBitmapStringHighlight(diff, fx - 62.f, yy, cAlert, ofColor(255));//text fps diff

		ofNoFill();
		ofSetLineWidth(2.0f);
		ofSetColor(cAlert);
		ofDrawRectangle(fx, fy, fwMax, fh);//rect border

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
	if (ENABLE_Keys)
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
			SHOW_Debug = !SHOW_Debug;
			ofLogVerbose("ofxWindowApp") << "changed draw debug: " << (SHOW_Debug ? "ON" : "OFF");
		}

		//switch window mode
		else if (key == 'F')
		{
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

				//WORKAROUND: to fit window and his bar visible into the screen
				float windowBar_h = 25;

				//TODO:
				//rare behaviour with black screen in secondary monitors..

				//WORKAROUND
				//kick a little down to avoid hidden window title bar
				window_Y = MAX(ofGetWindowPositionY(), windowBar_h);//avoid negative out of screen. minimal h is 25
				window_X = ofGetWindowPositionX();
				ofSetWindowPosition(window_X, window_Y);

				////window_W = ofGetWindowWidth();
				////window_H = ofGetWindowHeight();

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
}
