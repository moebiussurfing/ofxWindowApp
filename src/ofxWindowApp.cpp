#include "ofxWindowApp.h"

#ifdef OFX_WINDOW_APP__USE_OFX_WATCHER
	#include "ofxWatcher.h"
#endif

//--------------------------------------------------------------
ofxWindowApp::ofxWindowApp() {
	// Log
#ifdef OFX_WINDOW_APP__DEVELOP_DEBUG
	ofSetLogLevel("ofxWindowApp", OF_LOG_VERBOSE);
	bShowDebug = true;
#else
	ofSetLogLevel("ofxWindowApp", OF_LOG_NOTICE);
#endif

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
	getDiplaysMonitorsDesktopCanvas();

	//--

	// Default settings folder:
	// bin/data/ofxWindowApp/ofxWindowApp.json
	path_folder = "ofxWindowApp";
	path_filename = "ofxWindowApp.json";
	path_settings = path_folder + "/" + path_filename;

	//--

	// Custom font
	if (1) { // Disable to use bitmap OF fonts instead of font files
		fontSize = 10;
		string _path = "assets/fonts/"; // assets fonts folder
		bool b = font.load(_path + "GeistMono-Bold.ttf", fontSize);
		if (!b) b = font.load(_path + "Geist-Bold.ttf", fontSize);
		if (!b) b = font.load(_path + "JetBrainsMono-Bold.ttf", fontSize);
		if (!b) ofLogWarning("ofxWindowApp:setup()") << "Error loading ttf font file. Will use internal OF bitmap font.";
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
	paramsSession.add(bShowDebug);
	paramsSession.add(bShowInfo);
	paramsSession.add(bShowInfoPerformanceAlways);
	paramsSession.add(bDisableAutoSave);
	paramsSession.add(bKeys);

#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
	paramsSession.add(bStayOnTop);
#endif

#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
	paramsSession.add(bConsoleWindow);
#endif
	// Extra
	paramsExtra.add(paramsWindow);
	paramsExtra.add(paramsSession);

	// Group all
	params.add(paramsExtra);

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

	//TODO: Workaround OF/GLFW window management bugs.
	// BUG: Redo trick workaround bc sometimes first one is not enough...
	ofLogNotice("ofxWindowApp:startup()") << "Fix workaround: Call load again to fix problems.";
	loadSettings();

	//--

#ifdef OFX_WINDOW_APP__USE_OFX_WATCHER
	ofxWatchPath(path_settings, ofLoadJson, [this](const ofJson & json, const std::filesystem::path & path) {
		if (bDisableCallback_FileChanged) return;

		ofLogNotice("ofxWindowApp:startup()") << "ofxWatchPath: File changed: " << path;
		//ofLogNotice("ofxWindowApp:startup()")  << "ofxWatchPath: "<< endl << json.dump(2) << endl;

		loadSettings();
	});
#endif

	//--

	bDoneStartup = true;
	ofLogVerbose("ofxWindowApp:startup()") << "----------------------startup()--> END";
	// When strtup() ends, we already have the JSON settings file loaded.
	// We will un block saving callbacks again, to allow save changes since now.
	
	ofLogNotice("ofxWindowApp:startup()")<<"Done.";
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
	ofLogVerbose("ofxWindowApp:windowMoved(resize)") << ofToString(resize.width) << ", " << ofToString(resize.height);

	this->windowChanged();
}

//--------------------------------------------------------------
void ofxWindowApp::windowResized(int w, int h) {
	ofLogVerbose("ofxWindowApp:windowMoved(w, h)") << ofToString(w) << ", " << ofToString(h);

	this->windowChanged();
}

//--------------------------------------------------------------
void ofxWindowApp::windowChanged() { // Merge/group/redirect all callbacks to this method!
	// Skip saving when auto save is disabled. Restart app will reload JSON unmodified.
	if (bDisableAutoSave) {
		ofLogVerbose("ofxWindowApp:windowChanged()") << "SKIP! (bDisableAutoSave)                       > FrameNum: " << ofGetFrameNum();
		return;
	}

	// Will gnores window changes when not ended setup or startup processes, or if its forced being flagged to bypass callbacks!
	if (!bDoneSetup) {
		ofLogVerbose("ofxWindowApp:windowChanged()") << "SKIP! (!bDoneSetup)                           > FrameNum: " << ofGetFrameNum();
		return;
	}
	if (!bDoneStartup) {
		ofLogVerbose("ofxWindowApp:windowChanged()") << "SKIP! (!bDoneStartup)                         > FrameNum: " << ofGetFrameNum();
		return;
	}

	if (bDisableCallback_windowMovedOrResized) {
		ofLogVerbose("ofxWindowApp:windowChanged()") << "SKIP! (bDisableCallback_windowMovedOrResized) > FrameNum: " << ofGetFrameNum();
		return;
	}

	ofLogVerbose("ofxWindowApp:windowChanged()");

	doSetWindowSettingsFromAppWindow(); // Get raw values/states from the app window to windowSettings

	bFlagToSave = true; // Flag to save json on next frame

	//bFlagWindowChanged = true; //TODO: Workaround OF/GLFW window management bugs. Get/apply delayed...

#ifdef OFX_WINDOW_APP__USE_TIMED_SAVER
	timeWhenToSaveFlag = ofGetElapsedTimef() + 0.5f; // Wait 500ms and then save.
#endif
}

//----

//--------------------------------------------------------------
void ofxWindowApp::update(ofEventArgs & args) {

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

	//--

#ifdef OFX_WINDOW_APP__USE_OFX_WATCHER
	// Workaround trick: 1000ms is the default time interval checking.
	const int ti = 1000;
	if (bDisableCallback_FileChanged && (ofGetElapsedTimeMillis() - tLastSave) >= ti) {
		bDisableCallback_FileChanged = false;
	}
#endif
}

//--------------------------------------------------------------
void ofxWindowApp::draw(ofEventArgs & args) {
	fpsReal = ofGetFrameRate();

	//--

	// Calc layouts Top/Bottom
	if (bShowInfo || bShowInfoPerformanceAlways) {
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
	}

	//--

	// Draw Debug
	if (bShowDebug) {
		drawDebug();
		drawDebugDisplayMonitors();
	}

	// Draw Info
	if (bShowInfo) {
		drawInfo();
		drawInfoPerformanceWidget();
	} else {
		if (bShowInfoPerformanceAlways) drawInfoPerformanceWidget();
	}

	//----

	////TODO:  Trying to fix som glfw/OF windows management problems...
	//// https://forum.openframeworks.cc/t/timing-of-windowresized/44492
	//if (bFlagWindowChanged) {
	//	bFlagWindowChanged = false;
	//	this->windowChanged();
	//}

	//--

#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
	// Workaround trick bc we need startup did the loadSettings() call and windows is already updated!
	if (bStayOnTop.get()) {
		static bool bDoneStartupForce_bStayOnTop = 0;
		if (bDoneStartup && !bDoneStartupForce_bStayOnTop) {
			bDoneStartupForce_bStayOnTop = 1;
			ofLogVerbose("ofxWindowApp:draw()") << "StartupForce: doApplyStayOnTop()";
			doApplyStayOnTop();
		}
	}
#endif

#ifdef OFX_WINDOW_APP__USE_CONSOLE_WINDOW
	// Workaround trick bc we need startup did the loadSettings() call and windows is already updated!
	if (!bConsoleWindow.get()) {
		static bool bDoneStartupForce_bConsoleWindow = 0;
		if (bDoneStartup && !bDoneStartupForce_bConsoleWindow) {
			bDoneStartupForce_bConsoleWindow = 1;
			ofLogVerbose("ofxWindowApp:draw()") << "StartupForce: doApplyConsoleWindowVisible()";
			doApplyConsoleWindowVisible();
		}
	}
#endif
}

//--------------------------------------------------------------
void ofxWindowApp::doSetWindowSettingsFromAppWindow() {
	ofLogVerbose("ofxWindowApp:doSetWindowSettingsFromAppWindow()");

	windowSettings.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
	windowSettings.setSize(ofGetWindowSize().x, ofGetWindowSize().y);

	windowSettings.windowMode = ofGetCurrentWindow()->getWindowMode();
	if (windowSettings.windowMode == ofWindowMode(0))
		bIsFullScreen = false;
	else if (windowSettings.windowMode == ofWindowMode(1))
		bIsFullScreen = true;
	else if (windowSettings.windowMode == ofWindowMode(2))
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

	ofLogNotice("ofxWindowApp:saveSettings()") << path_settings;

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

	//	// Check if we need to create data folder first
	//	folderCheckAndCreate(path_folder);
	////	folderCheckAndCreate(path_settings);//TODO: Should use path without filename?

	// Log
	logSettings();

	// Save file
	if (!bSlient) ofLogVerbose("ofxWindowApp:saveSettings()") << endl
															  << data.dump(4);

#ifdef OFX_WINDOW_APP__USE_OFX_WATCHER
	bDisableCallback_FileChanged = true;
	tLastSave = ofGetElapsedTimeMillis();
	ofSavePrettyJson(path_settings, data);
#else
	ofSavePrettyJson(path_settings, data);
#endif

	//--

	bFlagShowFeedbackDoneSaved = true;

	ofLogVerbose("ofxWindowApp:saveSettings()") << "----------------------saveSettings()--> END";
}

//--------------------------------------------------------------
void ofxWindowApp::loadSettings() {
	ofLogVerbose("ofxWindowApp:loadFileSettings()") << "----------------------loadSettings() <--BEGIN";

	ofFile file(path_settings);
	bool bFileExist = file.exists();
	if (bFileExist) {
		ofLogNotice("ofxWindowApp:loadFileSettings()") << "File: " << path_settings;

		//--

		// Load settings in one file
		ofJson data;
		data = ofLoadJson(path_settings);
		ofLogVerbose("ofxWindowApp:loadFileSettings()") << "JSON: \n"
														<< data.dump(4);

		//--

		ofJson jWindowSettings;
		ofJson jExtra;

		if (data.size() >= 2) {
			jWindowSettings = data[0]; //TODO: Ugly workaround wil break if differs the json object format!
			jExtra = data[1];

			// Recall both paramsExtra groups
			ofDeserialize(jExtra, paramsExtra);

			ofLogVerbose("ofxWindowApp:loadFileSettings()") << "\n\tSettings: \n"
															<< jWindowSettings.dump(4);
			ofLogVerbose("ofxWindowApp:loadFileSettings()") << "\n\tExtras: \n"
															<< ofToString(paramsExtra);
		} else {
			ofLogError("ofxWindowApp:loadFileSettings()") << "ERROR on data[] size = " << ofToString(data.size());
		}

		ofLogVerbose("ofxWindowApp:loadFileSettings()") << "\tWindow: \n"
														<< jWindowSettings;
		ofLogVerbose("ofxWindowApp:loadFileSettings()") << "\tExtras: \n"
														<< jExtra;

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

		//--

		// Bypass windowChanged callbacks (to avoid save json file again)
		bDisableCallback_windowMovedOrResized = true;
		doApplyWindowSettings();
		doApplyWindowExtraSettings();
		bDisableCallback_windowMovedOrResized = false;
	} else {
		ofLogError("ofxWindowApp:loadFileSettings()") << "File settings NOT found: " << path_settings;
	}
	ofLogVerbose("ofxWindowApp:loadFileSettings()") << "----------------------loadSettings()--> END";
}

//--------------------------------------------------------------
void ofxWindowApp::drawDebug() {
//TODO: Reset default shape. Kicks window inside main monitor.
#ifdef OFX_WINDOW_APP__FORCE_RESET_WINDOW_IF_ITS_OUT_OF_DESKTOP_CANVAS
	if (getWindowPositionAtDisplay() == glm::vec2(-1, -1)) doResetWindowDefault();
#endif

#ifdef OFX_WINDOW_APP__DEVELOP_DEBUG
	// Window title
	string t = "ofxWindowApp  DEVELOP_DEBUG |";
	string ts = "Size:" + ofToString(ofGetWindowSize().x) + "x" + ofToString(ofGetWindowSize().y);
	string tp = "PosDesktop:" + ofToString(ofGetWindowPositionX()) + "," + ofToString(ofGetWindowPositionY());
	string tpd = "PosDisplay:" + ofToString(getWindowPositionAtDisplay().x) + "," + ofToString(getWindowPositionAtDisplay().y);
	t = t + "    " + ts + "  " + tp + "  " + tpd;
	ofSetWindowTitle(t);
#endif

	//--

	// Text box
	string s;
	s += "ofxWindowApp | DEBUG\n";
	if (bFlagShowFeedbackDoneSaved)
		s += "> SAVE";
	else
		s += "      ";
	s += "\n";

	string screenSizeStr = ofToString(ofGetWindowWidth()) + "x" + ofToString(ofGetWindowHeight()) + " px";
	string screenPosStr = ofToString(ofGetWindowPositionX()) + "," + ofToString(ofGetWindowPositionY());
	string fpsRealStr = ofToString(fpsReal, 0);
	string fpsTargetStr = ofToString(fpsTarget);
	string vsyncStr = ofToString(bvSync ? "ON " : "OFF");

	string str = "";
	str += "Size  : " + screenSizeStr + "\n";
	str += "Pos   : " + screenPosStr + "\n";
	str += "FPS   : " + fpsRealStr + " [" + fpsTargetStr + "]\n";
	str += "VSYNC : " + vsyncStr + "\n";

	s += str;
	s += "\n";

	if (bKeys) {
		s += "> PRESS KEY\n\n";
		s += "SHOW\n";
		s += "d : DEBUG\n";
		s += "    & DISPLAYS\n";
		s += "i : INFO\n";
		s += "    & PERFORMANCE\n";
		s += "\n";
		s += "PRESETS\n";
		s += "c : Centered\n";
		s += "q : Squared 800x800 px\n";
		s += "w : Squared w x w px\n";
		s += "\n";
		s += "INSTAGRAM\n";
		s += "1 : Cover Photo IGTV\n";
		s += "2 : Landscape Photo\n";
		s += "3 : Portrait\n";
		s += "4 : Story\n";
		s += "5 : Square\n";
		s += "\n";
		s += "BACKSPACE : Reset \n";
	} else {
		s += "> KEYS ARE DISABLED!\n";
		s += "\n";
	}

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
		if (positionLayout == DEBUG_POSITION_TOP) {
			ofPushMatrix();
			ofTranslate(0, 18);
			ofxSurfingHelpersLite::ofxWindowApp::ofDrawBitmapStringBox(s, 0);
			ofPopMatrix();
		} else {
			ofxSurfingHelpersLite::ofxWindowApp::ofDrawBitmapStringBox(s, 0);
		}
	}

	if (bFlagShowFeedbackDoneSaved) bFlagShowFeedbackDoneSaved = 0;
}

//--------------------------------------------------------------
void ofxWindowApp::drawInfo() {
	// Debug overlay screen modes

	string str = "";
	string strPad = "  ";
	string fpsRealStr = "";
	string fpsTargetStr = "";
	string screenSizeStr = "";
	string screenPosStr = "";
	string screenMode = "";

	// Size
	screenSizeStr = ofToString(ofGetWindowWidth()) + "x" + ofToString(ofGetWindowHeight());
	screenSizeStr += "px";

	// Pos
	screenPosStr = ofToString(ofGetWindowPositionX()) + "," + ofToString(ofGetWindowPositionY());

	// Fps
	fpsRealStr = ofToString(fpsReal, 0);
	fpsTargetStr = ofToString(fpsTarget);

	str += + "ofxWindowApp | "+ofToString(bKeys ? "i:" : "  ")+"INFO";
	
	str += strPad + (bKeys ? "d:" : "") + "DEBUG_" + ofToString(bShowDebug ? "ON " : "OFF");
	str +=  strPad + "|" ;
	
	str += strPad + "Size:" + screenSizeStr;
	str += strPad + "Pos:" + screenPosStr;
	str += strPad + "|";
	
	str += strPad + "FPS:" + fpsRealStr;
	str += " [" + fpsTargetStr + "]";

	//TODO: Modifier keys are not working fine at least in macOS.
	// So, we disable all keycommands combinations!
	//	str += strPad + "ALT +";

	str += strPad + (bKeys ? "v:" : "") + "VSYNC_" + ofToString(bvSync ? "ON " : "OFF");

	bool _bModeFullScreen = false;
	if (ofGetWindowMode() == OF_WINDOW) // Is full screen
	{
		_bModeFullScreen = false;
	} else if (ofGetWindowMode() == OF_FULLSCREEN) // Is window mode
	{
		_bModeFullScreen = true;
	}
	screenMode = (bKeys ? "f:" : "") + ofToString(_bModeFullScreen ? "FULLSCREEN_MODE" : "WINDOW_MODE");
	str += strPad + screenMode;

	str += strPad + (bKeys ? "l:" : "") + ofToString(bDisableAutoSave ? "AUTOSAVE_OFF" : "AUTOSAVE_ON ");

#ifdef TARGET_WIN32
	#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
	str += strPad + (bKeys ? "t:" : "") + ofToString(bStayOnTop ? "STAY_ON_TOP_ON " : "STAY_ON_TOP_OFF");
	#endif
#endif

#ifdef TARGET_WIN32
	#ifdef OFX_WINDOW_APP__USE_CONSOLE_WINDOW
	str += strPad + (bKeys ? "e:" : "") + ofToString(bConsoleWindow ? "CONSOLE_ON " : "CONSOLE_OFF");
	#endif
#endif

//	str += strPad;

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
		bb.growToInclude(ofGetWindowWidth(), previewY); // Grow to right window border
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
	bAlertPre = (fpsReal < fpsTarget - (fpsThreshold / 2.f)); // below 5 fps starts showing bar
	bAlert = (fpsReal < (fpsTarget - fpsThreshold)); // B. by absolute fps diff

	//--

	// Value bar

	if (bAlertPre) // To draw only if fps performance is under threshold
	{
		float fx, fy, fw, fh, fwMax, fPad;
		fwMax = 100; // max width fps widget rect

		if (font.isLoaded()) {
			fh = fontSize + previewPad - 4;
			fPad = 4.f;
		} else {
			fh = 16.f;
			fPad = 8.f;
		}

		// Position
		fx = ofGetWindowWidth() - fwMax - fPad;

		fw = ofMap(fpsReal, 0.0f * fpsTarget, fpsTarget, 0, fwMax, true);
		int fa = 150; // alpha
		int iDiff = (int)fpsTarget - fpsReal;
		string diff;
		diff += "-";
		if(iDiff<100)diff += " ";
		diff += ofToString(iDiff, 0);
		
		// Threshold alert
		if (iDiff < 10) diff = " " + diff;
		diff += " FPS";

		ofColor cAlert(bAlert ? (ofColor(ofColor::red, fa)) : (ofColor(ofColor::black, fa)));

		ofPushStyle();

		// Fps label
		string str = diff;
		if (font.isLoaded()) {
			float _xx = fx - 84;
			float _yy = previewY;

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

			// Offset bar widget
			fy = bb.getY() + 1;
			fh = bb.getHeight() - 2;
			fx -= 5;
		} else {
			float _xx = fx - 75;
			float _yy = previewY;
			
			ofDrawBitmapStringHighlight(diff, _xx, _yy, cAlert, ofColor(255)); // text fps diff
			
			// Offset bar widget
			fy = previewY - fh + 4;
			fx += 3;
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

	if (mod_ALT) {
		ofLogVerbose("ofxWindowApp:keyPressed") << "mod_ALT     (true)" << " FrameNum: " << ofGetFrameNum();
	}
	if (mod_COMMAND) {
		ofLogVerbose("ofxWindowApp:keyPressed") << "mod_COMMAND (true)" << " FrameNum: " << ofGetFrameNum();
	}
	if (mod_CONTROL) {
		ofLogVerbose("ofxWindowApp:keyPressed") << "mod_CONTROL (true)" << " FrameNum: " << ofGetFrameNum();
	}
	if (mod_SHIFT) {
		ofLogVerbose("ofxWindowApp:keyPressed") << "mod_SHIFT   (true)" << " FrameNum: " << ofGetFrameNum();
	}

	//https://blog.openframeworks.cc/post/173223240829/events
	//	ofLogNotice("ofxWindowApp:keyPressed") << "key.codepoint: "<< (eventArgs.key.codepoint);

#endif

	//--

	// Debug
#ifdef OFX_WINDOW_APP__DEVELOP_DEBUG
	if (bShowDebug) {
		// Set a window shape
		if (key == OF_KEY_F1) {
			ofLogNotice("ofxWindowApp:keyPressed") << "F1: Set a window shape";
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
			ofLogNotice("ofxWindowApp:keyPressed") << "F2: Log";
			logSettings();
			return;
		}
		// Refresh + log
		if (key == OF_KEY_F3) {
			ofLogNotice("ofxWindowApp:keyPressed") << "F3: Refresh + log";
			doSetWindowSettingsFromAppWindow();
			logSettings();
			return;
		}
		// Load
		if (key == OF_KEY_F4) {
			ofLogNotice("ofxWindowApp:keyPressed") << "F4: Load";
			loadSettings();
			return;
		}
		// Save
		if (key == OF_KEY_F5) {
			ofLogNotice("ofxWindowApp:keyPressed") << "F: Save";
			saveSettings();
			return;
		}
	}
#endif

	//--

	//TODO: Modifier keys are not working fine at least in macOS.
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
		//		ofSetVerticalSync(bvSync);
	}

	// Toggle disable auto save lock
	else if (key == 'l') {
		ofLogNotice("ofxWindowApp:keyPressed") << "l: Toggle DisableAutoSave";
		setDisableAutoSave(!bDisableAutoSave);
		saveSettings();
	}

	// Set window centered
	else if (key == 'c') {
		ofLogNotice("ofxWindowApp:keyPressed") << "c: Set window centered";
		setWindowCentered();
	}

#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
	// Stay on top
	else if (key == 't') {
		ofLogNotice("ofxWindowApp:keyPressed") << "t: Toggle stay on top";
		setToggleStayOnTop();
	}
#endif

#ifdef OFX_WINDOW_APP__USE_CONSOLE_WINDOW
	// Console Window
	else if (key == 'e') {
		ofLogNotice("ofxWindowApp:keyPressed") << "e: Toggle consolde window";
		setToggleConsoleWindowVisible();
	}
#endif

	// Reset default window
	else if (key == OF_KEY_BACKSPACE) {
		ofLogNotice("ofxWindowApp:keyPressed") << "BACKSPACE: Reset default";
		for (int i = 0; i < 3; i++) { //TODO: Force repeat fix
			doResetWindowDefault();
		}
	}

	// Layout position top or bottom
	else if (key == 'p') {
		ofLogNotice("ofxWindowApp:keyPressed") << "p: Toggle info layout";
		doTogglePositionDisplayInfo();
	}

	else {
		// Window presets
		ofxSurfingHelpersLite::ofxWindowApp::keyPressedToSetWindowShape(key);
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

	if (mod_ALT) {
		ofLogNotice("ofxWindowApp:keyReleased") << "mod_ALT     (false)" << " FrameNum: " << ofGetFrameNum();
	}
	if (mod_COMMAND) {
		ofLogNotice("ofxWindowApp:keyReleased") << "mod_COMMAND (false)" << " FrameNum: " << ofGetFrameNum();
	}
	if (mod_CONTROL) {
		ofLogNotice("ofxWindowApp:keyReleased") << "mod_CONTROL (false)" << " FrameNum: " << ofGetFrameNum();
	}
	if (mod_SHIFT) {
		ofLogNotice("ofxWindowApp:keyReleased") << "mod_SHIFT   (false)" << " FrameNum: " << ofGetFrameNum();
	}
#endif
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

#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
//--------------------------------------------------------------
void ofxWindowApp::doApplyStayOnTop() {
	ofLogVerbose("ofxWindowApp:doApplyStayOnTop()");

	#if defined(TARGET_WIN32)
	HWND hWndInsertAfter;
	if (bStayOnTop.get()) {
		// Enable window Stay on top
		hWndInsertAfter = HWND_TOPMOST;
		ofLogNotice("ofxWindowApp") << "Set Stay on top: Enabled";
	} else {
		// Disable window Stay on top
		hWndInsertAfter = HWND_NOTOPMOST;
		ofLogNotice("ofxWindowApp") << "Set Stay on top: Disabled";
	}
	HWND W = GetActiveWindow();
	SetWindowPos(W, hWndInsertAfter, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);

	//TODO:
	#elif defined(TARGET_OSX)
	ofLogVerbose("ofxWindowApp") << "Stay On Top not implemented for OSX platform. Only TARGET_WIN32 yet!";
	#else
	ofLogVerbose("ofxWindowApp") << "Stay On Top not implemented for current platform. Only TARGET_WIN32 yet!";
	#endif
}
#endif

//--------------------------------------------------------------
void ofxWindowApp::doApplyWindowExtraSettings() {
	ofLogVerbose("ofxWindowApp:doApplyWindowExtraSettings()");

	ofLogVerbose("ofxWindowApp:doApplyWindowExtraSettings()") << "FpsTarget: " << fpsTarget;
	ofLogVerbose("ofxWindowApp:doApplyWindowExtraSettings()") << "vSync: " << bvSync;

	ofSetFrameRate(int(fpsTarget.get()));
	ofSetVerticalSync(bvSync.get());

	doApplyStayOnTop();
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

	if (name == bDisableAutoSave.getName()) {
		bFlagToSave = true;
		return;
	}

#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
	if (name == bStayOnTop.getName()) {
		doApplyStayOnTop();
		if (!bDisableAutoSave)
			bFlagToSave = true;
		return;
	}
#endif

#ifdef OFX_WINDOW_APP__USE_STAY_ON_TOP
	if (name == bConsoleWindow.getName()) {
		setConsoleWindowVisible(bConsoleWindow.get());
		if (!bDisableAutoSave)
			bFlagToSave = true;
		return;
	}
#endif

	if (name == bvSync.getName()) {
		ofSetVerticalSync(bvSync.get());
		if (!bDisableAutoSave)
			bFlagToSave = true;
		return;
	}

	if (name == fpsTarget.getName()) {
		ofSetFrameRate(int(fpsTarget.get()));
		if (!bDisableAutoSave)
			bFlagToSave = true;
		return;
	}

	if (name == bKeys.getName()) {
		if (!bDisableAutoSave)
			bFlagToSave = true;
		return;
	}

	if (name == bShowDebug.getName()) {
		if (!bDisableAutoSave)
			bFlagToSave = true;
		return;
	}

	if (name == bShowInfo.getName()) {
		if (!bDisableAutoSave)
			bFlagToSave = true;
		return;
	}

	if (name == bShowInfoPerformanceAlways.getName()) {
		if (!bDisableAutoSave)
			bFlagToSave = true;
		return;
	}
}

//----

//--------------------------------------------------------------
glm::vec2 ofxWindowApp::getWindowPositionAtDisplay() {
	//ofLogVerbose("ofxWindowApp:getWindowPositionAtDisplay()");
	glm::vec2 p(-1, -1);

	int index = getDisplayIndexForWindow();

	if (index == -1) {
		ofLogVerbose("ofxWindowApp:getWindowPositionAtDisplay()") << "Display index not idetified around OS displays.";
		return p;
	} else {
		//ofLogVerbose("ofxWindowApp:getWindowPositionAtDisplay()") << "Display index: " << index;
	}

	if (index >= monitorRects.size()) {
		ofLogError("ofxWindowApp:getWindowPositionAtDisplay()") << "Display index out of range";
		return p;
	}

	ofRectangle r = monitorRects[index];
	// Window position related to current display
	p.x = ofGetWindowPositionX() - r.getPosition().x;
	p.y = ofGetWindowPositionY() - r.getPosition().y;

	return p;
}

//--------------------------------------------------------------
int ofxWindowApp::getDisplayIndexForWindow() {
	//ofLogVerbose("ofxWindowApp:getDisplayIndexForWindow()");

	int gap = 1; // Tiny offset to fix 0,0 returns "out of canvas desktop"
	auto pos = glm::vec2(ofGetWindowPositionX() + gap, ofGetWindowPositionY() + gap);
	int index = -1;
	int i = 0;
	for (auto & monitorRect : monitorRects) {
		if (monitorRect.inside(pos)) {
			index = i;
		}
		i++;
	}

	//TODO: Not working
	if (index == -1 && int(ofGetElapsedTimef()) % 5 == 0) {
		ofLogError("ofxWindowApp:getDisplayIndexForWindow()") << "Display index not idetified around OS displays.";
		ofLogError("ofxWindowApp:getDisplayIndexForWindow()") << "Probably App Window (top main bar) is out of the desktop canvas!";
		ofLogVerbose("ofxWindowApp:getDisplayIndexForWindow()") << "Check bottom right corner instead of main top window bar";
		auto pos_ = glm::vec2(pos.x /*+ ofGetWindowWidth()*/, pos.y + ofGetWindowHeight());
		int j = 0;
		for (auto & monitorRect : monitorRects) {
			if (monitorRect.inside(pos_)) {
				index = j;
			}
			j++;
		}
	}

	return index;
}

//--------------------------------------------------------------
void ofxWindowApp::getDiplaysMonitorsDesktopCanvas() {
	ofLogVerbose("ofxWindowApp:getDiplaysMonitorsDesktopCanvas()");
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

		string s1 = "";
		s1 += "\t" + ofToString(xpos) + "," + ofToString(ypos);
		s1 += "     \t" + ofToString(width) + "x" + ofToString(height);
		monitorNames.push_back(s0 + " " + s1);
	}
}

//--------------------------------------------------------------
void ofxWindowApp::drawDebugDisplayMonitors() {
	auto pos = glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY());
	ofPushMatrix();
	ofPushStyle();

	ofTranslate(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f);
	ofScale(0.07f);

	int a = 180;

	int index = -1;
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
			index = i;
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
	// Blink
	int d = 1000;
	int v = ofGetElapsedTimeMillis() % d;
	int a_ = (v < d * 0.65) ? 255 : 0;
	ofSetColor(0, 255, 0, a_);
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
	//ofDrawRectangle(monitorsCanvasRect); // Canvas rect
	ofPushMatrix();
	ofTranslate(monitorsCanvasRect.getBottomLeft());

	// Monitor/displays names list
	s = "\n\n\nDISPLAYS (Desktop Canvas)\n\n";
	i = 0;
	for (auto & monitorName : monitorNames) {
		if (index == i) // Mark current monitor
			s += "> ";
		else
			s += "  ";
		s += "#" + ofToString(i) + " " + monitorName + "\n";
		i++;
	}
	ofSetColor(255, 255, 255, a);
	ofDrawBitmapString(s, 0, 0);
	ofPopMatrix();

	ofPopStyle();
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofxWindowApp::setWindowCentered() {
	ofLogNotice("ofxWindowApp:setWindowCentered()");
	int index = getDisplayIndexForWindow();
	if (index == -1) {
		if ((int)ofGetElapsedTimef() % 5 == 0)
			ofLogError("ofxWindowApp:setWindowCentered()") << "Display index not idetified around OS displays.";
		ofLogError("ofxWindowApp:setWindowCentered()") << "Probably App Window (top main bar) is out of the desktop canvas!";
		//TODO: Kick window inside main display ?
		return;
	} else {
		ofLogVerbose("ofxWindowApp:setWindowCentered()") << "Display index: " << index;
	}

	ofRectangle r;
	r = monitorRects[index];

	// Window shape
	int ww = ofGetWindowWidth();
	int wh = ofGetWindowHeight();
	// Screen shape
	int sw = r.getWidth();
	int sh = r.getHeight();
	// Screen position
	int spx = r.getPosition().x;
	int spy = r.getPosition().y;

	int x = spx + sw / 2 - ww / 2;
	int y = spy + sh / 2 - wh / 2;
	ofSetWindowPosition(x, y);

	//--

	//string s = "Centered: " + ofToString(x) + "," + ofToString(y) + " " + ofToString(ww) + "x" + ofToString(wh) + " px";
	//return s;
}

//--

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
