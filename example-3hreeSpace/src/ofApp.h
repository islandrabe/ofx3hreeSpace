#pragma once

#include "ofMain.h"
#include "ofx3hreeSpace.h"

class ofApp : public ofBaseApp
{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    // setting to initialize two webcams of the same typ
    unsigned int frameRate, cameraWidth, cameraHeight;
    ofVideoGrabber cameraFront, cameraTop;
    // pixel arrays that get pushed on to 3hreeSpace
    ofPixels cameraFrontPixels, cameraTopPixels;
    // initialize ofx3hreeSpace
    ofx3hreeSpace my3hreeSpace;
    // variables for 3hreeSpace
    unsigned int myNumberAreasOfMovement;
    float myTrackingAreaSize;
    // setting the values for rectification (equalization) depending on the used camera (FOV, field of view)
    vector<float> myEqualParameters;
    // storage for the current tracking results from 3hreeSpace
    vector<float> myTrackingValues;
    // display the tracked areas, for adjustment
    bool switchPreview;
    ofImage previewImageFront, previewImageTop;
    ofEasyCam cam;
    // settings stored in an XML-file
    ofXml XML;

};
