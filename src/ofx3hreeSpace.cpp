#include "ofx3hreeSpace.h"

ofx3hreeSpace::ofx3hreeSpace() {}
ofx3hreeSpace::~ofx3hreeSpace() {}

void ofx3hreeSpace::setThresholds(unsigned int myThresholdPixelIsChanged, float myThresholdMovement)
{
    thresholdPixelIsChanged = myThresholdPixelIsChanged;
    thresholdMovement = myThresholdMovement;
    cout << "3hreeSpace// threshold defining a changed pixel: " << thresholdPixelIsChanged << "; threshold for the minimum amount of movement: " << thresholdMovement << endl;
}

void ofx3hreeSpace::setNumberAreasOfMovement(unsigned int myNumberAreasOfMovement)
{
    // setting the size of areas of movement for each dimension (x,y,z)
    numberAreasOfMovement = myNumberAreasOfMovement;
    cout << "3hreeSpace// areas of movement for each dimension set to: " << numberAreasOfMovement << ";" << endl;
}

void ofx3hreeSpace::setEqualParameter(vector<float>& myEqualParameterFrontCamera, vector<float>& myEqualParameterTopCamera)
{
    // setting the values for rectification (equalization) for each layer
    cout << "3hreeSpace// values for rectification (equalization): camera front -";
    equalParametersFront.resize(numberAreasOfMovement);
    for (unsigned int i = 0; i < equalParametersFront.size(); i++)
    {
        cout << " ";
        equalParametersFront[i] = myEqualParameterFrontCamera[i];
        cout << equalParametersFront[i];
    }
    cout << " & camera top - ";
    equalParametersTop.resize(numberAreasOfMovement);
    for (unsigned int i = 0; i < equalParametersTop.size(); i++)
    {
        cout << " ";
        equalParametersTop[i] = myEqualParameterTopCamera[i];
        cout << equalParametersTop[i];
    }
    cout << ";" << endl;
}

void ofx3hreeSpace::setTrackingArea(unsigned int myCameraFrontWidth, unsigned int myCameraFrontHeight, float myTrackingAreaSizeCameraFront, unsigned int myCameraTopWidth, unsigned int myCameraTopHeight, float myTrackingAreaSizeCameraTop)
{
    // setting the size of each tracking area
    trackingAreaSizeCameraFront = int(myCameraFrontHeight*myTrackingAreaSizeCameraFront);
    trackingAreaSizeCameraTop = int(myCameraTopHeight*myTrackingAreaSizeCameraTop);
    // setting the upper left corners of each tracking area
    cornersTrackingArea.resize(4);
    cornersTrackingArea[0] = (myCameraFrontWidth - trackingAreaSizeCameraFront)/2;
    cornersTrackingArea[1] = (myCameraFrontHeight - trackingAreaSizeCameraFront)/2;
    cornersTrackingArea[2] = (myCameraTopWidth - trackingAreaSizeCameraTop)/2;
    cornersTrackingArea[3] = (myCameraTopHeight - trackingAreaSizeCameraTop)/2;
    // setting the width of a single area of movement
    areaOfMovementSizeCameraFront = trackingAreaSizeCameraFront/numberAreasOfMovement;
    areaOfMovementSizeCameraTop = trackingAreaSizeCameraTop/numberAreasOfMovement;
    // setting the offset for each layer of rectification (equalization) from the borders of the tracking area
    equalLayerStartCameraFront.resize(numberAreasOfMovement);
    for (unsigned int i = 0; i < equalLayerStartCameraFront.size(); i++)
    {
        equalLayerStartCameraFront[i] = int((trackingAreaSizeCameraFront*equalParametersFront[i])/2);
    }
    equalLayerStartCameraTop.resize(numberAreasOfMovement);
    for (unsigned int i = 0; i < equalLayerStartCameraFront.size(); i++)
    {
        equalLayerStartCameraTop[i] = int((myTrackingAreaSizeCameraTop*equalParametersTop[i])/2);
    }
    //setting up the vectors for storing the values of changed pixels (= movement)
    valuesFront.resize(numberAreasOfMovement*numberAreasOfMovement*numberAreasOfMovement);
    resetTrackingValues(valuesFront);
    valuesTop.resize(numberAreasOfMovement*numberAreasOfMovement*numberAreasOfMovement);
    resetTrackingValues(valuesTop);
    valuesFrontTop.resize(numberAreasOfMovement*numberAreasOfMovement*numberAreasOfMovement);
    resetTrackingValues(valuesFrontTop);
    // Control
    cout << "3hreeSpace// set size of tracking area front: " << trackingAreaSizeCameraFront << "px & top: " << trackingAreaSizeCameraTop << "px;" << endl;
    cout << "3hreeSpace// ready for tracking" << endl;
}

void ofx3hreeSpace::resetTrackingValues(vector<float>& values)
{
    for (unsigned int i = 0; i < values.size(); i++)
    {
        values[i] = 0.0;
    }
}

void ofx3hreeSpace::updateValues3hreeSpace(vector<float>& myTrackingValues, ofPixels& myCameraFrontPixels, ofPixels& myCameraTopPixels)
{
    // extract the pixels used for tracking
    myCameraFrontPixels.crop(cornersTrackingArea[0],cornersTrackingArea[1],trackingAreaSizeCameraFront, trackingAreaSizeCameraFront);
    myCameraTopPixels.crop(cornersTrackingArea[2],cornersTrackingArea[3],trackingAreaSizeCameraTop, trackingAreaSizeCameraTop);
    // preparing the values for the current values; default is 0.0 = no movement
    resetTrackingValues(valuesFrontTop);
    // looking for changed pixels in each area of movement for each layer of rectification (equalization)
    if (previousPixelsCameraFront.size() > 0 && previousPixelsCameraTop.size() > 0) // checking the pixel array size to avoid using the first (empty) pixel array
    {
        resetTrackingValues(valuesFront);
        for (unsigned int i = equalLayerStartCameraFront[0]*trackingAreaSizeCameraFront*3; i < (trackingAreaSizeCameraFront*trackingAreaSizeCameraFront*3) - (equalLayerStartCameraFront[0]*trackingAreaSizeCameraFront*3); i+=3)
        {
            if (movementDetected(myCameraFrontPixels[i], myCameraFrontPixels[i+1], myCameraFrontPixels[i+2], previousPixelsCameraFront[i], previousPixelsCameraFront[i+1], previousPixelsCameraFront[i+2]))
            {
                addValuesOfTracking(i, trackingAreaSizeCameraFront, valuesFront, equalLayerStartCameraFront, equalParametersFront);
            }
        }
        equalizeValues(valuesFront, areaOfMovementSizeCameraFront, equalParametersFront);
        resetTrackingValues(valuesTop);
        for (unsigned int i = equalLayerStartCameraTop[0]*trackingAreaSizeCameraTop*3; i < (trackingAreaSizeCameraTop*trackingAreaSizeCameraTop*3) - (equalLayerStartCameraTop[0]*trackingAreaSizeCameraTop*3); i+=3)
        {
            if (movementDetected(myCameraTopPixels[i], myCameraTopPixels[i+1], myCameraTopPixels[i+2], previousPixelsCameraTop[i], previousPixelsCameraTop[i+1], previousPixelsCameraTop[i+2]))
            {
                addValuesOfTracking(i, trackingAreaSizeCameraTop, valuesTop, equalLayerStartCameraTop, equalParametersTop);
            }
        }
        equalizeValues(valuesTop, areaOfMovementSizeCameraTop, equalParametersTop);
        turnValues(valuesTop, numberAreasOfMovement);
        combineValuesFrontTop(valuesFrontTop, valuesFront, valuesTop);
    }
    myTrackingValues = valuesFrontTop;
    // copy current pixel arrays for comparing in the next loop
    previousPixelsCameraFront = myCameraFrontPixels;
    previousPixelsCameraTop = myCameraTopPixels;
}

bool ofx3hreeSpace::movementDetected(int redCurrent, int greenCurrent, int blueCurrent, int redPrevious, int greenPrevious, int bluePrevious)
{
    return ABS(redCurrent - redPrevious) > thresholdPixelIsChanged || ABS(blueCurrent - bluePrevious) > thresholdPixelIsChanged || ABS(greenCurrent - greenPrevious) > thresholdPixelIsChanged;
}

void ofx3hreeSpace::addValuesOfTracking(unsigned int indexPixels, unsigned int trackingAreaWidthHeight, vector<float>& values, vector<unsigned int> equalLayerValues, vector<float>& equalParameters)
{
    // calculating the xy-Position of the current Pixel based on its index
    unsigned int xPixel = (indexPixels % (trackingAreaWidthHeight*3))/3;
    unsigned int yPixel = indexPixels / (trackingAreaWidthHeight*3);
    unsigned int xyEqualPixels, indexValue;
    // checking each pixel for change (= movement); up to three times for a pixel, depending on of which layer of rectification (equalization) it is part
    for (unsigned int i = 0; i < numberAreasOfMovement; i++)
    {
        if (xPixel > equalLayerValues[i] && xPixel < (trackingAreaWidthHeight-equalLayerValues[i]) &&  yPixel > equalLayerValues[i] && yPixel < (trackingAreaWidthHeight-equalLayerValues[i]))
        {
            xyEqualPixels = (trackingAreaWidthHeight*(1-equalParameters[i]))/numberAreasOfMovement;
            indexValue = floor((xPixel-equalLayerValues[i]) / xyEqualPixels) + (floor((yPixel-equalLayerValues[i]) / xyEqualPixels))*numberAreasOfMovement;
            values[indexValue+(numberAreasOfMovement*numberAreasOfMovement*i)] = values[indexValue+(numberAreasOfMovement*numberAreasOfMovement*i)] + 1;
        }
    }
}

void ofx3hreeSpace::equalizeValues(vector<float>& values, unsigned int areasOfMovementWidthHeight, vector<float>& equalParameters)
{
    for (unsigned int i = 0; i < numberAreasOfMovement*numberAreasOfMovement; i++)
    {
        for (unsigned int u = 0; u < numberAreasOfMovement; u++)
        {
            values[i+(numberAreasOfMovement*numberAreasOfMovement*u)] = values[i+(numberAreasOfMovement*numberAreasOfMovement*u)]/(areasOfMovementWidthHeight*(1-equalParameters[u])*areasOfMovementWidthHeight*(1-equalParameters[u]));
        }
    }
}

void ofx3hreeSpace::turnValues(vector<float>& values, unsigned int areasOfMovementInOneDimension)
{
    // creating a temporary vector to avoid direct manipulation of the ingoing vector
    vector<float> valuesTemp;
    valuesTemp.resize(numberAreasOfMovement*numberAreasOfMovement*numberAreasOfMovement);
    // currently only a sorting method for 3 areas of movement per dimension exists, therefore the if condition
    if (areasOfMovementInOneDimension == 3)
    {
        for (unsigned int i = 0; i < numberAreasOfMovement; i++)
        {
            unsigned int idxTurned = (i*numberAreasOfMovement*numberAreasOfMovement) + 2;
            unsigned int idxOld = i*numberAreasOfMovement;
            for (unsigned int u = 0; u < numberAreasOfMovement*numberAreasOfMovement; u += 3)
            {
                for (unsigned int v = 0; v < numberAreasOfMovement; v++)
                {
                    valuesTemp[idxTurned+u-v] = values[idxOld+(u*numberAreasOfMovement)+v];
                }
            }
        }
    }
    else
    {
        cout << "A different sorting function is needed for " << areasOfMovementInOneDimension << " areas per dimension. Take a look at the turnValues() function." << endl;
        // !!!PLACE FOR AN OTHER SORTING FUNCTION FOR A DIFFERENT NUMBER OF TRACKING AREAS IN ONE DIMENSION
    }
    values = valuesTemp;
}

void ofx3hreeSpace::combineValuesFrontTop(vector<float>& resultingValues, vector<float>& inputOneValues, vector<float>& inputTwoValues)
{
    for (unsigned int i = 0; i < numberAreasOfMovement*numberAreasOfMovement*numberAreasOfMovement; i++)
    {
        if (inputOneValues[i] > thresholdMovement && inputTwoValues[i] > thresholdMovement)
        {
            float value = ((inputOneValues[i]+inputTwoValues[i])*0.5);
            resultingValues[i] = value;
        }
    }
}
