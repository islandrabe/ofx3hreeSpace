#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    // setting the basic values for the webcams; high values could result in a blind tracking as the image data gets not sent via USB
    switchPreview = false; // switch the display between the two camera input = tracking areas (true) and the 3D-view (false) by pressing ENTER/RETURN
    frameRate = 5;
    cameraWidth = 320;
    cameraHeight = 240;
    // initializing the webcam streams: 0 = front perspective, 1 = top perspective; for the setup take a look at ofx3hreeSpace-sketch.png;
    // because webcams often use auto exposure that causes the webcam to gain up and down depending on the moving objects; you could use GUVCView (http://guvcview.sourceforge.net/) to set the exposure to manual
    cameraFront.setDeviceID(0);
    cameraFront.setDesiredFrameRate(frameRate);
    cameraFront.initGrabber(cameraWidth,cameraHeight);
    cameraTop.setDeviceID(1);
    cameraTop.setDesiredFrameRate(frameRate);
    cameraTop.initGrabber(cameraWidth,cameraHeight);
    //
    ofSetFrameRate(frameRate);
    // load some of the 3hreeSpace parameters from a xml file; good to have in case of adjusting the setup after compiling
    if ( XML.load(ofToDataPath("settings.xml")))
    {
        cout << "settings.xml loaded!" << endl;
    }
    else
    {
        cout << "settings.xml NOT loaded!" << endl;
        ofExit(0);
    }
    // setting the values according bin/data/settings.xml; for the declaration of the values take a look at ofx3hreeSpace.h
    unsigned int myThresholdPixelIsChanged =    XML.getIntValue("//myThresholdPixelIsChanged");
    float myThresholdMovement =                 XML.getFloatValue("//myThresholdMovement");
    // currently ofx3hreeSpace only supports 3 areas of movement per dimension resulting in 27 areas of movement in the tracking area - for other settings the function turnValues() (in ofx3hreeSpace.cpp) and myEqualParameters (in the settings.xml) need to be adapted
    myNumberAreasOfMovement =                   3;
    // choosing the same values for both camera images, but those could be different; 0.85 means the tracking area is 85% of the camera height
    myTrackingAreaSize =                        0.85;
    // getting myEqualParameters could be done by: either figuring out with the camera image on site or calculating depending on the FOV of the camera (the values in settings.xml were used for a camera saying 60°)
    XML.setTo("//myEqualParameters");
    vector<float> myEqualParameters;
    myEqualParameters.resize(myNumberAreasOfMovement);
    myEqualParameters[0] =                      XML.getFloatValue("front");
    myEqualParameters[1] =                      XML.getFloatValue("mid");
    myEqualParameters[2] =                      XML.getFloatValue("back");
    // set up the tracking in that order of functions; for the declaration of the functions and settings take a look at ofx3hreeSpace.h
    my3hreeSpace.setThresholds(myThresholdPixelIsChanged, myThresholdMovement);
    my3hreeSpace.setNumberAreasOfMovement(myNumberAreasOfMovement);
    my3hreeSpace.setEqualParameter(myEqualParameters, myEqualParameters);
    my3hreeSpace.setTrackingArea(cameraWidth, cameraHeight, myTrackingAreaSize, cameraWidth, cameraHeight, myTrackingAreaSize);
    // initialize the storage for the values of tracking
    myTrackingValues.resize(myNumberAreasOfMovement*myNumberAreasOfMovement*myNumberAreasOfMovement);
    //
    cameraFrontPixels.allocate(cameraWidth, cameraHeight, 3);
    cameraTopPixels.allocate(cameraWidth, cameraHeight, 3);
}

//--------------------------------------------------------------
void ofApp::update()
{
    cameraFront.update();
    cameraTop.update();
    if (cameraFront.isFrameNew() && cameraTop.isFrameNew())
    {
        cameraFrontPixels = cameraFront.getPixelsRef();
        cameraTopPixels = cameraTop.getPixelsRef();
        // sending the pixel arrays for processing to 3hreeSpace and getting the vector myTrackingValues updated with the current values
        my3hreeSpace.updateValues3hreeSpace(myTrackingValues, cameraFrontPixels, cameraTopPixels);
        // print out the current values
        cout << ofGetFrameNum() << " Value of tracking: " << endl;
        for (unsigned int i = 0; i < myTrackingValues.size(); i++)
        {
            cout << i << ": " << myTrackingValues[i] << "; ";
        }
        cout << endl;
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    // the following could be used for adjusting the setup of 3hreeSpace - switch the preview mode by pressing ENTER/RETURN
    if (switchPreview)
    {
        // display the tracking areas (left: front view;  right: top view)
        ofSetColor(255,255,255,255);
        ofImage previewImageFront, previewImageTop;
        // cameraFrontPixels got already cropped by calling updateValues3hreeSpace()
        previewImageFront.setFromPixels(cameraFrontPixels);
        previewImageFront.draw(0,0, ofGetWidth()/2,ofGetHeight());
        // cameraTopPixels got already cropped by calling updateValues3hreeSpace()
        previewImageTop.setFromPixels(cameraTopPixels);
        previewImageTop.mirror(false, true);
        previewImageTop.draw(ofGetWidth()/2,0, ofGetWidth()/2,ofGetHeight());
    }
    else
    {
        //display the single areas of movement depending on the amount of changed pixels in 3D
        unsigned int cubeSize = 40;
        cam.begin();
        ofRotateX(ofRadToDeg(PI));
        ofRotateY(ofRadToDeg(-PI*0.70));
        ofBackground(0);
        for (unsigned int z = 0; z < myNumberAreasOfMovement; z++)
        {
            for (unsigned int y = 0; y < myNumberAreasOfMovement; y++)
            {
                for (unsigned int x = 0; x < myNumberAreasOfMovement; x++)
                {
                    unsigned int currentArea = x+y*myNumberAreasOfMovement+z*myNumberAreasOfMovement*myNumberAreasOfMovement;
                    ofPushMatrix();
                    ofTranslate((cubeSize*z)-(cubeSize*1.5),(cubeSize*y)-(cubeSize*1.0),cubeSize*x);
                    if(myTrackingValues[currentArea] > 0.0)
                    {
                        ofSetColor(255,0,0, int(255*myTrackingValues[currentArea]));
                        ofFill();
                    }
                    else
                    {
                        ofSetColor(255,255,255, 100);
                        ofNoFill();
                    }
                    ofDrawBox(cubeSize);
                    ofPopMatrix();
                }
            }
        }
        cam.end();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    switch(key) {
		case OF_KEY_RETURN:
			switchPreview = !switchPreview;
			break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}
