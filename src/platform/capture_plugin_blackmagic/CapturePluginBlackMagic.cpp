/*
 * This file is part of ALVAR, A Library for Virtual and Augmented Reality.
 *
 * Copyright 2007-2012 VTT Technical Research Centre of Finland
 *
 * Contact: VTT Augmented Reality Team <alvar.info@vtt.fi>
 *          <http://www.vtt.fi/multimedia/alvar.html>
 *
 * ALVAR is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ALVAR; if not, see
 * <http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>.
 */

#include "CapturePluginBlackMagic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

#include "DeckLinkAPI.h"

#include "cameradecklink.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <sstream>

using namespace std;

namespace alvar {
namespace plugins {

CaptureBlackMagic::CaptureBlackMagic(const CaptureDevice captureDevice)
    : Capture(captureDevice)
    , mChannels(-1)
    , mReturnFrame(NULL)
{
    
    HRESULT						result;
    

    camera = new CameraDecklink();
    deckLinkIterator = CreateDeckLinkIteratorInstance();


    if (!deckLinkIterator)
    {
        fprintf(stderr, "This application requires the DeckLink drivers installed.\n");
    }
    

}

CaptureBlackMagic::~CaptureBlackMagic()
{
    stop();
    delete camera;
    delete deckLinkIterator;
}

bool CaptureBlackMagic::start()
{
    if (isCapturing()) {
        return isCapturing();
    }
    { /// CONNECTING TO ONE CAMERA AND START THE STREAM
        /* Connect to the first DeckLink instance */
        result = deckLinkIterator->Next(&deckLink);
        if (result != S_OK)
        {
            fprintf(stderr, "No DeckLink PCI cards found.\n");
        }

        camera->initializeCamera(deckLink);
    }

   /* stringstream id(captureDevice().id());
    id.setf(ios_base::hex, ios_base::basefield);
    id >> mGUID.value[0]; id.get();
    id >> mGUID.value[1]; id.get();
    id >> mGUID.value[2]; id.get();
    id >> mGUID.value[3];

    if (mCamera->Connect(&mGUID) != FlyCapture2::PGRERROR_OK) {
        return false;
    }

    FlyCapture2::VideoMode videoMode;
    FlyCapture2::FrameRate frameRate;
    if (mCamera->GetVideoModeAndFrameRate (&videoMode, &frameRate) != FlyCapture2::PGRERROR_OK) {
        return false;
    }
    
    if (videoMode == FlyCapture2::VIDEOMODE_640x480RGB) {
      mChannels = 3;
      mXResolution = 640;
      mYResolution = 480;
    
    } else if (videoMode == FlyCapture2::VIDEOMODE_640x480Y8) {
      mChannels = 1;
      mXResolution = 640;
      mYResolution = 480;
    
    } else if (videoMode == FlyCapture2::VIDEOMODE_800x600RGB) {
      mChannels = 3;
      mXResolution = 800;
      mYResolution = 600;
    
    } else if (videoMode == FlyCapture2::VIDEOMODE_800x600Y8) {
      mChannels = 1;
      mXResolution = 800;
      mYResolution = 600;
    
    } else if (videoMode == FlyCapture2::VIDEOMODE_1024x768RGB) {
      mChannels = 3;
      mXResolution = 1024;
      mYResolution = 768;
    
    } else if (videoMode == FlyCapture2::VIDEOMODE_1024x768Y8) {
      mChannels = 1;
      mXResolution = 1024;
      mYResolution = 768;
    
    } else if (videoMode == FlyCapture2::VIDEOMODE_1280x960RGB) {
      mChannels = 3;
      mXResolution = 1280;
      mYResolution = 960;
    
    } else if (videoMode == FlyCapture2::VIDEOMODE_1280x960Y8) {
      mChannels = 1;
      mXResolution = 1280;
      mYResolution = 960;
    
    } else if (videoMode == FlyCapture2::VIDEOMODE_1600x1200RGB) {
      mChannels = 3;
      mXResolution = 1600;
      mYResolution = 1200;
    
    } else if (videoMode == FlyCapture2::VIDEOMODE_1600x1200Y8) {
      mChannels = 1;
      mXResolution = 1600;
      mYResolution = 1200;
    
    } else {
        return false;
    }
    */
    mReturnFrame = cvCreateImage(cvSize(mXResolution, mYResolution), IPL_DEPTH_8U, mChannels);
    if (mCamera->StartCapture() != FlyCapture2::PGRERROR_OK) {
        return false;
    }
    mIsCapturing = true;
    return isCapturing();
}

void CaptureBlackMagic::stop()
{
    if (isCapturing()) {
        camera->StopCapture();
        cvReleaseImage(&mReturnFrame);
    }
}

IplImage *CaptureBlackMagic::captureImage()
{
    if (!isCapturing()) {
        return NULL;
    }
    camera->readIplImage(mReturnFrame);

   /* if (mCamera->RetrieveBuffer(mImage) == FlyCapture2::PGRERROR_OK) {
        unsigned long length = mReturnFrame->widthStep * mYResolution;
        memcpy(mReturnFrame->imageData, mImage->GetData(), length);
    }*/
    return mReturnFrame;
}

bool CaptureBlackMagic::showSettingsDialog()
{
    return false;
}

string CaptureBlackMagic::SerializeId()
{
    return "CaptureBlackMagic";
}

bool CaptureBlackMagic::Serialize(Serialization *serialization)
{
    return false;
}

CapturePluginBlackMagic::CapturePluginBlackMagic(const string &captureType)
    : CapturePlugin(captureType)
{
}

CapturePluginBlackMagic::~CapturePluginBlackMagic()
{
}

CapturePlugin::CaptureDeviceVector CapturePluginBlackMagic::enumerateDevices()
{
    CaptureDeviceVector devices;


    if(deckLinkIterator!=NULL)
    {
        CaptureDevice captureDevice(mCaptureType, "input0", "description_for_input0");
        devices.push_back(captureDevice);
    }

    return devices;
}

Capture *CapturePluginBlackMagic::createCapture(const CaptureDevice captureDevice)
{
    return new CaptureBlackMagic(captureDevice);
}

void registerPlugin(const string &captureType, alvar::CapturePlugin *&capturePlugin)
{
    capturePlugin = new CapturePluginBlackMagic(captureType);
}

} // namespace plugins
} // namespace alvar
