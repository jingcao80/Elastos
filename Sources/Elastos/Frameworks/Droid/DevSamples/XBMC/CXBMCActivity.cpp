
#include "CXBMCActivity.h"
#include "XBMCCallbacks.h"
#include <os/NativeMessageQueue.h>
#include <stdio.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <utils/Log.h>
#include <cutils/properties.h>
#include <media/AudioSystem.h>

using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::CNativeContentView;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Os::NativeMessageQueue;
using Elastos::Droid::View::EIID_ISurfaceHolderCallback2;

namespace android
{
// ------------------------------------------------------------------------

struct ActivityWork {
    int32_t cmd;
    int32_t arg1;
    int32_t arg2;
};

enum {
    CMD_DEF_KEY = 1,
    CMD_FINISH,
    CMD_SET_WINDOW_FORMAT,
    CMD_SET_WINDOW_FLAGS,
    CMD_SHOW_SOFT_INPUT,
    CMD_HIDE_SOFT_INPUT,
};

// ------------------------------------------------------------------------

} // namespace android

namespace Org {
namespace XBMC {

static Boolean gStartAnimation;
static CXBMCActivity* sActivity = NULL;
static NativeCode* sCode = NULL;
//static void* sHandle = NULL;

static bool read_work(int fd, android::ActivityWork* outWork)
{
    int res = read(fd, outWork, sizeof(android::ActivityWork));
    // no need to worry about EINTR, poll loop will just come back again.
    if (res == sizeof(android::ActivityWork)) return true;

    if (res < 0) ALOGW("Failed reading work fd: %s", strerror(errno));
    else ALOGW("Truncated reading work fd: %d", res);
    return false;
}
/*
 * Callback for handling native events on the application's main thread.
 */
static int mainWorkCallback(int fd, int events, void* data)
{
    NativeCode* code = (NativeCode*)data;
    if ((events & POLLIN) == 0) {
        return 1;
    }

    android::ActivityWork work;
    if (!read_work(code->mMainWorkRead, &work)) {
        return 1;
    }

    ALOGD("mMainWorkCallback: cmd=%d", work.cmd);

    switch (work.cmd) {
        case android::CMD_DEF_KEY: {
            android::KeyEvent* keyEvent;
            while ((keyEvent=code->mNativeInputQueue->consumeUnhandledEvent()) != NULL) {
                // jobject inputEventObj = android_view_KeyEvent_fromNative(
                //         code->env, keyEvent);
                // jboolean handled;
                // if (inputEventObj) {
                //     handled = code->env->CallBooleanMethod(code->clazz,
                //             gNativeActivityClassInfo.dispatchUnhandledKeyEvent, inputEventObj);
                //     checkAndClearExceptionFromCallback(code->env, "dispatchUnhandledKeyEvent");
                //     code->env->DeleteLocalRef(inputEventObj);
                // } else {
                //     LOGE("Failed to obtain key event for dispatchUnhandledKeyEvent.");
                //     handled = false;
                // }
                code->mNativeInputQueue->finishEvent(keyEvent, false, true);
            }
            int seq;
            while ((keyEvent=code->mNativeInputQueue->consumePreDispatchingEvent(&seq)) != NULL) {
                // jobject inputEventObj = android_view_KeyEvent_fromNative(
                //         code->env, keyEvent);
                // if (inputEventObj) {
                //     code->env->CallVoidMethod(code->clazz,
                //             gNativeActivityClassInfo.preDispatchKeyEvent, inputEventObj, seq);
                //     checkAndClearExceptionFromCallback(code->env, "preDispatchKeyEvent");
                //     code->env->DeleteLocalRef(inputEventObj);
                // } else {
                //     LOGE("Failed to obtain key event for preDispatchKeyEvent.");
                // }
                struct android_app* app = (struct android_app*)code->instance;
                if (app !=NULL && app->onInputEvent != NULL)
                    app->onInputEvent(app, (AInputEvent*)keyEvent);
                code->mNativeInputQueue->finishEvent(keyEvent, false, true);
            }
        } break;
        case android::CMD_FINISH: {
            //code->env->CallVoidMethod(code->clazz, gNativeActivityClassInfo.finish);
            //checkAndClearExceptionFromCallback(code->env, "finish");
        } break;
        case android::CMD_SET_WINDOW_FORMAT: {
            // code->env->CallVoidMethod(code->clazz,
            //         gNativeActivityClassInfo.setWindowFormat, work.arg1);
            // checkAndClearExceptionFromCallback(code->env, "setWindowFormat");
        } break;
        case android::CMD_SET_WINDOW_FLAGS: {
            // code->env->CallVoidMethod(code->clazz,
            //        gNativeActivityClassInfo.setWindowFlags, work.arg1, work.arg2);
            // checkAndClearExceptionFromCallback(code->env, "setWindowFlags");
        } break;
        case android::CMD_SHOW_SOFT_INPUT: {
            // code->env->CallVoidMethod(code->clazz,
            //         gNativeActivityClassInfo.showIme, work.arg1);
            // checkAndClearExceptionFromCallback(code->env, "showIme");
        } break;
        case android::CMD_HIDE_SOFT_INPUT: {
            // code->env->CallVoidMethod(code->clazz,
            //         gNativeActivityClassInfo.hideIme, work.arg1);
            // checkAndClearExceptionFromCallback(code->env, "hideIme");
        } break;
        default:
            ALOGW("Unknown work command: %d", work.cmd);
            break;
    }

    return 1;
}

CXBMCActivity::CXBMCActivity()
    : mDestroyed(FALSE)
{}

CXBMCActivity::~CXBMCActivity()
{
}

PInterface CXBMCActivity::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_ISurfaceHolderCallback2) {
        return this;
    }

    return Activity::Probe(riid);
}

UInt32 CXBMCActivity::AddRef()
{
    return Activity::AddRef();
}

UInt32 CXBMCActivity::Release()
{
    return Activity::Release();
}

ECode CXBMCActivity::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return Activity::GetInterfaceID(pObject, pIID);
}


ECode CXBMCActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    ALOGD("CXBMCActivity::OnCreate\n");
    // start boot animation
    //property_set("ctl.start", "bootanim");
    gStartAnimation = TRUE;

    //mIMM = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
    //mInputMethodCallback = new InputMethodCallback(this);

    GetWindow()->TakeSurface(this);
    GetWindow()->TakeInputQueue(this);
    GetWindow()->SetFormat(4/*ElPixelFormat::RGB_565*/);
    GetWindow()->SetSoftInputMode(
            IWindowManagerLayoutParams::SOFT_INPUT_STATE_UNSPECIFIED
            | IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_RESIZE);

    CNativeContentView::New(this, (IView**)&mNativeContentView);
    assert(mNativeContentView != NULL);
    //mNativeContentView.mActivity = this;
    SetContentView(mNativeContentView);

    Boolean res;
    mNativeContentView->RequestFocus(&res);
    //mNativeContentView.getViewTreeObserver().addOnGlobalLayoutListener(this);

    LoadNativeCode();

    sActivity = this;

    property_set("ctl.stop", "bootanim");

    return Activity::OnCreate(savedInstanceState);
}

ECode CXBMCActivity::LoadNativeCode()
{
    if (sCode != NULL) {
        return NOERROR;
    }

    const char* pathStr = "/data/data/com.elastos.runtime/elastos/XBMC/lib/libxbmc.so";
    void* handle = dlopen(pathStr, RTLD_LAZY);
    assert(handle != NULL);

    if (handle != NULL) {
        ANativeActivity_createFunc* createActivityFunc = NULL;
        const char* funcStr = "ANativeActivity_onCreate";

        createActivityFunc = (ANativeActivity_createFunc*)dlsym(handle, funcStr);

        assert(createActivityFunc != NULL);

        sCode = new NativeCode();

        AutoPtr<ILooperHelper> helper;
        assert(SUCCEEDED(CLooperHelper::AcquireSingleton((ILooperHelper**)&helper)));
        AutoPtr<IMessageQueue> queue;
        helper->MyQueue((IMessageQueue**)&queue);
        NativeMessageQueue* messageQueue = NULL;
        assert(SUCCEEDED(queue->GetNativeMessageQueue((Handle32*)&messageQueue))
               && (messageQueue != NULL));

        sCode->mLooper = messageQueue->GetLooper();
        if (sCode->mLooper == NULL) {
            //LOGW("Unable to retrieve MessageQueue's Looper");
            delete sCode;
            return 0;
        }

        int msgpipe[2];
        if (pipe(msgpipe)) {
            //LOGW("could not create pipe: %s", strerror(errno));
            delete sCode;
            return 0;
        }

        sCode->mMainWorkRead = msgpipe[0];
        sCode->mMainWorkWrite = msgpipe[1];
        int result = fcntl(sCode->mMainWorkRead, F_SETFL, O_NONBLOCK);
        assert(result == 0);
        // SLOGW_IF(result != 0, "Could not make main work read pipe "
        //         "non-blocking: %s", strerror(errno));
        result = fcntl(sCode->mMainWorkWrite, F_SETFL, O_NONBLOCK);
        assert(result == 0);
        // SLOGW_IF(result != 0, "Could not make main work write pipe "
        //         "non-blocking: %s", strerror(errno));
        sCode->mLooper->addFd(sCode->mMainWorkRead, 0, ALOOPER_EVENT_INPUT, mainWorkCallback, sCode);

        sCode->ANativeActivity::callbacks = &sCode->mCallbacks;
        // if (env->GetJavaVM(&sCode->vm) < 0) {
        //     LOGW("NativeActivity GetJavaVM failed");
        //     delete sCode;
        //     return 0;
        // }

        sCode->env = (JNIEnv*)CreateXBMCCallbacks();
        // sCode->clazz = env->NewGlobalRef(clazz);

        // const char* dirStr = env->GetStringUTFChars(internalDataDir, NULL);
        // sCode->internalDataPathObj = dirStr;
        // sCode->internalDataPath = sCode->internalDataPathObj.string();
        // env->ReleaseStringUTFChars(internalDataDir, dirStr);

        // dirStr = env->GetStringUTFChars(externalDataDir, NULL);
        // sCode->externalDataPathObj = dirStr;
        // sCode->externalDataPath = sCode->externalDataPathObj.string();
        // env->ReleaseStringUTFChars(externalDataDir, dirStr);

        // sCode->sdkVersion = sdkVersion;

        AutoPtr<IAssetManager> assetMgr;
        GetAssets((IAssetManager**)&assetMgr);
        assert(assetMgr != NULL);
        assetMgr->GetNativeAssetManager((Int32*)&sCode->assetManager);

        // dirStr = env->GetStringUTFChars(obbDir, NULL);
        // sCode->obbPathObj = dirStr;
        // sCode->obbPath = sCode->obbPathObj.string();
        // env->ReleaseStringUTFChars(obbDir, dirStr);

        // jbyte* rawSavedState = NULL;
        // jsize rawSavedSize = 0;
        // if (savedState != NULL) {
        //     rawSavedState = env->GetByteArrayElements(savedState, NULL);
        //     rawSavedSize = env->GetArrayLength(savedState);
        // }

        //createActivityFunc(sCode, rawSavedState, rawSavedSize);
        createActivityFunc(sCode, NULL, 0);

        // if (rawSavedState != NULL) {
        //     env->ReleaseByteArrayElements(savedState, rawSavedState, 0);
        // }
    }

    return NOERROR;
}

ECode CXBMCActivity::OnStart()
{
    if (sCode->mCallbacks.onStart != NULL) {
        sCode->mCallbacks.onStart(sCode);
    }
    return NOERROR;
}

ECode CXBMCActivity::OnResume()
{
    return NOERROR;
}

ECode CXBMCActivity::OnPause()
{
    return NOERROR;
}

ECode CXBMCActivity::OnStop()
{
    ALOGD("CXBMCActivity::OnStop\n");
    if (sCode->mCallbacks.onStop != NULL) {
        sCode->mCallbacks.onStop(sCode);
    }
    return NOERROR;
}

ECode CXBMCActivity::OnDestroy()
{
    ALOGD("CXBMCActivity::OnDestroy\n");
    if (sCode->mCallbacks.onDestroy != NULL) {
        sCode->mCallbacks.onDestroy(sCode);
    }

    if (sActivity == NULL) {
        UnLoadNativeCode();
    }

    return NOERROR;
}

ECode CXBMCActivity::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

ECode CXBMCActivity::SurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    ALOGD("CXBMCActivity::SurfaceCreated\n");
    if (!mDestroyed) {
        mCurSurfaceHolder = holder;
        AutoPtr<ISurface> surface;
        holder->GetSurface((ISurface**)&surface);
        sCode->SetSurface(surface);
        if (sCode->mNativeWindow != NULL && sCode->mCallbacks.onNativeWindowCreated != NULL) {
            sCode->mCallbacks.onNativeWindowCreated(sCode, sCode->mNativeWindow);
        }
    }

    return NOERROR;
}

static Int32 GetWindowProp(ANativeWindow* window, Int32 what)
{
    Int32 value;
    Int32 res = window->query(window, what, &value);
    return res < 0 ? res : value;
}

ECode CXBMCActivity::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    ALOGD("CXBMCActivity::SurfaceChanged\n");
    if (!mDestroyed) {
        mCurSurfaceHolder = holder;
        AutoPtr<ISurface> surface;
        holder->GetSurface((ISurface**)&surface);
        ANativeWindow* oldNativeWindow = sCode->mNativeWindow;
        sCode->SetSurface(surface);
        if (oldNativeWindow != sCode->mNativeWindow) {
            if (oldNativeWindow != NULL && sCode->mCallbacks.onNativeWindowDestroyed != NULL) {
                sCode->mCallbacks.onNativeWindowDestroyed(sCode, oldNativeWindow);
            }

            if (sCode->mNativeWindow != NULL) {
                if (sCode->mCallbacks.onNativeWindowCreated != NULL) {
                    sCode->mCallbacks.onNativeWindowCreated(
                        sCode, sCode->mNativeWindow);
                }
                sCode->mLastWindowWidth = GetWindowProp(sCode->mNativeWindow,
                    NATIVE_WINDOW_WIDTH);
                sCode->mLastWindowHeight = GetWindowProp(sCode->mNativeWindow,
                    NATIVE_WINDOW_HEIGHT);
            }
        }
        else {
            // Maybe it resized?
            Int32 newWidth = GetWindowProp(sCode->mNativeWindow,
                    NATIVE_WINDOW_WIDTH);
            Int32 newHeight = GetWindowProp(sCode->mNativeWindow,
                    NATIVE_WINDOW_HEIGHT);
            if (newWidth != sCode->mLastWindowWidth
                    || newHeight != sCode->mLastWindowHeight) {
                if (sCode->mCallbacks.onNativeWindowResized != NULL) {
                    sCode->mCallbacks.onNativeWindowResized(sCode,
                        sCode->mNativeWindow);
                }
            }
        }
    }

    return NOERROR;
}

ECode CXBMCActivity::SurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    ALOGD("CXBMCActivity::SurfaceDestroyed\n");
    mCurSurfaceHolder = NULL;
    if (!mDestroyed) {
        if (sCode->mNativeWindow != NULL && sCode->mCallbacks.onNativeWindowDestroyed != NULL) {
            sCode->mCallbacks.onNativeWindowDestroyed(
                sCode, sCode->mNativeWindow);
        }
        sCode->SetSurface(NULL);
    }
    return NOERROR;
}

ECode CXBMCActivity::SurfaceRedrawNeeded(
    /* [in] */ ISurfaceHolder* holder)
{
    if (!mDestroyed) {
        mCurSurfaceHolder = holder;
        //onSurfaceRedrawNeededNative(mNativeHandle, holder.getSurface());
    }
    return NOERROR;
}

ECode CXBMCActivity::OnWindowFocusChanged(
    /* [in] */ Boolean hasFocus)
{
    ALOGD("CXBMCActivity::OnWindowFocusChanged\n");
    Activity::OnWindowFocusChanged(hasFocus);
    if (!mDestroyed) {
        if (sCode->mCallbacks.onWindowFocusChanged != NULL) {
            sCode->mCallbacks.onWindowFocusChanged(sCode, hasFocus ? 1 : 0);
        }
    }

    if (gStartAnimation) {
        //usleep(5000000);
        //property_set("ctl.stop", "bootanim");
        gStartAnimation = FALSE;
    }

    return NOERROR;
}

ECode CXBMCActivity::OnInputQueueCreated(
    /* [in] */ IInputQueue* queue)
{
    ALOGD("CXBMCActivity::OnInputQueueCreated\n");
    if (!mDestroyed) {
        mCurInputQueue = queue;
        AutoPtr<IInputChannel> channel;
        queue->GetInputChannel((IInputChannel**)&channel);
        android::status_t err = sCode->SetInputChannel(channel);
        if (err != android::OK) {
            assert(0);
            return E_FAIL;
        }
        if (sCode->mCallbacks.onInputQueueCreated != NULL) {
            sCode->mCallbacks.onInputQueueCreated(
                sCode, sCode->mNativeInputQueue);
        }
    }
    return NOERROR;
}

ECode CXBMCActivity::OnInputQueueDestroyed(
    /* [in] */ IInputQueue* queue)
{
    ALOGD("CXBMCActivity::OnInputQueueDestroyed\n");
    mCurInputQueue = NULL;
    if (!mDestroyed) {
        if (sCode->mNativeInputQueue != NULL
            && sCode->mCallbacks.onInputQueueDestroyed != NULL) {
            sCode->mCallbacks.onInputQueueDestroyed(
                sCode, sCode->mNativeInputQueue);
        }
        sCode->SetInputChannel(NULL);
    }

    return NOERROR;
}

void CXBMCActivity::UnLoadNativeCode()
{
    mDestroyed = TRUE;
    if (mCurSurfaceHolder != NULL) {
        if (sCode->mNativeWindow != NULL && sCode->mCallbacks.onNativeWindowDestroyed != NULL) {
            sCode->mCallbacks.onNativeWindowDestroyed(
                sCode, sCode->mNativeWindow);
        }
        sCode->SetSurface(NULL);
        mCurSurfaceHolder = NULL;
    }

    if (mCurInputQueue != NULL) {
        if (sCode->mNativeInputQueue != NULL
            && sCode->mCallbacks.onInputQueueDestroyed != NULL) {
            sCode->mCallbacks.onInputQueueDestroyed(
                sCode, sCode->mNativeInputQueue);
        }
        sCode->SetInputChannel(NULL);
        mCurInputQueue = NULL;
    }

    delete sCode;
    sCode = NULL;
}

} // namespace XBMC
} // namespace Org

void start_browser()
{
    AutoPtr<IServiceManager> sm;
    AutoPtr<IIActivityManager> am;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    sm->GetService(IContext::ACTIVITY_SERVICE, (IInterface**)&am);

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetPackage(String("Snowfox"));
    intent->SetAction(String("android.intent.action.MAIN"));

    Int32 status;
    if (FAILED(am->StartActivity(NULL, intent, String(NULL),
            NULL, String(NULL), 0, 0, String(NULL), NULL, NULL, &status))) {
    }
}

void start_calculator()
{
    AutoPtr<IServiceManager> sm;
    AutoPtr<IIActivityManager> am;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    sm->GetService(String("ActivityManagerService"), (IInterface**)&am);

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);

    intent->SetPackage(String("com.android.calculator2"));
    intent->SetAction(String("android.intent.action.MAIN"));

//    Int32 status;
//    am->StartActivity(NULL, intent, String(NULL),
//            NULL, 0, NULL, String(NULL), -1, FALSE, FALSE, &status);
}

void finish_activity()
{
    Org::XBMC::sActivity->Finish();
    Org::XBMC::sActivity = NULL;
}
