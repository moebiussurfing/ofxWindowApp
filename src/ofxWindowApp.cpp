#include "ofxWindowApp.h"

//--------------------------------------------------------------
ofxWindowApp::ofxWindowApp()
{
	ofSetLogLevel(__FUNCTION__, OF_LOG_NOTICE);

	//default

	BigWindow.setPosition(glm::vec2(10, 10));
	BigWindow.setSize(800, 600);
	BigWindow.windowMode = ofGetCurrentWindow()->getWindowMode();

	MiniWindow.setPosition(glm::vec2(20, 20));
	MiniWindow.setSize(200, 200);
	MiniWindow.windowMode = ofGetCurrentWindow()->getWindowMode();

	//auto call setup
	setup();
}

//--------------------------------------------------------------
ofxWindowApp::~ofxWindowApp()
{
	ofLogNotice(__FUNCTION__) << "DONE!";

	exit();
}

//--------------------------------------------------------------
void ofxWindowApp::exit()
{
	ofLogNotice(__FUNCTION__);
	if (autoSaveLoad)
	{
		refreshGetWindowSettings();
		saveFileWindow();
	}

	ofRemoveListener(ofEvents().update, this, &ofxWindowApp::update);
	ofRemoveListener(ofEvents().draw, this, &ofxWindowApp::draw);
	ofRemoveListener(ofEvents().keyPressed, this, &ofxWindowApp::keyPressed);
}

//--------------------------------------------------------------
void ofxWindowApp::setup()
{
	ofLogNotice(__FUNCTION__);

	//default folders
	path_folder = "ofxWindowApp";
	path_filename = "ofxWindowApp.json";

	//callbacks to auto call update/draw/keyPressed
	ofAddListener(ofEvents().update, this, &ofxWindowApp::update);
	ofAddListener(ofEvents().draw, this, &ofxWindowApp::draw);
	ofAddListener(ofEvents().keyPressed, this, &ofxWindowApp::keyPressed);

	//extra settings
	params_Extra.add(vSync);
	params_Extra.add(targetFps);
	params_Extra.add(SHOW_Debug);
	params_Extra.add(SHOW_PerformanceAllways);
	params_Extra.add(bModeMini);
	params_Extra.add(bLock);

	//load
	if (autoSaveLoad) loadFileSettings();

	//default
	setShowPerformanceAllways(true);

	//mini settings
	params_Extra.add(vSync);
	params_Extra.add(targetFps);
	params_Extra.add(SHOW_Debug);
	params_Extra.add(SHOW_PerformanceAllways);
	params_Extra.add(bModeMini);

	windowResized(ofGetWindowSize().x, ofGetWindowSize().y);
}

//--------------------------------------------------------------
void ofxWindowApp::update(ofEventArgs & args)
{
	//ofLogVerbose(__FUNCTION__);

	//TODO: WIP lock mode
	//if (isChanged()) {
	//	ofLogNotice(__FUNCTION__) << "isChanged()";

	//	if (bLock) {// we want to lock windowResize changed. reload settings from file
	//		ofLogWarning(__FUNCTION__) << "Force lock!";

	//		//restore last state
	//		//loadFileSettings();
	//		ofSetWindowPosition(WindowPRE.getPosition().x, WindowPRE.getPosition().y);
	//		ofSetWindowShape(WindowPRE.getWidth(), WindowPRE.getHeight());
	//	}
	//}

	////TODO:
	////store last states
	//WindowPRE.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
	//WindowPRE.setSize(ofGetWindowSize().x, ofGetWindowSize().y);
}

//--------------------------------------------------------------
void ofxWindowApp::draw(ofEventArgs & args)
{
	//ofLogVerbose(__FUNCTION__);

	//--

	realFps = ofGetFrameRate();

	////workaround:
	//until windowResize well implemented
	//window_X = ofGetWindowPositionX();
	//window_Y = ofGetWindowPositionY();
	window_W = ofGetWindowSize().x;
	window_H = ofGetWindowSize().y;

	if (positionLayout == DEBUG_POSITION_BOTTOM)
	{
		yy = window_H - 6;
	}
	else if (positionLayout == DEBUG_POSITION_TOP)
	{
		yy = 15;
	}

	//--

	if (SHOW_Debug)
	{
		drawDEBUG();
		drawPerformance();
	}
	else
	{
		if (SHOW_PerformanceAllways)
			drawPerformance();
	}
}

//--------------------------------------------------------------
void ofxWindowApp::refreshGetWindowSettings()
{
	ofLogVerbose(__FUNCTION__);

	if (!bModeMini) {// big
		BigWindow.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
		BigWindow.setSize(ofGetWindowSize().x, ofGetWindowSize().y);
		BigWindow.windowMode = ofGetCurrentWindow()->getWindowMode();
	}
	else {// mini
		MiniWindow.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
		MiniWindow.setSize(ofGetWindowSize().x, ofGetWindowSize().y);
		MiniWindow.windowMode = ofGetCurrentWindow()->getWindowMode();//ignored
	}
}

//--------------------------------------------------------------
void ofxWindowApp::saveFileWindow()
{
	string __path = path_folder + "/" + path_filename;
	ofLogNotice(__FUNCTION__) << __path;

	//force mini to window, not fullscreen
	MiniWindow.windowMode = ofWindowMode(OF_WINDOW);

	//save window settings

	ofJson jApp;
	ofJson jMini;
	ofJson jExtra;

	to_json(jApp, BigWindow);
	to_json(jMini, MiniWindow);
	ofSerialize(jExtra, params_Extra);

	//A. using 2 files
	//ofSavePrettyJson(path_folder + "/"+path_filename, j);
	//TODO:
	//we can't get framerate and vsync mode from window app.
	//should be setted by hand
	//extra settings could be mixed in one json only for both
	//TEST:
	//ofSavePrettyJson(path_folder + "/"+path_filename2, jMini);

	//B. settings in one file
	ofJson data;
	data.push_back(jApp);
	data.push_back(jMini);
	data.push_back(jExtra);

	//check if we need to create data folder first
	folderCheckAndCreate(path_folder);

	//save file
	ofLogNotice(__FUNCTION__) << data.dump(4);
	ofSavePrettyJson(__path, data);
}

//--------------------------------------------------------------
void ofxWindowApp::loadFileSettings()
{
	string __path = path_folder + "/" + path_filename;

	//TODO: check if file exist
	ofFile file(__path);
	bool _b = file.exists();
	if (_b)
	{
		ofLogNotice(__FUNCTION__) << "File found: " << __path;

		//-

		//load settings

		//A. using 2 files
		//ofJson j = ofLoadJson(path_folder + "/"+path_filename);
		//ofx::Serializer::ApplyWindowSettings(j);
		////extra settings could be mixed in one json only for both
		////TEST:
		//ofJson jMini;
		//jMini = ofLoadJson(path_folder + "/"+path_filename2);
		//ofLogVerbose(__FUNCTION__) << "json: " << jMini;
		//ofDeserialize(jMini, params_Extra);

		//B. settings in one file
		ofJson data;
		data = ofLoadJson(__path);
		ofLogNotice(__FUNCTION__) << "All json: " << data;

		ofJson jBig;
		ofJson jMini;
		ofJson jExtra;

		if (data.size() >= 3) {
			jBig = data[0];//TODO: ugly workaround
			jMini = data[1];
			jExtra = data[2];

			//recall both params groups
			ofDeserialize(jExtra, params_Extra);
		}
		else ofLogError(__FUNCTION__) << "ERROR on data[] size = " << ofToString(data.size());

		ofLogVerbose(__FUNCTION__) << "jBig  : " << jBig;
		ofLogVerbose(__FUNCTION__) << "jMini : " << jMini;
		ofLogVerbose(__FUNCTION__) << "jExtra: " << jExtra;

		//-

		int jx, jy, jw, jh;
		string jm;

		//mini
		jx = jMini["position"]["x"];
		jy = jMini["position"]["y"];
		jw = jMini["size"]["width"];
		jh = jMini["size"]["height"];
		ofLogVerbose(__FUNCTION__) << "jx: " << jx;
		ofLogVerbose(__FUNCTION__) << "jy: " << jy;
		ofLogVerbose(__FUNCTION__) << "jw: " << jw;
		ofLogVerbose(__FUNCTION__) << "jh: " << jh;
		MiniWindow.setPosition(glm::vec2(jx, jy));
		MiniWindow.setSize(jw, jh);

		//big
		jx = jBig["position"]["x"];
		jy = jBig["position"]["y"];
		jw = jBig["size"]["width"];
		jh = jBig["size"]["height"];
		jm = jBig["window_mode"];
		ofLogVerbose(__FUNCTION__) << "jx: " << jx;
		ofLogVerbose(__FUNCTION__) << "jy: " << jy;
		ofLogVerbose(__FUNCTION__) << "jw: " << jw;
		ofLogVerbose(__FUNCTION__) << "jh: " << jh;
		ofLogVerbose(__FUNCTION__) << "jm: " << jm;


		if (jy < SIZE_SECURE_GAP_INISDE_SCREEN) {
			jy = (int)SIZE_SECURE_GAP_INISDE_SCREEN;
		}

		BigWindow.setPosition(glm::vec2(jx, jy));
		BigWindow.setSize(jw, jh);
		windowBigMode = jm;

		////apply
		//if (bModeMini) {
		//	ofx::Serializer::ApplyWindowSettings(jBig);
		//}
		//else {
		//	ofx::Serializer::ApplyWindowSettings(jMini);
		//}
	}
	else
	{
		ofLogError(__FUNCTION__) << "File NOT found: " << __path;
	}

	//-

	//apply
	applyExtra();
	applyMode();
}

//--------------------------------------------------------------
void ofxWindowApp::drawDEBUG()
{
	//----

	//debug overlay screen modes

	string vSyncStr;
	string fpsRealStr;
	string fpsTargetStr;
	string strPad = "  ";//add spaces
	string str;
	//str = __FUNCTION__ + strPad;
	string screenStr = "";
	string screenPosStr = "";
	string screenMode = "";

	screenStr = ofToString(window_W) + "x" + ofToString(window_H);
	vSyncStr = ofToString((vSync ? "ON " : "OFF"));
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
	screenMode += bMode ? "FULL-SCREEN_MODE" : "WINDOW_MODE";
	screenMode += bModeMini ? " [MINI]" : " [BIG]";

	str += "FPS " + fpsRealStr;
	str += " [" + fpsTargetStr + "]";
	str += strPad + "VSYNC-" + vSyncStr;
	str += strPad + "SIZE " + screenStr;
	str += strPad + "POSITION" + screenPosStr;
	str += strPad + screenMode;
	str += strPad + (bLock ? "LOCKED ON" : "LOCKED OFF");

	ofDrawBitmapStringHighlight(str, xx, yy);
}

//--------------------------------------------------------------
void ofxWindowApp::drawPerformance()
{
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
		string diff;
		diff += "-";
		diff += ofToString(iDiff, 0);
		if (iDiff < 10) diff = " " + diff;
		diff += " FPS";

		ofPushStyle();

		ofFill();
		ofColor cAlert(bAlert ? (ofColor(ofColor::red, fa)) : (ofColor(ofColor::black, fa)));
		ofSetColor(cAlert);
		ofDrawRectangle(fx, fy, fw, fh);//out-performance bar
		ofDrawBitmapStringHighlight(diff, fx - 68.f, yy, cAlert, ofColor(255));//text fps diff

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
	ofLogNotice(__FUNCTION__) << ofToString(w) << "," << ofToString(h);

	window_W = w;
	window_H = h;
	window_X = ofGetWindowPositionX();
	window_Y = ofGetWindowPositionY();

	refreshGetWindowSettings();

	bChangedWindow = true;
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
			ofLogNotice(__FUNCTION__) << "'" << (char)key << "' [" << key << "]";

		//disable draw debug
		if (mod_COMMAND && key == 'w' || mod_CONTROL && key == 'w' ||
			key == 'W')
		{
			SHOW_Debug = !SHOW_Debug;
			ofLogNotice(__FUNCTION__) << "changed draw debug: " << (SHOW_Debug ? "ON" : "OFF");
		}

		//switch window mode
		else if (key == 'F')
		{
			if (ofGetWindowMode() == OF_WINDOW)//go full screen
			{
				ofSetFullscreen(true);

				////workaround
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
		else if (key == 'M')//switch window mode big/mini
		{
			toggleModeWindowBigMini();
		}
		else if (key == 'L')//toggle lock
		{
			bLock = !bLock;
		}
		else if (key == 'R')//reset to full HD
		{
			BigWindow.setPosition(glm::vec2(0, SIZE_SECURE_GAP_INISDE_SCREEN));
			BigWindow.setSize(1920, 1080);
			ofSetWindowPosition(BigWindow.getPosition().x, BigWindow.getPosition().y);
			ofSetWindowShape(BigWindow.getWidth(), BigWindow.getHeight());
		}
	}
}

//--------------------------------------------------------------
void ofxWindowApp::folderCheckAndCreate(string _path)
{
	ofLogNotice(__FUNCTION__);

	ofDirectory dataDirectory(ofToDataPath(_path, true));

	//check if target data folder exist
	if (!dataDirectory.isDirectory())
	{
		ofLogError("__FUNCTION__") << "FOLDER DOES NOT EXIST!";

		//create folder
		bool b = dataDirectory.createDirectory(ofToDataPath(_path, true));

		//debug if creation has been succed
		if (b)
			ofLogNotice("__FUNCTION__") << "FOLDER '" << _path << "' CREATED SUCCESSFULLY!";
		else
			ofLogError("__FUNCTION__") << "UNABLE TO CREATE '" << _path << "' FOLDER!";
	}
}
