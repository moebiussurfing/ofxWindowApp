#pragma once

#include "ofMain.h"
#include "ofxSerializer.h"

//TODO:
//+++ add screenSetup addon to bundle all features
//+ add windowResize subscribed listener to auto refresh

class ofxWindowApp : public ofBaseApp
{

public:

	ofxWindowApp();
	~ofxWindowApp();

	//-

	//API

	void setup();
	void update(ofEventArgs & args);
	void draw(ofEventArgs & args);
	void windowResized(int w, int h);

	//-

	void saveWindow();
	void loadWindow();

	void setPathFolder(string s)
	{
		path_folder = s;
	}

	void setPathFilename(string s)
	{
		path_filename = s;
	}

	void setAutoSaveLoad(bool b)
	{
		autoSaveLoad = b;
	}
	
	void setShowDebug(bool b = true)
	{
		ENABLE_Debug = b;
	}
	
	//-

	//callback to check if some settings have changed
	bool isChanged()
	{
		if (bChanged)
		{
			bChanged = false;
			return true;
		}

		else
			return false;
	}

	//setters. required when used for first time into your project
	void setSettingsFps(float f)
	{
		targetFps = f;
		ofSetFrameRate(targetFps);
	}
	void setSettingsVsync(bool b)
	{
		vSync = b;
		ofSetVerticalSync(vSync);
	}
	
	//-

	void applySettings()
	{
		ofLogNotice("ofxWindowApp") << "applySettings()";
		ofLogNotice("ofxWindowApp") << "targetFps: " << targetFps;
        ofLogNotice("ofxWindowApp") << "vSync: " << vSync;
        ofLogNotice("ofxWindowApp") << "SHOW DEBUG: " <<ENABLE_Debug.get();

		ofSetFrameRate(targetFps);
		ofSetVerticalSync(vSync);
	}

	void setDEBUG_Position(int POS)
	{
		layout_DEBUG_Position = POS;
	}

	void setEnableKeys(bool b)
	{
		ENABLE_Keys = b;
	}

	//-

	//getters
	float getSettingsFps()
	{
		return targetFps.get();
	}
	bool getSettingsVsync()
	{
		return vSync.get();
	}
	float getWindowWidth()
	{
		return window_W;
	}
	float getWindowHeight()
	{
		return window_H;
	}
	float getWindowPositionX()
	{
		return window_X;
	}
	float getWindowPositionY()
	{
		return window_Y;
	}

	//-

private:

	enum DEBUG_Position
	{
		DEBUG_TOP = 0,
		DEBUG_BOTTOM
	};
	int layout_DEBUG_Position = DEBUG_BOTTOM;

	//default folders
	//this is to folder all files to avoid mixing with other addons data
	string path_folder = "ofxWindowApp/";//maybe should create this folder manually into project /data
	string path_filename = "AppWindow.json";
	//string path_filename2 = "AppWindow_Extra.json";

	ofParameter<int> window_W, window_H, window_X, window_Y;

	bool autoSaveLoad = true;
	bool bChanged = false;

	ofParameterGroup params_Settings{ "extra settings" };
	ofParameter<bool> vSync{ "vsync", false };
	ofParameter<float> targetFps{ "targetFps", 60.5, 1, 120 };
    ofParameter<bool> ENABLE_Debug{"debug", true};
	//TODO: add full screen/window bool

    void drawDEBUG();

	bool ENABLE_Keys = true;//keys enabled by default
    void keyPressed(ofKeyEventArgs &eventArgs);
};


