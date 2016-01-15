
#include "elastos/droid/media/CRemoteDisplay.h"
#include <binder/IServiceManager.h>
#include <media/IRemoteDisplay.h>
#include <media/IMediaPlayerService.h>
#include <media/IRemoteDisplayClient.h>
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/view/CSurface.h"

using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::Graphics::ISurfaceTexture;

namespace Elastos {
namespace Droid {
namespace Media {

//---------------------------------------------
//    CRemoteDisplay::NotifyDisplayConnectedRun
//---------------------------------------------

CRemoteDisplay::NotifyDisplayConnectedRun::NotifyDisplayConnectedRun(
    /* [in] */ CRemoteDisplay* owner,
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 flags)
    : mOwner(owner)
    , mSurface(surface)
    , mWidth(width)
    , mHeight(height)
    , mFlags(flags)
{
}

ECode CRemoteDisplay::NotifyDisplayConnectedRun::Run()
{
    return mOwner->mListener->OnDisplayConnected(mSurface, mWidth, mHeight, mFlags);
}

//------------------------------------------------
//    CRemoteDisplay::NotifyDisplayDisconnectedRun
//------------------------------------------------

CRemoteDisplay::NotifyDisplayDisconnectedRun::NotifyDisplayDisconnectedRun(
    /* [in] */ CRemoteDisplay* owner)
    : mOwner(owner)
{}

ECode CRemoteDisplay::NotifyDisplayDisconnectedRun::Run()
{
    return mOwner->mListener->OnDisplayDisconnected();
}

//------------------------------------------------
//    CRemoteDisplay::NotifyDisplayErrorRun
//------------------------------------------------

CRemoteDisplay::NotifyDisplayErrorRun::NotifyDisplayErrorRun(
    /* [in] */ CRemoteDisplay* owner,
    /* [in] */ Int32 error)
    : mOwner(owner)
    , mError(error)
{}

ECode CRemoteDisplay::NotifyDisplayErrorRun::Run()
{
    return mOwner->mListener->OnDisplayError(mError);
}

//------------------------------------------------

CRemoteDisplay::CRemoteDisplay()
    : mPtr(0)
{
}

CRemoteDisplay::~CRemoteDisplay()
{
    Finalize();
}

ECode CRemoteDisplay::constructor(
    /* [in] */ IRemoteDisplayListener* listener,
    /* [in] */ IHandler* handler)
{
    mListener = listener;
    mHandler = handler;

    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mGuard);

    return NOERROR;
}

ECode CRemoteDisplay::Dispose()
{
    Dispose(FALSE);
    return NOERROR;
}

/*static*/
ECode CRemoteDisplay::Listen(
    /* [in] */ const String& iface,
    /* [in] */ IRemoteDisplayListener* listener,
    /* [in] */ IHandler* handler,
    /* [out] */ IRemoteDisplay** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (iface == NULL) {
        Logger::E("RemoteDisplay", "iface must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (listener == NULL) {
        Logger::E("RemoteDisplay", "listener must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (handler == NULL) {
        Logger::E("RemoteDisplay", "handler must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<CRemoteDisplay> display;
    CRemoteDisplay::NewByFriend(listener, handler, (CRemoteDisplay**)&display);

    display->StartListening(iface);
    *result = (IRemoteDisplay*)display.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void CRemoteDisplay::Finalize() // throws Throwable
{
//    try {
        Dispose(TRUE);
//    } finally {
//        super.finalize();
//    }
}

//------------------------------------------------
//    JNI Method : android_media_RemoteDisplay.cpp
//------------------------------------------------

class NativeRemoteDisplayClient
     : public android::BnRemoteDisplayClient
     , public ElRefBase
{
public:
    NativeRemoteDisplayClient(
        /* [in] */ CRemoteDisplay* remoteDisplayObj)
         : mRemoteDisplayObjGlobal(remoteDisplayObj)
    {}

protected:
    ~NativeRemoteDisplayClient()
    {}

public:
    virtual void onDisplayConnected(
        /* [in] */ const android::sp<android::ISurfaceTexture>& surfaceTexture,
        /* [in] */ uint32_t width,
        /* [in] */ uint32_t height,
        /* [in] */ uint32_t flags)
    {
        if (surfaceTexture == NULL) {
            return;
        }

        android::sp<android::Surface> surface(new android::Surface(surfaceTexture));
        if (surface == NULL) {
            Logger::E("NativeRemoteDisplayClient", "Could not create Surface from surface texture %p provided by media server.",
                surfaceTexture.get());
            return;
        }

        AutoPtr<ISurface> surfaceObj;
        CSurface::New((ISurface**)&surfaceObj);
        if (surfaceObj == NULL) {
            Logger::E("NativeRemoteDisplayClient", "Could not create instance of Surface from ISurfaceTexture.");
            return;
        }

        surfaceObj->SetSurface((Handle32)surface.get());

        mRemoteDisplayObjGlobal->NotifyDisplayConnected(surfaceObj, width, height, flags);

        CheckAndClearExceptionFromCallback("notifyDisplayConnected");
    }

    virtual void onDisplayDisconnected()
    {
        mRemoteDisplayObjGlobal->NotifyDisplayDisconnected();
        CheckAndClearExceptionFromCallback("notifyDisplayDisconnected");
    }

    virtual void onDisplayError(
        /* [in] */ Int32 error)
    {
        mRemoteDisplayObjGlobal->NotifyDisplayError(error);
        CheckAndClearExceptionFromCallback("notifyDisplayError");
    }

private:
    AutoPtr<CRemoteDisplay> mRemoteDisplayObjGlobal;

   static void CheckAndClearExceptionFromCallback(const char* methodName) {
       // if (env->ExceptionCheck()) {
       //     ALOGE("An exception was thrown by callback '%s'.", methodName);
       //     LOGE_EX(env);
       //     env->ExceptionClear();
       // }
   }
};

class NativeRemoteDisplay
    : public ElRefBase
{
public:
    NativeRemoteDisplay(
        android::IRemoteDisplay* display,
        const android::sp<NativeRemoteDisplayClient>& client)
         : mDisplay(display)
         , mClient(client)
    {}

    ~NativeRemoteDisplay()
    {
        mDisplay->dispose();
    }

private:
    android::sp<android::IRemoteDisplay> mDisplay;
    android::sp<NativeRemoteDisplayClient> mClient;
};

Handle32 CRemoteDisplay::NativeListen(
    /* [in] */ const String& iface)
{
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    android::sp<android::IMediaPlayerService> service = android::interface_cast<android::IMediaPlayerService>(
            sm->getService(android::String16("media.player")));
    if (service == NULL) {
        Logger::E("RemoteDisplay", "Could not obtain IMediaPlayerService from service manager");
        return 0;
    }

    android::sp<NativeRemoteDisplayClient> client = new NativeRemoteDisplayClient(this);
    android::sp<android::IRemoteDisplay> display = service->listenForRemoteDisplay(client, android::String8(iface.string()));
    if (display == NULL) {
         Logger::E("RemoteDisplay", "Media player service rejected request to listen for remote display '%s'.", iface.string());
         return 0;
    }

    AutoPtr<NativeRemoteDisplay> wrapper = new NativeRemoteDisplay(display.get(), client);
    return (Handle32)wrapper.Get();
}

void CRemoteDisplay::NativeDispose(
    /* [in] */ Handle32 ptr)
{
    NativeRemoteDisplay* wrapper = reinterpret_cast<NativeRemoteDisplay*>(ptr);
    delete wrapper;
}

//------------------------------------------------

void CRemoteDisplay::Dispose(
    /* [in] */ Boolean finalized)
{
    if (mPtr != 0) {
        if (mGuard != NULL) {
            if (finalized) {
                mGuard->WarnIfOpen();
            } else {
                mGuard->Close();
            }
        }
        NativeDispose(mPtr);
        mPtr = 0;
    }
}

void CRemoteDisplay::StartListening(
    /* [in] */ const String& iface)
{
    mPtr = NativeListen(iface);
    if (mPtr == 0) {
        Logger::E("RemoteDisplay", "Could not start listening for remote display connection on \"%s\"",
            iface.string());
    }

    mGuard->Open(String("dispose"));
}

void CRemoteDisplay::NotifyDisplayConnected(
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 flags)
{
    Boolean tempState;
    AutoPtr<IRunnable> runnable = new NotifyDisplayConnectedRun(this, surface, width, height, flags);
    mHandler->Post(runnable, &tempState);
}

void CRemoteDisplay::NotifyDisplayDisconnected()
{
    Boolean tempState;
    AutoPtr<IRunnable> runnable = new NotifyDisplayDisconnectedRun(this);
    mHandler->Post(runnable, &tempState);
}

void CRemoteDisplay::NotifyDisplayError(
    /* [in] */ Int32 error)
{
    Boolean tempState;
    AutoPtr<IRunnable> runnable = new NotifyDisplayErrorRun(this, error);
    mHandler->Post(runnable, &tempState);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
