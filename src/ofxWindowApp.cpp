#include "ofxWindowApp.h"

//--------------------------------------------------------------
ofxWindowApp::ofxWindowApp()
{
	ofSetLogLevel("ofxWindowApp", OF_LOG_NOTICE);

	// default
	int _h = BAR_HEIGHT;//bar height
	BigWindow.setPosition(glm::vec2(0, _h));
	//BigWindow.setSize(800, 600);
	BigWindow.setSize(1920, 1080 - _h);
	BigWindow.windowMode = ofGetCurrentWindow()->getWindowMode();


#ifdef USE_MINI_WINDOW
	MiniWindow.setPosition(glm::vec2(20, 20));
	MiniWindow.setSize(200, 200);
	MiniWindow.windowMode = ofGetCurrentWindow()->getWindowMode();
#endif

	setFrameRate(60);
	setVerticalSync(false);
	setShowDebug(false);

	// auto call setup
	setup();
}

//--------------------------------------------------------------
ofxWindowApp::~ofxWindowApp()
{
	ofLogNotice("ofxWindowApp") << (__FUNCTION__) << "DONE!";

	exit();
}

//--------------------------------------------------------------
void ofxWindowApp::exit()
{
	ofLogNotice("ofxWindowApp") << (__FUNCTION__);
	if (bAutoSaveLoad)
	{
		refreshGetWindowSettings();
		saveFileWindow();
	}

	ofRemoveListener(ofEvents().update, this, &ofxWindowApp::update);
	ofRemoveListener(ofEvents().draw, this, &ofxWindowApp::draw);
	ofRemoveListener(ofEvents().keyPressed, this, &ofxWindowApp::keyPressed);
	ofRemoveListener(ofEvents().keyReleased, this, &ofxWindowApp::keyReleased);
}

//--------------------------------------------------------------
void ofxWindowApp::setup()
{
	ofLogNotice("ofxWindowApp") << (__FUNCTION__);

	// default folders
	path_folder = "ofxWindowApp";
	path_filename = "ofxWindowApp.json";

	// callbacks to auto call update/draw/keyPressed
	ofAddListener(ofEvents().update, this, &ofxWindowApp::update);
	ofAddListener(ofEvents().draw, this, &ofxWindowApp::draw);
	ofAddListener(ofEvents().keyPressed, this, &ofxWindowApp::keyPressed);
	ofAddListener(ofEvents().keyReleased, this, &ofxWindowApp::keyReleased);
	//ofAddListener(ofEvents().windowMoved, this, &ofxWindowApp::windowIsMoved);
	//ofAddListener(ofEvents().windowResized, this, &ofxWindowApp::windowIsResized);

	// extra settings
	params_Extra.add(vSync);
	params_Extra.add(targetFps);
	params_Extra.add(bDebug);
	params_Extra.add(bShowPerformanceAlways);

#ifdef USE_MINI_WINDOW
	params_Extra.add(bModeMini);
#endif
	params_Extra.add(bLock);

	// load
	if (bAutoSaveLoad) loadFileSettings();

	// default
	setShowPerformanceAllways(true);

	//// mini settings
	//params_Extra.add(vSync);
	//params_Extra.add(targetFps);
	//params_Extra.add(bDebug);
	//params_Extra.add(bShowPerformanceAlways);
	//params_Extra.add(bModeMini);

	//-

	// workarounds
	windowResized(ofGetWindowSize().x, ofGetWindowSize().y);

#ifdef USE_MINI_WINDOW
	if (!bModeMini)
#endif
	{
		if (bigFullScreen) {
			ofSetFullscreen(bigFullScreen);
			ofSetWindowPosition(0, 0);
			ofSetWindowShape(ofGetWidth(), ofGetHeight());
		}
	}

	//--

#ifdef USE_CUSTOM_FONT
	// font
	fontSize = 10;
	string _path = "assets/fonts/"; // assets folder
	string _f = "JetBrainsMono-Bold.ttf";
	_path += _f;
	bool b = font.load(_path, fontSize);
	if (!b) font.load(OF_TTF_MONO, fontSize);
#endif
}

//--------------------------------------------------------------
void ofxWindowApp::update(ofEventArgs& args)
{
	if (ofGetFrameNum() == 1) {
		// workaround: 

		//// works but slow
		//refreshTogleWindowMode();
		//refreshTogleWindowMode();

		ofToggleFullscreen();
		ofToggleFullscreen();
	}

	//TODO: 
	// WIP lock mode
	//if (isChanged()) {
	//	ofLogNotice("ofxWindowApp")<<(__FUNCTION__) << "isChanged()";
	//	if (bLock) {// we want to lock windowResize changed. reload settings from file
	//		ofLogWarning("ofxWindowApp")<<(__FUNCTION__) << "Force lock!";
	//		//restore last state
	//		//loadFileSettings();
	//		ofSetWindowPosition(WindowPRE.getPosition().x, WindowPRE.getPosition().y);
	//		ofSetWindowShape(WindowPRE.getWidth(), WindowPRE.getHeight());
	//	}
	//}

	////TODO:
	//// store last states
	//WindowPRE.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
	//WindowPRE.setSize(ofGetWindowSize().x, ofGetWindowSize().y);

	////TODO: fix
	//// autosave
	//if (isChanged()) 
	//{
	//	if (bAutoSaveLoad)
	//	{
	//		ofLogNotice("ofxWindowApp")<<(__FUNCTION__) << "Just saved after window been resized";
	//		//refreshGetWindowSettings();
	//		saveFileWindow();
	//	}
	//}

	//--

	// Autosaver timer
	// is no required to resize the window or to close the app window to save.
	// then the app can carsh an window shape will be stored 1 time each 10 seconds by default.
	if (bAutoSaverTimed)
		if ((ofGetElapsedTimeMillis() - timerSaver) > timerSaverMax)
		{
			saveFileWindow();
			timerSaver = ofGetElapsedTimeMillis();
		}
}

//--------------------------------------------------------------
void ofxWindowApp::draw(ofEventArgs& args)
{
	//ofLogVerbose("ofxWindowApp")<<(__FUNCTION__);

	//--

	realFps = ofGetFrameRate();

	//// workaround:
	//until windowResize well implemented
	//window_X = ofGetWindowPositionX();
	//window_Y = ofGetWindowPositionY();
	window_W = ofGetWindowSize().x;
	window_H = ofGetWindowSize().y;

	if (positionLayout == DEBUG_POSITION_BOTTOM)
	{
#ifndef USE_CUSTOM_FONT
		yy = window_H - 10; // little air
#endif
#ifdef USE_CUSTOM_FONT
		yy = window_H - fontSize + 5;
#endif
	}

	else if (positionLayout == DEBUG_POSITION_TOP)
	{
#ifndef USE_CUSTOM_FONT
		yy = 15;
#endif
#ifdef USE_CUSTOM_FONT
		yy = fontSize;
#endif
	}

	//--

	if (bDebug)
	{
		drawDebug();
		drawPerformance();
	}
	else
	{
		if (bShowPerformanceAlways)
			drawPerformance();
	}
}

//--------------------------------------------------------------
void ofxWindowApp::refreshGetWindowSettings()
{
	ofLogVerbose("ofxWindowApp") << (__FUNCTION__);

#ifdef USE_MINI_WINDOW
	if (!bModeMini)
#endif
	{
		// big
		BigWindow.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
		BigWindow.setSize(ofGetWindowSize().x, ofGetWindowSize().y);
		BigWindow.windowMode = ofGetCurrentWindow()->getWindowMode();

		//?
		if (BigWindow.windowMode == ofWindowMode(0)) bigFullScreen = false;
		else if (BigWindow.windowMode == ofWindowMode(1)) bigFullScreen = true;
		else if (BigWindow.windowMode == ofWindowMode(2)) bigFullScreen = false;
	}

#ifdef USE_MINI_WINDOW
	else
	{
		// mini
		MiniWindow.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
		MiniWindow.setSize(ofGetWindowSize().x, ofGetWindowSize().y);
		MiniWindow.windowMode = ofGetCurrentWindow()->getWindowMode();//ignored
	}
#endif
}

//--------------------------------------------------------------
void ofxWindowApp::saveFileWindow()
{
	string __path = path_folder + "/" + path_filename;
	ofLogNotice("ofxWindowApp") << (__FUNCTION__) << __path;

	// force mini to window, not full screen
#ifdef USE_MINI_WINDOW
	MiniWindow.windowMode = ofWindowMode(OF_WINDOW);
#endif

	// save window settings

	ofJson jApp;
	ofJson jMini;
	ofJson jExtra;

	to_json(jApp, BigWindow);

#ifdef USE_MINI_WINDOW
	to_json(jMini, MiniWindow);
#endif

	ofSerialize(jExtra, params_Extra);

	jApp["Preset"] = "Big";


#ifdef USE_MINI_WINDOW
	jMini["Preset"] = "Mini";
#endif

	// A. using 2 files
	//ofSavePrettyJson(path_folder + "/"+path_filename, j);
	//TODO:
	// we can't get framerate and vsync mode from window app.
	// should be settled by hand
	// extra settings could be mixed in one json only for both
	// TEST:
	//ofSavePrettyJson(path_folder + "/"+path_filename2, jMini);

	//B. settings in one file
	ofJson data;
	data.push_back(jApp);

#ifdef USE_MINI_WINDOW
	data.push_back(jMini);
#endif
	data.push_back(jExtra);

	// check if we need to create data folder first
	folderCheckAndCreate(path_folder);

	// save file
	ofLogNotice("ofxWindowApp") << data.dump(4);
	ofSavePrettyJson(__path, data);
}

//--------------------------------------------------------------
void ofxWindowApp::loadFileSettings()
{
	ofSetFullscreen(false);

	string __path = path_folder + "/" + path_filename;

	//TODO: check if file exist
	ofFile file(__path);
	bool _b = file.exists();
	if (_b)
	{
		ofLogNotice("ofxWindowApp") << (__FUNCTION__) << "File found: " << __path;

		//-

		// Load settings

		// A. using 2 files
		//ofJson j = ofLoadJson(path_folder + "/"+path_filename);
		//ofx::Serializer::ApplyWindowSettings(j);
		//// extra settings could be mixed in one json only for both
		//// TEST:
		//ofJson jMini;
		//jMini = ofLoadJson(path_folder + "/"+path_filename2);
		//ofLogVerbose("ofxWindowApp")<<(__FUNCTION__) << "json: " << jMini;
		//ofDeserialize(jMini, params_Extra);

		//B. settings in one file
		ofJson data;
		data = ofLoadJson(__path);
		ofLogNotice("ofxWindowApp") << "All json: " << data;

		ofJson jBig;

#ifdef USE_MINI_WINDOW
		ofJson jMini;
		ofJson jExtra;

		if (data.size() >= 3) {
			jBig = data[0];//TODO: ugly workaround
			jMini = data[1];//TODO: should add a tag mini or extra to mark the file section
			jExtra = data[2];

			//recall both params groups
			ofDeserialize(jExtra, params_Extra);
		}
		else ofLogError("ofxWindowApp") << "ERROR on data[] size = " << ofToString(data.size());

		ofLogVerbose("ofxWindowApp") << "jBig  : " << jBig;
		ofLogVerbose("ofxWindowApp") << "jMini : " << jMini;
		ofLogVerbose("ofxWindowApp") << "jExtra: " << jExtra;
#endif

#ifndef USE_MINI_WINDOW
		ofJson jExtra;

		if (data.size() >= 2) {
			jBig = data[0];//TODO: ugly workaround
			jExtra = data[1];

			//recall both params groups
			ofDeserialize(jExtra, params_Extra);
		}
		else ofLogError("ofxWindowApp") << "ERROR on data[] size = " << ofToString(data.size());

		ofLogVerbose("ofxWindowApp") << "jBig  : " << jBig;
		ofLogVerbose("ofxWindowApp") << "jExtra: " << jExtra;
#endif

		//-

		int jx, jy, jw, jh;
		string jm;

#ifdef USE_MINI_WINDOW
		// mini
		jx = jMini["position"]["x"];
		jy = jMini["position"]["y"];
		jw = jMini["size"]["width"];
		jh = jMini["size"]["height"];

		ofLogVerbose("ofxWindowApp") << "jx: " << jx;
		ofLogVerbose("ofxWindowApp") << "jy: " << jy;
		ofLogVerbose("ofxWindowApp") << "jw: " << jw;
		ofLogVerbose("ofxWindowApp") << "jh: " << jh;

		MiniWindow.setPosition(glm::vec2(jx, jy));
		MiniWindow.setSize(jw, jh);
		MiniWindow.windowMode = ofWindowMode(0);// ? allways windowed

		if (jm == "OF_WINDOW") MiniWindow.windowMode = ofWindowMode(0);
		else if (jm == "OF_FULLSCREEN") MiniWindow.windowMode = ofWindowMode(1);
		else if (jm == "OF_GAME_MODE") MiniWindow.windowMode = ofWindowMode(2);
#endif

		//-

		// big
		jx = jBig["position"]["x"];
		jy = jBig["position"]["y"];
		jw = jBig["size"]["width"];
		jh = jBig["size"]["height"];
		jm = ofToString(jBig["window_mode"]);
		//jm = jBig["window_mode"];

		ofLogVerbose("ofxWindowApp") << "jx: " << jx;
		ofLogVerbose("ofxWindowApp") << "jy: " << jy;
		ofLogVerbose("ofxWindowApp") << "jw: " << jw;
		ofLogVerbose("ofxWindowApp") << "jh: " << jh;
		ofLogVerbose("ofxWindowApp") << "jm: " << jm;

		// TODO:
		// workaround
		if (jy < SIZE_SECURE_GAP_INISDE_SCREEN) {
			jy = (int)SIZE_SECURE_GAP_INISDE_SCREEN;
		}

		//OF_WINDOW = 0
		//OF_FULLSCREEN = 1
		//OF_GAME_MODE = 2
		if (jm == "OF_WINDOW") BigWindow.windowMode = ofWindowMode(0);
		else if (jm == "OF_FULLSCREEN") BigWindow.windowMode = ofWindowMode(1);
		else if (jm == "OF_GAME_MODE") BigWindow.windowMode = ofWindowMode(2);

		if (jm == "OF_WINDOW") bigFullScreen = false;
		else if (jm == "OF_FULLSCREEN") bigFullScreen = true;
		else if (jm == "OF_GAME_MODE") bigFullScreen = false;

		if (!bigFullScreen) {
			BigWindow.setPosition(glm::vec2(jx, jy));
			BigWindow.setSize(jw, jh);
		}
		else {
			BigWindow.setPosition(glm::vec2(0, 0));
			BigWindow.setSize(jw, jh);
			//BigWindow.setSize(ofGetWidth(), ofGetHeight);
		}

		//windowBigMode = jm;// ?

		//// apply
		//if (bModeMini) {
		//	ofx::Serializer::ApplyWindowSettings(jBig);
		//}
		//else {
		//	ofx::Serializer::ApplyWindowSettings(jMini);
		//}
	}
	else
	{
		ofLogError("ofxWindowApp") << "File NOT found: " << __path;
	}

	//-

	// apply
	//applyExtra();
	applyMode();
}

//--------------------------------------------------------------
void ofxWindowApp::drawDebug()
{
	//----

	// debug overlay screen modes

	string vSyncStr;
	string fpsRealStr;
	string fpsTargetStr;
	string strPad = "  ";//add spaces
	string str;
	string screenStr = "";
	string screenPosStr = "";
	string screenMode = "";

	screenStr = ofToString(window_W) + "x" + ofToString(window_H);
	vSyncStr = ((vSync ? "ON " : "OFF"));
	fpsRealStr = ofToString(realFps, 0);
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

#ifdef USE_MINI_WINDOW
	screenMode += bModeMini ? " [MINI]" : " [BIG]";
#endif

	str += "FPS " + fpsRealStr;
	str += " [" + fpsTargetStr + "]";
	str += strPad + "VSYNC-" + vSyncStr;
	str += strPad + "SIZE " + screenStr;
	str += strPad + "POSITION" + screenPosStr;
	str += strPad + screenMode;
	str += strPad + (bLock ? "LOCKED ON" : "LOCKED OFF");

#ifndef USE_CUSTOM_FONT
	ofDrawBitmapStringHighlight(str, xx, yy);
#endif
#ifdef USE_CUSTOM_FONT
	ofPushStyle();
	ofFill();
	auto bb = font.getStringBoundingBox(str, xx, yy);
	bb.setWidth(bb.getWidth() + pad);
	bb.setHeight(bb.getHeight() + pad);
	bb.translateX(-pad / 2);
	bb.translateY(-pad / 2);
	ofSetColor(0);
	ofDrawRectangle(bb);
	ofSetColor(255);
	font.drawString(str, xx, yy);
	ofPopStyle();
#endif
}

//--------------------------------------------------------------
void ofxWindowApp::drawPerformance()
{
	// monitor fps performance alert

	bool bPreShow;//starts draw black
	bool bAlert;//draws red
	float fpsThreshold;

	//// A. Thresholds by factor fps between target and real fps
	//fpsThreshold = 0.9f;//below this trigs alert red state
	//bPreShow = (realFps < targetFps*0.999);//by ratio
	//bAlert = (realFps < targetFps*fpsThreshold);//A. by ratio

	// B. Thresholds by absolute fps difference between desired target and real fps
	// monitor fps performance
	fpsThreshold = 10.f;//num frames below this trigs alert red state
	bPreShow = (realFps < targetFps - 5); //below 5 fps starts showing bar
	bAlert = (realFps < (targetFps - fpsThreshold));//B. by absolute fps diff

	//-

	// Bar

	if (bPreShow) // to draw only under pre threshold
	{
		float fx, fy, fw, fh, fwMax, fPad;
		fwMax = 100.f;//max width

#ifndef USE_CUSTOM_FONT
		fh = 10.f;
		fPad = 5.f;//pad to border
#endif
#ifdef USE_CUSTOM_FONT
		fPad = 4.f;//pad to border
		//fh = fontSize + pad - 4;
#endif

		// position
		fx = window_W - fwMax - fPad;

#ifndef USE_CUSTOM_FONT
		//fy = yy - fh - 50.0f;//little air
		fy = yy - fh + 1.0f;//to the border
#endif

		fw = ofMap(realFps, 0.0f * targetFps, targetFps, 0, fwMax, true);
		int fa = 150;//alpha
		int iDiff = (int)targetFps - realFps;
		string diff;
		diff += "-";
		diff += ofToString(iDiff, 0);
		if (iDiff < 10) diff = " " + diff;
		diff += " FPS";

		ofColor cAlert(bAlert ? (ofColor(ofColor::red, fa)) : (ofColor(ofColor::black, fa)));

		ofPushStyle();

		float _xx = fx - 68.f;
		float _yy = yy;

		// fps label
		string str = diff;
#ifndef USE_CUSTOM_FONT
		//ofDrawBitmapStringHighlight(str, xx, yy);
		ofDrawBitmapStringHighlight(diff, fx - 68.f, yy, cAlert, ofColor(255));//text fps diff
#endif
#ifdef USE_CUSTOM_FONT
		ofPushStyle();
		ofFill();
		auto bb = font.getStringBoundingBox(str, _xx, _yy);
		bb.setWidth(bb.getWidth() + pad);
		bb.setHeight(bb.getHeight() + pad);
		bb.translateX(-pad / 2);
		bb.translateY(-pad / 2);
		ofSetColor(cAlert);
		ofDrawRectangle(bb);
		ofSetColor(255);
		font.drawString(str, _xx, _yy);
		ofPopStyle();
#endif

#ifdef USE_CUSTOM_FONT
		fy = bb.getY();
		fh = bb.getHeight()  - 2;
#endif

		// out-performance bar
		ofFill();
		ofSetColor(cAlert);
		ofDrawRectangle(fx, fy, fw, fh);

		// rect border
		ofNoFill();
		ofSetLineWidth(2.0f);
		ofSetColor(cAlert);
		ofDrawRectangle(fx, fy, fwMax, fh); 

		ofPopStyle();
	}
}

//--------------------------------------------------------------
void ofxWindowApp::windowResized(int w, int h)
{
	ofLogNotice("ofxWindowApp") << (__FUNCTION__) << ofToString(w) << "," << ofToString(h);

	window_W = w;
	window_H = h;
	window_X = ofGetWindowPositionX();
	window_Y = ofGetWindowPositionY();

	refreshGetWindowSettings();

	bChangedWindow = true;

	//TODO: fix
	if (bAutoSaveLoad)
	{
		ofLogNotice("ofxWindowApp") << (__FUNCTION__) << "Just saved after window been resized";
		saveFileWindow();
	}
}

//--------------------------------------------------------------
void ofxWindowApp::keyPressed(ofKeyEventArgs& eventArgs)
{
	if (bKeys)
	{
		const int& key = eventArgs.key;

		//modifier
		mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);//macOS
		mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);//Windows. not working
		mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);

		if (false)
			ofLogNotice("ofxWindowApp") << (__FUNCTION__) << "'" << (char)key << "' [" << key << "]";

		//disable draw debug
		if (mod_CONTROL && key == 'w' || mod_CONTROL && key == 'w' ||
			key == 'W')
		{
			bDebug = !bDebug;
			ofLogNotice("ofxWindowApp") << (__FUNCTION__) << "changed draw debug: " << (bDebug ? "ON" : "OFF");
		}

		//switch window mode
		else if (key == 'F')
		{
			refreshTogleWindowMode();
		}
		else if (key == 'V')//switch Vsync mode
		{
			vSync = !vSync;
			ofSetVerticalSync(vSync);
		}


#ifdef USE_MINI_WINDOW
		else if (key == 'M' && mod_ALT)//switch window mode big/mini
			//else if (key == 'M')//switch window mode big/mini
			//else if (key == 'M' && mod_CONTROL)//switch window mode big/mini
		{
			toggleModeWindowBigMini();
		}
#endif

		else if (key == 'L' && mod_ALT)//toggle lock
		{
			bLock = !bLock;
		}
		else if (key == 'R' && mod_ALT)//reset to full HD
		{
			BigWindow.setPosition(glm::vec2(0, SIZE_SECURE_GAP_INISDE_SCREEN));
			BigWindow.setSize(1920, 1080);

			ofSetWindowPosition(BigWindow.getPosition().x, BigWindow.getPosition().y);
			ofSetWindowShape(BigWindow.getWidth(), BigWindow.getHeight());
		}

		else if (key == OF_KEY_BACKSPACE && mod_CONTROL) {
			doReset();
		}
	}
}
//--------------------------------------------------------------
void ofxWindowApp::keyReleased(ofKeyEventArgs& eventArgs)
{
	if (bKeys)
	{
		const int& key = eventArgs.key;

		// modifier
		mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);//macOS
		mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);//Windows. not working
		mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	}
}

////--------------------------------------------------------------
//void ofxWindowApp::dragEvent(ofDragInfo info) {
//}

//--------------------------------------------------------------
void ofxWindowApp::folderCheckAndCreate(string _path)
{
	ofLogNotice("ofxWindowApp") << (__FUNCTION__);

	ofDirectory dataDirectory(ofToDataPath(_path, true));

	// check if target data folder exist
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

//--------------------------------------------------------------
void ofxWindowApp::refreshTogleWindowMode()
{
	if (ofGetWindowMode() == OF_WINDOW) // go full screen
	{
		ofSetFullscreen(true);

		////TODO. not works
		////workaround to force windows correct full screen place...
		//float _offset = 500;
		//window_Y = ofGetWindowPositionY();
		//window_X = ofGetWindowPositionX() + _offset;
		//ofSetWindowPosition(window_X, window_Y);
		//window_X -= _offset;
		////ofSetWindowPosition(window_X, window_Y);
		//////workaround
		////window_X = ofGetWindowPositionX();
		////window_Y = 0;//align to top border
		////ofSetWindowPosition(window_X, window_Y);

		bigFullScreen = true;
	}
	else if (ofGetWindowMode() == OF_FULLSCREEN) // go window mode
	{
		ofSetFullscreen(false);

		//TODO:
		// rare behavior with black screen in secondary monitors..

		// workaround:
		// to fit window and his bar visible into the screen
		float windowBar_h = 25;

		// workaround
		// it's window mode..
		// kick a little down to avoid hidden window title barF
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

		bigFullScreen = false;
	}

	//update
	windowResized(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofxWindowApp::applyMode()
{
	ofLogVerbose("ofxWindowApp") << (__FUNCTION__);

	//apply extra
	ofSetVerticalSync(vSync);
	ofSetFrameRate(targetFps);

	//-

	// mini preset

#ifdef USE_MINI_WINDOW
	if (bModeMini)
	{
		ofSetWindowPosition(MiniWindow.getPosition().x, MiniWindow.getPosition().y);
		ofSetWindowShape(MiniWindow.getWidth(), MiniWindow.getHeight());
		ofSetFullscreen(false);
	}

	// big preset
	else
#endif
	{
		//ofSetFullscreen(bigFullScreen);

		if (bigFullScreen) {
			ofSetWindowPosition(0, 0);
			ofSetWindowShape(ofGetWidth(), ofGetHeight());
		}
		else {
			ofSetWindowPosition(BigWindow.getPosition().x, BigWindow.getPosition().y);
			ofSetWindowShape(BigWindow.getWidth(), BigWindow.getHeight());
		}

		//ofSetFullscreen(false);
		ofSetFullscreen(bigFullScreen);

		////TODO:
		//if (BigWindow.windowMode == ofWindowMode(OF_FULLSCREEN)) ofSetFullscreen(true);
		//if (windowBigMode == ("OF_FULLSCREEN")) ofSetFullscreen(true);
	}

	//-

	//// apply
	//if (bModeMini) {
	//	ofx::Serializer::ApplyWindowSettings(BigWindow);
	//}
	//else {
	//	ofx::Serializer::ApplyWindowSettings(MiniWindow);
	//}

	//-

	//if (!bModeMini) {
	//	ofSetFullscreen(false);
	//	ofSetFullscreen(bigFullScreen);
	//	if (bigFullScreen) ofSetWindowPosition(0, 0);
	//}
}