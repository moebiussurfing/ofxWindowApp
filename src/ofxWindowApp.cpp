#include "ofxWindowApp.h"

#ifdef SURFING_WINDOW_APP__USE_STATIC
#if defined(TARGET_WIN32)			

ofxWindowApp* ofxWindowApp::instance = nullptr; // Initialize the static pointer

//--------------------------------------------------------------
void ofxWindowApp::setInstance(ofxWindowApp* app)
{
	instance = app; // Set the instance
}

//--

//--------------------------------------------------------------
void ofxWindowApp::windowMoved(GLFWwindow* window, int xpos, int ypos)
{
	// Ignore if not pos changed
	static int xpos_ = -1;
	static int ypos_ = -1;
	bool bChanged = 0;
	if (xpos != xpos_) {
		xpos_ = xpos;
		bChanged = 1;
	}
	if (ypos != ypos_) {
		ypos_ = ypos;
		bChanged = 1;
	}
	if (!bChanged) return;

	//--

	if (instance)
	{
		if (!instance->bDoneSetup) return;
		if (!instance->bDoneStartup) return;

		instance->window_X = xpos;
		instance->window_Y = ypos;

		instance->doRefreshGetWindowSettings();

		ofLogVerbose("ofxWindowApp::windowMoved") << ofToString(xpos) << ", " << ofToString(ypos);

		instance->bChangedWindowEasyCallback = true;

		instance->bFlagSave = true;

#ifdef SURFING_WINDOW_APP__USE_TIMED_SAVER
		instance->timeWhenToSaveFlag = ofGetElapsedTimef() + 0.5f;
#endif
	}
}
#endif
#endif

//--------------------------------------------------------------
ofxWindowApp::ofxWindowApp()
{
	ofSetLogLevel("ofxWindowApp", OF_LOG_NOTICE);

	// Default
	int _h = OFX_WINDOW_APP_BAR_HEIGHT;//bar height
	BigWindow.setPosition(glm::vec2(0, _h));//force
	BigWindow.setSize(1920, 1080 - _h);//force
	BigWindow.windowMode = ofGetCurrentWindow()->getWindowMode();//from main.cpp 

#ifdef USE_MINI_WINDOW
	MiniWindow.setPosition(glm::vec2(20, 20));
	MiniWindow.setSize(200, 200);
	MiniWindow.windowMode = ofGetCurrentWindow()->getWindowMode();
#endif

	setFrameRate(60);
	setVerticalSync(false);
	setShowDebug(false);
}

//--------------------------------------------------------------
ofxWindowApp::~ofxWindowApp()
{
	ofLogNotice("ofxWindowApp") << "Destructor";

	ofRemoveListener(ofEvents().update, this, &ofxWindowApp::update);
	ofRemoveListener(ofEvents().draw, this, &ofxWindowApp::draw);
	ofRemoveListener(ofEvents().keyPressed, this, &ofxWindowApp::keyPressed);
	ofRemoveListener(ofEvents().keyReleased, this, &ofxWindowApp::keyReleased);
	ofRemoveListener(ofEvents().windowResized, this, &ofxWindowApp::windowResized);

	ofRemoveListener(params.parameterChangedE(), this, &ofxWindowApp::Changed_Params);

	//exit();//removed to avoid possible exceptions
}

//--------------------------------------------------------------
void ofxWindowApp::startup()
{
	if (bDoneStartup) return;

	ofLogNotice("ofxWindowApp::startup()");
	ofLogNotice("ofxWindowApp::startup()") << " at frame num: " << ofGetFrameNum();

	//--

	// Load
	if (bAutoSaveLoad) loadSettings();

	//--

	// Fix workaround
	if (bIsFullScreenInSettings)
	{
		ofSetFullscreen(true);

		//TODO: full screen on different monitors make troubles to re load..
		//ofSetWindowPosition(BigWindow.getPosition().x, BigWindow.getPosition().y);
		//ofSetWindowShape(ofGetWidth(), ofGetHeight());
	}

	//--

	// On top
	if (!bIsFullScreenInSettings)
	{
		// Workaround
		// Refresh
#if defined(TARGET_WIN32)			
		HWND W = GetActiveWindow();
		SetWindowPos(W, HWND_NOTOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
#endif
		// Re trig
		bOnTop = bOnTop;
	}

	//--

	bDoneStartup = true;
}

//--------------------------------------------------------------
void ofxWindowApp::exit()
{
	ofLogNotice("ofxWindowApp::exit()");

#ifdef SURFING_IMGUI__ENABLE_SAVE_ON_EXIT
	if (bAutoSaveLoad && !bDisableAutoSave)
	{
		save();
	}
#endif
}

//--------------------------------------------------------------
void ofxWindowApp::setup()
{
	if (bDoneSetup) return;

	ofLogNotice("ofxWindowApp::setup()");

	//--

#ifdef SURFING_WINDOW_APP__USE_STATIC
	GLFWwindow* glfwWindow = glfwGetCurrentContext();
	glfwSetWindowPosCallback(glfwWindow, windowMoved);
#endif

	//--

	// Default folders:
	// data/ofxWindowApp/ofxWindowApp.json
	path_folder = "ofxWindowApp";
	path_filename = "ofxWindowApp.json";

	//--

#ifdef USE_CUSTOM_FONT
	// Font
	fontSize = 10;
	string _path = "assets/fonts/"; // assets folder
	string _f = "JetBrainsMono-Bold.ttf";
	_path += _f;
	bool b = font.load(_path, fontSize);
	if (!b) font.load(OF_TTF_MONO, fontSize);
#endif

	//--

	// Callbacks to auto call update/draw/keyPressed
	ofAddListener(ofEvents().update, this, &ofxWindowApp::update);
	ofAddListener(ofEvents().draw, this, &ofxWindowApp::draw, OF_EVENT_ORDER_AFTER_APP);
	ofAddListener(ofEvents().keyPressed, this, &ofxWindowApp::keyPressed);
	ofAddListener(ofEvents().keyReleased, this, &ofxWindowApp::keyReleased);
	ofAddListener(ofEvents().windowResized, this, &ofxWindowApp::windowResized);

	//--

	// Extra settings
	paramsWindow.add(vSync);
	paramsWindow.add(fpsTarget);
	paramsSession.add(bDebug);
	paramsSession.add(bShowPerformanceAlways);
	paramsSession.add(bOnTop);
	paramsSession.add(bDisableAutoSave);
	params.add(paramsWindow);
	params.add(paramsSession);

	// Mini settings
#ifdef USE_MINI_WINDOW
	//params.add(vSync);
	//params.add(fpsTarget);
	//params.add(bDebug);
	//params.add(bShowPerformanceAlways);
	//params.add(bModeMini);
	params.add(bModeMini);
#endif

	ofAddListener(params.parameterChangedE(), this, &ofxWindowApp::Changed_Params); // setup()

	// Default
	setShowPerformanceAllways(true);

#ifdef USE_MINI_WINDOW
	if (!bModeMini)
	{
		if (bIsFullScreen)
		{
			ofSetFullscreen(bIsFullScreen);
			ofSetWindowPosition(0, 0);
			ofSetWindowShape(ofGetWidth(), ofGetHeight());
		}
	}
#endif

	//--

	bDoneSetup = true;
}

//--------------------------------------------------------------
void ofxWindowApp::update(ofEventArgs& args)
{
	// Auto call setup on first frame.
	if (!bDoneSetup) {
		if (ofGetFrameNum() >= 0)
		{
			setup();
		}
	}
	// Auto call startup but after setup is done.
	if (bDoneSetup) {
		if (!bDoneStartup) {
			if (ofGetFrameNum() > 0)
			{
				// Workaround
				startup();
			}
		}
	}

	//--

	// Check if flagged to save
	if (bFlagSave)
	{
#ifdef SURFING_WINDOW_APP__USE_TIMED_SAVER
		if (ofGetElapsedTimef() >= timeWhenToSaveFlag)
#endif
		{
			bFlagSave = 0;

			bChangedWindowEasyCallback = true;

			ofLogNotice("ofxWindowApp::Changed") << "Just saved after window changed";

			saveSettings();
		}
	}

	//--

#ifdef SURFING_WINDOW_APP__USE_TIMED_SAVER
	// Auto saver timer
	// is no required to resize the window or to close the app window to save.
	// then the app can crash an window shape will be stored 1 time each 10 seconds by default.
	if (bAutoSaverTimed && !bDisableAutoSave)
	{
		auto t = ofGetElapsedTimeMillis() - timeLastAutoSaveCheck;
		if (t > timePeriodToCheckIfSave)
		{
			bool bModeSaveOnlyIfWindowMoved = 1;
			if (bModeSaveOnlyIfWindowMoved) {
				bool bHasBeenMoved = 0;
				static int posx;
				static int posy;
				if (posx != ofGetWindowPositionX()) {
					posx = ofGetWindowPositionX();
					bHasBeenMoved = 1;
				}
				if (posy != ofGetWindowPositionY()) {
					posy = ofGetWindowPositionY();
					bHasBeenMoved = 1;
				}
				if (bHasBeenMoved) saveSettings(true);
			}
			else {
				saveSettings(true);
			}

			timeLastAutoSaveCheck = ofGetElapsedTimeMillis();
		}
	}
#endif
}

//--------------------------------------------------------------
void ofxWindowApp::draw(ofEventArgs& args)
{
	fpsReal = ofGetFrameRate();

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
		drawHelpInfo();
		drawPerformanceWidget();
	}
	else
	{
		if (bShowPerformanceAlways) drawPerformanceWidget();
	}
}

//--------------------------------------------------------------
void ofxWindowApp::doRefreshGetWindowSettings()
{
	ofLogVerbose("ofxWindowApp") << "doRefreshGetWindowSettings()";

#ifndef USE_MINI_WINDOW
	// Big
	BigWindow.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
	BigWindow.setSize(ofGetWindowSize().x, ofGetWindowSize().y);
	BigWindow.windowMode = ofGetCurrentWindow()->getWindowMode();

	if (BigWindow.windowMode == ofWindowMode(0)) bIsFullScreen = false;
	else if (BigWindow.windowMode == ofWindowMode(1)) bIsFullScreen = true;
	else if (BigWindow.windowMode == ofWindowMode(2)) bIsFullScreen = false;
#else
	if (!bModeMini)
	{
		// Big
		BigWindow.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
		BigWindow.setSize(ofGetWindowSize().x, ofGetWindowSize().y);
		BigWindow.windowMode = ofGetCurrentWindow()->getWindowMode();

		// ?
		if (BigWindow.windowMode == ofWindowMode(0)) bIsFullScreen = false;
		else if (BigWindow.windowMode == ofWindowMode(1)) bIsFullScreen = true;
		else if (BigWindow.windowMode == ofWindowMode(2)) bIsFullScreen = false;
	}
	else
	{
		// Mini
		MiniWindow.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
		MiniWindow.setSize(ofGetWindowSize().x, ofGetWindowSize().y);
		MiniWindow.windowMode = ofGetCurrentWindow()->getWindowMode();//ignored
	}
#endif
}

//--------------------------------------------------------------
void ofxWindowApp::save()
{
	saveSettingsAfterRefresh();
}
//--------------------------------------------------------------
void ofxWindowApp::saveSettingsAfterRefresh()
{
	doRefreshGetWindowSettings();
	saveSettings();
}
//--------------------------------------------------------------
void ofxWindowApp::saveSettings(bool bSlient)
{
	string path_ = path_folder + "/" + path_filename;
	ofLogNotice("ofxWindowApp::saveSettings") << path_;

	// Force mini to window, not full screen

#ifdef USE_MINI_WINDOW
	MiniWindow.windowMode = ofWindowMode(OF_WINDOW);
#endif

	//--

	// Save window settings

	ofJson jApp;
	ofJson jMini;
	ofJson jExtra;

	to_json(jApp, BigWindow);

#ifdef USE_MINI_WINDOW
	to_json(jMini, MiniWindow);
#endif

	ofSerialize(jExtra, params);

#ifdef USE_MINI_WINDOW
	//jApp["Preset"] = "Big";
	jMini["Preset"] = "Mini";
#endif

	//--

	// A. Using 2 files

#ifdef USE_MINI_WINDOW
	//ofSavePrettyJson(path_folder + "/"+path_filename, j);
	//TODO:
	// We can't get frame rate and v sync mode from window app.
	// should be settled by hand
	// extra settings could be mixed in one json only for both
	// TEST:
	//ofSavePrettyJson(path_folder + "/"+path_filename2, jMini);
#endif

	//--

	// B. Settings in one file

	ofJson data;
	data.push_back(jApp);

#ifdef USE_MINI_WINDOW
	data.push_back(jMini);
#endif

	data.push_back(jExtra);

	// Check if we need to create data folder first
	folderCheckAndCreate(path_folder);

	// Save file
	if (!bSlient) ofLogNotice("ofxWindowApp") << data.dump(4);
	ofSavePrettyJson(path_, data);

	//--

	window_W = ofGetWindowSize().x;
	window_H = ofGetWindowSize().y;

	//--

	bFlagDoneSaved = true;
}

//--------------------------------------------------------------
void ofxWindowApp::loadSettings()
{
	ofSetFullscreen(false);

	string path_ = path_folder + "/" + path_filename;

	//TODO: Check if file exist
	ofFile file(path_);
	bool bFileExist = file.exists();
	if (bFileExist)
	{
		ofLogNotice("ofxWindowApp") << "loadFileSettings(): File found: " << path_;

		//--

		// Load settings

#ifdef USE_MINI_WINDOW
		// A. using 2 files

		//ofJson j = ofLoadJson(path_folder + "/" + path_filename);
		//ofx::Serializer::ApplyWindowSettings(j);
		//// Extra settings could be mixed in one json only for both
		//// TEST:
		//ofJson jMini;
		//jMini = ofLoadJson(path_folder + "/"+path_filename2);
		//ofLogVerbose("ofxWindowApp")<<(__FUNCTION__) << "json: " << jMini;
		//ofDeserialize(jMini, params);
#endif

		//--

		// B. Settings in one file

		ofJson data;
		data = ofLoadJson(path_);
		ofLogNotice("ofxWindowApp") << "File JSON: " << data.dump(4);

		ofJson jBig;

#ifdef USE_MINI_WINDOW
		ofJson jMini;
		ofJson jExtra;

		if (data.size() >= 3)
		{
			jBig = data[0];//TODO: ugly workaround
			jMini = data[1];//TODO: should add a tag mini or extra to mark the file section
			jExtra = data[2];

			// recall both params groups
			ofDeserialize(jExtra, params);
		}
		else ofLogError("ofxWindowApp") << "ERROR on data[] size = " << ofToString(data.size());

		ofLogVerbose("ofxWindowApp") << "jBig  : " << jBig;
		ofLogVerbose("ofxWindowApp") << "jMini : " << jMini;
		ofLogVerbose("ofxWindowApp") << "jExtra: " << jExtra;
#endif

		//--

#ifndef USE_MINI_WINDOW
		ofJson jExtra;

		if (data.size() >= 2)
		{
			jBig = data[0]; //TODO: Ugly workaround
			jExtra = data[1];

			// Recall both params groups
			ofDeserialize(jExtra, params);

			ofLogNotice("ofxWindowApp") << "Settings: " << jBig.dump(4);
			ofLogNotice("ofxWindowApp") << "Extras: " << ofToString(params);
		}
		else
		{
			ofLogError("ofxWindowApp") << "ERROR on data[] size = " << ofToString(data.size());
		}

		ofLogVerbose("ofxWindowApp") << "Window: " << jBig;
		ofLogVerbose("ofxWindowApp") << "Extras: " << jExtra;
#endif

		//--

		int jx, jy, jw, jh;
		string jm;

#ifdef USE_MINI_WINDOW
		// Mini
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
		MiniWindow.windowMode = ofWindowMode(0);// ? always windowed

		if (jm == "OF_WINDOW") MiniWindow.windowMode = ofWindowMode(0);
		else if (jm == "OF_FULLSCREEN") MiniWindow.windowMode = ofWindowMode(1);
		else if (jm == "OF_GAME_MODE") MiniWindow.windowMode = ofWindowMode(2);
#endif

		//--

		// Big
		jx = jBig["position"]["x"];
		jy = jBig["position"]["y"];
		jw = jBig["size"]["width"];
		jh = jBig["size"]["height"];
		jm = ofToString(jBig["window_mode"]);

		// Remove from name the added \" by the serializer
		ofStringReplace(jm, "\"", "");

		ofLogVerbose("ofxWindowApp") << "jx: " << jx;
		ofLogVerbose("ofxWindowApp") << "jy: " << jy;
		ofLogVerbose("ofxWindowApp") << "jw: " << jw;
		ofLogVerbose("ofxWindowApp") << "jh: " << jh;
		ofLogVerbose("ofxWindowApp") << "jm: " << jm;

		// TODO:
		// Workaround
		if (jy < SIZE_SECURE_GAP_INISDE_SCREEN)
		{
			jy = (int)SIZE_SECURE_GAP_INISDE_SCREEN;
		}

		// Screen modes
		//OF_WINDOW = 0
		//OF_FULLSCREEN = 1
		//OF_GAME_MODE = 2

		if (jm == "OF_WINDOW") BigWindow.windowMode = ofWindowMode(0);
		else if (jm == "OF_FULLSCREEN") BigWindow.windowMode = ofWindowMode(1);
		else if (jm == "OF_GAME_MODE") BigWindow.windowMode = ofWindowMode(2);

		if (jm == "OF_WINDOW") bIsFullScreen = false;
		else if (jm == "OF_FULLSCREEN") bIsFullScreen = true;
		else if (jm == "OF_GAME_MODE") bIsFullScreen = false;
		bIsFullScreenInSettings = bIsFullScreen;

		if (!bIsFullScreen) {//window
			BigWindow.setPosition(glm::vec2(jx, jy));
			BigWindow.setSize(jw, jh);
		}
		else {//full screen
			//BigWindow.setPosition(glm::vec2(0, 0));
			BigWindow.setPosition(glm::vec2(jx, jy));
			BigWindow.setSize(jw, jh);
			//BigWindow.setSize(ofGetWidth(), ofGetHeight);
		}

		ofLogNotice("ofxWindowApp") << "WindowMode:" << ofToString(BigWindow.windowMode);
		ofLogNotice("ofxWindowApp") << "OF_WINDOW/OF_FULLSCREEN/OF_GAME_MODE";
		ofLogNotice("ofxWindowApp") << "Position: " << ofToString(BigWindow.getPosition());
		ofLogNotice("ofxWindowApp") << "Width: " << ofToString(BigWindow.getWidth());
		ofLogNotice("ofxWindowApp") << "Height: " << ofToString(BigWindow.getHeight());
		ofLogNotice("ofxWindowApp") << "Done load settings.";
	}
	else
	{
		ofLogError("ofxWindowApp") << "File NOT found: " << path_;
	}

	//--

	// Apply
	doApplyWindowMode();
	doApplyExtraSettings();
}

//--------------------------------------------------------------
void ofxWindowApp::drawHelpInfo()
{
	// Debug overlay screen modes

	string vSyncStr;
	string fpsRealStr;
	string fpsTargetStr;
	string strPad = "  "; //add spaces
	string str;
	string screenStr = "";
	string screenPosStr = "";
	string screenMode = "";

	screenStr = ofToString(window_W) + "x" + ofToString(window_H);
	screenStr += "px";
	vSyncStr = ((vSync ? "ON" : "OFF"));
	vSyncStr += ((vSync ? "[V] " : "[V]"));
	fpsRealStr = ofToString(fpsReal, 0);
	fpsTargetStr = ofToString(fpsTarget);
	screenPosStr = " " + ofToString(ofGetWindowPositionX()) + "," + ofToString(ofGetWindowPositionY());

	bool bMode;
	if (ofGetWindowMode() == OF_WINDOW) // Go full screen
	{
		bMode = false;
	}
	else if (ofGetWindowMode() == OF_FULLSCREEN) // Go window mode
	{
		bMode = true;
	}
	screenMode += bMode ? "FULLSCREEN_MODE" : "WINDOW_MODE";
	screenMode += "[F]";

#ifdef USE_MINI_WINDOW
	screenMode += bModeMini ? " [MINI]" : " [BIG]";
#endif

	str += "FPS " + fpsRealStr;
	str += "[" + fpsTargetStr + "]";
	str += strPad + "VSYNC-" + vSyncStr;
	str += strPad + "SIZE " + screenStr;
	str += strPad + "POSITION" + screenPosStr;
	str += strPad + screenMode;
	str += strPad + (bDisableAutoSave ? "LOCKED-ON" : "LOCKED-OFF");
	str += (bDisableAutoSave ? "[L] " : "[L]");
	str += strPad + (bOnTop ? "ON-TOP:TRUE" : "ON-TOP:FALSE") + "[T]";
	str += strPad + "  ";
	str += strPad + "[MOD ALT] ";//TODO: show mod key. hardcoded
	str += strPad + (mod_COMMAND ? "CMD" : "   ");
	str += strPad + (mod_ALT ? "ALT" : "   ");
	str += strPad + (mod_CONTROL ? "CTRL" : "    ");
	str += strPad + (mod_SHIFT ? "SHIFT" : "     ");

	//--

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
void ofxWindowApp::drawPerformanceWidget()
{
	// monitor fps performance alert

	bool bPreShow;//starts draw black
	bool bAlert;//draws red
	float fpsThreshold;

	//// A. Thresholds by factor fps between target and real fps
	//fpsThreshold = 0.9f;//below this trigs alert red state
	//bPreShow = (fpsReal < fpsTarget*0.999);//by ratio
	//bAlert = (fpsReal < fpsTarget*fpsThreshold);//A. by ratio

	// B. Thresholds by absolute fps difference between desired target and real fps
	// monitor fps performance
	fpsThreshold = 10.f;//num frames below this trigs alert red state
	bPreShow = (fpsReal < fpsTarget - 5); //below 5 fps starts showing bar
	bAlert = (fpsReal < (fpsTarget - fpsThreshold));//B. by absolute fps diff

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

		fw = ofMap(fpsReal, 0.0f * fpsTarget, fpsTarget, 0, fwMax, true);
		int fa = 150;//alpha
		int iDiff = (int)fpsTarget - fpsReal;
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
		fh = bb.getHeight() - 2;
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
	window_W = w;
	window_H = h;
	window_X = ofGetWindowPositionX();
	window_Y = ofGetWindowPositionY();

	doRefreshGetWindowSettings();

	bFlagSave = 1;

	ofLogVerbose("ofxWindowApp::windowResized") << ofToString(w) << ", " << ofToString(h);

	bChangedWindowEasyCallback = true;

#ifdef SURFING_WINDOW_APP__USE_TIMED_SAVER
	timeWhenToSaveFlag = ofGetElapsedTimef() + 0.5f;
#endif
}

//--------------------------------------------------------------
void ofxWindowApp::windowResized(ofResizeEventArgs& resize)
{
	static int w = -1;
	static int h = -1;
	bool bChanged = 0;
	if (w != resize.width) {
		bChanged = 1;
		w = resize.width;
	}
	if (h != resize.height) {
		bChanged = 1;
		h = resize.height;
	}
	if (bChanged) windowResized(w, h);
}

//--------------------------------------------------------------
void ofxWindowApp::keyPressed(ofKeyEventArgs& eventArgs)
{
	if (bKeys)
	{
		const int& key = eventArgs.key;

		// modifier

		////TODO:
		//mod_COMMAND = key == OF_KEY_COMMAND; // macOS
		//mod_CONTROL = key == OF_KEY_CONTROL; // Windows. not working
		//mod_ALT = key == OF_KEY_ALT;
		//mod_SHIFT = key == OF_KEY_SHIFT;

		mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);//macOS
		mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);//Windows. not working
		mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
		mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

		// Pick and enable your preferred key modifier to allow the key commands
		// *or to ignore keys when un pressed)
		// global required modifier
		//if (!mod_COMMAND) return;
		//if (!mod_CONTROL) return;
		if (!mod_ALT) return;

		//--

		if (0) ofLogNotice("ofxWindowApp::keyPressed") << "'" << (char)key << "' [" << key << "]";

		// disable draw debug
		//if (mod_CONTROL && key == 'w' || 
		//mod_CONTROL && key == 'w' ||
		if (key == 'W')
		{
			bDebug = !bDebug;
			ofLogNotice("ofxWindowApp") << "changed draw debug: " << (bDebug ? "ON" : "OFF");
		}

		// Switch window mode
		else if (key == 'F')
		{
			//TODO; fix monitor jump when fullscreen.
			// try settings.windowMode = OF_GAME_MODE;
			//auto m = ofGetWindowMode();
			//ofSetWindow
			//ofSetupScreen()

			doRefreshToggleWindowMode();
		}
		else if (key == 'V') // switch v-sync mode
		{
			vSync = !vSync;
			ofSetVerticalSync(vSync);
		}

		else if (key == 'L') // toggle lock
		{
			bDisableAutoSave = !bDisableAutoSave;
		}
		//#ifdef SURFING_WINDOW_APP__USE_FULLHD_COMMAND
		//		else if (key == 'R') // reset to full HD
		//		{
		//			//setToggleAlwaysOnTop();
		//			//BigWindow.setPosition(glm::vec2(0, SIZE_SECURE_GAP_INISDE_SCREEN));
		//			BigWindow.setPosition(glm::vec2(0, 0));
		//			BigWindow.setSize(1920, 1080);
		//
		//			ofSetWindowPosition(BigWindow.getPosition().x, BigWindow.getPosition().y);
		//			ofSetWindowShape(BigWindow.getWidth(), BigWindow.getHeight());
		//
		//			doRefreshGetWindowSettings();
		//		}
		//#endif

		else if (key == OF_KEY_BACKSPACE) {
			doReset();
		}

		else if (key == 'L')
		{
			bDisableAutoSave = !bDisableAutoSave;
		}

		else if (key == 'T')
		{
			setToggleAlwaysOnTop();
		}

#ifdef USE_MINI_WINDOW
		else if (key == 'M')//switch window mode big/mini
			//else if (key == 'M')//switch window mode big/mini
			//else if (key == 'M' && mod_CONTROL)//switch window mode big/mini
		{
			toggleModeWindowBigMini();
		}
#endif
		//--

		////TODO:
		////Release modifiers
		//mod_COMMAND = false; // macOS
		//mod_CONTROL = false; // Windows. not working
		//mod_ALT = false;
		//mod_SHIFT = false;
	}
}
//--------------------------------------------------------------
void ofxWindowApp::keyReleased(ofKeyEventArgs& eventArgs)
{
	if (bKeys)
	{
		const int& key = eventArgs.key;

		//// modifier
		//if (eventArgs.hasModifier(OF_KEY_COMMAND)) mod_COMMAND = false; // macOS
		//if (eventArgs.hasModifier(OF_KEY_CONTROL)) mod_CONTROL = false; // Windows. not working
		//if (eventArgs.hasModifier(OF_KEY_ALT)) mod_ALT = false;
		//if (eventArgs.hasModifier(OF_KEY_SHIFT)) mod_SHIFT = false;

		//Release modifiers
		mod_COMMAND = key == OF_KEY_COMMAND; // macOS
		mod_CONTROL = key == OF_KEY_CONTROL; // Windows. not working
		mod_ALT = key == OF_KEY_ALT;
		mod_SHIFT = key == OF_KEY_SHIFT;
	}
}

//--------------------------------------------------------------
void ofxWindowApp::folderCheckAndCreate(string _path)
{
	ofLogNotice("ofxWindowApp") << "folderCheckAndCreate(). Path: " << _path;

	ofDirectory dataDirectory(ofToDataPath(_path, true));

	// Check if target data folder exist
	if (!dataDirectory.isDirectory())
	{
		ofLogError("ofxWindowApp") << "folderCheckAndCreate(). FOLDER DOES NOT EXIST!";

		// Create folder
		bool b = dataDirectory.createDirectory(ofToDataPath(_path, true));

		// Debug if creation has been succed
		if (b) ofLogNotice("ofxWindowApp") << "FOLDER '" << _path << "' CREATED SUCCESSFULLY!";
		else ofLogError("ofxWindowApp") << "UNABLE TO CREATE '" << _path << "' FOLDER!";
	}
}

//--------------------------------------------------------------
void ofxWindowApp::doRefreshToggleWindowMode()
{
	// Toggle Mode

	if (ofGetWindowMode() == OF_WINDOW) // go full screen
	{
		ofSetFullscreen(true);
		bIsFullScreen = true;
	}
	else if (ofGetWindowMode() == OF_FULLSCREEN) // go window mode
	{
		ofSetFullscreen(false);

		// Workaround:
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

		bIsFullScreen = false;
	}

	// update
	windowResized(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofxWindowApp::doApplyExtraSettings()
{
	ofLogVerbose("ofxWindowApp") << "doApplyExtraSettings()";

	ofLogVerbose("ofxWindowApp") << "FpsTarget: " << fpsTarget;
	ofLogVerbose("ofxWindowApp") << "vSync: " << vSync;

	//ofLogVerbose("ofxWindowApp") << "Show Debug: " << bDebug.get();
	//ofLogVerbose("ofxWindowApp") << "bDisableAutoSave: " << bDisableAutoSave.get();

#ifdef USE_MINI_WINDOW
	ofLogVerbose("ofxWindowApp") << "bModeMini  : " << bModeMini.get();
#endif

	ofSetFrameRate(int(fpsTarget.get()));
	ofSetVerticalSync(vSync.get());
}

//--------------------------------------------------------------
void ofxWindowApp::doApplyWindowMode()
{
	ofLogVerbose("ofxWindowApp") << "doApplyWindowMode()";

	//// Apply extra
	//ofSetVerticalSync(vSync);
	//ofSetFrameRate(fpsTarget);

	//--

#ifndef USE_MINI_WINDOW
	if (bIsFullScreen) {//fullscreen
		//ofSetWindowPosition(0, 0);
		ofSetWindowPosition(BigWindow.getPosition().x, BigWindow.getPosition().y);
		ofSetWindowShape(ofGetWidth(), ofGetHeight());
	}
	else {//window
		ofSetWindowPosition(BigWindow.getPosition().x, BigWindow.getPosition().y);
		ofSetWindowShape(BigWindow.getWidth(), BigWindow.getHeight());
	}

	if (bIsFullScreen) ofSetFullscreen(true);
#endif

#ifdef USE_MINI_WINDOW
	// Mini preset
	if (bModeMini)
	{
		ofSetWindowPosition(MiniWindow.getPosition().x, MiniWindow.getPosition().y);
		ofSetWindowShape(MiniWindow.getWidth(), MiniWindow.getHeight());
		ofSetFullscreen(false);
	}

	// Big preset
	else
	{
		//ofSetFullscreen(bIsFullScreen);

		if (bIsFullScreen) {
			ofSetWindowPosition(0, 0);
			ofSetWindowShape(ofGetWidth(), ofGetHeight());
		}
		else {
			ofSetWindowPosition(BigWindow.getPosition().x, BigWindow.getPosition().y);
			ofSetWindowShape(BigWindow.getWidth(), BigWindow.getHeight());
		}

		//ofSetFullscreen(false);
		ofSetFullscreen(bIsFullScreen);

		////TODO:
		//if (BigWindow.windowMode == ofWindowMode(OF_FULLSCREEN)) ofSetFullscreen(true);
		//if (windowBigMode == ("OF_FULLSCREEN")) ofSetFullscreen(true);
	}

	//--

	//// Apply
	//if (bModeMini) {
	//	ofx::Serializer::ApplyWindowSettings(BigWindow);
	//}
	//else {
	//	ofx::Serializer::ApplyWindowSettings(MiniWindow);
	//}

	//--

	//if (!bModeMini) {
	//	ofSetFullscreen(false);
	//	ofSetFullscreen(bIsFullScreen);
	//	if (bIsFullScreen) ofSetWindowPosition(0, 0);
	//}
#endif
}

//--------------------------------------------------------------
void ofxWindowApp::Changed_Params(ofAbstractParameter& e)
{
	if (!bDoneSetup) return;
	//if (!bDoneStartup) return;

	//--

	string name = e.getName();
	ofLogNotice("ofxWindowApp::Changed") << " " << name << " : " << e;

	if (name == bOnTop.getName())
	{
#if defined(TARGET_WIN32)
		if (bOnTop.get())
		{
			// Make app always on top
			HWND W = GetActiveWindow();
			SetWindowPos(W, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
			ofLogNotice("ofxWindowApp") << "Set on top Enabled";

		}
		else {
			// Disable make app always on top
			HWND W = GetActiveWindow();
			SetWindowPos(W, HWND_NOTOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
			ofLogNotice("ofxWindowApp") << "Set on top Disabled";
		}
		isOnTop = bOnTop;
#else
		ofLogError("ofxWindowApp") << "Not implemented for current platform. Only TARGET_WIN32 yet!";
#endif
	}
}