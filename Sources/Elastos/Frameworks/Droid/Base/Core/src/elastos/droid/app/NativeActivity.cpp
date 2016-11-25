
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/app/NativeActivity.h"
#include "elastos/droid/app/CNativeContentView.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/NativeMessageQueue.h"
#include "elastos/droid/view/Surface.h"
#include <elastos/utility/logging/Logger.h>

#include <poll.h>
#include <dlfcn.h>
#include <fcntl.h>

#include <input/InputTransport.h>
#include <gui/Surface.h>
#include <system/window.h>
#include <utils/Looper.h>

#include <android/native_activity.h>
#include <androidfw/AssetManager.h>
#include <nativebridge/native_bridge.h>

using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::NativeMessageQueue;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::Surface;
using Elastos::Droid::View::EIID_ISurfaceHolderCallback;
using Elastos::Droid::View::EIID_ISurfaceHolderCallback2;
using Elastos::Droid::View::EIID_IInputQueueCallback;
using Elastos::Droid::View::EIID_IOnGlobalLayoutListener;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

//========================================================================
// NativeActivity::NativeContentView
//========================================================================
CAR_INTERFACE_IMPL_4(NativeActivity::InnerListener, Object, \
    ISurfaceHolderCallback2, ISurfaceHolderCallback, \
    IInputQueueCallback, IOnGlobalLayoutListener)

NativeActivity::InnerListener::InnerListener(
    /* [in] */ NativeActivity* host)
    : mHost(host)
{}

ECode NativeActivity::InnerListener::SurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    return mHost->SurfaceCreated(holder);
}

ECode NativeActivity::InnerListener::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return mHost->SurfaceChanged(holder, format, width, height);
}

ECode NativeActivity::InnerListener::SurfaceRedrawNeeded(
    /* [in] */ ISurfaceHolder* holder)
{
    return mHost->SurfaceRedrawNeeded(holder);
}

ECode NativeActivity::InnerListener::SurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    return mHost->SurfaceDestroyed(holder);
}

ECode NativeActivity::InnerListener::OnInputQueueCreated(
    /* [in] */ IInputQueue* queue)
{
    return mHost->OnInputQueueCreated(queue);
}

ECode NativeActivity::InnerListener::OnInputQueueDestroyed(
    /* [in] */ IInputQueue* queue)
{
    return mHost->OnInputQueueDestroyed(queue);
}

ECode NativeActivity::InnerListener::OnGlobalLayout()
{
    return mHost->OnGlobalLayout();
}

//========================================================================
// NativeActivity::NativeContentView
//========================================================================
CAR_INTERFACE_IMPL(NativeActivity::NativeContentView, Elastos::Droid::View::View, INativeContentView)

NativeActivity::NativeContentView::NativeContentView()
{
}

NativeActivity::NativeContentView::~NativeContentView()
{}

ECode NativeActivity::NativeContentView::SetNativeActivity(
    /* [in] */ INativeActivity* activity)
{
    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(activity);
    mWeakNativeActivity = NULL;
    wrs->GetWeakReference((IWeakReference**)&mWeakNativeActivity);
    return NOERROR;
}

ECode NativeActivity::NativeContentView::GetNativeActivity(
    /* [out] */ INativeActivity** activity)
{
    VALIDATE_NOT_NULL(activity)
    AutoPtr<INativeActivity> obj;
    mWeakNativeActivity->Resolve(EIID_INativeActivity, (IInterface**)&obj);
    *activity = obj;
    REFCOUNT_ADD(*activity)
    return NOERROR;
}

ECode NativeActivity::NativeContentView::constructor(
    /* [in] */ IContext* ctx)
{
    return Elastos::Droid::View::View::constructor(ctx);
}

ECode NativeActivity::NativeContentView::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return Elastos::Droid::View::View::constructor(ctx, attrs);
}

//========================================================================
// NativeActivity
//========================================================================
const String NativeActivity::KEY_NATIVE_SAVED_STATE("android:native_state");

CAR_INTERFACE_IMPL_5(NativeActivity, Activity, INativeActivity, \
    ISurfaceHolderCallback2, ISurfaceHolderCallback, \
    IInputQueueCallback, IOnGlobalLayoutListener)

NativeActivity::NativeActivity()
    : mNativeHandle(0)
    , mLastContentX(0)
    , mLastContentY(0)
    , mLastContentWidth(0)
    , mLastContentHeight(0)
    , mDispatchingUnhandledKey(FALSE)
    , mDestroyed(FALSE)
{
    mLocation = ArrayOf<Int32>::Alloc(2);
}

NativeActivity::~NativeActivity()
{}

ECode NativeActivity::constructor()
{
    return Activity::constructor();
}

ECode NativeActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    String libname("main");
    String funcname("ANativeActivity_onCreate");
    AutoPtr<IActivityInfo> ai;
    AutoPtr<InnerListener> listener = new InnerListener(this);

    AutoPtr<IInterface> obj;
    GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    mIMM = IInputMethodManager::Probe(obj);

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    window->TakeSurface(listener);
    window->TakeInputQueue(listener);
    window->SetFormat(IPixelFormat::RGB_565);
    window->SetSoftInputMode(
        IWindowManagerLayoutParams::SOFT_INPUT_STATE_UNSPECIFIED
        | IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_RESIZE);

    CNativeContentView::New(this, (INativeContentView**)&mNativeContentView);
    mNativeContentView->SetNativeActivity(this);
    IView* view = IView::Probe(mNativeContentView);
    SetContentView(view);
    Boolean bval;
    view->RequestFocus(&bval);
    AutoPtr<IViewTreeObserver> vto;
    view->GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->AddOnGlobalLayoutListener(listener);

    // try {
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IComponentName> cn;
    intent->GetComponent((IComponentName**)&cn);
    AutoPtr<IPackageManager> pm;
    ECode ec = pm->GetActivityInfo(cn, IPackageManager::GET_META_DATA, (IActivityInfo**)&ai);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        return E_RUNTIME_EXCEPTION;
    }
    AutoPtr<IBundle> bundle;
    IPackageItemInfo::Probe(ai)->GetMetaData((IBundle**)&bundle);
    if (bundle != NULL) {
        String ln;
        bundle->GetString(INativeActivity::META_DATA_LIB_NAME, &ln);
        if (ln != NULL) libname = ln;
        bundle->GetString(INativeActivity::META_DATA_FUNC_NAME, &ln);
        if (ln != NULL) funcname = ln;
    }
    // } catch (PackageManager.NameNotFoundException e) {
    //     throw new RuntimeException("Error getting activity info", e);
    // }

    String path;

    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(ai)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    String nativeLibraryDir;
    appInfo->GetNativeLibraryDir(&nativeLibraryDir);
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String name;
    system->MapLibraryName(libname, &name);
    AutoPtr<IFile> libraryFile;
    CFile::New(nativeLibraryDir, name, (IFile**)&libraryFile);
    libraryFile->Exists(&bval);
    if (bval) {
        libraryFile->GetPath(&path);
    }

    if (path == NULL) {
        Logger::E("NativeActivity", "Unable to find native library: %s", libname.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > nativeSavedState;
    if (savedInstanceState != NULL) {
        savedInstanceState->GetByteArray(KEY_NATIVE_SAVED_STATE, (ArrayOf<Byte>**)&nativeSavedState);
    }

    AutoPtr<IFile> filesDir, obbDir, externalFilesDir;
    GetFilesDir((IFile**)&filesDir);
    GetObbDir((IFile**)&obbDir);
    GetExternalFilesDir(String(NULL), (IFile**)&externalFilesDir);
    AutoPtr<IAssetManager> assetMgr;
    GetAssets((IAssetManager**)&assetMgr);
    mNativeHandle = LoadNativeCode(path, funcname, Looper::GetMyQueue(),
        GetAbsolutePath(filesDir), GetAbsolutePath(obbDir),
        GetAbsolutePath(externalFilesDir),
        Build::VERSION::SDK_INT, assetMgr, nativeSavedState);

    if (mNativeHandle == 0) {
        Logger::E("NativeActivity", "Unable to load native library: %s", path.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return Activity::OnCreate(savedInstanceState);
}

String NativeActivity::GetAbsolutePath(
    /* [in] */ IFile* file)
{
    String path;
    if (file != NULL) {
        file->GetAbsolutePath(&path);
    }
    return path;
}

ECode NativeActivity::OnDestroy()
{
    mDestroyed = TRUE;
    if (mCurSurfaceHolder != NULL) {
        OnSurfaceDestroyedNative(mNativeHandle);
        mCurSurfaceHolder = NULL;
    }
    if (mCurInputQueue != NULL) {
        Int64 ptr;
        mCurInputQueue->GetNativePtr(&ptr);
        OnInputQueueDestroyedNative(mNativeHandle, ptr);
        mCurInputQueue = NULL;
    }
    UnLoadNativeCode(mNativeHandle);
    return Activity::OnDestroy();
}

ECode NativeActivity::OnPause()
{
    Activity::OnPause();
    return OnPauseNative(mNativeHandle);
}

ECode NativeActivity::OnResume()
{
    Activity::OnResume();
    return OnResumeNative(mNativeHandle);
}

ECode NativeActivity::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    Activity::OnSaveInstanceState(outState);
    AutoPtr<ArrayOf<Byte> > array;
    OnSaveInstanceStateNative(mNativeHandle, (ArrayOf<Byte>**)&array);
    if (array != NULL) {
        outState->PutByteArray(KEY_NATIVE_SAVED_STATE, array);
    }
    return NOERROR;
}

ECode NativeActivity::OnStart()
{
    Activity::OnStart();
    return OnStartNative(mNativeHandle);
}

ECode NativeActivity::OnStop()
{
    Activity::OnStop();
    return OnStopNative(mNativeHandle);
}

ECode NativeActivity::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    Activity::OnConfigurationChanged(newConfig);
    if (!mDestroyed) {
        return OnConfigurationChangedNative(mNativeHandle);
    }
    return NOERROR;
}

ECode NativeActivity::OnLowMemory()
{
    Activity::OnLowMemory();
    if (!mDestroyed) {
        return OnLowMemoryNative(mNativeHandle);
    }
    return NOERROR;
}

ECode NativeActivity::OnWindowFocusChanged(
    /* [in] */ Boolean hasFocus)
{
    Activity::OnWindowFocusChanged(hasFocus);
    if (!mDestroyed) {
        return OnWindowFocusChangedNative(mNativeHandle, hasFocus);
    }
    return NOERROR;
}

ECode NativeActivity::SurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    if (!mDestroyed) {
        mCurSurfaceHolder = holder;
        AutoPtr<ISurface> surface;
        holder->GetSurface((ISurface**)&surface);
        return OnSurfaceCreatedNative(mNativeHandle, surface);
    }
    return NOERROR;
}

ECode NativeActivity::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (!mDestroyed) {
        mCurSurfaceHolder = holder;
        AutoPtr<ISurface> surface;
        holder->GetSurface((ISurface**)&surface);
        return OnSurfaceChangedNative(mNativeHandle, surface, format, width, height);
    }
    return NOERROR;
}

ECode NativeActivity::SurfaceRedrawNeeded(
    /* [in] */ ISurfaceHolder* holder)
{
    if (!mDestroyed) {
        mCurSurfaceHolder = holder;
        AutoPtr<ISurface> surface;
        holder->GetSurface((ISurface**)&surface);
        return OnSurfaceRedrawNeededNative(mNativeHandle, surface);
    }
    return NOERROR;
}

ECode NativeActivity::SurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    mCurSurfaceHolder = NULL;
    if (!mDestroyed) {
        return OnSurfaceDestroyedNative(mNativeHandle);
    }
    return NOERROR;
}

ECode NativeActivity::OnInputQueueCreated(
    /* [in] */ IInputQueue* queue)
{
    if (!mDestroyed) {
        mCurInputQueue = queue;
        Int64 ptr;
        queue->GetNativePtr(&ptr);
        return OnInputQueueCreatedNative(mNativeHandle, ptr);
    }
    return NOERROR;
}

ECode NativeActivity::OnInputQueueDestroyed(
    /* [in] */ IInputQueue* queue)
{
    if (!mDestroyed) {
        Int64 ptr;
        queue->GetNativePtr(&ptr);
        OnInputQueueDestroyedNative(mNativeHandle, ptr);
        mCurInputQueue = NULL;
    }
    return NOERROR;
}

ECode NativeActivity::OnGlobalLayout()
{
    IView* view = IView::Probe(mNativeContentView);
    view->GetLocationInWindow(mLocation);
    Int32 w, h;
    view->GetWidth(&w);
    view->GetHeight(&h);
    if ((*mLocation)[0] != mLastContentX || (*mLocation)[1] != mLastContentY
            || w != mLastContentWidth || h != mLastContentHeight) {
        mLastContentX = (*mLocation)[0];
        mLastContentY = (*mLocation)[1];
        mLastContentWidth = w;
        mLastContentHeight = h;
        if (!mDestroyed) {
            return OnContentRectChangedNative(mNativeHandle, mLastContentX,
                mLastContentY, mLastContentWidth, mLastContentHeight);
        }
    }
    return NOERROR;
}

ECode NativeActivity::SetWindowFlags(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 mask)
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return window->SetFlags(flags, mask);
}

ECode NativeActivity::SetWindowFormat(
    /* [in] */ Int32 format)
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    return window->SetFormat(format);
}

ECode NativeActivity::ShowIme(
    /* [in] */ Int32 mode)
{
    IView* view = IView::Probe(mNativeContentView);
    Boolean bval;
    return mIMM->ShowSoftInput(view, mode, &bval);
}

ECode NativeActivity::HideIme(
    /* [in] */ Int32 mode)
{
    AutoPtr<IBinder> token;
    IView::Probe(mNativeContentView)->GetWindowToken((IBinder**)&token);
    Boolean bval;
    return mIMM->HideSoftInputFromWindow(token, mode, &bval);
}

//============================================================================
// native code
//============================================================================
#define LOG_TRACE(...)
//#define LOG_TRACE(...) ALOG(LOG_DEBUG, LOG_TAG, __VA_ARGS__)

/*
 * Native state for interacting with the NativeActivity class.
 */
class NativeCode : public ANativeActivity
{
public:
    NativeCode(void* _dlhandle, ANativeActivity_createFunc* _createFunc)
    {
        memset((ANativeActivity*)this, 0, sizeof(ANativeActivity));
        memset(&callbacks, 0, sizeof(callbacks));
        dlhandle = _dlhandle;
        createActivityFunc = _createFunc;
        nativeWindow = NULL;
        mainWorkRead = mainWorkWrite = -1;
    }

    ~NativeCode()
    {
        if (callbacks.onDestroy != NULL) {
            callbacks.onDestroy(this);
        }

        if (mLooper != NULL && mainWorkRead >= 0) {
            mLooper->removeFd(mainWorkRead);
        }
        setSurface(NULL);
        if (mainWorkRead >= 0) close(mainWorkRead);
        if (mainWorkWrite >= 0) close(mainWorkWrite);
        if (dlhandle != NULL) {
            // for now don't unload...  we probably should clean this
            // up and only keep one open dlhandle per proc, since there
            // is really no benefit to unloading the code.
            //dlclose(dlhandle);
        }
    }

    void setSurface(ISurface* _surface)
    {
        if (_surface != NULL) {
            Int64 nativeSurf;
            _surface->GetNativeSurface(&nativeSurf);
            nativeWindow = (ANativeWindow*)reinterpret_cast<android::Surface*>(nativeSurf);
        } else {
            nativeWindow = NULL;
        }
    }

public:
    ANativeActivityCallbacks callbacks;

    void* dlhandle;
    ANativeActivity_createFunc* createActivityFunc;

    android::String8 internalDataPathObj;
    android::String8 externalDataPathObj;
    android::String8 obbPathObj;

    android::sp<ANativeWindow> nativeWindow;
    int32_t lastWindowWidth;
    int32_t lastWindowHeight;

    // These are used to wake up the main thread to process work.
    int mainWorkRead;
    int mainWorkWrite;
    android::sp<android::Looper> mLooper;
    AutoPtr<NativeActivity> mNativeActivity;
};


struct ActivityWork {
    int32_t cmd;
    int32_t arg1;
    int32_t arg2;
};

enum {
    CMD_FINISH = 1,
    CMD_SET_WINDOW_FORMAT,
    CMD_SET_WINDOW_FLAGS,
    CMD_SHOW_SOFT_INPUT,
    CMD_HIDE_SOFT_INPUT,
};

static void write_work(int fd, int32_t cmd, int32_t arg1=0, int32_t arg2=0) {
    ActivityWork work;
    work.cmd = cmd;
    work.arg1 = arg1;
    work.arg2 = arg2;

    LOG_TRACE("write_work: cmd=%d", cmd);

restart:
    int res = write(fd, &work, sizeof(work));
    if (res < 0 && errno == EINTR) {
        goto restart;
    }

    if (res == sizeof(work)) return;

    if (res < 0) ALOGW("Failed writing to work fd: %s", strerror(errno));
    else ALOGW("Truncated writing to work fd: %d", res);
}

static bool read_work(int fd, ActivityWork* outWork) {
    int res = read(fd, outWork, sizeof(ActivityWork));
    // no need to worry about EINTR, poll loop will just come back again.
    if (res == sizeof(ActivityWork)) return true;

    if (res < 0) ALOGW("Failed reading work fd: %s", strerror(errno));
    else ALOGW("Truncated reading work fd: %d", res);
    return false;
}

/*
 * Callback for handling native events on the application's main thread.
 */
static int mainWorkCallback(int fd, int events, void* data) {
    NativeCode* code = (NativeCode*)data;
    if ((events & POLLIN) == 0) {
        return 1;
    }

    ActivityWork work;
    if (!read_work(code->mainWorkRead, &work)) {
        return 1;
    }

    LOG_TRACE("mainWorkCallback: cmd=%d", work.cmd);

    switch (work.cmd) {
        case CMD_FINISH: {
            code->mNativeActivity->Finish();
        } break;
        case CMD_SET_WINDOW_FORMAT: {
            code->mNativeActivity->SetWindowFormat(work.arg1);
        } break;
        case CMD_SET_WINDOW_FLAGS: {
            code->mNativeActivity->SetWindowFlags(work.arg1, work.arg2);
        } break;
        case CMD_SHOW_SOFT_INPUT: {
            code->mNativeActivity->ShowIme(work.arg1);
        } break;
        case CMD_HIDE_SOFT_INPUT: {
            code->mNativeActivity->HideIme(work.arg1);
        } break;
        default:
            ALOGW("Unknown work command: %d", work.cmd);
            break;
    }

    return 1;
}

Int64 NativeActivity::LoadNativeCode(
    /* [in] */ const String& path,
    /* [in] */ const String& funcName,
    /* [in] */ IMessageQueue* queue,
    /* [in] */ const String& internalDataDir,
    /* [in] */ const String& obbDir,
    /* [in] */ const String& externalDataDir,
    /* [in] */ Int32 sdkVersion,
    /* [in] */ IAssetManager* assetMgr,
    /* [in] */ ArrayOf<Byte>* savedState)
{
    const char* pathStr = path.string();
    NativeCode* code = NULL;
    bool needNativeBridge = false;

    void* handle = dlopen(pathStr, RTLD_LAZY);
    if (handle == NULL) {
        if (android::NativeBridgeIsSupported(pathStr)) {
            handle = android::NativeBridgeLoadLibrary(pathStr, RTLD_LAZY);
            needNativeBridge = true;
        }
    }

    if (handle != NULL) {
        void* funcPtr = NULL;
        const char* funcStr = funcName.string();
        if (needNativeBridge) {
            funcPtr = android::NativeBridgeGetTrampoline(handle, funcStr, NULL, 0);
        } else {
            funcPtr = dlsym(handle, funcStr);
        }

        code = new NativeCode(handle, (ANativeActivity_createFunc*)funcPtr);

        if (code->createActivityFunc == NULL) {
            ALOGW("ANativeActivity_onCreate not found");
            delete code;
            return 0;
        }

        Handle64 mptr;
        queue->GetNativeMessageQueue(&mptr);
        AutoPtr<NativeMessageQueue> mq = reinterpret_cast<NativeMessageQueue*>(mptr);
        code->mLooper = mq->mLooper;
        if (code->mLooper == NULL) {
            ALOGW("Unable to retrieve native MessageQueue");
            delete code;
            return 0;
        }

        int msgpipe[2];
        if (pipe(msgpipe)) {
            ALOGW("could not create pipe: %s", strerror(errno));
            delete code;
            return 0;
        }
        code->mainWorkRead = msgpipe[0];
        code->mainWorkWrite = msgpipe[1];
        int result = fcntl(code->mainWorkRead, F_SETFL, O_NONBLOCK);
        SLOGW_IF(result != 0, "Could not make main work read pipe "
                "non-blocking: %s", strerror(errno));
        result = fcntl(code->mainWorkWrite, F_SETFL, O_NONBLOCK);
        SLOGW_IF(result != 0, "Could not make main work write pipe "
                "non-blocking: %s", strerror(errno));
        code->mLooper->addFd(
            code->mainWorkRead, 0, ALOOPER_EVENT_INPUT, mainWorkCallback, code);

        code->ANativeActivity::callbacks = &code->callbacks;
        code->mNativeActivity = this;

        code->internalDataPathObj = internalDataDir.string();
        code->internalDataPath = code->internalDataPathObj.string();

        if (externalDataDir != NULL) {
            code->externalDataPathObj = externalDataDir.string();
        }
        code->externalDataPath = code->externalDataPathObj.string();

        code->sdkVersion = sdkVersion;

        // assetMgr->GetNativeAssetManager(&mptr);
        code->assetManager = (android::AssetManager*)mptr;

        if (obbDir != NULL) {
            code->obbPathObj = obbDir.string();
        }
        code->obbPath = code->obbPathObj.string();

        jbyte* rawSavedState = NULL;
        Int32 rawSavedSize = 0;
        if (savedState != NULL) {
            rawSavedState = (jbyte*)savedState->GetPayload();
            rawSavedSize = savedState->GetLength();
        }

        code->createActivityFunc(code, rawSavedState, rawSavedSize);
    }

    return (Int64)code;
}

ECode NativeActivity::UnLoadNativeCode(
    /* [in] */ Int64 handle)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        delete code;
    }
    return NOERROR;
}

ECode NativeActivity::OnStartNative(
    /* [in] */ Int64 handle)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        if (code->callbacks.onStart != NULL) {
            code->callbacks.onStart(code);
        }
    }
    return NOERROR;
}

ECode NativeActivity::OnResumeNative(
    /* [in] */ Int64 handle)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        if (code->callbacks.onResume != NULL) {
            code->callbacks.onResume(code);
        }
    }
    return NOERROR;
}

ECode NativeActivity::OnSaveInstanceStateNative(
    /* [in] */ Int64 handle,
    /* [out, callee] */ ArrayOf<Byte>** state)
{
    VALIDATE_NOT_NULL(state)
    *state = NULL;

    AutoPtr<ArrayOf<Byte> > array;
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        if (code->callbacks.onSaveInstanceState != NULL) {
            size_t len = 0;
            Byte* state = (Byte*)code->callbacks.onSaveInstanceState(code, &len);
            if (len > 0) {
                array = ArrayOf<Byte>::Alloc(len);
                array->Copy(state, len);
            }
            if (state != NULL) {
                free(state);
            }
        }
    }

    *state = array;
    REFCOUNT_ADD(*state)
    return NOERROR;
}

ECode NativeActivity::OnPauseNative(
    /* [in] */ Int64 handle)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        if (code->callbacks.onPause != NULL) {
            code->callbacks.onPause(code);
        }
    }
    return NOERROR;
}

ECode NativeActivity::OnStopNative(
    /* [in] */ Int64 handle)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        if (code->callbacks.onStop != NULL) {
            code->callbacks.onStop(code);
        }
    }
    return NOERROR;
}

ECode NativeActivity::OnConfigurationChangedNative(
    /* [in] */ Int64 handle)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        if (code->callbacks.onConfigurationChanged != NULL) {
            code->callbacks.onConfigurationChanged(code);
        }
    }
    return NOERROR;
}

ECode NativeActivity::OnLowMemoryNative(
    /* [in] */ Int64 handle)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        if (code->callbacks.onLowMemory != NULL) {
            code->callbacks.onLowMemory(code);
        }
    }
    return NOERROR;
}

ECode NativeActivity::OnWindowFocusChangedNative(
    /* [in] */ Int64 handle,
    /* [in] */ Boolean focused)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        if (code->callbacks.onWindowFocusChanged != NULL) {
            code->callbacks.onWindowFocusChanged(code, focused ? 1 : 0);
        }
    }
    return NOERROR;
}

ECode NativeActivity::OnSurfaceCreatedNative(
    /* [in] */ Int64 handle,
    /* [in] */ ISurface* surface)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        code->setSurface(surface);
        if (code->nativeWindow != NULL && code->callbacks.onNativeWindowCreated != NULL) {
            code->callbacks.onNativeWindowCreated(code,
                    code->nativeWindow.get());
        }
    }
    return NOERROR;
}

static int32_t getWindowProp(ANativeWindow* window, int what) {
    int value;
    int res = window->query(window, what, &value);
    return res < 0 ? res : value;
}

ECode NativeActivity::OnSurfaceChangedNative(
    /* [in] */ Int64 handle,
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        android::sp<ANativeWindow> oldNativeWindow = code->nativeWindow;
        code->setSurface(surface);
        if (oldNativeWindow != code->nativeWindow) {
            if (oldNativeWindow != NULL && code->callbacks.onNativeWindowDestroyed != NULL) {
                code->callbacks.onNativeWindowDestroyed(code,
                        oldNativeWindow.get());
            }
            if (code->nativeWindow != NULL) {
                if (code->callbacks.onNativeWindowCreated != NULL) {
                    code->callbacks.onNativeWindowCreated(code,
                            code->nativeWindow.get());
                }
                code->lastWindowWidth = getWindowProp(code->nativeWindow.get(),
                        NATIVE_WINDOW_WIDTH);
                code->lastWindowHeight = getWindowProp(code->nativeWindow.get(),
                        NATIVE_WINDOW_HEIGHT);
            }
        } else {
            // Maybe it resized?
            int32_t newWidth = getWindowProp(code->nativeWindow.get(),
                    NATIVE_WINDOW_WIDTH);
            int32_t newHeight = getWindowProp(code->nativeWindow.get(),
                    NATIVE_WINDOW_HEIGHT);
            if (newWidth != code->lastWindowWidth
                    || newHeight != code->lastWindowHeight) {
                if (code->callbacks.onNativeWindowResized != NULL) {
                    code->callbacks.onNativeWindowResized(code,
                            code->nativeWindow.get());
                }
            }
        }
    }
    return NOERROR;
}

ECode NativeActivity::OnSurfaceRedrawNeededNative(
    /* [in] */ Int64 handle,
    /* [in] */ ISurface* surface)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        if (code->nativeWindow != NULL && code->callbacks.onNativeWindowRedrawNeeded != NULL) {
            code->callbacks.onNativeWindowRedrawNeeded(code, code->nativeWindow.get());
        }
    }
    return NOERROR;
}

ECode NativeActivity::OnSurfaceDestroyedNative(
    /* [in] */ Int64 handle)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        if (code->nativeWindow != NULL && code->callbacks.onNativeWindowDestroyed != NULL) {
            code->callbacks.onNativeWindowDestroyed(code,
                    code->nativeWindow.get());
        }
        code->setSurface(NULL);
    }
    return NOERROR;
}

ECode NativeActivity::OnInputQueueCreatedNative(
    /* [in] */ Int64 handle,
    /* [in] */ Int64 queuePtr)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        if (code->callbacks.onInputQueueCreated != NULL) {
            AInputQueue* queue = reinterpret_cast<AInputQueue*>(queuePtr);
            code->callbacks.onInputQueueCreated(code, queue);
        }
    }
    return NOERROR;
}

ECode NativeActivity::OnInputQueueDestroyedNative(
    /* [in] */ Int64 handle,
    /* [in] */ Int64 queuePtr)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        if (code->callbacks.onInputQueueDestroyed != NULL) {
            AInputQueue* queue = reinterpret_cast<AInputQueue*>(queuePtr);
            code->callbacks.onInputQueueDestroyed(code, queue);
        }
    }
    return NOERROR;
}

ECode NativeActivity::OnContentRectChangedNative(
    /* [in] */ Int64 handle,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    if (handle != 0) {
        NativeCode* code = (NativeCode*)handle;
        if (code->callbacks.onContentRectChanged != NULL) {
            ARect rect;
            rect.left = x;
            rect.top = y;
            rect.right = x+w;
            rect.bottom = y+h;
            code->callbacks.onContentRectChanged(code, &rect);
        }
    }
    return NOERROR;
}


} // namespace App
} // namespace Droid
} // namespace Elastos
