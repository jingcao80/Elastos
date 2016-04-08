
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"

using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::ISurfaceView;
using Elastos::Droid::View::ISurfaceHolderCallback;
using Elastos::Droid::View::IViewOnClickListener;

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Media::IMediaPlayer;
using Elastos::Droid::Media::IMediaPlayerOnCompletionListener;
using Elastos::Droid::Media::Audiofx::IEqualizer;
using Elastos::Droid::Media::Audiofx::IVisualizer;
using Elastos::Droid::Media::Audiofx::IVisualizerOnDataCaptureListener;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;

using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::ISurfaceView;
using Elastos::Droid::View::CSurfaceView;
using Elastos::Droid::View::ISurfaceHolderCallback;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AudioFxDemo {

class CActivityOne : public Activity
{
public:
    class MyMediaPlayerOnCompletionListener
        : public ElRefBase
        , public IMediaPlayerOnCompletionListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyMediaPlayerOnCompletionListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnCompletion(
            /* [in] */ IMediaPlayer* mp);
    private:
        CActivityOne* mHost;
    };

    class mySeekBarOnSeekBarChangeListener
        : public ElRefBase
        , public ISeekBarOnSeekBarChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        mySeekBarOnSeekBarChangeListener(
            /* [in] */ CActivityOne* host,
            /* [in] */ Int32 band,
            /* [in] */ Int32 minEQLevel);

        CARAPI OnProgressChanged(
            /* [in] */ ISeekBar* seekBar,
            /* [in] */ Int32 progress,
            /* [in] */ Boolean fromUser);

        CARAPI OnStartTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        CARAPI OnStopTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

    private:
        CActivityOne* mHost;
        Int32 mBand;
        Int32 mMinEQLevel;
    };

    class myVisualizerOnDataCaptureListener
        : public ElRefBase
        , public IVisualizerOnDataCaptureListener
    {
    public:
        CAR_INTERFACE_DECL()

        myVisualizerOnDataCaptureListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnWaveFormDataCapture(
            /* [in] */ IVisualizer* visualizer,
            /* [in] */ ArrayOf<Byte>* waveform,
            /* [in] */ Int32 samplingRate);

        CARAPI OnFftDataCapture(
            /* [in] */ IVisualizer* visualizer,
            /* [in] */ ArrayOf<Byte>* fft,
            /* [in] */ Int32 samplingRate);

    private:
        CActivityOne* mHost;
        static Int32 num;
    };

    class CMySurfaceHolderCallback:
        public ElRefBase,
        public ISurfaceHolderCallback
    {
    public:
        CMySurfaceHolderCallback(
            /* [in] */ CActivityOne* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI SurfaceCreated(
            /* [in] */ ISurfaceHolder* holder);

        CARAPI SurfaceChanged(
            /* [in] */ ISurfaceHolder* holder,
            /* [in] */ Int32 format,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI SurfaceDestroyed(
            /* [in] */ ISurfaceHolder* holder);

    private:
        CActivityOne*     mHost;
    };

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI SetupEqualizerFxAndUI();
    CARAPI SetupVisualizerFxAndUI();

private:
    static const String TAG;

    static const Float VISUALIZER_HEIGHT_DIP;

    AutoPtr<IMediaPlayer> mMediaPlayer;
    AutoPtr<IVisualizer> mVisualizer;
    AutoPtr<IEqualizer> mEqualizer;

    AutoPtr<ILinearLayout> mLinearLayout;
    //AutoPtr<VisualizerView> mVisualizerView;
    AutoPtr<ITextView> mStatusTextView;

    AutoPtr<ArrayOf<Byte> > mBytes;

    AutoPtr<ISurfaceView>   mSurfaceView;
    AutoPtr<ISurfaceHolder>  mSurfaceHolder;
    AutoPtr<CMySurfaceHolderCallback>  mSurfaceHolderCallback;
};

} // namespace AudioFxDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
