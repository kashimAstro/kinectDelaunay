#include "testApp.h"

bool realTime = false;

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(60);
    kinect.init();
    kinect.open();
    kinect.setRegistration(true);
    blob.allocate(640,480,OF_IMAGE_COLOR);//OF_IMAGE_GRAYSCALE);

    noiseAmount = 0;  //
    pointSkip   = 1;  //
    colorAlpha  = 255;//

    showGui = true;
    kinect.setCameraTiltAngle(0);
    ofSetVerticalSync(true);
    ofEnableDepthTest();
}

void testApp::generatedMesh(){
   if(kinect.isFrameNew()) {
	del.reset();
	unsigned char* pix = new unsigned char[640*480];
	unsigned char* gpix = new unsigned char[640*480];
	for(int x=0;x<640;x+=1) {
           for(int y=0;y<480;y+=1) {
		float distance = kinect.getDistanceAt(x, y);
		int pIndex = x + y * 640;
		pix[pIndex] = 0;
		if(distance > 100 && distance < 1100) { // 
			pix[pIndex] = 255;
		}
	   }
	}
	blob.setFromPixels(pix, 640, 480, OF_IMAGE_GRAYSCALE);
	int numPoints = 0;
	for(int x=0;x<640;x+=pointSkip) {
	   for(int y=0;y<480;y+=pointSkip) {
		int pIndex = x + 640 * y;
		if(blob.getPixels()[pIndex]> 0) {
			ofVec3f wc = kinect.getWorldCoordinateAt(x, y);
			wc.x = x - 320.0;
	                wc.y = y - 240.0;
			if(abs(wc.z) > 100 && abs(wc.z ) < 2000) { //  
				wc.z = -wc.z;
				wc.x = ofClamp(wc.x, -320,320);
				wc.y = ofClamp(wc.y, -240,240);
				del.addPoint(wc);
			}
			numPoints++;
		}
	    }
	}

	if(numPoints >0)
		del.triangulate();
	for(int i=0;i<del.triangleMesh.getNumVertices();i++) { del.triangleMesh.addColor(ofColor(0,0,0)); }
	for(int i=0;i<del.triangleMesh.getNumIndices()/3;i+=1) {
		ofVec3f v = del.triangleMesh.getVertex(del.triangleMesh.getIndex(i*3));
		v.x = ofClamp(v.x, -319,319);
		v.y = ofClamp(v.y, -239, 239);
		ofColor c = kinect.getColorAt(v.x+320.0, v.y+240.0);
			c.a = colorAlpha;
			del.triangleMesh.setColor(del.triangleMesh.getIndex(i*3),c);
			del.triangleMesh.setColor(del.triangleMesh.getIndex(i*3+1),c);
			del.triangleMesh.setColor(del.triangleMesh.getIndex(i*3+2),c);
	}

	convertedMesh.clear();
	wireframeMesh.clear();
	wireframeMesh.setMode(OF_PRIMITIVE_TRIANGLES);

	for(int i=0;i<del.triangleMesh.getNumIndices()/3;i+=1) {
		int indx1 = del.triangleMesh.getIndex(i*3);
		ofVec3f p1 = del.triangleMesh.getVertex(indx1);
		int indx2 = del.triangleMesh.getIndex(i*3+1);
		ofVec3f p2 = del.triangleMesh.getVertex(indx2);
		int indx3 = del.triangleMesh.getIndex(i*3+2);
		ofVec3f p3 = del.triangleMesh.getVertex(indx3);
		ofVec3f triangleCenter = (p1+p2+p3)/3.0;
		triangleCenter.x += 320;
		triangleCenter.y += 240;
		triangleCenter.x = floor(ofClamp(triangleCenter.x, 0,640));
		triangleCenter.y = floor(ofClamp(triangleCenter.y, 0,480));
		int pixIndex = triangleCenter.x + triangleCenter.y * 640;
		if(pix[pixIndex] > 0) {
			convertedMesh.addVertex(p1);
			convertedMesh.addColor(del.triangleMesh.getColor(indx1));
			convertedMesh.addVertex(p2);
			convertedMesh.addColor(del.triangleMesh.getColor(indx2));
			convertedMesh.addVertex(p3);
			convertedMesh.addColor(del.triangleMesh.getColor(indx3));
			wireframeMesh.addVertex(p1);
			wireframeMesh.addVertex(p2);
			wireframeMesh.addVertex(p3);
		}
	}
	delete pix;
	delete gpix;
    }
}

//--------------------------------------------------------------
void testApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    kinect.update();
    if(realTime){
         pointSkip = 3*2;
         generatedMesh();
    }
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackgroundGradient(0,5);

	glEnable(GL_DEPTH_TEST);
	ofPushMatrix();
	cam.begin();
	cam.setScale(1,-1,1);
	ofSetColor(255,255,255);
	ofTranslate(0, -80,1100);
	ofFill();
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glShadeModel(GL_FLAT);
	glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
	convertedMesh.drawFaces();
	glShadeModel(GL_SMOOTH);
	glPopAttrib();
	ofPushMatrix();
	ofTranslate(0, 0,0.5);
	ofSetColor(0,0,0, 255);
        wireframeMesh.drawWireframe();
	ofPopMatrix();
 	
	cam.end();
	ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'f')	
           ofToggleFullscreen();
	if(key == 'e')
		wireframeMesh.save("mesh.ply");
	if(key == 's')
		ofSaveFrame();
	if(key == OF_KEY_UP){
                angle++;
        	if(angle>30) angle=30;
                kinect.setCameraTiltAngle(angle);
	}
        if(key == OF_KEY_DOWN){
                angle--;
                if(angle<-30) angle=-30;
                kinect.setCameraTiltAngle(angle);
	}
        if(key == 'g'){
		pointSkip  = 1;
		generatedMesh();
	}
	if(key == 'r')
		realTime=!realTime;
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

void testApp::exit() {
	kinect.setCameraTiltAngle(0);
	kinect.close();
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
