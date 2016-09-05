
#ifndef __ELASTOS_DROID_APP_NATIVE_ACTIVITY_H__
#define __ELASTOS_DROID_APP_NATIVE_ACTIVITY_H__

#include "elastos/droid/app/Activity.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::View::View;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::IInputQueue;
using Elastos::Droid::View::ISurfaceHolderCallback;
using Elastos::Droid::View::ISurfaceHolderCallback2;
using Elastos::Droid::View::IInputQueueCallback;
using Elastos::Droid::View::IOnGlobalLayoutListener;
using Elastos::Droid::View::InputMethod::IInputMethodManager;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Convenience for implementing an activity that will be implemented
 * purely in native code.  That is, a game (or game-like thing).  There
 * is no need to derive from this class; you can simply declare it in your
 * manifest, and use the NDK APIs from there.
 *
 * <p>A typical manifest would look like:
 *
 * {@sample development/ndk/platforms/android-9/samples/native-activity/AndroidManifest.xml
 *      manifest}
 *
 * <p>A very simple example of native code that is run by NativeActivity
 * follows.  This reads input events from the user and uses OpenGLES to
 * draw into the native activity's window.
 *
 * {@sample development/ndk/platforms/android-9/samples/native-activity/jni/main.c all}
 */
class NativeActivity
    : public Activity
    , public INativeActivity
{
private:
    class InnerListener
        : public Object
        , public ISurfaceHolderCallback2
        , public ISurfaceHolderCallback
        , public IInputQueueCallback
        , public IOnGlobalLayoutListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("NativeActivity::InnerListener")

        InnerListener(
            /* [in] */ NativeActivity* host);

        CARAPI SurfaceCreated(
            /* [in] */ ISurfaceHolder* holder);

        CARAPI SurfaceChanged(
            /* [in] */ ISurfaceHolder* holder,
            /* [in] */ Int32 format,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI SurfaceRedrawNeeded(
            /* [in] */ ISurfaceHolder* holder);

        CARAPI SurfaceDestroyed(
            /* [in] */ ISurfaceHolder* holder);

        CARAPI OnInputQueueCreated(
            /* [in] */ IInputQueue* queue);

        CARAPI OnInputQueueDestroyed(
            /* [in] */ IInputQueue* queue);

        CARAPI OnGlobalLayout();

    private:
        NativeActivity* mHost;
    };

public:
    class NativeContentView
        : public Elastos::Droid::View::View
        , public INativeContentView
    {
    public:
        CAR_INTERFACE_DECL()

        NativeContentView();

        virtual ~NativeContentView();

        CARAPI SetNativeActivity(
            /* [in] */ INativeActivity* activity);

        CARAPI GetNativeActivity(
            /* [out] */ INativeActivity** activity);

        CARAPI constructor(
            /* [in] */ IContext* ctx);

        CARAPI constructor(
            /* [in] */ IContext* ctx,
            /* [in] */ IAttributeSet* attrs);

    private:
        AutoPtr<IWeakReference> mWeakNativeActivity;
    };

public:
    CAR_INTERFACE_DECL()

    NativeActivity();

    virtual ~NativeActivity();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    //@Override
    CARAPI OnLowMemory();

    //@Override
    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasFocus);

    CARAPI SurfaceCreated(
        /* [in] */ ISurfaceHolder* holder);

    CARAPI SurfaceChanged(
        /* [in] */ ISurfaceHolder* holder,
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI SurfaceRedrawNeeded(
        /* [in] */ ISurfaceHolder* holder);

    CARAPI SurfaceDestroyed(
        /* [in] */ ISurfaceHolder* holder);

    CARAPI OnInputQueueCreated(
        /* [in] */ IInputQueue* queue);

    CARAPI OnInputQueueDestroyed(
        /* [in] */ IInputQueue* queue);

    CARAPI OnGlobalLayout();

    CARAPI SetWindowFlags(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 mask);

    CARAPI SetWindowFormat(
        /* [in] */ Int32 format);

    CARAPI ShowIme(
        /* [in] */ Int32 mode);

    CARAPI HideIme(
        /* [in] */ Int32 mode);

protected:
    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI OnStop();

private:
    static String GetAbsolutePath(
        /* [in] */ IFile* file);

private:
    Int64 LoadNativeCode(
        /* [in] */ const String& path,
        /* [in] */ const String& funcname,
        /* [in] */ IMessageQueue* queue,
        /* [in] */ const String& internalDataPath,
        /* [in] */ const String& obbPath,
        /* [in] */ const String& externalDataPath,
        /* [in] */ Int32 sdkVersion,
        /* [in] */ IAssetManager* assetMgr,
        /* [in] */ ArrayOf<Byte>* savedState);

    CARAPI UnLoadNativeCode(
        /* [in] */ Int64 handle);

    CARAPI OnStartNative(
        /* [in] */ Int64 handle);

    CARAPI OnResumeNative(
        /* [in] */ Int64 handle);

    CARAPI OnSaveInstanceStateNative(
        /* [in] */ Int64 handle,
        /* [out, callee] */ ArrayOf<Byte>** state);

    CARAPI OnPauseNative(
        /* [in] */ Int64 handle);

    CARAPI OnStopNative(
        /* [in] */ Int64 handle);

    CARAPI OnConfigurationChangedNative(
        /* [in] */ Int64 handle);

    CARAPI OnLowMemoryNative(
        /* [in] */ Int64 handle);

    CARAPI OnWindowFocusChangedNative(
        /* [in] */ Int64 handle,
        /* [in] */ Boolean focused);

    CARAPI OnSurfaceCreatedNative(
        /* [in] */ Int64 handle,
        /* [in] */ ISurface* surface);

    CARAPI OnSurfaceChangedNative(
        /* [in] */ Int64 handle,
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI OnSurfaceRedrawNeededNative(
        /* [in] */ Int64 handle,
        /* [in] */ ISurface* surface);

    CARAPI OnSurfaceDestroyedNative(
        /* [in] */ Int64 handle);

    CARAPI OnInputQueueCreatedNative(
        /* [in] */ Int64 handle,
        /* [in] */ Int64 queuePtr);

    CARAPI OnInputQueueDestroyedNative(
        /* [in] */ Int64 handle,
        /* [in] */ Int64 queuePtr);

    CARAPI OnContentRectChangedNative(
        /* [in] */ Int64 handle,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

private:
    static const String KEY_NATIVE_SAVED_STATE;// = "android:native_state";

    AutoPtr<INativeContentView> mNativeContentView;
    AutoPtr<IInputMethodManager> mIMM;

    Int64 mNativeHandle;

    AutoPtr<IInputQueue> mCurInputQueue;
    AutoPtr<ISurfaceHolder> mCurSurfaceHolder;

    AutoPtr<ArrayOf<Int32> > mLocation;
    Int32 mLastContentX;
    Int32 mLastContentY;
    Int32 mLastContentWidth;
    Int32 mLastContentHeight;

    Boolean mDispatchingUnhandledKey;

    Boolean mDestroyed;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_NATIVE_ACTIVITY_H__