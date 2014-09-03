#pragma once
/* kashim */
#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxDelaunay.h"

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void exit();
		void generatedMesh();
    		ofxKinect kinect;
		ofEasyCam cam;
		bool showGui;
		int colorAlpha;
		float noiseAmount;
		int pointSkip;
		int angle;
  		ofMesh convertedMesh;
		ofMesh wireframeMesh;
		ofxDelaunay del;
		ofImage blob;
};
