#include "ofxWindowApp.h"

#ifdef SURFING_WINDOW_APP__USE_STATIC
	#if defined(TARGET_WIN32)

ofxWindowApp * ofxWindowApp::instance = nullptr; // Initialize the static pointer

//--------------------------------------------------------------
void ofxWindowApp::setInstance(ofxWindowApp * app) {
	ofLogNotice("ofxWindowApp:setInstance(ofxWindowApp * app)");
	instance = app; // Set the instance
}

//--

//--------------------------------------------------------------
void ofxWindowApp::windowMoved(GLFWwindow * window, int xpos, int ypos) {
	if (!instance) return;
	if (instance->bDisableCallback_windowMovedOrResized) return;

	ofLogNotice("ofxWindowApp:windowMoved(GLFWwindow * window..)") << ofToString(xpos) << ", " << ofToString(ypos);

	if (!instance->bDoneSetup) return;
	if (!instance->bDoneStartup) return;

	instance->doGetWindowSettings(); // get raw values/states from the app window to windowSettings

	instance->bFlagToSave = true; // flag to save json on next frame

		#ifdef SURFING_WINDOW_APP__USE_TIMED_SAVER
	instance->timeWhenToSaveFlag = ofGetElapsedTimef() + 0.5f;
		#endif
}

	#endif
#endif

//--------------------------------------------------------------
void ofxWindowApp::windowResized(ofResizeEventArgs & resize) {
	if (bDisableCallback_windowMovedOrResized) return;

	ofLogNotice("ofxWindowApp:windowResized(ofResizeEventArgs & resize)") << ofToString(resize.width) << ", " << ofToString(resize.height);
	ofLogNotice("ofxWindowApp:windowResized(ofResizeEventArgs & resize)") << "at frameNum: " << ofGetFrameNum();

	windowResized(resize.width, resize.height);
}

//--------------------------------------------------------------
void ofxWindowApp::windowResized(int w, int h) {
	if (bDisableCallback_windowMovedOrResized) return;

	ofLogNotice("ofxWindowApp:windowResized(w,h)") << ofToString(w) << ", " << ofToString(h);
	ofLogNotice("ofxWindowApp:windowResized(w,h)") << "at frameNum: " << ofGetFrameNum();

	doGetWindowSettings(); // get raw values/states from the app window to windowSettings

	bFlagToSave = true; // flag to save json on next frame

#ifdef SURFING_WINDOW_APP__USE_TIMED_SAVER
	timeWhenToSaveFlag = ofGetElapsedTimef() + 0.5f;
#endif
}

//----

//--------------------------------------------------------------
ofxWindowApp::ofxWindowApp() {
	ofSetLogLevel("ofxWindowApp", OF_LOG_NOTICE);

	ofLogNotice("ofxWindowApp:ofxWindowApp()") << "at frameNum: " << ofGetFrameNum();

	doResetWindowExtraSettings();
	doResetWindowSettings();

	bDisableCallback_windowMovedOrResized = true;
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

#ifdef SURFING_WINDOW_APP__USE_STATIC
	GLFWwindow * glfwWindow = glfwGetCurrentContext();
	if (glfwWindow) {
		glfwSetWindowPosCallback(glfwWindow, nullptr);
	}
#endif

	ofRemoveListener(paramsExtra.parameterChangedE(), this, &ofxWindowApp::Changed_ParamsExtra);
}

//--------------------------------------------------------------
void ofxWindowApp::exit() {
	ofLogNotice("ofxWindowApp:exit()") << "Constructor";

#ifdef SURFING_WINDOW_APP__ENABLE_SAVE_ON_EXIT
	if (bAutoSaveLoad && !bDisableAutoSave) {
		save();
	}
#endif
}

//----

//--------------------------------------------------------------
void ofxWindowApp::setup() {
	if (bDoneSetup) {
		ofLogWarning("ofxWindowApp:setup()") << "Skip! at frameNum: " << ofGetFrameNum();
		return;
	}

	ofLogNotice("ofxWindowApp:setup()") << "at frameNum: " << ofGetFrameNum();

	//--

	// Create callback for window moved
#ifdef SURFING_WINDOW_APP__USE_STATIC
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

	// Font
	fontSize = 10;
	string _path = "assets/fonts/"; // assets fonts folder
	bool b = font.load(_path + "GeistMono-Bold.ttf", fontSize);
	if (!b) b = font.load(_path + "Geist-Bold.ttf", fontSize);
	if (!b) b = font.load(_path + "JetBrainsMono-Bold.ttf", fontSize);
	if (!b) ofLogError("ofxWindowApp:setup()") << "Error loading ttf font file";

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
}

//--------------------------------------------------------------
void ofxWindowApp::setupParams() {
	ofLogNotice("ofxWindowApp:setupParams()");

	// Window
	paramsWindow.add(vSync);
	paramsWindow.add(fpsTarget);

	// Session
	paramsSession.add(bShowInfo);
	paramsSession.add(bShowInfoPerformanceAlways);
	paramsSession.add(bDisableAutoSave);
#ifdef SURFING_USE_STAY_ON_TOP
	paramsSession.add(bWindowStayOnTop);
#endif

	// Extra
	paramsExtra.add(paramsWindow);
	paramsExtra.add(paramsSession);

	ofAddListener(paramsExtra.parameterChangedE(), this, &ofxWindowApp::Changed_ParamsExtra);
}

//--------------------------------------------------------------
void ofxWindowApp::startup() {
	if (bDoneStartup) {
		ofLogWarning("ofxWindowApp::startup()") << "Skip! at frameNum: " << ofGetFrameNum();
		return;
	}
	ofLogNotice("ofxWindowApp::startup()") << "at frameNum: " << ofGetFrameNum();

	//--

	// Load
	if (bAutoSaveLoad) loadSettings();

	//--

	//// Fix workaround
	//if (bIsFullScreen) {
	//	ofSetFullscreen(true);
	//	//TODO: full screen on different monitors make troubles to re load..
	//	//ofSetWindowPosition(windowSettings.getPosition().x, windowSettings.getPosition().y);
	//	//ofSetWindowShape(ofGetWidth(), ofGetHeight());
	//}

	//--

	//#ifdef SURFING_USE_STAY_ON_TOP
	//	// On top
	//	if (!bIsFullScreen) {
	//		// Workaround
	//		// Refresh
	//	#if defined(TARGET_WIN32)
	//		HWND W = GetActiveWindow();
	//		SetWindowPos(W, HWND_NOTOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
	//	#endif
	//		// Re trig
	//		bWindowStayOnTop = bWindowStayOnTop;
	//	}
	//#endif

	//--

	bDoneStartup = true;
}

//----

//--------------------------------------------------------------
void ofxWindowApp::update(ofEventArgs & args) {
	// Auto call setup on first frame if required.
	if (!bDoneSetup) {
		if (ofGetFrameNum() >= 0) {
			setup();
		}
	}

	// Auto call startup but after setup is done if required.
	if (bDoneSetup) {
		if (!bDoneStartup) {
			if (ofGetFrameNum() > 0) {
				// Workaround
				startup();
			}
		}
	}

	//--

	////TODO
	//// Force ignore saving at startup
	//auto fm = ofGetFrameNum();
	//if (fm < 120) {
	//	if (bFlagToSave) {
	//		bFlagToSave = false;
	//	}
	//}

	// Check if flagged to save
	if (bFlagToSave) {
#ifdef SURFING_WINDOW_APP__USE_TIMED_SAVER
		if (ofGetElapsedTimef() >= timeWhenToSaveFlag)
#endif
		{
			bFlagToSave = false;
			bFlagIsChanged = true;

			ofLogNotice("ofxWindowApp:update()") << "Going to saveSettings() bc flagged (bFlagToSave) ...";

			saveSettings();
		}
	}

	//--

#ifdef SURFING_WINDOW_APP__USE_TIMED_SAVER
	// Auto saver timer
	// is no required to resize the window or to close the app window to save.
	// then the app can crash an window shape will be stored 1 time each 10 seconds by default.
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

	if (positionLayout == DEBUG_POSITION_BOTTOM) {
		if (!font.isLoaded()) {
			previewX = 10;
			previewY = ofGetWindowHeight() - 10;
		} else {
			previewY = ofGetWindowHeight() - fontSize + 5;
		}
	} else if (positionLayout == DEBUG_POSITION_TOP) {
		if (font.isLoaded()) {
			previewY = fontSize;
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
}

//--------------------------------------------------------------
void ofxWindowApp::doGetWindowSettings() {
	ofLogNotice("ofxWindowApp") << "doGetWindowSettings()";

	windowSettings.setPosition(glm::vec2(ofGetWindowPositionX(), ofGetWindowPositionY()));
	windowSettings.setSize(ofGetWindowSize().x, ofGetWindowSize().y);

	windowSettings.windowMode = ofGetCurrentWindow()->getWindowMode();
	if (windowSettings.windowMode == ofWindowMode(OF_WINDOW))
		bIsFullScreen = false;
	else if (windowSettings.windowMode == ofWindowMode(OF_FULLSCREEN))
		bIsFullScreen = true;
	else if (windowSettings.windowMode == ofWindowMode(OF_GAME_MODE)) //TODO
		bIsFullScreen = false;

	logSettings();
}

//--------------------------------------------------------------
void ofxWindowApp::save() {
	saveSettingsAfterRefresh();
}

//--------------------------------------------------------------
void ofxWindowApp::saveSettingsAfterRefresh() {
	doGetWindowSettings();
	saveSettings();
}

//--------------------------------------------------------------
void ofxWindowApp::saveSettings(bool bSlient) {
	string path;
	if (path_folder == "" && path_filename == "")
		path = "ofxWindowApp.json";
	else
		path = path_folder + "/" + path_filename;

	ofLogNotice("ofxWindowApp:saveSettings()") << path;

	//--

	// Save window settings

	ofJson jApp;
	ofJson jExtra;

	logSettings();
	ofLogNotice("ofxWindowApp") << "Ready to save `windowSettings`...";

	ofxSerializer::ofxWindowApp::to_json(jApp, windowSettings);

	ofSerialize(jExtra, paramsExtra);

	//--

	// Settings in one file

	ofJson data;
	data.push_back(jApp);
	data.push_back(jExtra);

	// Check if we need to create data folder first
	folderCheckAndCreate(path_folder);

	// Save file
	if (!bSlient) ofLogNotice("ofxWindowApp") << data.dump(4);
	ofSavePrettyJson(path, data);

	//--

	bFlagShowFeedbackDoneSaved = true;
}

//--------------------------------------------------------------
void ofxWindowApp::loadSettings() {
	ofSetFullscreen(false);

	string path = path_folder + "/" + path_filename;

	ofFile file(path);
	bool bFileExist = file.exists();
	if (bFileExist) {
		ofLogNotice("ofxWindowApp:loadFileSettings()") << "File found: " << path;

		//--

		// Load settings
		// Settings in one file

		ofJson data;
		data = ofLoadJson(path);
		ofLogNotice("ofxWindowApp") << "File JSON: \n"
									<< data.dump(4);

		ofJson jBig;

		//--

		ofJson jExtra;

		if (data.size() >= 2) {
			jBig = data[0]; //TODO: Ugly workaround wil break if differs json object format!
			jExtra = data[1];

			// Recall both paramsExtra groups
			ofDeserialize(jExtra, paramsExtra);

			ofLogNotice("ofxWindowApp") << "\tSettings: \m" << jBig.dump(4);
			ofLogNotice("ofxWindowApp") << "\tExtras: \n"
										<< ofToString(paramsExtra);
		} else {
			ofLogError("ofxWindowApp") << "ERROR on data[] size = " << ofToString(data.size());
		}

		ofLogVerbose("ofxWindowApp") << "Window: \n"
									 << jBig;
		ofLogVerbose("ofxWindowApp") << "Extras: \n"
									 << jExtra;

		//--

		int jx, jy, jw, jh;
		string jm;

		//--

		// Big
		jx = jBig["position"]["x"];
		jy = jBig["position"]["y"];
		jw = jBig["size"]["width"];
		jh = jBig["size"]["height"];
		jm = ofToString(jBig["window_mode"]);

		// Remove from name the added \" by the serializer
		ofStringReplace(jm, "\"", "");

		ofLogVerbose("ofxWindowApp") << "x: " << jx;
		ofLogVerbose("ofxWindowApp") << "y: " << jy;
		ofLogVerbose("ofxWindowApp") << "w: " << jw;
		ofLogVerbose("ofxWindowApp") << "h: " << jh;
		ofLogVerbose("ofxWindowApp") << "m: " << jm;

		// Screen modes
		// OF_WINDOW = 0
		// OF_FULLSCREEN = 1
		// OF_GAME_MODE = 2

		if (jm == "0")
			windowSettings.windowMode = ofWindowMode(0);
		else if (jm == "1")
			windowSettings.windowMode = ofWindowMode(1);
		else if (jm == "2")
			windowSettings.windowMode = ofWindowMode(2);

		if (windowSettings.windowMode == ofWindowMode(0)) //window
			bIsFullScreen = false;
		else if (windowSettings.windowMode == ofWindowMode(1)) //fullscreen
			bIsFullScreen = true;
		else if (windowSettings.windowMode == ofWindowMode(2)) //game //TODO
			bIsFullScreen = false;

		windowSettings.setPosition(glm::vec2(jx, jy));
		windowSettings.setSize(jw, jh);

		logSettings();

		ofLogNotice("ofxWindowApp") << "Done load settings!";
		ofLogNotice("ofxWindowApp") << "-------------------";

		//--

		bDisableCallback_windowMovedOrResized = true;
		doApplyWindowSettings();
		doApplyWindowExtraSettings();
		bDisableCallback_windowMovedOrResized = false;

		//--

#ifdef SURFING_USE_STAY_ON_TOP
		// On top
		if (!bIsFullScreen) {
			// Workaround
			// Refresh
	#if defined(TARGET_WIN32)
			HWND W = GetActiveWindow();
			SetWindowPos(W, HWND_NOTOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
	#endif
			// Re trig
			bWindowStayOnTop = bWindowStayOnTop;
		}
#endif
	} else {
		ofLogError("ofxWindowApp") << "File settings NOT found: " << path;
	}
}

//--------------------------------------------------------------
void ofxWindowApp::drawDebug() {
	// window title
	string tp = ofToString(ofGetWindowPositionX()) + "," + ofToString(ofGetWindowPositionY());
	string ts = ofToString(ofGetWindowSize().x) + "x" + ofToString(ofGetWindowSize().y);
	ofSetWindowTitle("ofxWindowApp    DEBUG    " + tp + "    " + ts);

	// text box
	string s;
	s += "ofxWindowApp         DEBUG\n";
	if (bFlagShowFeedbackDoneSaved)
		s += "SAVE";
	else
		s += "    ";
	s += "\n";

	s += "> PRESS ALT +\n\n";
	s += "D : SHOW DEBUG & MONITORS\n";
	s += "W : SHOW INFO & PERFORMANCE\n";

	s += "\n";
	s += "PRESETS\n";
	s += "Q : Squared 800 x 800\n";
	s += "q : Squared W x W\n";
	s += "1 : IGTV Cover Photo\n";
	s += "2 : IG Landscape Photo\n";
	s += "3 : IG Portrait\n";
	s += "4 : IG Story\n";
	s += "5 : IG Square\n";
	s += "BKSP : Reset default\n";

	//#define SURFING_WINDOW_APP__DEBUG_TIMER
#ifdef SURFING_WINDOW_APP__DEBUG_TIMER
	s += "\n";
	#ifndef SURFING_WINDOW_APP__USE_TIMED_SAVER
	s += "SAVES IF WINDOW IS\n";
	s += "RESIZED OR MOVED";
	#else
	s += "SAVES IF WINDOW IS\n";
	s += "RESIZED\n";
	if (bAutoSaverTimed) {
		s += "MOVED AND TIMER FINISHED\n";
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
		int x = 4;
		int y = 18;
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
		ofxSurfingHelpersLite::ofxWindowApp::ofDrawBitmapStringBox(s, 0);
	}

	if (bFlagShowFeedbackDoneSaved) bFlagShowFeedbackDoneSaved = 0;
}

//--------------------------------------------------------------
void ofxWindowApp::drawInfo() {
	// Debug overlay screen modes

	string vSyncStr;
	string fpsRealStr;
	string fpsTargetStr;
	string strPad = "  "; // Add spaces
	string str;
	string screenStr = "";
	string screenPosStr = "";
	string screenMode = "";

	// Size
	screenStr = ofToString(ofGetWindowWidth()) + "x" + ofToString(ofGetWindowHeight());

	screenStr += "px";
	// Pos
	screenPosStr = ofToString(ofGetWindowPositionX()) + "," + ofToString(ofGetWindowPositionY());

	// Fps
	fpsRealStr = ofToString(fpsReal, 1);
	fpsTargetStr = ofToString(fpsTarget);

	str += strPad + strPad + "SIZE:" + screenStr;
	str += strPad + "POS:" + screenPosStr;
	str += strPad + "FPS:" + fpsRealStr;
	str += "[" + fpsTargetStr + "]";

	str += strPad + strPad + "|" + strPad + strPad + "ALT +" + strPad + "[W]_INFO";

	vSyncStr = ofToString(vSync ? "ON " : "OFF");
	str += strPad + "[V]_VSYNC_" + vSyncStr;

	bool bModeFullScreen = false;
	if (ofGetWindowMode() == OF_WINDOW) // Is full screen
	{
		bModeFullScreen = false;
	} else if (ofGetWindowMode() == OF_FULLSCREEN) // Is window mode
	{
		bModeFullScreen = true;
	}
	screenMode = "[F]_";
	screenMode += bModeFullScreen ? "FULLSCREEN_MODE" : "WINDOW_MODE";
	str += strPad + screenMode;

	str += strPad + "[L]_" + ofToString(bDisableAutoSave ? "NO_SAVE  " : "AUTO_SAVE");

#ifdef SURFING_USE_STAY_ON_TOP
	str += strPad + "[T]_" + ofToString(bWindowStayOnTop ? "ON_TOP:TRUE " : "ON_TOP:FALSE");
#endif

	str += strPad + "[D]_DEBUG_" + ofToString(bShowDebug ? "ON " : "OFF");
	str += strPad;

	// debug mods
	//str += strPad + "  ";
	//str += " " + ofToString(mod_ALT ? "ALT" : "   ");
	//str += " " + ofToString(mod_CONTROL ? "CTRL" : "    ");
	//str += " " + ofToString(mod_SHIFT ? "SHIFT" : "     ");
	//str += " " + ofToString(mod_COMMAND ? "CMD" : "   ");

	//--

	if (font.isLoaded()) {
#if 1
		// A.tiny squared
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
		// B. rounded text box
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

	bool bPreShow; // starts draw black
	bool bAlert; // draws red
	float fpsThreshold;

	//// A. Thresholds by factor fps between target and real fps
	//fpsThreshold = 0.9f; // below this trigs alert red state
	//bPreShow = (fpsReal < fpsTarget*0.999); // by ratio
	//bAlert = (fpsReal < fpsTarget*fpsThreshold); // A. by ratio

	// B. Thresholds by absolute fps difference between desired target and real fps
	// monitor fps performance
	fpsThreshold = 10.f; // num frames below this trigs alert red state
	bPreShow = (fpsReal < fpsTarget - 5); // below 5 fps starts showing bar
	bAlert = (fpsReal < (fpsTarget - fpsThreshold)); // B. by absolute fps diff

	//-

	// Bar

	if (bPreShow) // to draw only under pre threshold
	{
		float fx, fy, fw, fh, fwMax, fPad;
		fwMax = 100.f; // max width

		if (font.isLoaded()) {
			fh = fontSize + previewPad - 4;
			//fh = 10.f;
			fPad = 4.f; //previewPad to border
		} else {
			fh = 10.f;
			fPad = 5.f; //previewPad to border
		}

		fy = previewY - fh + 1.0f; // to the border
		//fy = previewY - fh - 50.0f;// little air

		// position
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

		// fps label
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

			fy = bb.getY();
			fh = bb.getHeight() - 2;
		} else {
			ofDrawBitmapStringHighlight(diff, fx - 68.f, previewY, cAlert, ofColor(255)); // text fps diff
		}

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
void ofxWindowApp::keyPressed(ofKeyEventArgs & eventArgs) {
	if (!bKeys) return;

	const int & key = eventArgs.key;

	// modifiers
	mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND); // macOS
	mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL); // Windows. not working
	mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

	if (!mod_ALT) return;

	//--

	ofLogVerbose("ofxWindowApp::keyPressed") << "'" << (char)key << "' [" << key << "]";

	// Draw info
	if (key == 'W') {
		bShowInfo = !bShowInfo;
		ofLogNotice("ofxWindowApp") << "changed draw info: " << (bShowInfo ? "ON" : "OFF");
	}

	// Draw debug
	else if (key == 'D') {
		bShowDebug = !bShowDebug;
		ofLogNotice("ofxWindowApp") << "changed draw debug: " << (bShowInfo ? "ON" : "OFF");
	}

	// Switch window mode
	else if (key == 'F') {
		doApplyToggleWindowMode();
	} else if (key == 'V') // switch v-sync mode
	{
		vSync = !vSync;
		ofSetVerticalSync(vSync);
	}

	else if (key == 'L') // toggle disableSave
	{
		bDisableAutoSave = !bDisableAutoSave;
	}

#ifdef SURFING_USE_STAY_ON_TOP
	else if (key == 'T') {
		setToggleStayOnTop();
	}
#endif

	else if (key == OF_KEY_BACKSPACE) {
		doResetWindowSettings();
	}

	else {
		ofxSurfingHelpersLite::ofxWindowApp::keyPressedToSetWindowShape(key);
		// set some custom common sizes:
		// instagram, portrait, landscape, squared etc
		//s += "PRESETS\n";
		//s += "Q : Squared 800 x 800\n";
		//s += "q : Squared W x W\n";
		//s += "1 : IGTV Cover Photo\n";
		//s += "2 : IG Landscape Photo\n";
		//s += "3 : IG Portrait\n";
		//s += "4 : IG Story\n";
		//s += "5 : IG Square\n";
		//s += "BKSP : Reset default\n";
	}
}

//--------------------------------------------------------------
void ofxWindowApp::keyReleased(ofKeyEventArgs & eventArgs) {
	if (!bKeys) return;

	const int & key = eventArgs.key;

	// Release modifiers
	mod_COMMAND = key == OF_KEY_COMMAND; // macOS
	mod_CONTROL = key == OF_KEY_CONTROL; // Windows. not working
	mod_ALT = key == OF_KEY_ALT;
	mod_SHIFT = key == OF_KEY_SHIFT;
}

//--------------------------------------------------------------
void ofxWindowApp::folderCheckAndCreate(string _path) {
	ofLogNotice("ofxWindowApp") << "folderCheckAndCreate(). Path: " << _path;
	if (_path == "") return;

	ofDirectory dataDirectory(ofToDataPath(_path, true));

	// Check if target data folder exist
	if (!dataDirectory.isDirectory()) {
		ofLogError("ofxWindowApp") << "folderCheckAndCreate(). FOLDER DOES NOT EXIST!";

		// Create folder
		bool b = dataDirectory.createDirectory(ofToDataPath(_path, true));

		// Debug if creation has been succed
		if (b)
			ofLogNotice("ofxWindowApp") << "FOLDER '" << _path << "' CREATED SUCCESSFULLY!";
		else
			ofLogError("ofxWindowApp") << "UNABLE TO CREATE '" << _path << "' FOLDER!";
	}
}

//--------------------------------------------------------------
void ofxWindowApp::doApplyToggleWindowMode() {
	ofLogNotice("ofxWindowApp") << "doApplyToggleWindowMode()";

	// Toggle Mode
	if (ofGetWindowMode() == OF_WINDOW) {
		ofSetFullscreen(true);
	} else if (ofGetWindowMode() == OF_FULLSCREEN) {
		ofSetFullscreen(false);
	}

	doGetWindowSettings();
}

//--------------------------------------------------------------
void ofxWindowApp::doApplyWindowExtraSettings() {
	ofLogNotice("ofxWindowApp") << "doApplyWindowExtraSettings()";

	ofLogVerbose("ofxWindowApp") << "FpsTarget: " << fpsTarget;
	ofLogVerbose("ofxWindowApp") << "vSync: " << vSync;

	ofSetFrameRate(int(fpsTarget.get()));
	ofSetVerticalSync(vSync.get());

	logSettings();
}

//--------------------------------------------------------------
void ofxWindowApp::doApplyWindowSettings() {
	ofLogNotice("ofxWindowApp") << "doApplyWindowSettings()";

	//TODO
	ofSetWindowPosition(windowSettings.getPosition().x, windowSettings.getPosition().y);
	ofSetWindowShape(windowSettings.getWidth(), windowSettings.getHeight());

	// full screen
	if (bIsFullScreen) {
		ofSetFullscreen(true);
	}

	// window mode
	else {
		ofSetFullscreen(false);
	}

	logSettings();
}

//--------------------------------------------------------------
void ofxWindowApp::Changed_ParamsExtra(ofAbstractParameter & e) {
	if (!bDoneSetup) return;
	if (!bDoneStartup) return;

	//--

	string name = e.getName();
	ofLogNotice("ofxWindowApp::Changed") << " " << name << " : " << e;

#ifdef SURFING_USE_STAY_ON_TOP
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
	#else
		ofLogError("ofxWindowApp") << "Not implemented for current platform. Only TARGET_WIN32 yet!";
	#endif
		return;
	}
#endif

	if (name == vSync.getName()) {
		bFlagToSave = true;
		return;
	}

	if (name == fpsTarget.getName()) {
		bFlagToSave = true;
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
		s1 += "    \t\t  " + ofToString(width) + "x" + ofToString(height);
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
		//canvas rect
		if (i == 0)
			monitorsCanvasRect.set(monitorRect);
		else
			monitorsCanvasRect.growToInclude(monitorRect);

		//monitor rect
		//check if pos is inside the ofRectangle
		if (monitorRect.inside(pos)) {
			ofSetColor(255, 255, 0, a); //window app is on this monitor
		} else {
			ofSetColor(255, 255, 255, a);
		}
		ofNoFill();
		ofDrawRectangle(monitorRect);

		//label
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

		//label center num
		ofPushMatrix();
		ofTranslate(monitorRect.getCenter());
		s = ofToString(i);
		ofDrawBitmapString(s, 0, 0);
		ofPopMatrix();

		i++;
	}

	//app window
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

	//label center name
	ofPushMatrix();
	ofTranslate(rw.getCenter());
	s = "App\nWindow";
	ofDrawBitmapString(s, 0, 0);
	ofPopMatrix();

	//canvas
	ofNoFill();
	//monitorsCanvasRect.scaleFromCenter(1.1f);
	//ofSetColor(0, 0, 0, a);
	//ofDrawRectangle(monitorsCanvasRect); //canvas rect
	ofPushMatrix();
	ofTranslate(monitorsCanvasRect.getBottomLeft());

	//monitor names list
	s = "\n\n\nSYSTEM DISPLAYS\n\n";
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
