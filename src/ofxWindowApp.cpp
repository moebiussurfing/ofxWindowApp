#include "ofxWindowApp.h"

//--------------------------------------------------------------
ofxWindowApp::ofxWindowApp() {
	ofSetLogLevel("ofxWindowApp", OF_LOG_NOTICE);

	ofLogVerbose("ofxWindowApp:ofxWindowApp()") << "Constructor. FrameNum: " << ofGetFrameNum();

	doResetWindowExtraSettings();
	doResetWindowSettings();

	bDisableCallback_windowMovedOrResized = true; // Apply without saving json settings
	doApplyWindowSettings();
	doApplyWindowExtraSettings();
	bDisableCallback_windowMovedOrResized = false;
}

//--------------------------------------------------------------
ofxWindowApp::~ofxWindowApp() {
	ofLogNotice("ofxWindowApp:~ofxWindowApp()") << "Destructor";

	ofRemoveListener(ofEvents().update, this, &ofxWindowApp::update);
	ofRemoveListener(ofEvents().draw, this, &ofxWindowApp::draw);
	ofRemoveListener(ofEvents().keyPressed, this, &ofxWindowApp::keyPressed);
	ofRemoveListener(ofEvents().keyReleased, this, &ofxWindowApp::keyReleased);
	ofRemoveListener(ofEvents().windowResized, this, &ofxWindowApp::windowResized);

#ifdef OFX_WINDOW_APP__USE_STATIC
	GLFWwindow * glfwWindow = glfwGetCurrentContext();
	if (glfwWindow) {
		glfwSetWindowPosCallback(glfwWindow, nullptr);
	}
#endif

	ofRemoveListener(paramsExtra.parameterChangedE(), this, &ofxWindowApp::ChangedParamsExtra);
}

//--------------------------------------------------------------
void ofxWindowApp::exit() {
	ofLogNotice("ofxWindowApp:exit()");

#ifdef OFX_WINDOW_APP__ENABLE_SAVE_ON_EXIT
	if (!bDisableAutoSave) {
		save();
	}
#endif
}

//----

//--------------------------------------------------------------
void ofxWindowApp::setup() {
	ofLogVerbose("ofxWindowApp:setup()") << "----------------------setup() <--BEGIN";

	if (bDoneSetup) {
		ofLogWarning("ofxWindowApp:setup()") << "SKIP! FrameNum: " << ofGetFrameNum();
		return;
	}

	ofLogVerbose("ofxWindowApp:setup()") << "FrameNum: " << ofGetFrameNum();

	//--

	// Create callback for window moved
#ifdef OFX_WINDOW_APP__USE_STATIC
	GLFWwindow * glfwWindow = glfwGetCurrentContext();
	glfwSetWindowPosCallback(glfwWindow, windowMoved);
#endif

	//--

	// Get rectanlges from os displays/monitors
	checkMonitors();

	//--

	// Default settings folder:
	// bin/data/ofxWindowApp/ofxWindowApp.json
	path_folder = "ofxWindowApp";
	path_filename = "ofxWindowApp.json";

	//--

	// Custom font
	if (1) { // Disable to use bitmap OF fonts instead of font files
		fontSize = 10;
		string _path = "assets/fonts/"; // assets fonts folder
		bool b = font.load(_path + "GeistMono-Bold.ttf", fontSize);
		if (!b) b = font.load(_path + "Geist-Bold.ttf", fontSize);
		if (!b) b = font.load(_path + "JetBrainsMono-Bold.ttf", fontSize);
		if (!b) ofLogError("ofxWindowApp:setup()") << "Error loading ttf font file";
	}

	//--

	setupParams();

	//--

	// Callbacks to auto call update/draw/keyPressed
	ofAddListener(ofEvents().update, this, &ofxWindowApp::update);
	ofAddListener(ofEvents().draw, this, &ofxWindowApp::draw, OF_EVENT_ORDER_AFTER_APP);
	ofAddListener(ofEvents().keyPressed, this, &ofxWindowApp::keyPressed);
	ofAddListener(ofEvents().keyReleased, this, &ofxWindowApp::keyReleased);
	ofAddListener(ofEvents().windowResized, this, &ofxWindowApp::windowResized);

	//--

	bDoneSetup = true;
	ofLogVerbose("ofxWindowApp:setup()") << "----------------------setup()--> END";
	
	//--
	
	startup();
}

//--------------------------------------------------------------
void ofxWindowApp::setupParams() {
	ofLogVerbose("ofxWindowApp:setupParams()");

	// Window
	paramsWindow.add(bvSync);
	paramsWindow.add(fpsTarget);

	// Session
	paramsSession.add(bShowInfo);
	paramsSession.add(bShowInfoPerformanceAlways);
	paramsSession.add(bDisableAutoSave);
	
//#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
	paramsSession.add(bWindowStayOnTop);
//#endif

	// Extra
	paramsExtra.add(paramsWindow);
	paramsExtra.add(paramsSession);

	ofAddListener(paramsExtra.parameterChangedE(), this, &ofxWindowApp::ChangedParamsExtra);
}

//--------------------------------------------------------------
void ofxWindowApp::startup() {
	ofLogVerbose("ofxWindowApp:startup()") << "----------------------startup() <--BEGIN";

	if (bDoneStartup) {
		ofLogWarning("ofxWindowApp:startup()") << "SKIP! FrameNum: " << ofGetFrameNum();
		return;
	}
	ofLogVerbose("ofxWindowApp:startup()") << "FrameNum: " << ofGetFrameNum();

	//--

	// Load
	loadSettings();
	
	// BUG: Redo trick workaround bc sometimes first one is not enough
	ofLogNotice("ofxWindowApp:startup()") << "Fix: call again to skip problems.";
	loadSettings();
	
	//--

	bDoneStartup = true;
	ofLogVerbose("ofxWindowApp:startup()") << "----------------------startup()--> END";
}

//----

#ifdef OFX_WINDOW_APP__USE_STATIC

ofxWindowApp * ofxWindowApp::instance = nullptr; // Initialize the static pointer

//--------------------------------------------------------------
void ofxWindowApp::setInstance(ofxWindowApp * app) {
	ofLogVerbose("ofxWindowApp:setInstance(ofxWindowApp * app)");
	instance = app; // Set the instance
}

//--------------------------------------------------------------
void ofxWindowApp::windowMoved(GLFWwindow * window, int xpos, int ypos) {
	if (!instance) return;
	ofLogVerbose("ofxWindowApp:windowMoved(window, xpos, ypos)") << ofToString(xpos) << ", " << ofToString(ypos);

	instance->windowChanged();
}

#endif

//----

// Window changed callbacks

//--------------------------------------------------------------
void ofxWindowApp::windowResized(ofResizeEventArgs & resize) {
	ofLogNotice("ofxWindowApp:windowMoved(resize)") << ofToString(resize.width) << ", " << ofToString(resize.height);

	this->windowChanged();
}

//--------------------------------------------------------------
void ofxWindowApp::windowResized(int w, int h) {
	ofLogNotice("ofxWindowApp:windowMoved(w, h)") << ofToString(w) << ", " << ofToString(h);

	this->windowChanged();
}

//--------------------------------------------------------------
void ofxWindowApp::windowChanged() { // Merge/group/redirect all callbacks to this method!
	//if (bDisableAutoSave) {
	//	ofLogNotice("ofxWindowApp:windowChanged()") << "SKIP! (bDisableAutoSaveLock) > FrameNum: " << ofGetFrameNum();
	//	return;
	//}
	
	// ignored changes when not ended setup, startup processes or is forced flagged to bypass callbacks
	if (!bDoneSetup) {
		ofLogVerbose("ofxWindowApp:windowChanged()") << "SKIP! (!bDoneSetup) > FrameNum: " << ofGetFrameNum();
		return;
	}
	if (!bDoneStartup) {
		ofLogVerbose("ofxWindowApp:windowChanged()") << "SKIP! (!bDoneStartup) > FrameNum: " << ofGetFrameNum();
		return;
	}

	if (bDisableCallback_windowMovedOrResized) {
		ofLogVerbose("ofxWindowApp:windowChanged()") << "SKIP! (bDisableCallback_windowMovedOrResized) > FrameNum: " << ofGetFrameNum();
		return;
	}

	ofLogNotice("ofxWindowApp:windowChanged()") << "FrameNum: " << ofGetFrameNum();

	doSetWindowSettingsFromAppWindow(); // Get raw values/states from the app window to windowSettings

	bFlagToSave = true; // Flag to save json on next frame
	
	//bFlagWindowChanged = true;

#ifdef OFX_WINDOW_APP__USE_TIMED_SAVER
	timeWhenToSaveFlag = ofGetElapsedTimef() + 0.5f;
#endif
}

//----

//--------------------------------------------------------------
void ofxWindowApp::update(ofEventArgs & args) {
//	// Auto call setup on first frame (or before?) if required.
//	if (!bDoneSetup) {
//		if (ofGetFrameNum() >= 0) {
//			setup();
//		}
//	}
////  // Debug
////	if (ofGetFrameNum() == 1) {
////		ofLogNotice("ofxWindowApp:update()") << "FrameNum: " << ofGetFrameNum();
////	}
//
//	// Auto call startup but after setup is done if required.
//	if (bDoneSetup) {
//		if (!bDoneStartup) {
//			if (ofGetFrameNum() >= 0) { // after or in framenum 0
//				startup();
//			}
//		}
//	}

	//--

	// Check if flagged to save
	if (bFlagToSave) {
#ifdef OFX_WINDOW_APP__USE_TIMED_SAVER
		if (ofGetElapsedTimef() >= timeWhenToSaveFlag)
#endif
		{
			bFlagToSave = false;

			ofLogVerbose("ofxWindowApp:update()") << "Going to call saveSettings() bc flagged (bFlagToSave)...";
			saveSettings();

			bFlagIsChanged = true;
		}
	}

	//--

#ifdef OFX_WINDOW_APP__USE_TIMED_SAVER
	// Auto saver timer mode:
	// It's no required to resize the window or to close the app window to save.
	// Then the app can crash an window shape will be stored 1 time each 10 seconds by default.
	if (bAutoSaverTimed && !bDisableAutoSave) {
		auto t = ofGetElapsedTimeMillis() - timeLastAutoSaveCheck;
		if (t > timePeriodToCheckIfSave) {
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
			} else {
				saveSettings(true);
			}

			timeLastAutoSaveCheck = ofGetElapsedTimeMillis();
		}
	}
#endif
}

//--------------------------------------------------------------
void ofxWindowApp::draw(ofEventArgs & args) {
	fpsReal = ofGetFrameRate();
	
	//--

	// Layouts top/bottom
	if (positionLayout == DEBUG_POSITION_BOTTOM) {
		if (font.isLoaded()) {
			previewY = ofGetWindowHeight() - fontSize + 5;
		} else {
			previewX = 10;
			previewY = ofGetWindowHeight() - 10;
		}
	} else if (positionLayout == DEBUG_POSITION_TOP) {
		if (font.isLoaded()) {
			previewY = fontSize + 4;
		} else {
			previewY = 15;
		}
	}

	//--

	if (bShowInfo) {
		drawInfo();
		drawInfoPerformanceWidget();
	} else {
		if (bShowInfoPerformanceAlways) drawInfoPerformanceWidget();
	}

	if (bShowDebug) {
		drawDebug();
		drawDebugSystemMonitors();
	}

	//--

	////TODO:  Trying to fix som glfw/OF windows management problems...
	//// https://forum.openframeworks.cc/t/timing-of-windowresized/44492
	//if (bFlagWindowChanged) {
	//	bFlagWindowChanged = false;
	//	this->windowChanged();
	//}
}

//--------------------------------------------------------------
void ofxWindowApp::doSetWindowSettingsFromAppWindow() {
	ofLogVerbose("ofxWindowApp:doSetWindowSettingsFromAppWindow()");

	windowSettings.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
	windowSettings.setSize(ofGetWindowSize().x, ofGetWindowSize().y);

	windowSettings.windowMode = ofGetCurrentWindow()->getWindowMode();
	if(windowSettings.windowMode == ofWindowMode(0))
		bIsFullScreen = false;
	else if(windowSettings.windowMode == ofWindowMode(1))
		bIsFullScreen = true;
	else if(windowSettings.windowMode == ofWindowMode(2))
		bIsFullScreen = true;
}

//--------------------------------------------------------------
void ofxWindowApp::save() {
	saveSettingsAfterRefresh();
}

//--------------------------------------------------------------
void ofxWindowApp::saveSettingsAfterRefresh() {
	doSetWindowSettingsFromAppWindow();
	saveSettings();
}

//--------------------------------------------------------------
void ofxWindowApp::saveSettings(bool bSlient) {
	ofLogVerbose("ofxWindowApp:saveSettings()") << "----------------------saveSettings() <--BEGIN";

	string path = path_folder + "/" + path_filename;
	ofLogNotice("ofxWindowApp:saveSettings()") << path;

	//--

	// Save window settings

	ofJson jWindowSettings;
	ofJson jExtra;

	ofLogVerbose("ofxWindowApp:saveSettings()") << "Ready to save `windowSettings`...";

	ofxSerializer::ofxWindowApp::to_json(jWindowSettings, windowSettings);

	ofSerialize(jExtra, paramsExtra);

	//--

	// Settings in one file

	ofJson data;
	data.push_back(jWindowSettings);
	data.push_back(jExtra);

	// Check if we need to create data folder first
	folderCheckAndCreate(path_folder);

	// Log
	logSettings();

	// Save file
	if (!bSlient) ofLogVerbose("ofxWindowApp:saveSettings()") << endl << data.dump(4);
	ofSavePrettyJson(path, data);

	//--

	bFlagShowFeedbackDoneSaved = true;

	ofLogVerbose("ofxWindowApp:saveSettings()") << "----------------------saveSettings()--> END";
}

//--------------------------------------------------------------
void ofxWindowApp::loadSettings() {
	ofLogVerbose("ofxWindowApp:loadFileSettings()") << "----------------------loadSettings() <--BEGIN";

	string path = path_folder + "/" + path_filename;

	ofFile file(path);
	bool bFileExist = file.exists();
	if (bFileExist) {
		ofLogNotice("ofxWindowApp:loadFileSettings()") << "File: " << path;

		//--

		// Load settings in one file
		ofJson data;
		data = ofLoadJson(path);
		ofLogVerbose("ofxWindowApp:loadFileSettings()") << "JSON: \n" << data.dump(4);

		//--

		ofJson jWindowSettings;
		ofJson jExtra;

		if (data.size() >= 2) {
			jWindowSettings = data[0]; //TODO: Ugly workaround wil break if differs the json object format!
			jExtra = data[1];

			// Recall both paramsExtra groups
			ofDeserialize(jExtra, paramsExtra);

			ofLogVerbose("ofxWindowApp:loadFileSettings()") << "\n\tSettings: \n" << jWindowSettings.dump(4);
			ofLogVerbose("ofxWindowApp:loadFileSettings()") << "\n\tExtras: \n" << ofToString(paramsExtra);
		} else {
			ofLogError("ofxWindowApp:loadFileSettings()") << "ERROR on data[] size = " << ofToString(data.size());
		}

		ofLogVerbose("ofxWindowApp:loadFileSettings()") << "\tWindow: \n" << jWindowSettings;
		ofLogVerbose("ofxWindowApp:loadFileSettings()") << "\tExtras: \n" << jExtra;

		//--

		int jx, jy, jw, jh;
		string jm;

		//--

		//TODO: no using ofxSerializer when loading... could remove and make new approach..
		jx = jWindowSettings["position"]["x"];
		jy = jWindowSettings["position"]["y"];
		jw = jWindowSettings["size"]["width"];
		jh = jWindowSettings["size"]["height"];
		jm = ofToString(jWindowSettings["window_mode"]);

		// Remove from name the added \" by the serializer
		ofStringReplace(jm, "\"", "");

		ofLogVerbose("ofxWindowApp:loadFileSettings()") << "x: " << jx;
		ofLogVerbose("ofxWindowApp:loadFileSettings()") << "y: " << jy;
		ofLogVerbose("ofxWindowApp:loadFileSettings()") << "w: " << jw;
		ofLogVerbose("ofxWindowApp:loadFileSettings()") << "h: " << jh;
		ofLogVerbose("ofxWindowApp:loadFileSettings()") << "m: " << jm;

		// Set windowSettings:

		// Screen modes
		// OF_WINDOW = 0
		// OF_FULLSCREEN = 1
		// OF_GAME_MODE = 2 //TODO

		if (jm == "0")
			windowSettings.windowMode = ofWindowMode(0);
		else if (jm == "1")
			windowSettings.windowMode = ofWindowMode(1);
		else if (jm == "2")
			windowSettings.windowMode = ofWindowMode(2);

		if (windowSettings.windowMode == ofWindowMode(0))
			bIsFullScreen = false;
		else if (windowSettings.windowMode == ofWindowMode(1))
			bIsFullScreen = true;
		else if (windowSettings.windowMode == ofWindowMode(2))
			bIsFullScreen = true;

		windowSettings.setPosition(glm::vec2(jx, jy));
		windowSettings.setSize(jw, jh);

		ofLogNotice("ofxWindowApp:loadFileSettings()") << "Done load settings!";

//		// Log
//		ofLogNotice("ofxWindowApp:loadFileSettings()") << "PRE";
//		logSettings();

		//--

		// Bypass windowChanged callbacks (to avoid save json file again)
		bDisableCallback_windowMovedOrResized = true;
		doApplyWindowSettings();
		doApplyWindowExtraSettings();
		bDisableCallback_windowMovedOrResized = false;

//		// Log
//		ofLogNotice("ofxWindowApp:loadFileSettings()") << "POST";
//		logSettings();

		//TODO: CHECK IF DIFFERS AND FORCE WINDOW TO JSON SETTINGS!
		// Then we can fixit here manually.

		//--
		
#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
#if defined(TARGET_WIN32)
		// On top
		if (!bIsFullScreen) {
			// Workaround refresh

			HWND W = GetActiveWindow();
			SetWindowPos(W, HWND_NOTOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
	
			// Re trig
			bWindowStayOnTop = bWindowStayOnTop;
		}
#endif
#endif
	} else {
		ofLogError("ofxWindowApp:loadFileSettings()") << "File settings NOT found: " << path;
	}
	ofLogVerbose("ofxWindowApp:loadFileSettings()") << "----------------------loadSettings()--> END";
}

//--------------------------------------------------------------
void ofxWindowApp::drawDebug() {
//#ifdef OFX_WINDOW_APP__DEVELOP_DEBUG
//	// Window title
//	string tp = ofToString(ofGetWindowPositionX()) + "," + ofToString(ofGetWindowPositionY());
//	string ts = ofToString(ofGetWindowSize().x) + "x" + ofToString(ofGetWindowSize().y);
//	string t = "ofxWindowApp    DEBUG                    " + tp + "    " + ts;
//	ofSetWindowTitle(t);
//#endif
	
	// Text box
	string s;
	s += "ofxWindowApp          DEBUG\n";
	if (bFlagShowFeedbackDoneSaved)
		s += "SAVE";
	else
		s += "    ";
	s += "\n";
	
	string screenSizeStr = ofToString(ofGetWindowWidth()) + "x" + ofToString(ofGetWindowHeight());
	screenSizeStr += " px";
	string screenPosStr = ofToString(ofGetWindowPositionX()) + "," + ofToString(ofGetWindowPositionY());
	string fpsRealStr = ofToString(fpsReal, 0);
	string str="";
	str += "Size : " + screenSizeStr+"\n";
	str += "Pos  : " + screenPosStr+"\n";
	str += "FPS  : " + fpsRealStr+"\n";
	s += str;
	s += "\n";
	
	if(bKeys){
	s += "> PRESS KEY\n\n";
	s += "d : SHOW DEBUG & DISPLAYS\n";
	s += "i : SHOW INFO & PERFORMANCE\n";

	s += "\n";
	s += "PRESETS\n";
	s += "q : Squared 800 x 800\n";
	s += "w : Squared W x W\n";
	s += "1 : IGTV Cover Photo\n";
	s += "2 : IG Landscape Photo\n";
	s += "3 : IG Portrait\n";
	s += "4 : IG Story\n";
	s += "5 : IG Square\n";
	s += "BCKSP : Reset Default\n";
	}
	else{
		s += "> KEYS ARE DISABLED!\n";
		s += "\n";}

	// Debug
#ifdef OFX_WINDOW_APP__DEVELOP_DEBUG
#ifdef OFX_WINDOW_APP__USE_TIMED_SAVER
	if (bAutoSaverTimed) {
		s += "\n";
		s += "CHANGED AND TIMER FINISHED\n";
		s += "\n";
		s += "Autosaver Timed: " + ofToString((bAutoSaverTimed ? "ON " : "OFF"));
		s += "\n";
		s += ofToString(timePeriodToCheckIfSave / 10.f, 1) + "secs\n";
		auto t = ofGetElapsedTimeMillis() - timeLastAutoSaveCheck;
		int pct = ofMap(t, 0, timePeriodToCheckIfSave, 0, 100);
		s += ofToString(pct) + "%";
	}
#endif
#endif

	if (font.isLoaded()) {
		ofPushStyle();
		ofFill();
		int x = 5;
		int y = 17;
		// fix space when using top layout
		if (positionLayout == DEBUG_POSITION_TOP) {
			y += 18;
		}
		x += SURFING__PAD_TO_WINDOW_BORDERS;
		y += SURFING__PAD_TO_WINDOW_BORDERS;
		auto bb = font.getStringBoundingBox(s, x, y);
		bb.setWidth(bb.getWidth() + SURFING__STRING_BOX__DEFAULT_XPAD);
		bb.setHeight(bb.getHeight() + SURFING__STRING_BOX__DEFAULT_YPAD);
		bb.translateX(-SURFING__STRING_BOX__DEFAULT_XPAD / 2);
		bb.translateY(-SURFING__STRING_BOX__DEFAULT_YPAD / 2);
		ofSetColor(0);
		ofDrawRectRounded(bb, SURFING__STRING_BOX__DEFAULT_ROUND);
		ofSetColor(255);
		font.drawString(s, x, y);
		ofPopStyle();
	} else {
		// fix space when using top layout
		if (positionLayout == DEBUG_POSITION_TOP){
			ofPushMatrix();
			ofTranslate(0, 18);
			ofxSurfingHelpersLite::ofxWindowApp::ofDrawBitmapStringBox(s, 0);
			ofPopMatrix();
		}
		else {ofxSurfingHelpersLite::ofxWindowApp::ofDrawBitmapStringBox(s, 0);
		}
	}

	if (bFlagShowFeedbackDoneSaved) bFlagShowFeedbackDoneSaved = 0;
}

//--------------------------------------------------------------
void ofxWindowApp::drawInfo() {
	// Debug overlay screen modes

	string str = "";
	string fpsRealStr = "";
	string fpsTargetStr = "";
	string screenSizeStr = "";
	string screenPosStr = "";
	string screenMode = "";
	string strPad = "  ";

	// Size
	screenSizeStr = ofToString(ofGetWindowWidth()) + "x" + ofToString(ofGetWindowHeight());
	screenSizeStr += " px";

	// Pos
	screenPosStr = ofToString(ofGetWindowPositionX()) + "," + ofToString(ofGetWindowPositionY());

	// Fps
	fpsRealStr = ofToString(fpsReal, 0);
	fpsTargetStr = ofToString(fpsTarget);

	str += strPad + strPad + "Size:" + screenSizeStr;
	str += strPad + "Pos:" + screenPosStr;
	str += strPad + "FPS:" + fpsRealStr;
	str += "[" + fpsTargetStr + "]";

	str += strPad + strPad + "|" + strPad;
	
//	str += strPad + "ALT +";
	
	str += strPad + (bKeys?"[i]_":"")+"INFO";

	str += strPad + (bKeys?"[v]_":"")+"VSYNC_" + ofToString(bvSync ? "ON " : "OFF");

	bool _bModeFullScreen = false;
	if (ofGetWindowMode() == OF_WINDOW) // Is full screen
	{
		_bModeFullScreen = false;
	} else if (ofGetWindowMode() == OF_FULLSCREEN) // Is window mode
	{
		_bModeFullScreen = true;
	}
	screenMode = (bKeys?"[f]_":"") + ofToString(_bModeFullScreen ? "FULLSCREEN_MODE" : "WINDOW_MODE");
	str += strPad + screenMode;

	str += strPad + (bKeys?"[l]_":"") + ofToString(bDisableAutoSave ? "AUTO_SAVE_OFF" : "AUTO_SAVE_ON ");
	
#ifdef TARGET_WIN32
#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
	str += strPad + (bKeys?"[t]_":"") + ofToString(bWindowStayOnTop ? "ON_TOP_TRUE " : "ON_TOP_FALSE");
#endif
#endif
	
	str += strPad + (bKeys?"[d]_":"")+"DEBUG_" + ofToString(bShowDebug ? "ON " : "OFF");
	str += strPad;

	// Debug mod keys
	//str += strPad + "  ";
	//str += " " + ofToString(mod_ALT ? "ALT" : "   ");
	//str += " " + ofToString(mod_CONTROL ? "CTRL" : "    ");
	//str += " " + ofToString(mod_SHIFT ? "SHIFT" : "     ");
	//str += " " + ofToString(mod_COMMAND ? "CMD" : "   ");

	//--

	if (font.isLoaded()) {
#if 1
		// A. Tiny squared text box
		ofPushStyle();
		ofFill();
		auto bb = font.getStringBoundingBox(str, previewX, previewY);
		bb.setWidth(bb.getWidth() + previewPad);
		bb.setHeight(bb.getHeight() + previewPad);
		bb.translateX(-previewPad / 2);
		bb.translateY(-previewPad / 2);
		ofSetColor(0);
		ofDrawRectangle(bb);
		ofSetColor(255);
		font.drawString(str, previewX, previewY);
		ofPopStyle();
#else
		// B. Big rounded text box
		auto bb = ofxSurfingHelpersLite::ofxWindowApp::getBBBitmapStringBoxToLayout(str, ofxSurfingHelpersLite::ofxWindowApp::SURFING_LAYOUT_BOTTOM_LEFT);
		ofxSurfingHelpersLite::ofxWindowApp::ofDrawBitmapStringBox(str, ofxSurfingHelpersLite::ofxWindowApp::SURFING_LAYOUT_BOTTOM_LEFT);
#endif
	} else {
		ofDrawBitmapStringHighlight(str, previewX, previewY);
	}
}

//--------------------------------------------------------------
void ofxWindowApp::drawInfoPerformanceWidget() {
	// Monitor fps performance alert

	bool bAlertPre; // Starts drawing black
	bool bAlert; // Draws red
	float fpsThreshold; // Abs amount of frames drop down to trig alert!

	//// A. Thresholds by factor fps between target and real fps
	//fpsThreshold = 0.9f; // below this trigs alert red state
	//bPreShow = (fpsReal < fpsTarget*0.999); // by ratio
	//bAlert = (fpsReal < fpsTarget*fpsThreshold); // A. by ratio

	// B. Thresholds by absolute fps difference between desired target and real fps
	// monitor fps performance
	fpsThreshold = 10.f; // Set num frames here! below this value, will trig the alert in red state
	bAlertPre = (fpsReal < fpsTarget - (fpsThreshold/2.f)); // below 5 fps starts showing bar
	bAlert = (fpsReal < (fpsTarget - fpsThreshold)); // B. by absolute fps diff

	//--

	// Value bar

	if (bAlertPre) // To draw only if fps performance is under threshold
	{
		float fx, fy, fw, fh, fwMax, fPad;
		fwMax = 100.f; // max width

		if (font.isLoaded()) {
			fh = fontSize + previewPad - 4;
			fPad = 4.f;
		} else {
			fh = 10.f;
			fPad = 5.f;
		}

		// Position
		fx = ofGetWindowWidth() - fwMax - fPad;

		fw = ofMap(fpsReal, 0.0f * fpsTarget, fpsTarget, 0, fwMax, true);
		int fa = 150; //alpha
		int iDiff = (int)fpsTarget - fpsReal;
		string diff;
		diff += "-";
		diff += ofToString(iDiff, 0);
		if (iDiff < 10) diff = " " + diff;
		diff += " FPS";

		ofColor cAlert(bAlert ? (ofColor(ofColor::red, fa)) : (ofColor(ofColor::black, fa)));

		ofPushStyle();

		float _xx = fx - 68.f;
		float _yy = previewY;

		// Fps label
		string str = diff;
		if (font.isLoaded()) {
			ofPushStyle();
			ofFill();
			auto bb = font.getStringBoundingBox(str, _xx, _yy);
			bb.setWidth(bb.getWidth() + previewPad);
			bb.setHeight(bb.getHeight() + previewPad);
			bb.translateX(-previewPad / 2);
			bb.translateY(-previewPad / 2);
			ofSetColor(cAlert);
			ofDrawRectangle(bb);
			ofSetColor(255);
			font.drawString(str, _xx, _yy);
			ofPopStyle();

			fy = bb.getY()+1;
			fh = bb.getHeight() - 2;
		} else {
			fy = previewY - fh + 1;
			ofDrawBitmapStringHighlight(diff, fx - 68.f, previewY, cAlert, ofColor(255)); // text fps diff
		}

		// Drop down out-performance bar
		ofFill();
		ofSetColor(cAlert);
		ofDrawRectangle(fx, fy, fw, fh);

		// Rect border
		ofNoFill();
		ofSetLineWidth(2.0f);
		ofSetColor(cAlert);
		ofDrawRectangle(fx, fy, fwMax, fh);

		ofPopStyle();
	}
}

//--------------------------------------------------------------
void ofxWindowApp::keyPressed(ofKeyEventArgs & eventArgs) {
	if (!bKeys) return;

	const int & key = eventArgs.key;
	
#ifdef OFX_WINDOW_APP__DEVELOP_DEBUG
	
//	if(eventArgs.hasModifier(OF_KEY_CONTROL) && eventArgs.codepoint=='a'){
//		ofLogNotice("ofxWindowApp:keyPressed") << "OF_KEY_CONTROL a";
//	}
	
	// Modifiers
	mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND); // macOS
	mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL); // Windows. not working
	mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);
	
	if(mod_ALT){
		ofLogVerbose("ofxWindowApp:keyPressed") << "mod_ALT     (true)"<< " FrameNum: " << ofGetFrameNum();
	}if(mod_COMMAND){
		ofLogVerbose("ofxWindowApp:keyPressed") << "mod_COMMAND (true)"<< " FrameNum: " << ofGetFrameNum();
	}if(mod_CONTROL){
		ofLogVerbose("ofxWindowApp:keyPressed") << "mod_CONTROL (true)"<< " FrameNum: " << ofGetFrameNum();
	}if(mod_SHIFT){
		ofLogVerbose("ofxWindowApp:keyPressed") << "mod_SHIFT   (true)"<< " FrameNum: " << ofGetFrameNum();
	}
	
	//https://blog.openframeworks.cc/post/173223240829/events
//	ofLogNotice("ofxWindowApp:keyPressed") << "key.codepoint: "<< (eventArgs.key.codepoint);
	
#endif
	
	//--

	// Debug
#ifdef OFX_WINDOW_APP__DEVELOP_DEBUG
	if (bShowDebug) {
		// set a window shape
		if (key == OF_KEY_F1) {
			ofLogNotice("ofxWindowApp:keyPressed") << "F1";
			for (int i = 0; i < 2; i++) { // Force repeat fix
				// 1
				ofSetWindowPosition(500, 500);
				ofSetWindowShape(500, 500);
//				// 2
//				ofSetWindowPosition(-1111, 1111);
//				ofSetWindowShape(1000, 1000);
			}
			logSettings();
			return;
		}
		// Log
		if (key == OF_KEY_F2) {
			ofLogNotice("ofxWindowApp:keyPressed") << "F2";
			logSettings();
			return;
		}
		// Refresh + log
		if (key == OF_KEY_F3) {
			ofLogNotice("ofxWindowApp:keyPressed") << "F3";
			doSetWindowSettingsFromAppWindow();
			logSettings();
			return;
		}
		// Load
		if (key == OF_KEY_F4) {
			ofLogNotice("ofxWindowApp:keyPressed") << "F4";
			loadSettings();
			return;
		}
		// Save
		if (key == OF_KEY_F5) {
			ofLogNotice("ofxWindowApp:keyPressed") << "F5";
			saveSettings();
			return;
		}
	}
#endif
	
	//--

	//TODO: modifier keys are not working fine at least in macOS.
	// So, we disable all keycommands combinations!
	
//	if (!mod_ALT) return; // Using ALT modifier

	//--

	ofLogVerbose("ofxWindowApp:keyPressed") << "char " << (char)key;

	// Draw info
	if (key == 'i') {
		bShowInfo = !bShowInfo;
		ofLogNotice("ofxWindowApp:keyPressed") << "i: Toggle bShowInfo: " << (bShowInfo ? "ON" : "OFF");
		saveSettings();
	}

	// Draw debug
	else if (key == 'd') {
		bShowDebug = !bShowDebug;
		ofLogNotice("ofxWindowApp:keyPressed") << "d: Toggle bShowDebug: " << (bShowDebug ? "ON" : "OFF");
	}

	// Switch window mode
	else if (key == 'f') {
		ofLogNotice("ofxWindowApp:keyPressed") << "f: Toggle window mode";
		doApplyToggleWindowMode();
	}

	// Switch vsync mode
	else if (key == 'v') {
		ofLogNotice("ofxWindowApp:keyPressed") << "v: Toggle vsync";
		bvSync = !bvSync;
		ofSetVerticalSync(bvSync);
	}

	// Toggle disable auto save lock
	else if (key == 'l') {
		ofLogNotice("ofxWindowApp:keyPressed") << "l: Toggle DisableAutoSave";
		setDisableAutoSave(!bDisableAutoSave);
		saveSettings();
	}
	
#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
	// Stay on top
	else if (key == 't') {
		ofLogNotice("ofxWindowApp:keyPressed") << "t: Toggle bWindowStayOnTop";
		setToggleStayOnTop();
		saveSettings();
	}
#endif
	
	// Reset default window
	else if (key == OF_KEY_BACKSPACE) {
		ofLogNotice("ofxWindowApp:keyPressed") << "BACKSPACE: Reset default";
		for (int i = 0; i < 3; i++) {//TODO: Force repeat fix
			doResetWindowSettings();
			doApplyWindowSettings();
		}
	}

	// Layout position top or bottom
	else if (key == 'p') {
		ofLogNotice("ofxWindowApp:keyPressed") << "p: Toggle info layout";
		doTogglePositionDisplayInfo();
	}

	else {
		// Window presets
		ofLogNotice("ofxWindowApp:keyPressed") << ofxSurfingHelpersLite::ofxWindowApp::keyPressedToSetWindowShape(key);
//		doSetWindowSettingsFromAppWindow();
		// Set some custom common sizes:
		// Instagram, portrait, landscape, squared etc
		// "PRESETS\n";
		// "q : Squared 800 x 800\n";
		// "w : Squared W x W\n";
		// "1 : IGTV Cover Photo\n";
		// "2 : IG Landscape Photo\n";
		// "3 : IG Portrait\n";
		// "4 : IG Story\n";
		// "5 : IG Square\n";
		// "BACKSPACE : Reset default\n";
	}
}

//--------------------------------------------------------------
void ofxWindowApp::keyReleased(ofKeyEventArgs & eventArgs) {
	if (!bKeys) return;

	const int & key = eventArgs.key;
	
#ifdef OFX_WINDOW_APP__DEVELOP_DEBUG
	// Modifiers
	mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND); // macOS
	mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL); // Windows. not working
	mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);
	
//	if(ofGetKeyPressed(OF_KEY_ALT)){
//		mod_ALT=false;
//		ofLogNotice("ofxWindowApp:keyReleased") << "mod_ALT (false)"<< " FrameNum: " << ofGetFrameNum();
//	}if(ofGetKeyPressed(OF_KEY_CONTROL)){
//		
//	}if(ofGetKeyPressed(OF_KEY_SHIFT)){
//		
//	}
	
	if(mod_ALT){
		ofLogNotice("ofxWindowApp:keyReleased") << "mod_ALT     (false)"<< " FrameNum: " << ofGetFrameNum();
	}if(mod_COMMAND){
		ofLogNotice("ofxWindowApp:keyReleased") << "mod_COMMAND (false)"<< " FrameNum: " << ofGetFrameNum();
	}if(mod_CONTROL){
		ofLogNotice("ofxWindowApp:keyReleased") << "mod_CONTROL (false)"<< " FrameNum: " << ofGetFrameNum();
	}if(mod_SHIFT){
		ofLogNotice("ofxWindowApp:keyReleased") << "mod_SHIFT   (false)"<< " FrameNum: " << ofGetFrameNum();
	}
#endif
}

//--------------------------------------------------------------
void ofxWindowApp::folderCheckAndCreate(string _path) {
	ofLogVerbose("ofxWindowApp:folderCheckAndCreate()") << "Path: " << _path;
	if (_path == "") return;

	ofDirectory dataDirectory(ofToDataPath(_path, true));

	// Check if target data folder exist
	if (!dataDirectory.isDirectory()) {
		ofLogWarning("ofxWindowApp:folderCheckAndCreate()") << "Folder does not exist!";

		// Create folder
		bool b = dataDirectory.createDirectory(ofToDataPath(_path, true));

		// Debug if creation has been succed
		if (b)
			ofLogNotice("ofxWindowApp:folderCheckAndCreate()") << "Folder '" << _path << "' created successfully!";
		else
			ofLogError("ofxWindowApp:folderCheckAndCreate()") << "Unable to create '" << _path << "' folder!";
	}
}

//--------------------------------------------------------------
void ofxWindowApp::doApplyToggleWindowMode() {
	ofLogVerbose("ofxWindowApp:doApplyToggleWindowMode()");

	// Toggle Mode
	if (ofGetWindowMode() == OF_WINDOW) {
		ofSetFullscreen(true);
		bIsFullScreen = true;
	} else if (ofGetWindowMode() == OF_FULLSCREEN) {
		ofSetFullscreen(false);
		bIsFullScreen = false;
	}

	doSetWindowSettingsFromAppWindow();
}

//--------------------------------------------------------------
void ofxWindowApp::doApplyWindowExtraSettings() {
	ofLogVerbose("ofxWindowApp:doApplyWindowExtraSettings()");

	ofLogVerbose("ofxWindowApp:doApplyWindowExtraSettings()") << "FpsTarget: " << fpsTarget;
	ofLogVerbose("ofxWindowApp:doApplyWindowExtraSettings()") << "vSync: " << bvSync;

	ofSetFrameRate(int(fpsTarget.get()));
	ofSetVerticalSync(bvSync.get());
}

//--------------------------------------------------------------
void ofxWindowApp::doApplyWindowSettings() {
	ofLogVerbose("ofxWindowApp:doApplyWindowSettings()");

	// Apply settings from windowSettings
	ofSetWindowPosition(windowSettings.getPosition().x, windowSettings.getPosition().y);
	ofSetWindowShape(windowSettings.getWidth(), windowSettings.getHeight());

	// Full screen
	if (bIsFullScreen) {
		ofSetFullscreen(true);
	}
	// Window mode
	else {
		ofSetFullscreen(false);
	}
}

//--------------------------------------------------------------
void ofxWindowApp::ChangedParamsExtra(ofAbstractParameter & e) {
	if (!bDoneSetup) return;
	if (!bDoneStartup) return;

	//--

	string name = e.getName();
	ofLogVerbose("ofxWindowApp::ChangedParamsExtra") << " " << name << " : " << e;

#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
	if (name == bWindowStayOnTop.getName()) {
	#if defined(TARGET_WIN32)
		if (bWindowStayOnTop.get()) {
			// Make app always on top
			HWND W = GetActiveWindow();
			SetWindowPos(W, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
			ofLogNotice("ofxWindowApp") << "Set on top Enabled";

		} else {
			// Disable make app always on top
			HWND W = GetActiveWindow();
			SetWindowPos(W, HWND_NOTOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
			ofLogNotice("ofxWindowApp") << "Set on top Disabled";
		}
		bIsWindowStayOnTop = bWindowStayOnTop;
	#elif defined(TARGET_OSX)
		//TODO:
		ofLogError("ofxWindowApp") << "Not implemented for OSX platform. Only TARGET_WIN32 yet!";
	#else
		ofLogError("ofxWindowApp") << "Not implemented for current platform. Only TARGET_WIN32 yet!";
	#endif
		return;
	}
#endif

	if (name == bvSync.getName()) {
		bFlagToSave = true;
		ofSetVerticalSync(bvSync.get());
		return;
	}

	if (name == fpsTarget.getName()) {
		bFlagToSave = true;
		ofSetFrameRate(int(fpsTarget.get()));
		return;
	}
}

//----

//--------------------------------------------------------------
void ofxWindowApp::checkMonitors() {
	int numberOfMonitors = 0;
	GLFWmonitor ** monitors = glfwGetMonitors(&numberOfMonitors);
	monitorRects.clear();
	monitorNames.clear();
	for (int iC = 0; iC < numberOfMonitors; iC++) {
		int xM;
		int yM;
		glfwGetMonitorPos(monitors[iC], &xM, &yM);
		const GLFWvidmode * desktopMode = glfwGetVideoMode(monitors[iC]);
		ofRectangle monitorRect(xM, yM, desktopMode->width, desktopMode->height);
		monitorRects.push_back(monitorRect);

		string s0 = glfwGetMonitorName(monitors[iC]);
		int xpos = 0;
		int ypos = 0;
		int width = 0;
		int height = 0;
		glfwGetMonitorWorkarea(monitors[iC], &xpos, &ypos, &width, &height);
		string s1 = " \t " + ofToString(xpos) + "," + ofToString(ypos);
		s1 += " \t\t " + ofToString(width) + "x" + ofToString(height);
		monitorNames.push_back(s0 + " " + s1);
	}
}

//--------------------------------------------------------------
void ofxWindowApp::drawDebugSystemMonitors() {
	auto pos = glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY());
	ofPushMatrix();
	ofPushStyle();

	ofTranslate(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f);
	ofScale(0.07f);

	int a = 180;

	int i = 0;
	for (auto & monitorRect : monitorRects) {
		// Canvas rect
		if (i == 0)
			monitorsCanvasRect.set(monitorRect);
		else
			monitorsCanvasRect.growToInclude(monitorRect);

		// Monitor rect
		// Check if pos is inside the ofRectangle
		if (monitorRect.inside(pos)) {
			ofSetColor(255, 255, 0, a); // Window app is on this monitor
		} else {
			ofSetColor(255, 255, 255, a);
		}
		ofNoFill();
		ofDrawRectangle(monitorRect);

		// Label
		ofPushMatrix();
		ofFill();
		ofTranslate(monitorRect.getX(), monitorRect.getY());
		ofTranslate(0, -30);
		string s = "";
		s += "#" + ofToString(i);
		s += " " + ofToString(monitorRect.getX());
		s += "," + ofToString(monitorRect.getY());
		s += " " + ofToString(monitorRect.width);
		s += "x" + ofToString(monitorRect.height);
		ofDrawBitmapString(s, 0, 0);
		ofPopMatrix();

		// Label center num
		ofPushMatrix();
		ofTranslate(monitorRect.getCenter());
		s = ofToString(i);
		ofDrawBitmapString(s, 0, 0);
		ofPopMatrix();

		i++;
	}

	// App window
	ofPushMatrix();
	ofNoFill();
	ofSetColor(0, 255, 0, a);
	ofRectangle rw = ofRectangle(pos.x, pos.y, ofGetWindowWidth(), ofGetWindowHeight());
	ofDrawRectangle(rw);
	string s = "";
	s += " " + ofToString(rw.getX());
	s += "," + ofToString(rw.getY());
	s += " " + ofToString(rw.width);
	s += "x" + ofToString(rw.height);
	ofTranslate(rw.getX(), rw.getY());
	ofTranslate(0, 170);
	ofDrawBitmapString(s, 0, 0);
	ofPopMatrix();

	// Label center name
	ofPushMatrix();
	ofTranslate(rw.getCenter());
	s = "App\nWindow";
	ofDrawBitmapString(s, 0, 0);
	ofPopMatrix();

	// Canvas
	ofNoFill();
	//monitorsCanvasRect.scaleFromCenter(1.1f);
	//ofSetColor(0, 0, 0, a);
	//ofDrawRectangle(monitorsCanvasRect); //canvas rect
	ofPushMatrix();
	ofTranslate(monitorsCanvasRect.getBottomLeft());

	// Monitor/displays names list
	s = "\n\n\nDISPLAYS\n\n";
	i = 0;
	for (auto & monitorName : monitorNames) {
		s += "#" + ofToString(i) + " " + monitorName + "\n";
		i++;
	}
	ofSetColor(255, 255, 255, a);
	ofDrawBitmapString(s, 0, 0);
	ofPopMatrix();

	ofPopStyle();
	ofPopMatrix();
}
