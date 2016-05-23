#include "elastos/droid/media/audiofx/CVisualizer.h"
#include "elastos/droid/media/audiofx/CVisualizerMeasurementPeakRms.h"
#include "elastos/droid/os/CLooperHelper.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>

#include <media/Visualizer.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::EIID_IByte;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

const String CVisualizer::TAG("Visualizer-JAVA");
const Int32 CVisualizer::NATIVE_EVENT_PCM_CAPTURE = 0;
const Int32 CVisualizer::NATIVE_EVENT_FFT_CAPTURE = 1;
const Int32 CVisualizer::NATIVE_EVENT_SERVER_DIED = 2;

CAR_INTERFACE_IMPL(CVisualizer, Object, IVisualizer)

CAR_OBJECT_IMPL(CVisualizer)

CVisualizer::NativeEventHandler::NativeEventHandler(
    /* [in] */ CVisualizer* v,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mVisualizer(v)
{
}

ECode CVisualizer::NativeEventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (mVisualizer == NULL) {
        return NOERROR;
    }

    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
        case NATIVE_EVENT_PCM_CAPTURE:
        case NATIVE_EVENT_FFT_CAPTURE:
            HandleCaptureMessage(msg);
            break;
        case NATIVE_EVENT_SERVER_DIED:
            HandleServerDiedMessage(msg);
            break;
        default:
            Logger::E(TAG, "Unknown native event: %d", what);
            break;
    }

    return NOERROR;
}

void CVisualizer::NativeEventHandler::HandleCaptureMessage(
    /* [in]*/ IMessage* msg)
{
    AutoPtr<IVisualizerOnDataCaptureListener> l;
    {
        Object& lock = mVisualizer->mListenerLock;
        AutoLock syncLock(lock);
        l = mVisualizer->mCaptureListener;
    }
    if (l != NULL) {
        AutoPtr< ArrayOf<Byte> > data;
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);
        Int32 count;
        array->GetLength(&count);
        data = ArrayOf<Byte>::Alloc(count);
        for(Int32 i = 0; i < count; i++) {
            obj = NULL;
            array->Get(i, (IInterface**)&obj);
            AutoPtr<IByte> ib = IByte::Probe(obj);
            Byte b;
            ib->GetValue(&b);
            data->Set(i, b);
        }

        Int32 samplingRate;
        msg->GetArg1(&samplingRate);
        Int32 what;
        msg->GetWhat(&what);

        switch(what) {
            case NATIVE_EVENT_PCM_CAPTURE:
                l->OnWaveFormDataCapture(IVisualizer::Probe(mVisualizer), data, samplingRate);
                break;
            case NATIVE_EVENT_FFT_CAPTURE:
                l->OnFftDataCapture(IVisualizer::Probe(mVisualizer), data, samplingRate);
                break;
            default:
                Logger::E(TAG, "Unknown native event: %d", what);
                break;
        }
    }
}

void CVisualizer::NativeEventHandler::HandleServerDiedMessage(
    /* [in]*/ IMessage* msg)
{
    AutoPtr<IVisualizerOnServerDiedListener> l;
    {
        Object& lock = mVisualizer->mListenerLock;
        AutoLock syncLock(lock);
        l = mVisualizer->mServerDiedListener;
    }
    if (l != NULL)
        l->OnServerDied();
}

CVisualizer::CVisualizer()
    : mState(IVisualizer::STATE_UNINITIALIZED)
    , mId(0)
    , mNativeVisualizer(0)
    , mJniData(0)
{}

CVisualizer::~CVisualizer()
{
    Native_Finalize();
}

ECode CVisualizer::constructor(
    /* [in] */ Int32 audioSession)
{
    AutoPtr<ArrayOf<Int32> > id = ArrayOf<Int32>::Alloc(1);
    AutoLock syncLock(mStateLock);
    mState = IVisualizer::STATE_UNINITIALIZED;

    // native initialization
    Int32 result = Native_Setup(audioSession, id);
    if (result != IVisualizer::SUCCESS && result != IVisualizer::ALREADY_EXISTS) {
       Logger::E(TAG, "Error code %d when initializing Visualizer.", result);
        switch (result) {
            case IVisualizer::ERROR_INVALID_OPERATION:
               // throw (new UnsupportedOperationException("Effect library not loaded"));
                return E_UNSUPPORTED_OPERATION_EXCEPTION;
            default:
               // throw (new RuntimeException("Cannot initialize Visualizer engine, error: "
               //         +result));
                return E_RUNTIME_EXCEPTION;
        }
    }
    mId = (*id)[0];
    if (Native_GetEnabled()) {
        mState = IVisualizer::STATE_ENABLED;
    } else {
        mState = IVisualizer::STATE_INITIALIZED;
    }
    return NOERROR;
}

ECode CVisualizer::ReleaseResources()
{
    AutoLock syncLock(mStateLock);
    Native_Release();
    mState = IVisualizer::STATE_UNINITIALIZED;
    return NOERROR;
}

ECode CVisualizer::SetEnabled(
    /* [in] */ Boolean enabled,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock syncLock(mStateLock);
    if (mState == IVisualizer::STATE_UNINITIALIZED) {
       // throw(new IllegalStateException("setEnabled() called in wrong state: "+mState));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 status = IVisualizer::SUCCESS;
    if ((enabled && (mState == IVisualizer::STATE_INITIALIZED)) ||
        (!enabled && (mState == IVisualizer::STATE_ENABLED))) {
        status = Native_SetEnabled(enabled);
        if (status == IVisualizer::SUCCESS) {
            mState = enabled ? IVisualizer::STATE_ENABLED : IVisualizer::STATE_INITIALIZED;
        }
    }
    *result = status;
    return NOERROR;
}

ECode CVisualizer::GetEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);

    AutoLock syncLock(mStateLock);
    if (mState == IVisualizer::STATE_UNINITIALIZED) {
        // throw(new IllegalStateException("getEnabled() called in wrong state: "+mState));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *enabled = Native_GetEnabled();
    return NOERROR;
}

ECode CVisualizer::GetCaptureSizeRange(
    /* [out] */ ArrayOf<Int32>** captureSizeRange)
{
    AutoPtr<ArrayOf<Int32> > jRange = ArrayOf<Int32>::Alloc(2);
    (*jRange)[0] = android::Visualizer::getMinCaptureSize();
    (*jRange)[1] = android::Visualizer::getMaxCaptureSize();
    *captureSizeRange = jRange;
    REFCOUNT_ADD(*captureSizeRange);
    return NOERROR;
}

ECode CVisualizer::GetMaxCaptureRate(
    /* [out] */ Int32* captureRate)
{
    *captureRate = android::Visualizer::getMaxCaptureRate();
    return NOERROR;
}

ECode CVisualizer::SetCaptureSize(
    /* [in] */ Int32 size,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock syncLock(mStateLock);
    if (mState == IVisualizer::STATE_INITIALIZED) {
        // throw(new IllegalStateException("setCaptureSize() called in wrong state: "+mState));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = Native_SetCaptureSize(size);
    return NOERROR;

}

ECode CVisualizer::GetCaptureSize(
    /* [out] */ Int32* captureSize)
{
    VALIDATE_NOT_NULL(captureSize);

    AutoLock syncLock(mStateLock);
    if (mState == IVisualizer::STATE_UNINITIALIZED) {
        // throw(new IllegalStateException("getCaptureSize() called in wrong state: "+mState));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *captureSize = Native_GetCaptureSize();
    return NOERROR;
}

ECode CVisualizer::SetScalingMode(
    /* [in] */ Int32 mode,
    /* [out]*/ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock syncLock(mStateLock);
    if (mState == STATE_UNINITIALIZED) {
        // throw(new IllegalStateException("setScalingMode() called in wrong state: "+ mState));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = Native_SetScalingMode(mode);
    return NOERROR;
}

ECode CVisualizer::GetScalingMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock syncLock(mStateLock);
    if (mState == STATE_UNINITIALIZED) {
        // throw(new IllegalStateException("getScalingMode() called in wrong state: " + mState));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = Native_GetScalingMode();
    return NOERROR;

}

CARAPI CVisualizer::SetMeasurementMode(
    /* [in] */ Int32 mode,
    /* [out] */ Int32* result)
{
    AutoLock syncLock(mStateLock);
    if (mState == IVisualizer::STATE_UNINITIALIZED) {
        // throw(new IllegalStateException("setMeasurementMode() called in wrong state: "
        //         + mState));
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = Native_SetMeasurementMode(mode);
    return NOERROR;
}

CARAPI CVisualizer::GetMeasurementMode(
    /* [out] */ Int32* result)
{
    AutoLock syncLock(mStateLock);
    if (mState == IVisualizer::STATE_UNINITIALIZED) {
        // throw(new IllegalStateException("getMeasurementMode() called in wrong state: "
        //         + mState));
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = Native_GetMeasurementMode();
    return NOERROR;
}

ECode CVisualizer::GetSamplingRate(
    /* [out] */ Int32* samplingRate)
{
    VALIDATE_NOT_NULL(samplingRate);

    AutoLock syncLock(mStateLock);
    if (mState == IVisualizer::STATE_UNINITIALIZED) {
        // throw(new IllegalStateException("getSamplingRate() called in wrong state: "+mState));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *samplingRate = Native_GetSamplingRate();
    return NOERROR;
}

ECode CVisualizer::GetWaveForm(
    /* [in] */ ArrayOf<Byte>* waveform,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock syncLock(mStateLock);
    if (mState == IVisualizer::STATE_ENABLED) {
        // throw(new IllegalStateException("getWaveForm() called in wrong state: "+mState));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    ArrayOf<Byte>* x = waveform;
    *result = Native_GetWaveForm(x);
    return NOERROR;
}

ECode CVisualizer::GetFft(
    /* [in] */ ArrayOf<Byte>* fft,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock syncLock(mStateLock);
    if (mState == IVisualizer::STATE_ENABLED) {
        // throw(new IllegalStateException("getFft() called in wrong state: "+mState));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = Native_GetFft(fft);
    return NOERROR;
}

ECode CVisualizer::GetMeasurementPeakRms(
    /* [in] */ IVisualizerMeasurementPeakRms * measurement,
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);

    if (measurement == NULL) {
        Logger::E(TAG, "Cannot store measurements in a NULL object");
        *result = IVisualizer::ERROR_BAD_VALUE;
        return NOERROR;
    }
    AutoLock syncLock(mStateLock);
    if (mState != IVisualizer::STATE_ENABLED) {
        // throw (new IllegalStateException("getMeasurementPeakRms() called in wrong state: "
        //         + mState));
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = Native_GetPeakRms(measurement);
    return NOERROR;
}

ECode CVisualizer::SetDataCaptureListener(
    /* [in] */ IVisualizerOnDataCaptureListener* listener,
    /* [in] */ Int32 rate,
    /* [in] */ Boolean waveform,
    /* [in] */ Boolean fft,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock syncLock(mListenerLock);
        mCaptureListener = listener;
    }
    if (listener == NULL) {
        // make sure capture callback is stopped in native code
        waveform = FALSE;
        fft = FALSE;
    }
    Int32 status = Native_SetPeriodicCapture(rate, waveform, fft);
    if (status == IVisualizer::SUCCESS) {
        if ((listener != NULL) && (mNativeEventHandler == NULL)) {
            AutoPtr<ILooper> myLooper;
            AutoPtr<ILooper> mainLooper;
            AutoPtr<ILooperHelper> helper;
            CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
            helper->GetMyLooper((ILooper**)&myLooper);
            helper->GetMainLooper((ILooper**)&mainLooper);
            if (myLooper != NULL) {
                mNativeEventHandler = new NativeEventHandler(this, myLooper);
            } else if (mainLooper != NULL) {
                mNativeEventHandler = new NativeEventHandler(this, mainLooper);
            } else {
                mNativeEventHandler = NULL;
                status = ERROR_NO_INIT;
            }
        }
    }

    *result = status;
    return NOERROR;
}

ECode CVisualizer::SetServerDiedListener(
    /* [in] */ IVisualizerOnServerDiedListener* listener,
    /* [out] */ Int32* result)
{
    AutoLock syncLock(mListenerLock);
    mServerDiedListener = listener;
    *result = SUCCESS;
    return NOERROR;
}

struct visualizer_callback_cookie {
    //jclass      visualizer_class;  // Visualizer class
    //jobject     visualizer_ref;    // Visualizer object instance
    CVisualizer*  visualizer_ref;

    // Lazily allocated arrays used to hold callback data provided to java
    // applications.  These arrays are allocated during the first callback and
    // reallocated when the size of the callback data changes.  Allocating on
    // demand and saving the arrays means that applications cannot safely hold a
    // reference to the provided data (they need to make a copy if they want to
    // hold onto outside of the callback scope), but it avoids GC thrash caused
    // by constantly allocating and releasing arrays to hold callback data.
    android::Mutex  callback_data_lock;
    AutoPtr<ArrayOf<Byte> > waveform_data;
    AutoPtr<ArrayOf<Byte> > fft_data;
};

class visualizerJniStorage {
    public:
        visualizer_callback_cookie mCallbackData;
    visualizerJniStorage() {
    }

    ~visualizerJniStorage() {
    }

};

#define VISUALIZER_SUCCESS                      0
#define VISUALIZER_ERROR                       -1
#define VISUALIZER_ERROR_ALREADY_EXISTS        -2
#define VISUALIZER_ERROR_NO_INIT               -3
#define VISUALIZER_ERROR_BAD_VALUE             -4
#define VISUALIZER_ERROR_INVALID_OPERATION     -5
#define VISUALIZER_ERROR_NO_MEMORY             -6
#define VISUALIZER_ERROR_DEAD_OBJECT           -7

#define NATIVE_EVENT_PCM_CAPTURE                0
#define NATIVE_EVENT_FFT_CAPTURE                1

static Int32 translateError(Int32 code) {
    switch(code) {
    case android::NO_ERROR:
        return VISUALIZER_SUCCESS;
    case android::ALREADY_EXISTS:
        return VISUALIZER_ERROR_ALREADY_EXISTS;
    case android::NO_INIT:
        return VISUALIZER_ERROR_NO_INIT;
    case android::BAD_VALUE:
        return VISUALIZER_ERROR_BAD_VALUE;
    case android::INVALID_OPERATION:
        return VISUALIZER_ERROR_INVALID_OPERATION;
    case android::NO_MEMORY:
        return VISUALIZER_ERROR_NO_MEMORY;
    case android::DEAD_OBJECT:
        return VISUALIZER_ERROR_DEAD_OBJECT;
    default:
        return VISUALIZER_ERROR;
    }
}

// ----------------------------------------------------------------------------
static void ensureArraySize(
    /* [in] */ ArrayOf<Byte>* arrayIn,
    /* [in] */ uint32_t size,
    /* [out] */ ArrayOf<Byte>** arrayOut)
{
    if (arrayIn != NULL && arrayIn->GetPayload() != NULL) {
        uint32_t len = arrayIn->GetLength();
        if (len == size) {
            *arrayOut = arrayIn;
            REFCOUNT_ADD(*arrayOut);
            return;
        }
    }

    AutoPtr<ArrayOf<Byte> > localRef = ArrayOf<Byte>::Alloc(size);
    if (NULL != localRef) {
        // Promote to global ref.
        *arrayOut = localRef;
        REFCOUNT_ADD(*arrayOut);
    }
}

void captureCallback(
    /* [in] */ void* user,
    /* [in] */ uint32_t waveformSize,
    /* [in] */ uint8_t *waveform,
    /* [in] */ uint32_t fftSize,
    /* [in] */ uint8_t *fft,
    /* [in] */ uint32_t samplingrate)
{
    visualizer_callback_cookie *callbackInfo = (visualizer_callback_cookie *)user;
    android::AutoMutex lock(&callbackInfo->callback_data_lock);

    ALOGV("captureCallback: callbackInfo %p, visualizer_ref %p",
            callbackInfo,
            callbackInfo->visualizer_ref);

    if (!user) {
        ALOGW("captureCallback error user %p", user);
        return;
    }

    AutoPtr<IByte> byte;
    AutoPtr<IArrayOf> obj;

    if (waveformSize != 0 && waveform != NULL) {
        AutoPtr<ArrayOf<Byte> > jArray;

        ensureArraySize(callbackInfo->waveform_data, waveformSize, (ArrayOf<Byte>**)&jArray);
        if (jArray != NULL) {
            Byte* nArray = jArray->GetPayload();
            memcpy(nArray, waveform, waveformSize);

            CArrayOf::New(EIID_IByte, waveformSize,(IArrayOf**)&obj);
            for(Int32 i = 0; i < waveformSize; i++) {
                byte = NULL;
                CByte::New((*jArray)[i], (IByte**)&byte);
                obj->Set(i, byte);
            }

            CVisualizer::PostEventFromNative(callbackInfo->visualizer_ref, NATIVE_EVENT_PCM_CAPTURE, samplingrate, 0, obj);
        }
    }

    if (fftSize != 0 && fft != NULL) {
        AutoPtr<ArrayOf<Byte> > jArray;

        ensureArraySize(callbackInfo->fft_data, fftSize, (ArrayOf<Byte>**)&jArray);
        if (jArray != NULL) {
            Byte* nArray = jArray->GetPayload();
            memcpy(nArray, fft, fftSize);

            CArrayOf::New(EIID_IByte, fftSize,(IArrayOf**)&obj);
            for(Int32 i = 0; i < fftSize; i++) {
                byte = NULL;
                CByte::New((*jArray)[i], (IByte**)&byte);
                obj->Set(i, byte);
            }

            CVisualizer::PostEventFromNative(callbackInfo->visualizer_ref, NATIVE_EVENT_FFT_CAPTURE, samplingrate, 0, obj);
        }
    }
}

void elastos_media_visualizer_effect_callback(
    /* [in] */ int event,
    /* [in] */ void* user,
    /* [in] */ void* info)
{
    if ((event == android::AudioEffect::EVENT_ERROR) &&
        (*((android::status_t*)info) == android::DEAD_OBJECT)) {
        visualizerJniStorage* lpJniStorage = (visualizerJniStorage*)user;
        visualizer_callback_cookie* callbackInfo = &lpJniStorage->mCallbackData;

        CVisualizer::PostEventFromNative(callbackInfo->visualizer_ref, CVisualizer::NATIVE_EVENT_SERVER_DIED, 0, 0, 0);
    }
}

void CVisualizer::Native_Init()
{
}

Int32 CVisualizer::Native_Setup(
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<Int32>* jId)
{
    visualizerJniStorage* lpJniStorage = NULL;
    Int32 lStatus = VISUALIZER_ERROR_NO_MEMORY;
    android::Visualizer* lpVisualizer = NULL;

    lpJniStorage = new visualizerJniStorage();
    if (lpJniStorage == NULL) {
        goto setup_failure;
    }

    lpJniStorage->mCallbackData.visualizer_ref = this;

    if (jId == NULL) {
        lStatus = VISUALIZER_ERROR_BAD_VALUE;
        goto setup_failure;
    }

    // create the native Visualizer object
    lpVisualizer = new android::Visualizer(0,
        elastos_media_visualizer_effect_callback, lpJniStorage, sessionId);

    if (lpVisualizer == NULL) {
        ALOGE("Error creating Visualizer");
        goto setup_failure;
    }

    lStatus = translateError(lpVisualizer->initCheck());
    if (lStatus != VISUALIZER_SUCCESS && lStatus != VISUALIZER_ERROR_ALREADY_EXISTS) {
        goto setup_failure;
    }

    (*jId)[0] = lpVisualizer->id();

    mNativeVisualizer = (Int32)lpVisualizer;

    mJniData = (Int32)lpJniStorage;

    return VISUALIZER_SUCCESS;

    // failures:
    setup_failure:

    if (lpVisualizer) {
        delete lpVisualizer;
    }

    mNativeVisualizer = 0;

    if (lpJniStorage) {
        delete lpJniStorage;
    }

    mJniData = 0;

    return lStatus;
}

void CVisualizer::Native_Finalize()
{
    // delete the Visualizer object
    android::Visualizer* lpVisualizer = (android::Visualizer *)mNativeVisualizer;
    if (lpVisualizer) {
        lpVisualizer->cancelCaptureCallBack();
        delete lpVisualizer;
    }

    // delete the JNI data
    visualizerJniStorage* lpJniStorage = (visualizerJniStorage*)mJniData;
    if (lpJniStorage) {
        delete lpJniStorage;
    }
}

void CVisualizer::Native_Release()
{
    // do everything a call to finalize would
    Native_Finalize();
    mNativeVisualizer = 0;
    mJniData = 0;
}

Int32 CVisualizer::Native_SetEnabled(
    /* [in] */ Boolean enabled)
{
    android::Visualizer* lpVisualizer = (android::Visualizer *)mNativeVisualizer;
    if (lpVisualizer == NULL) {
        return VISUALIZER_ERROR_NO_INIT;
    }

    return translateError(lpVisualizer->setEnabled(enabled));
}

Boolean CVisualizer::Native_GetEnabled()
{
    android::Visualizer* lpVisualizer = (android::Visualizer *)mNativeVisualizer;
    if (lpVisualizer == NULL) {
        return FALSE;
    }

    return (Boolean)lpVisualizer->getEnabled();
}

Int32 CVisualizer::Native_SetCaptureSize(
    /* [in] */ Int32 size)
{
    android::Visualizer* lpVisualizer = (android::Visualizer *)mNativeVisualizer;
    if (lpVisualizer == NULL) {
        return VISUALIZER_ERROR_NO_INIT;
    }

    return translateError(lpVisualizer->setCaptureSize(size));
}

Int32 CVisualizer::Native_GetCaptureSize()
{
    android::Visualizer* lpVisualizer = (android::Visualizer *)mNativeVisualizer;
    if (lpVisualizer == NULL) {
        return -1;
    }
    return lpVisualizer->getCaptureSize();
}

Int32 CVisualizer::Native_SetScalingMode(
        /* [in] */ Int32 mode)
{
    android::Visualizer* lpVisualizer = (android::Visualizer *)mNativeVisualizer;
    if (lpVisualizer == NULL) {
        return VISUALIZER_ERROR_NO_INIT;
    }

    return translateError(lpVisualizer->setScalingMode(mode));
}

Int32 CVisualizer::Native_GetScalingMode()
{
    android::Visualizer* lpVisualizer = (android::Visualizer *)mNativeVisualizer;
    if (lpVisualizer == NULL) {
        return -1;
    }
    return lpVisualizer->getScalingMode();
}

Int32 CVisualizer::Native_SetMeasurementMode(
    /* [in] */ Int32 mode)
{
    android::Visualizer* lpVisualizer = (android::Visualizer *)mNativeVisualizer;
    if (lpVisualizer == NULL) {
        return VISUALIZER_ERROR_NO_INIT;
    }
    return translateError(lpVisualizer->setMeasurementMode(mode));
}

Int32 CVisualizer::Native_GetMeasurementMode()
{
    android::Visualizer* lpVisualizer = (android::Visualizer *)mNativeVisualizer;
    if (lpVisualizer == NULL) {
        return MEASUREMENT_MODE_NONE;
    }
    return lpVisualizer->getMeasurementMode();
}

Int32 CVisualizer::Native_GetSamplingRate()
{
    android::Visualizer* lpVisualizer = (android::Visualizer *)mNativeVisualizer;
    if (lpVisualizer == NULL) {
        return -1;
    }
    return lpVisualizer->getSamplingRate();
}

Int32 CVisualizer::Native_GetWaveForm(
    /* [in] */ ArrayOf<Byte>* waveform)
{
    android::Visualizer* lpVisualizer = (android::Visualizer *)mNativeVisualizer;
    if (lpVisualizer == NULL) {
        return VISUALIZER_ERROR_NO_INIT;
    }

    return translateError(lpVisualizer->getWaveForm(waveform->GetPayload()));
}

Int32 CVisualizer::Native_GetFft(
    /* [in] */ ArrayOf<Byte>* fft)
{
    android::Visualizer* lpVisualizer = (android::Visualizer *)mNativeVisualizer;
    if (lpVisualizer == NULL) {
        return VISUALIZER_ERROR_NO_INIT;
    }

    return translateError(lpVisualizer->getFft(fft->GetPayload()));
}

Int32 CVisualizer::Native_GetPeakRms(
    /* [in] */ IVisualizerMeasurementPeakRms * measurement)
{
    android::Visualizer* lpVisualizer = (android::Visualizer *)mNativeVisualizer;
    if (lpVisualizer == NULL) {
        return VISUALIZER_ERROR_NO_INIT;
    }
    int32_t measurements[2];
    Int32 status = translateError(
                lpVisualizer->getIntMeasurements(MEASUREMENT_MODE_PEAK_RMS,
                        2, measurements));
    if (status == VISUALIZER_SUCCESS) {
        // measurement worked, write the values to the java object
        ((CVisualizerMeasurementPeakRms*)measurement)->mPeak = measurements[MEASUREMENT_IDX_PEAK];
        ((CVisualizerMeasurementPeakRms*)measurement)->mRms = measurements[MEASUREMENT_IDX_RMS];
    }
    return status;
}

Int32 CVisualizer::Native_SetPeriodicCapture(
    /* [in] */ Int32 rate,
    /* [in] */ Boolean waveForm,
    /* [in] */ Boolean fft)
{
    android::Visualizer* lpVisualizer = (android::Visualizer *)mNativeVisualizer;
    if (lpVisualizer == NULL) {
        return VISUALIZER_ERROR_NO_INIT;
    }
    visualizerJniStorage* lpJniStorage = (visualizerJniStorage*)mJniData;
    if (lpJniStorage == NULL) {
        return VISUALIZER_ERROR_NO_INIT;
    }

    Int32 flags = android::Visualizer::CAPTURE_CALL_JAVA;
    if (waveForm) flags |= android::Visualizer::CAPTURE_WAVEFORM;
    if (fft) flags |= android::Visualizer::CAPTURE_FFT;
    android::Visualizer::capture_cbk_t cbk = captureCallback;
    if (!waveForm && !fft) cbk = NULL;

    return translateError(lpVisualizer->setCaptureCallBack(cbk,
                                                &lpJniStorage->mCallbackData,
                                                flags,
                                                rate));
}

void CVisualizer::PostEventFromNative(
    /* [in] */ CVisualizer* effect_ref,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    CVisualizer* visu = effect_ref;
    if (visu == NULL) {
        return;
    }
    if (visu->mNativeEventHandler != NULL) {
        AutoPtr<IHandler> handler = visu->mNativeEventHandler;

        AutoPtr<IMessage> msg;
        handler->ObtainMessage(what, arg1, arg2, obj, (IMessage**)&msg);
        Boolean result;
        handler->SendMessage(msg, &result);
    }
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
