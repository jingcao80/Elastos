
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CSurfaceTexture.h"
#include "elastos/droid/os/Looper.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <gui/GLConsumer.h>
#include <gui/Surface.h>
#include <gui/IGraphicBufferProducer.h>
#include <gui/IGraphicBufferConsumer.h>
#include <gui/BufferQueue.h>
#include <utils/misc.h>
#include <utils/Errors.h>

using Elastos::Droid::Os::Looper;
using android::GLConsumer;
using android::BufferQueue;
using android::IGraphicBufferProducer;
using android::IGraphicBufferConsumer;

namespace Elastos {
namespace Droid {
namespace Graphics {

CSurfaceTexture::AvailableHandler::AvailableHandler(
    /* [in] */ IOnFrameAvailableListener* listener,
    /* [in] */ CSurfaceTexture* host)
    : mHost(host)
    , mListener(listener)
{}

ECode CSurfaceTexture::AvailableHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return mListener->OnFrameAvailable(mHost);
}

CAR_OBJECT_IMPL(CSurfaceTexture);

CAR_INTERFACE_IMPL(CSurfaceTexture, Object, ISurfaceTexture);

CSurfaceTexture::CSurfaceTexture()
    : mSurfaceTexture(0)
{}

CSurfaceTexture::~CSurfaceTexture()
{
    NativeFinalize();
}

ECode CSurfaceTexture::constructor(
    /* [in] */ Int32 texName)
{
    return constructor(texName, FALSE);
}

ECode CSurfaceTexture::constructor(
    /* [in] */ Int32 texName,
    /* [in] */ Boolean singleBufferMode)
{
    mCreatorLooper = Looper::GetMyLooper();
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    return NativeInit(FALSE, texName, singleBufferMode, wr);
}

ECode CSurfaceTexture::constructor(
    /* [in] */ Boolean singleBufferMode)
{
    mCreatorLooper = Looper::GetMyLooper();
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    return NativeInit(TRUE, 0, singleBufferMode, wr);
}

ECode CSurfaceTexture::SetOnFrameAvailableListener(
    /* [in] */ IOnFrameAvailableListener* listener)
{
    return SetOnFrameAvailableListener(listener, NULL);
}

ECode CSurfaceTexture::SetOnFrameAvailableListener(
    /* [in] */ /*@Nullable*/ IOnFrameAvailableListener* listener,
    /* [in] */ /*@Nullable*/ IHandler* handler)
{
    if (listener != NULL) {
        // Although we claim the thread is arbitrary, earlier implementation would
        // prefer to send the callback on the creating looper or the main looper
        // so we preserve this behavior here.
        AutoPtr<ILooper> tmp;
        AutoPtr<ILooper> looper = handler != NULL ? (handler->GetLooper((ILooper**)&tmp), tmp) :
                mCreatorLooper != NULL ? mCreatorLooper : Looper::GetMainLooper();
        mOnFrameAvailableHandler = new AvailableHandler(listener, this);
        mOnFrameAvailableHandler->constructor(looper);
    }
    else {
        mOnFrameAvailableHandler = NULL;
    }
    return NOERROR;
}

ECode CSurfaceTexture::SetDefaultBufferSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    NativeSetDefaultBufferSize(width, height);
    return NOERROR;
}

ECode CSurfaceTexture::UpdateTexImage()
{
    return NativeUpdateTexImage();
}

ECode CSurfaceTexture::ReleaseTexImage()
{
    return NativeReleaseTexImage();
}

ECode CSurfaceTexture::DetachFromGLContext()
{
    Int32 err = NativeDetachFromGLContext();
    if (err != 0) {
        //throw new RuntimeException("Error during detachFromGLContext (see logcat for details)");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CSurfaceTexture::AttachToGLContext(
    /* [in] */ Int32 texName)
{
    Int32 err = NativeAttachToGLContext(texName);
    if (err != 0) {
        //throw new RuntimeException("Error during attachToGLContext (see logcat for details)");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CSurfaceTexture::GetTransformMatrix(
    /* [in] */ ArrayOf<Float>* mtx)
{
    // Note we intentionally don't check mtx for null, so this will result in a
    // NullPointerException. But it's safe because it happens before the call to native.
    if (mtx->GetLength() != 16) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    NativeGetTransformMatrix(mtx);
    return NOERROR;
}

ECode CSurfaceTexture::GetTimestamp(
    /* [out] */ Int64* timestamp)
{
    VALIDATE_NOT_NULL(timestamp);
    *timestamp = NativeGetTimestamp();
    return NOERROR;
}

ECode CSurfaceTexture::ReleaseResources()
{
    NativeRelease();
    return NOERROR;
}

void CSurfaceTexture::PostEventFromNative(
    /* [in] */ IWeakReference/*<SurfaceTexture>*/* weakSelf)
{
    AutoPtr<ISurfaceTexture> st;
    weakSelf->Resolve(EIID_ISurfaceTexture, (IInterface**)&st);

    if (st != NULL) {
        AutoPtr<IHandler> handler = ((CSurfaceTexture*)st.Get())->mOnFrameAvailableHandler;
        if (handler != NULL) {
            Boolean result = FALSE;
            handler->SendEmptyMessage(0, &result);
        }
    }
}

ECode CSurfaceTexture::GetSurfaceTexture(
    /* [out] */ Int64* texture)
{
    VALIDATE_NOT_NULL(texture)
    *texture = mSurfaceTexture;
    return NOERROR;
}

ECode CSurfaceTexture::GetProducer(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mProducer;
    return NOERROR;
}

static void SurfaceTexture_setSurfaceTexture(
    /* [in] */ CSurfaceTexture* thiz,
    /* [in] */ const android::sp<GLConsumer>& surfaceTexture)
{
    GLConsumer* const p = (GLConsumer*)thiz->mSurfaceTexture;
    if (surfaceTexture.get()) {
        surfaceTexture->incStrong((void*)SurfaceTexture_setSurfaceTexture);
    }
    if (p) {
        p->decStrong((void*)SurfaceTexture_setSurfaceTexture);
    }
    thiz->mSurfaceTexture = (Int64)surfaceTexture.get();
}

static void SurfaceTexture_setProducer(
    /* [in] */ CSurfaceTexture* thiz,
    /* [in] */ const android::sp<IGraphicBufferProducer>& producer)
{
    IGraphicBufferProducer* const p =
        (IGraphicBufferProducer*)thiz->mProducer;
    if (producer.get()) {
        producer->incStrong((void*)SurfaceTexture_setProducer);
    }
    if (p) {
        p->decStrong((void*)SurfaceTexture_setProducer);
    }
    thiz->mProducer = (Int64)producer.get();
}

static void SurfaceTexture_setFrameAvailableListener(
    /* [in] */ CSurfaceTexture* thiz,
    /* [in] */ android::sp<GLConsumer::FrameAvailableListener> listener)
{
    GLConsumer::FrameAvailableListener* const p = (GLConsumer::FrameAvailableListener*)thiz->mFrameAvailableListener;
    if (listener.get()) {
        listener->incStrong((void*)SurfaceTexture_setSurfaceTexture);
    }
    if (p) {
        p->decStrong((void*)SurfaceTexture_setSurfaceTexture);
    }
    thiz->mFrameAvailableListener = (Int64)listener.get();
}

android::sp<GLConsumer> SurfaceTexture_getSurfaceTexture(
    /* [in] */ CSurfaceTexture* thiz)
{
    return (GLConsumer*)thiz->mSurfaceTexture;
}

// ----------------------------------------------------------------------------
class DroidSurfaceTextureContext : public GLConsumer::FrameAvailableListener
{
public:
    DroidSurfaceTextureContext(
        /* [in] */ IWeakReference* weakThiz);
    virtual ~DroidSurfaceTextureContext();
    virtual void onFrameAvailable();

private:
    AutoPtr<IWeakReference> mWeakThiz;
};

DroidSurfaceTextureContext::DroidSurfaceTextureContext(
    /* [in] */ IWeakReference* weakThiz)
    : mWeakThiz(weakThiz)
{}

DroidSurfaceTextureContext::~DroidSurfaceTextureContext()
{
    mWeakThiz = NULL;
}

void DroidSurfaceTextureContext::onFrameAvailable()
{
    CSurfaceTexture::PostEventFromNative(mWeakThiz);
}
// ----------------------------------------------------------------------------

ECode CSurfaceTexture::NativeInit(
    /* [in] */ Boolean isDetached,
    /* [in] */ Int32 texName,
    /* [in] */ Boolean singleBufferMode,
    /* [in] */ IWeakReference/*<SurfaceTexture>*/* weakSelf) /*throws Surface.OutOfResourcesException*/
{
    android::sp<IGraphicBufferProducer> producer;
    android::sp<IGraphicBufferConsumer> consumer;
    BufferQueue::createBufferQueue(&producer, &consumer);

    if (singleBufferMode) {
        consumer->disableAsyncBuffer();
        consumer->setDefaultMaxBufferCount(1);
    }

    android::sp<GLConsumer> surfaceTexture;
    if (isDetached) {
        surfaceTexture = new GLConsumer(consumer, GL_TEXTURE_EXTERNAL_OES,
                true, true);
    } else {
        surfaceTexture = new GLConsumer(consumer, texName,
                GL_TEXTURE_EXTERNAL_OES, true, true);
    }

    if (surfaceTexture == 0) {
        // jniThrowException(env, OutOfResourcesException,
        //         "Unable to create native SurfaceTexture");
        return E_OUT_OF_RESOURCES_EXCEPTION;
    }
    SurfaceTexture_setSurfaceTexture(this, surfaceTexture);
    SurfaceTexture_setProducer(this, producer);

    // jclass clazz = env->GetObjectClass(thiz);
    // if (clazz == NULL) {
    //     // jniThrowRuntimeException(env,
    //     //         "Can't find android/graphics/SurfaceTexture");
    //     return E_RUNTIME_EXCEPTION;
    // }

    android::sp<DroidSurfaceTextureContext> ctx(new DroidSurfaceTextureContext(weakSelf));
    surfaceTexture->setFrameAvailableListener(ctx);
    SurfaceTexture_setFrameAvailableListener(this, ctx);
    return NOERROR;
}

ECode CSurfaceTexture::NativeReleaseTexImage()
{
    android::sp<GLConsumer> surfaceTexture(SurfaceTexture_getSurfaceTexture(this));
    android::status_t err = surfaceTexture->releaseTexImage();
    if (err == android::INVALID_OPERATION) {
        // jniThrowException(env, IllegalStateException, "Unable to release texture contents (see "
        //         "logcat for details)");
        return E_ILLEGAL_STATE_EXCEPTION;
    } else if (err < 0) {
        // jniThrowRuntimeException(env, "Error during updateTexImage (see logcat for details)");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

void CSurfaceTexture::NativeFinalize()
{
    android::sp<GLConsumer> surfaceTexture(SurfaceTexture_getSurfaceTexture(this));
    surfaceTexture->setFrameAvailableListener(0);
    SurfaceTexture_setFrameAvailableListener(this, 0);
    SurfaceTexture_setSurfaceTexture(this, 0);
    SurfaceTexture_setProducer(this, 0);
}

void CSurfaceTexture::NativeGetTransformMatrix(
    /* [in] */ ArrayOf<Float>* mtxArray)
{
    android::sp<GLConsumer> surfaceTexture(SurfaceTexture_getSurfaceTexture(this));
    float* mtx = mtxArray->GetPayload();
    surfaceTexture->getTransformMatrix(mtx);
}

Int64 CSurfaceTexture::NativeGetTimestamp()
{
    android::sp<GLConsumer> surfaceTexture(SurfaceTexture_getSurfaceTexture(this));
    return surfaceTexture->getTimestamp();
}

void CSurfaceTexture::NativeSetDefaultBufferSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    android::sp<GLConsumer> surfaceTexture(SurfaceTexture_getSurfaceTexture(this));
    surfaceTexture->setDefaultBufferSize(width, height);
}

ECode CSurfaceTexture::NativeUpdateTexImage()
{
    android::sp<GLConsumer> surfaceTexture(SurfaceTexture_getSurfaceTexture(this));
    android::status_t err = surfaceTexture->updateTexImage();
    if (err == android::INVALID_OPERATION) {
        // jniThrowException(env, IllegalStateException, "Unable to update texture contents (see "
        //         "logcat for details)");
        return E_ILLEGAL_STATE_EXCEPTION;
    } else if (err < 0) {
        // jniThrowRuntimeException(env, "Error during updateTexImage (see logcat for details)");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

Int32 CSurfaceTexture::NativeDetachFromGLContext()
{
    android::sp<GLConsumer> surfaceTexture(SurfaceTexture_getSurfaceTexture(this));
    return surfaceTexture->detachFromContext();
}

Int32 CSurfaceTexture::NativeAttachToGLContext(
    /* [in] */ Int32 tex)
{
    android::sp<GLConsumer> surfaceTexture(SurfaceTexture_getSurfaceTexture(this));
    return surfaceTexture->attachToContext((GLuint)tex);
}

void CSurfaceTexture::NativeRelease()
{
    android::sp<GLConsumer> surfaceTexture(SurfaceTexture_getSurfaceTexture(this));
    surfaceTexture->abandon();
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
