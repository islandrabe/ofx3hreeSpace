#pragma once

#include "ofMain.h"

class ofx3hreeSpace
{
public:

//ofx3hreeSpace provides a method for tracking moving people and things in space by computing changes in RGB values of pixels from two camera images
    ofx3hreeSpace();
    ~ofx3hreeSpace();

// public function to adjust the tracking; for the camera setup (positioning and orientation) and the difference between "tracking area" and "areas of movement" take a look at the attached sketch
    // myThresholdPixelIsChanged defines the minimum difference a RGB-value must have to be detected as changed; myThresholdMovement defines the minimum amount of changed pixels (as float from 0.0-1.0) in an area of tracking to define it as active
    void setThresholds(unsigned int myThresholdPixelIsChanged, float myThresholdMovement);
    // define the number of areas of movement for each dimension of the tracking area; currently only 3 areas of movement (layers) are supported -> see turnValues()
    void setNumberAreasOfMovement(unsigned int myNumberAreasOfMovement);
    unsigned int getNumberAreasOfMovement() {
        return numberAreasOfMovement;
    }
    // define the values for rectification (equalization) depending on the amount of layers per dimension (= myNumberAreasOfMovement); for each camera a vector with the size of myNumberAreasOfMovement is used -> [0] holds the value for the closest layer to the camera/the smallest value, [1] the second closest and so on;
    void setEqualParameter(vector<float>& myEqualParameterFrontCamera, vector<float>& myEqualParameterTopCamera);
    // define the whole tracking area for each perspective depending on the width and hight of the camera images by setting the width=height of the tracking area as number from 0.0 - 1.0 in relation to the camera height; the tracking area is a square and the center of the tracking area is the center of the camera image;
    void setTrackingArea(unsigned int myCameraFrontWidth, unsigned int myCameraFrontHeight, float myTrackingAreaSizeCameraFront, unsigned int myCameraTopWidth, unsigned int myCameraTopHeight, float myTrackingAreaSizeCameraTop);
    // get the current active areas of movement (as values from 0.0-1.0 -> myTrackingValues) by pushing in the camera images as ofPixels
    void updateValues3hreeSpace(vector<float>& myTrackingValues, ofPixels& myCameraFrontPixels, ofPixels& myCameraTopPixels);

private:

// Values that need to be set
    // the minimum difference a RGB-value must have to be detected as changed (for calculating the difference see movementDetected())
    int thresholdPixelIsChanged;
    // the minimum amount of changed pixels (as float from 0.0-1.0) in an area of tracking to define it as active (on how the values get equalized take a look at equalizeValues())
    float thresholdMovement;
    // values for rectification (equalization) depending on the amount of layers per dimension;
    vector<float> equalParametersFront, equalParametersTop;
    // number of single areas of movement in one dimension of the whole tracking area; currently only 3 areas of movement are supported (to make changes take a look at turnValues());
    unsigned int numberAreasOfMovement;

// Values used in the background
    // values holding the upper left corner (CamFrontX, CamFrontY, CamTopX, CamTopY) of the tracking area
    vector <unsigned int> cornersTrackingArea;
    // size (width=height) of the tracking area for each camera; size (width=height) of an area of movement for each tracking area
    unsigned int trackingAreaSizeCameraFront, trackingAreaSizeCameraTop, areaOfMovementSizeCameraFront, areaOfMovementSizeCameraTop;
    // holding the values for rectification (equalization) for each camera
    vector<unsigned int> equalLayerStartCameraFront, equalLayerStartCameraTop;
    // storage for the pixels of each camera image to compare the previous pixels with the current
    ofPixels previousPixelsCameraFront, previousPixelsCameraTop;
    // storage for the values of changed pixels; each with the size of myNumberAreasOfMovement*myNumberAreasOfMovement*myNumberAreasOfMovement as values for each layer of rectification (equalization) get calculated
    vector<float> valuesFront, valuesTop, valuesFrontTop;

// Function for image processing and combining the values the
    // resets the values of the vector to 0.0 = defining an inactive area of movement
    void resetTrackingValues(vector<float>& values);
    // sorts the pixels (changed/unchanged) to each layer of rectification (equalization)
    void addValuesOfTracking(unsigned int indexPixels, unsigned int trackingAreaWidthHeight, vector<float>& values, vector<unsigned int> equalLayerValues, vector<float>& equalParameters);
    // equalizes the amount of changed pixels in one area of movement according to its size (width=height) from 0.0 - 1.0
    void equalizeValues(vector<float>& values, unsigned int areasOfMovementWidthHeight, vector<float>& equalParameters);
    // changes the order of the values in a vector (here: valuesTop) to make them fit to the corresponding values in the other vector (here: valuesFront); currently only a sorting function for 3 areas of movement per dimension exists
    void turnValues(vector<float>& values, unsigned int areasOfMovementInOneDimension);
    // combines the values for each area of movement in 3D if there are changed pixels in both corresponding areas of movement in front and top perspective
    void combineValuesFrontTop(vector<float>& resultingValues, vector<float>& inputOneValues, vector<float>& inputTwoValues);
    // defines a changed pixel (= movement) by comparing its RGB-values with the foregoing color information of the pixel at the given position
    bool movementDetected(int redCurrent, int greenCurrent, int blueCurrent, int redPrevious, int greenPrevious, int bluePrevious);

};
