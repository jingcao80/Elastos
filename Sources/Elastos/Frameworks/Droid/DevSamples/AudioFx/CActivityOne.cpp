#include "elastos/droid/ext/frameworkdef.h"
#include "CActivityOne.h"
#include "R.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Utility::Logging::Slogger;

using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Core::CStringWrapper;

using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IMediaPlayerHelper;
using Elastos::Droid::Media::CMediaPlayerHelper;
using Elastos::Droid::Media::Audiofx::CEqualizer;
using Elastos::Droid::Media::Audiofx::CVisualizer;
using Elastos::Droid::Media::Audiofx::CVisualizerHelper;
using Elastos::Droid::Media::Audiofx::IVisualizerHelper;
using Elastos::Droid::Media::EIID_IMediaPlayerOnCompletionListener;
using Elastos::Droid::Media::Audiofx::EIID_IVisualizerOnDataCaptureListener;
using Elastos::Droid::Widget::CSeekBar;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Widget::CLinearLayout;
using Elastos::Droid::Widget::ILinearLayoutLayoutParams;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::CSurfaceView;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::EIID_ISurfaceHolderCallback;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IPorterDuffXfermode;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::PorterDuffMode_CLEAR;
using Elastos::Droid::Graphics::PorterDuffMode_SRC;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::DevSamples::AudioFxDemo::R;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AudioFxDemo {

const String CActivityOne::TAG("AudioFxDemo");
const Float CActivityOne::VISUALIZER_HEIGHT_DIP = 50.0f;

CAR_INTERFACE_IMPL(CActivityOne::MyMediaPlayerOnCompletionListener, IMediaPlayerOnCompletionListener)

CActivityOne::MyMediaPlayerOnCompletionListener::MyMediaPlayerOnCompletionListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

ECode CActivityOne::MyMediaPlayerOnCompletionListener::OnCompletion(
    /* [in] */ IMediaPlayer* mp)
{
    Int32 result;
    Slogger::D(TAG, "CActivityOne::MyMediaPlayerOnCompletionListener::OnCompletion!!!!");
    return mHost->mVisualizer->SetEnabled(FALSE, &result);
}

CAR_INTERFACE_IMPL(CActivityOne::mySeekBarOnSeekBarChangeListener, ISeekBarOnSeekBarChangeListener)

CActivityOne::mySeekBarOnSeekBarChangeListener::mySeekBarOnSeekBarChangeListener(
    /* [in] */ CActivityOne* host,
    /* [in] */ Int32 band,
    /* [in] */ Int32 minEQLevel)
    : mHost(host)
    , mBand(band)
    , mMinEQLevel(minEQLevel)
{}

ECode CActivityOne::mySeekBarOnSeekBarChangeListener::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    //whe bar changed , set mEqualizer
    Slogger::D(TAG, "CActivityOne::mySeekBarOnSeekBarChangeListener::OnProgressChanged!!!!");
    return mHost->mEqualizer->SetBandLevel(mBand, (Int16)(progress + mMinEQLevel));
}

ECode CActivityOne::mySeekBarOnSeekBarChangeListener::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    return NOERROR;
}

ECode CActivityOne::mySeekBarOnSeekBarChangeListener::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    return NOERROR;
}

Int32 CActivityOne::myVisualizerOnDataCaptureListener::num=1;

CAR_INTERFACE_IMPL(CActivityOne::myVisualizerOnDataCaptureListener, IVisualizerOnDataCaptureListener)

CActivityOne::myVisualizerOnDataCaptureListener::myVisualizerOnDataCaptureListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

ECode CActivityOne::myVisualizerOnDataCaptureListener::OnWaveFormDataCapture(
    /* [in] */ IVisualizer* visualizer,
    /* [in] */ ArrayOf<Byte>* waveform,
    /* [in] */ Int32 samplingRate)
{
    AutoPtr<ICanvas> canvas;
    mHost->mSurfaceHolder->LockCanvas(NULL, (ICanvas**)&canvas);
    assert(canvas != NULL);

    AutoPtr<IPaint> clearPaint;
    CPaint::New((IPaint**)&clearPaint);
    AutoPtr<IPorterDuffXfermode> mode;
    CPorterDuffXfermode::New(PorterDuffMode_CLEAR, (IPorterDuffXfermode**)&mode);
    clearPaint->SetXfermode(mode);
    canvas->DrawPaint(clearPaint);
    mode = NULL;
    CPorterDuffXfermode::New(PorterDuffMode_SRC, (IPorterDuffXfermode**)&mode);
    clearPaint->SetXfermode(mode);

    Int32 canvasWidth, canvasHeight;
    canvas->GetWidth(&canvasWidth);
    canvas->GetWidth(&canvasHeight);
    PRINT_FILE_LINE_EX(" Canvas size %d:%d", canvasWidth, canvasHeight);
    canvas->DrawColor(0xFF008080);

    AutoPtr<IPaint> mPaint;
    CPaint::New((IPaint**)&mPaint);
    mPaint->SetStrokeWidth(1.0f);
    mPaint->SetAntiAlias(true);
    // AutoPtr<IColor> c;
    // CColor::AcquireSingleton((IColor**)&c);
    // Int32 rgb;
    // c->Rgb(0, 128, 255, &rgb);
    mPaint->SetColor(IColor::RED);

    AutoPtr<IRect> mRect;
    CRect::New((IRect**)&mRect);

    if (waveform == NULL) {
        return NOERROR;
    }

    Int32 waveLength;
    AutoPtr<ArrayOf<Float> > mPoints;
    waveLength = waveform->GetLength();
    if (mPoints == NULL || mPoints->GetLength() < waveLength * 4) {
        mPoints = NULL;
        mPoints = ArrayOf<Float>::Alloc(waveLength * 4);
    }

    Int32 width;
    Int32 height;
    mHost->mSurfaceView->GetWidth(&width);
    mHost->mSurfaceView->GetHeight(&height);

    mRect->Set(0, 0, width, height);

    mRect->GetWidth(&width);
    mRect->GetHeight(&height);
    for (Int32 i = 0; i < waveLength - 1; i++) {
        (*mPoints)[i * 4]     = (Float)(width * i) / (waveLength - 1);
        (*mPoints)[i * 4 + 1] = 50.0f * ((*waveform)[i] / 256.0f);
        (*mPoints)[i * 4 + 2] = (Float)(width * (i + 1)) / (waveLength - 1);
        (*mPoints)[i * 4 + 3] = 50.0f * ((*waveform)[i + 1] / 256.0f);
    }

    canvas->DrawLines(*mPoints, mPaint);

    mHost->mSurfaceHolder->UnlockCanvasAndPost(canvas);
    return NOERROR;
}

ECode CActivityOne::myVisualizerOnDataCaptureListener::OnFftDataCapture(
    /* [in] */ IVisualizer* visualizer,
    /* [in] */ ArrayOf<Byte>* fft,
    /* [in] */ Int32 samplingRate)
{
    return NOERROR;
}

CActivityOne::CMySurfaceHolderCallback::CMySurfaceHolderCallback(
    /* [in] */ CActivityOne* host) :
    mHost(host)
{
}

PInterface CActivityOne::CMySurfaceHolderCallback::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(ISurfaceHolderCallback*)this;
    }
    else if (riid == EIID_ISurfaceHolderCallback) {
        return (ISurfaceHolderCallback*)this;
    }

    return NULL;
}

UInt32 CActivityOne::CMySurfaceHolderCallback::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::CMySurfaceHolderCallback::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::CMySurfaceHolderCallback::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(ISurfaceHolderCallback*)this) {
        *pIID = EIID_ISurfaceHolderCallback;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

//In SurfaceCreated function, you could create a thread to draw your image.
ECode CActivityOne::CMySurfaceHolderCallback::SurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    PRINT_ENTER_LEAVE("CMySurfaceHolderCallback::SurfaceCreated");
    AutoPtr<ICanvas> canvas;
    holder->LockCanvas(NULL, (ICanvas**)&canvas);
    assert(canvas != NULL);

    Int32 canvasWidth, canvasHeight;
    canvas->GetWidth(&canvasWidth);
    canvas->GetWidth(&canvasHeight);
    PRINT_FILE_LINE_EX(" Canvas size %d:%d", canvasWidth, canvasHeight);
    holder->UnlockCanvasAndPost(canvas);
    return NOERROR;
}

ECode CActivityOne::CMySurfaceHolderCallback::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return NOERROR;
}

ECode CActivityOne::CMySurfaceHolderCallback::SurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    PRINT_FILE_LINE_EX("CMySurfaceHolderCallback::SurfaceDestroyed");
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    SetVolumeControlStream(IAudioManager::STREAM_MUSIC);

    CTextView::New(this, (ITextView**)&mStatusTextView);
    CLinearLayout::New(this, (ILinearLayout**)&mLinearLayout);
    mLinearLayout->SetOrientation(ILinearLayout::VERTICAL);
    mLinearLayout->AddView(mStatusTextView);
    SetContentView(mLinearLayout);

    AutoPtr<IMediaPlayerHelper> helper;
    CMediaPlayerHelper::AcquireSingleton((IMediaPlayerHelper**)&helper);
    helper->Create(this, R::raw::test_cbr, (IMediaPlayer**)&mMediaPlayer);

    SetupVisualizerFxAndUI();
    SetupEqualizerFxAndUI();

    Int32 result;
    mVisualizer->SetEnabled(TRUE, &result);
    AutoPtr<MyMediaPlayerOnCompletionListener> listener = new MyMediaPlayerOnCompletionListener(this);
    mMediaPlayer->SetOnCompletionListener((IMediaPlayerOnCompletionListener*)
            listener->Probe(EIID_IMediaPlayerOnCompletionListener));
    mMediaPlayer->Start();

    AutoPtr<ICharSequence> cSeq;
    CStringWrapper::New(String("Playing audio..."), (ICharSequence**)&cSeq);
    mStatusTextView->SetText(cSeq);
    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnStart");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnResume");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnPause");
    Activity::OnPause();

    Boolean result;
    IsFinishing(&result);
    if (result && mMediaPlayer != NULL) {
        mVisualizer->ReleaseResources();
        mEqualizer->ReleaseResources();
        mMediaPlayer->ReleaseResources();
        mMediaPlayer = NULL;
    }
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    PRINT_ENTER_LEAVE("OnStop");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    PRINT_ENTER_LEAVE("OnDestroy");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    PRINT_ENTER_LEAVE("OnActivityResult");
    return NOERROR;
}

ECode CActivityOne::SetupEqualizerFxAndUI()
{
    // Create the Equalizer object (an AudioEffect subclass) and attach it to our media player,
    // with a default priority (0).
    Int32 sessionID;
    mMediaPlayer->GetAudioSessionId(&sessionID);
    CEqualizer::New(0, sessionID, (IEqualizer**)&mEqualizer);
    Int32 result;
    mEqualizer->SetEnabled(TRUE, &result);

    AutoPtr<ITextView> eqTextView;
    CTextView::New(this, (ITextView**)&eqTextView);
    AutoPtr<ICharSequence> cSeq;
    CStringWrapper::New(String("Equalizer:"), (ICharSequence**)&cSeq);
    eqTextView->SetText(cSeq);
    mLinearLayout->AddView(eqTextView);

    Int16 bands;
    mEqualizer->GetNumberOfBands(&bands);

    AutoPtr<ArrayOf<Int16> > bandLevelRange;
    mEqualizer->GetBandLevelRange((ArrayOf<Int16>**)&bandLevelRange);
    const Int16 minEQLevel = (*bandLevelRange)[0];
    const Int16 maxEQLevel = (*bandLevelRange)[1];

    for (Int16 i = 0; i < bands; i++) {
        const Int16 band = i;
        AutoPtr<ITextView> freqTextView;
        CTextView::New(this, (ITextView**)&freqTextView);
        AutoPtr<IViewGroupLayoutParams> params1;
        CViewGroupLayoutParams::New(IViewGroupLayoutParams::FILL_PARENT,
                IViewGroupLayoutParams::WRAP_CONTENT, (IViewGroupLayoutParams**)&params1);
        freqTextView->SetLayoutParams(params1);

        freqTextView->SetGravity(IGravity::CENTER_HORIZONTAL);
        Int32 centerFreq;
        mEqualizer->GetCenterFreq(band, &centerFreq);
        AutoPtr<ICharSequence> cSeq1;
        StringBuilder sb1;
        sb1 += centerFreq / 1000;
        sb1 += " Hz";
        CStringWrapper::New(sb1.ToString(), (ICharSequence**)&cSeq1);
        freqTextView->SetText(cSeq1);
        mLinearLayout->AddView(freqTextView);

        AutoPtr<ILinearLayout> row;
        CLinearLayout::New(this, (ILinearLayout**)&row);
        row->SetOrientation(ILinearLayout::HORIZONTAL);

        AutoPtr<ITextView> minDbTextView;
        CTextView::New(this, (ITextView**)&minDbTextView);
        AutoPtr<IViewGroupLayoutParams> params2;
        CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
                IViewGroupLayoutParams::WRAP_CONTENT, (IViewGroupLayoutParams**)&params2);
        minDbTextView->SetLayoutParams(params2);
        AutoPtr<ICharSequence> cSeq2;
        StringBuilder sb2;
        sb2 += minEQLevel / 100;
        sb2 += " dB";
        CStringWrapper::New(sb2.ToString(), (ICharSequence**)&cSeq2);
        minDbTextView->SetText(cSeq2);

        AutoPtr<ITextView> maxDbTextView;
        CTextView::New(this, (ITextView**)&maxDbTextView);
        AutoPtr<IViewGroupLayoutParams> params3;
        CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
                IViewGroupLayoutParams::WRAP_CONTENT, (IViewGroupLayoutParams**)&params3);
        maxDbTextView->SetLayoutParams(params3);
        AutoPtr<ICharSequence> cSeq3;
        StringBuilder sb3;
        sb3 += maxEQLevel / 100;
        sb3 += " dB";
        CStringWrapper::New(sb3.ToString(), (ICharSequence**)&cSeq3);
        maxDbTextView->SetText(cSeq3);

        AutoPtr<ILinearLayoutLayoutParams> layoutParams;
        CLinearLayoutLayoutParams::New(IViewGroupLayoutParams::FILL_PARENT,
                IViewGroupLayoutParams::WRAP_CONTENT, (ILinearLayoutLayoutParams**)&layoutParams);
        layoutParams->SetWeight(1);

        AutoPtr<ISeekBar> bar;
        CSeekBar::New(this, (ISeekBar**)&bar);
        bar->SetLayoutParams(layoutParams);
        bar->SetMax(maxEQLevel - minEQLevel);
        Int16 level;
        mEqualizer->GetBandLevel(band, &level);
        bar->SetProgress(level);

        AutoPtr<mySeekBarOnSeekBarChangeListener> listener = new mySeekBarOnSeekBarChangeListener(this, band, minEQLevel);
        bar->SetOnSeekBarChangeListener((ISeekBarOnSeekBarChangeListener*)
            listener->Probe(EIID_ISeekBarOnSeekBarChangeListener));

        row->AddView(minDbTextView);
        row->AddView(bar);
        row->AddView(maxDbTextView);

        mLinearLayout->AddView(row);
    }

    return NOERROR;
}

ECode CActivityOne::SetupVisualizerFxAndUI()
{
    CSurfaceView::New(this,
            (ISurfaceView**)&mSurfaceView);
    assert(mSurfaceView != NULL);

    mSurfaceView->GetHolder((ISurfaceHolder**)&mSurfaceHolder);
    assert(mSurfaceHolder != NULL);
    mSurfaceHolderCallback = new CMySurfaceHolderCallback(this);

    assert(mSurfaceHolderCallback != NULL);
    mSurfaceHolder->AddCallback(mSurfaceHolderCallback);


    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);
    AutoPtr<IViewGroupLayoutParams> params;
    CViewGroupLayoutParams::New(IViewGroupLayoutParams::FILL_PARENT, (Int32)(VISUALIZER_HEIGHT_DIP * density), (IViewGroupLayoutParams**)&params);

    mSurfaceView->SetLayoutParams(params);
    mLinearLayout->AddView(mSurfaceView);


    Int32 sessionID;
    mMediaPlayer->GetAudioSessionId(&sessionID);
    CVisualizer::New(sessionID, (IVisualizer**)&mVisualizer);

    AutoPtr<ArrayOf<Int32> > captureSizeRange;
    AutoPtr<IVisualizerHelper> helper;
    CVisualizerHelper::AcquireSingleton((IVisualizerHelper**)&helper);
    helper->GetCaptureSizeRange((ArrayOf<Int32>**)&captureSizeRange);

    Int32 result;
    mVisualizer->SetCaptureSize((*captureSizeRange)[1], &result);
    AutoPtr<myVisualizerOnDataCaptureListener> listener = new myVisualizerOnDataCaptureListener(this);
    Int32 captureRate;
    helper->GetMaxCaptureRate(&captureRate);
    return mVisualizer->SetDataCaptureListener((IVisualizerOnDataCaptureListener*)
            listener->Probe(EIID_IVisualizerOnDataCaptureListener), captureRate / 2, TRUE, FALSE, &result);
}

} // namespace AudioFxDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
