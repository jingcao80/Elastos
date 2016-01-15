
#ifndef __CMAINACTIVITY_H__
#define __CMAINACTIVITY_H__

#include "elastos/droid/app/Activity.h"
#include "_CMainActivity.h"
#include <elastos/core/Math.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::Media::IAudioTrack;
using Elastos::Droid::Media::CAudioTrack;
using Elastos::Droid::Media::IAudioTrackHelper;
using Elastos::Droid::Media::CAudioTrackHelper;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioFormat;
using Elastos::Droid::Media::IOnPlaybackPositionUpdateListener;
using Elastos::Droid::Media::EIID_IOnPlaybackPositionUpdateListener;
using Elastos::Droid::Os::CHandler;
using Elastos::Core::Math;
using Elastos::Core::EIID_IRunnable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;

class CMainActivity : public Activity
{
public:
    ECode NewAudioTest(int streamType, int mode, int config, int format, int stateForMode, int channelCount);
    ECode CreateAudio(int *pBuffersize);
    ECode PlayAudio(String filename, int bufferSize);
    ArrayOf<Int16>* createSoundDataInInt16Array(int bufferSize, int sampleRate);

    ECode HandleMessage();
    ECode PostcppCallback();

    class MyListener : public IOnPlaybackPositionUpdateListener
    {
    public:
        MyListener(/* [in] */ CMainActivity* host) : mHost(host), mRef(0) {}

        CARAPI_(PInterface) Probe(/* [in]  */ REIID riid)
        {
            if (riid == EIID_IInterface) {
                return (PInterface)(IOnPlaybackPositionUpdateListener*)this;
            }
            else if (riid == EIID_IOnPlaybackPositionUpdateListener) {
                return (IOnPlaybackPositionUpdateListener*)this;
            }
            return NULL;
        }

        CARAPI_(UInt32) AddRef() {
            Int32 ref = atomic_inc(&mRef);
            return ++ref;
        }

        CARAPI_(UInt32) Release() {
            Int32 ref = atomic_dec(&mRef);
            if (--ref == 0) {
                delete this;
            }
            assert(ref >= 0);
            return ref;
        }

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID)
        {
            if (pIID == NULL) {
                return E_INVALID_ARGUMENT;
            }

            if (pObject == (IInterface*)(IOnPlaybackPositionUpdateListener*)this) {
                *pIID = EIID_IRunnable;
            }
            else {
                return E_INVALID_ARGUMENT;
            }
            return NOERROR;
        }

        CARAPI OnMarkerReached(
            /* [in] */ IAudioTrack * track)
        {
            printf("OnMarkerReached\n");
            mHost->mbMarked = true;
            return NOERROR;
        }

        CARAPI OnPeriodicNotification(
            /* [in] */  IAudioTrack * track)
        {
            printf("OnPeriodicNotification\n");
            mHost->mPeriodic++;
            return NOERROR;
        }

    private:
        AutoPtr<CMainActivity> mHost;
        Int32 mRef;
    };

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart() { printf("%s\n", __FUNCTION__);return NOERROR;}
    CARAPI OnResume() { printf("%s\n", __FUNCTION__);return NOERROR;}
    CARAPI OnPause() { printf("%s\n", __FUNCTION__);return NOERROR;}
    CARAPI OnStop() { printf("%s\n", __FUNCTION__);return NOERROR;}
    CARAPI OnDestroy() { printf("%s\n", __FUNCTION__);return NOERROR;}

private:
    static void* EntryRoutine(void *arg);
    static void* PlayAudioThread(void *arg);

    AutoPtr<IHandler> mDefaultHandler;
    AutoPtr<IAudioTrackHelper> mAudioTrackHelper;
    AutoPtr<IAudioTrack> mAudioTrack;

    AutoPtr<MyListener> mListener;
    Boolean mbMarked;
    int mPeriodic;
};

#endif // __CMAINACTIVITY_H__
