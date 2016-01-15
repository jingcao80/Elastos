
#ifndef __CXBMCACTIVITY_H__
#define __CXBMCACTIVITY_H__

#include "elastos/droid/app/Activity.h"
#include "_CXBMCActivity.h"
#include <elautoptr.h>
#include "android_native_app_glue.h"
#include "android_app_NativeActivity.h"
#include <utils/Looper.h>
#include <gui/Surface.h>
//#include "AInputQueue.h"
#include "NativeInputChannel.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::ISurfaceHolderCallback2;
using Elastos::Droid::View::IInputQueue;
using Elastos::Droid::View::IInputQueueCallback;

namespace Org {
namespace XBMC {

/*
 * Native state for interacting with the NativeActivity class.
 */
struct NativeCode : public ANativeActivity
{
    NativeCode()
    {
        memset((ANativeActivity*)this, 0, sizeof(ANativeActivity));
        memset(&mCallbacks, 0, sizeof(mCallbacks));
        mNativeWindow = NULL;
        // inputChannel = NULL;
        mNativeInputQueue = NULL;
        mMainWorkRead = mMainWorkWrite = -1;
    }

    ~NativeCode() {
        if (mCallbacks.onDestroy != NULL) {
            //mCallbacks.onDestroy(this);
        }

        if (mLooper != NULL && mMainWorkRead >= 0) {
            mLooper->removeFd(mMainWorkRead);
        }
        if (mNativeInputQueue != NULL) {
             mNativeInputQueue->mWorkWrite = -1;
        }
        SetSurface(NULL);
        SetInputChannel(NULL);
        if (mMainWorkRead >= 0)
            close(mMainWorkRead);
        if (mMainWorkWrite >= 0)
            close(mMainWorkWrite);
    }

    void SetSurface(
        /* [in] */ ISurface* surface)
    {
        if (surface) {
            Handle32 ptr;
            surface->GetSurface(&ptr);
            mNativeWindow = (ANativeWindow*)(android::Surface*)ptr;
        }
        else {
            mNativeWindow = NULL;
        }
    }

    android::status_t SetInputChannel(
        /* [in] */ IInputChannel* channel)
    {
        if (mInputChannel != NULL) {
            //TODO;
            //
            //delete mNativeInputQueue;
        }
        mInputChannel = channel;
        mNativeInputQueue = NULL;
        if (channel != NULL) {
            Handle32 nativeChannel;
            channel->GetNativeInputChannel(&nativeChannel);
            android::sp<android::InputChannel> ic =
                ((NativeInputChannel*)nativeChannel)->getInputChannel();
            if (ic != NULL) {
                mNativeInputQueue = new AInputQueue(ic, mMainWorkWrite);
            }
            else {
                return android::UNKNOWN_ERROR;
            }
        }
        return android::OK;
    }

    ANativeActivityCallbacks mCallbacks;

    // String8 internalDataPathObj;
    // String8 externalDataPathObj;
    // String8 obbPathObj;

    ANativeWindow* mNativeWindow;
    int32_t mLastWindowWidth;
    int32_t mLastWindowHeight;

    AutoPtr<IInputChannel> mInputChannel;
    struct AInputQueue* mNativeInputQueue;

    // These are used to wake up the main thread to process work.
    int mMainWorkRead;
    int mMainWorkWrite;
    android::sp<android::Looper> mLooper;
};

class CXBMCActivity
    : public Activity
    , public ISurfaceHolderCallback2
    , public IInputQueueCallback
{
public:
    CXBMCActivity();

    ~CXBMCActivity();

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

    CARAPI SurfaceRedrawNeeded(
        /* [in] */ ISurfaceHolder* holder);

    CARAPI OnInputQueueCreated(
        /* [in] */ IInputQueue* queue);

    CARAPI OnInputQueueDestroyed(
        /* [in] */ IInputQueue* queue);

    CARAPI_(void) UnLoadNativeCode();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasFocus);

private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI LoadNativeCode();

private:
    //NativeCode* mCode;
    AutoPtr<IView> mNativeContentView;
    AutoPtr<IInputQueue> mCurInputQueue;
    AutoPtr<ISurfaceHolder> mCurSurfaceHolder;

    //final int[] mLocation = new int[2];
    // int mLastContentX;
    // int mLastContentY;
    // int mLastContentWidth;
    // int mLastContentHeight;

    //Boolean mDispatchingUnhandledKey;

    Boolean mDestroyed;
};

} // namespace XBMC
} // namespace Org

#endif // __CXBMCACTIVITY_H__
