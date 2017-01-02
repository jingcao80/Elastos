//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/media/CRemoteDisplay.h"
#include "elastos/droid/view/CSurface.h"
#include <elastos/utility/logging/Logger.h>
#include <binder/IServiceManager.h>
#include <media/IRemoteDisplay.h>
#include <media/IMediaPlayerService.h>
#include <media/IRemoteDisplayClient.h>
#include <gui/Surface.h>

using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::View::CSurface;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Core::ICloseGuardHelper;

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
    return mOwner->mListener->OnDisplayConnected(mSurface, mWidth, mHeight, mFlags, mSession);
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

CAR_OBJECT_IMPL(CRemoteDisplay)

CAR_INTERFACE_IMPL(CRemoteDisplay, Object, IRemoteDisplay)

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
     , public Object
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
        /* [in] */ const android::sp<android::IGraphicBufferProducer>& bufferProducer,
        /* [in] */ uint32_t width,
        /* [in] */ uint32_t height,
        /* [in] */ uint32_t flags,
        /* [in] */ uint32_t session)
    {
        if (bufferProducer == NULL) {
            return;
        }

        android::sp<android::Surface> surface(new android::Surface(bufferProducer));
        if (surface == NULL) {
            Logger::E("NativeRemoteDisplayClient", "Could not create Surface from surface texture %p provided by media server.",
                bufferProducer.get());
            return;
        }

        AutoPtr<ISurface> surfaceObj;
        CSurface::New((ISurface**)&surfaceObj);
        if (surfaceObj == NULL) {
            Logger::E("NativeRemoteDisplayClient", "Could not create instance of Surface from ISurfaceTexture.");
            return;
        }

        mRemoteDisplayObjGlobal->NotifyDisplayConnected(surfaceObj, width, height, flags, session);

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
    : public Object
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

    void pause()
    {
        mDisplay->pause();
    }

    void resume()
    {
        mDisplay->resume();
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

void CRemoteDisplay::NativePause(
    /* [in] */ Handle32 ptr)
{
    NativeRemoteDisplay* wrapper = reinterpret_cast<NativeRemoteDisplay*>(ptr);
    wrapper->pause();
}

void CRemoteDisplay::NativeResume(
    /* [in] */ Handle32 ptr)
{
    NativeRemoteDisplay* wrapper = reinterpret_cast<NativeRemoteDisplay*>(ptr);
    wrapper->pause();
}

//------------------------------------------------
ECode CRemoteDisplay::Pause()
{
    NativePause(mPtr);
    return NOERROR;
}

ECode CRemoteDisplay::Resume()
{
    NativeResume(mPtr);
    return NOERROR;
}

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

    mGuard->Open(String("CRemoteDisplay::Dispose"));
}

void CRemoteDisplay::NotifyDisplayConnected(
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 session)
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
