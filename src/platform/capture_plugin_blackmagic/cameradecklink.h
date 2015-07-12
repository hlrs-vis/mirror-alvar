#ifndef CAMERADECKLINK_H
#define CAMERADECKLINK_H

#include "DeckLinkAPI.h"
#include <opencv/cv.h>

class DeckLinkCaptureDelegate : public IDeckLinkInputCallback
{
public:
    DeckLinkCaptureDelegate();
    ~DeckLinkCaptureDelegate();

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { return E_NOINTERFACE; }
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE  Release(void);
    virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags);
    virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame*, IDeckLinkAudioInputPacket*);
    
    IplImage* getLastImage();
    void getImage(IplImage* image );

    void convertFrameToOpenCV(void* frameBytes, IplImage * m_RGB);

    pthread_mutex_t		*sleepMutex;
    pthread_cond_t		*sleepCond;
    //        bool stopped;
    //        bool converting;
    void* frameBytes;
    long frameCount;
    BMDTimecodeFormat		g_timecodeFormat;

private:
    ULONG				m_refCount;
    pthread_mutex_t		m_mutex;
    IplImage* lastImage;

    int height, width;

};

class CameraDecklink
{
public:
    CameraDecklink();

    void initializeCamera(IDeckLink * _deckLink);
    void StopCapture();

    IplImage * captureLastFrame();
    void CameraDecklink::readIplImage(IplImage* image);
    cv::Mat captureLastCvMat();

private:
    DeckLinkCaptureDelegate 	*delegate;
    IDeckLinkInput		*deckLinkInput;
    IDeckLinkDisplayModeIterator *displayModeIterator;
    IDeckLinkDisplayMode	*displayMode;
    BMDVideoInputFlags	inputFlags;
    BMDDisplayMode selectedDisplayMode;
    BMDPixelFormat pixelFormat;
    int	displayModeCount;
    int	exitStatus;
    bool foundDisplayMode;
    HRESULT result;

};

#endif // CAMERADECKLINK_H
