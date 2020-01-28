#pragma once

#include "ofMain.h"
#include "ofxSerializer.h"

//TODO:
//add more control


class ofxWindowApp : public ofBaseApp
{

public:

	ofxWindowApp();
	~ofxWindowApp();

	string path_folder = "ofxWindowApp/";//this is to folder all files to avoid mixing with other addons data
	string path_filename = "AppWindow.json";

	void setup();
	void update(ofEventArgs & args);
	void draw(ofEventArgs & args);

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

	bool autoSaveLoad = true;
	void setAutoSaveLoad(bool b)
	{
		autoSaveLoad = b;
	}

	//TODO:
	//WORKAROUND bad window positioning..
	//callback
	bool bChanged = false;
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

	//-

	bool ENABLE_Debug = true;
	void drawDEBUG();
	
	bool bVSync;
	string vSyncStr;
	int fps;
	string fpsStr;

	ofParameter<int> window_W, window_H, window_X, window_Y;

};


