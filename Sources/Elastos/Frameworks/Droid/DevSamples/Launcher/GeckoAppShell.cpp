
#include <ext/frameworkdef.h>
#include "GeckoAppShell.h"
#include "GeckoApp.h"
#include "GeckoEvent.h"
#include "GeckoRunnable.h"
#include "CVideoPlayer.h"
#include "CGeckoBatteryManager.h"
#include "CGeckoNetworkManager.h"
#include "jansson.h"
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <elastos/utility/etl/List.h>
#include <elautoptr.h>
#include <elastos/core/Math.h>
//#include <elastos/ElPixelFormat.h>

//#include <elastos/ByteBuffer.h>
#include <elastos/core/StringBuilder.h>
#include "ElastosGeckoWrapper.h"
#include "wrapper.h"
#include <os/Build.h>
#include <R.h>
//#include <os/Environment.h>
#include <elastos/System.h>
//#include <net/Uri.h>

#include <unistd.h>

using Elastos::Utility::ILocale;
using Elastos::Utility::CLocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IDate;
using Elastos::Utility::CDate;
using Elastos::IO::CFile;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::Channels::IFileChannel;
using Elastos::IO::Channels::IWritableByteChannel;
using Elastos::IO::Channels::EIID_IWritableByteChannel;
using Elastos::Droid::R;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IClipboardManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Net::IUriBuilder;
//using Elastos::Droid::Net::CUriBuilder;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::App::IApplication;
using Elastos::Droid::App::IPendingIntentHelper;
//using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::Location::CCriteria;
using Elastos::Droid::Location::ICriteria;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::View::IContextThemeWrapper;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::View::IContextThemeWrapper;
using Elastos::Droid::View::CContextThemeWrapper;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Webkit::IMimeTypeMap;
using Elastos::Droid::Webkit::IMimeTypeMapHelper;
using Elastos::Droid::Webkit::CMimeTypeMap;
using Elastos::Droid::Webkit::CMimeTypeMapHelper;
using Elastos::Droid::Widget::CAbsoluteLayoutLayoutParams;
using Elastos::Droid::Widget::IAbsoluteLayoutLayoutParams;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

#define GeckoAppShell_LOG_FILE_NAME  "GeckoAppShell"
//AutoPtr<ITimer> CreateTimer()
//{
//    AutoPtr<ITimer> timer;
//    CTimer::New((ITimer**)&timer);
//    assert(timer.Get());
//    return timer;
//}

Mutex GeckoAppShell::IMEStateUpdater::mClassLock;
AutoPtr<GeckoAppShell::IMEStateUpdater> GeckoAppShell::IMEStateUpdater::instance;

Int32 GeckoAppShell::sPidColumn = -1;
Int32 GeckoAppShell::sUserColumn = -1;
List< AutoPtr<GeckoEvent> > GeckoAppShell::gPendingEvents;
Boolean GeckoAppShell::gRestartScheduled = FALSE;
//AutoPtr<ITimer> GeckoAppShell::mIMETimer = CreateTimer();
HashMap<Int32, AutoPtr<IAlertNotification> > GeckoAppShell::mAlertNotifications;
AutoPtr<IFile> GeckoAppShell::sCacheFile;
Int32 GeckoAppShell::sFreeSpace = -1;
AutoPtr<IHandler> GeckoAppShell::sHandler;
Mutex GeckoAppShell::mGeckoAppShell;
Mutex GeckoAppShell::sSyncCloseCameraMet;
AutoPtr<ICamera> GeckoAppShell::sCamera;
String GeckoAppShell::sClipboardText;
pthread_cond_t GeckoAppShell::mQueryResult = PTHREAD_COND_INITIALIZER;
pthread_mutex_t GeckoAppShell::mQueryMutex = PTHREAD_MUTEX_INITIALIZER;

typedef void (*GeckoAppShell_loadLibs_t)(EGEnv *env, const char*, bool);
typedef void (*GeckoAppShell_putenv_t)(EGEnv *env, const char*);
typedef void (*GeckoAppShell_nativeInit_t)(EGEnv *, eobject);
typedef void (*GeckoAppShell_notifyGeckoOfEvent_t)(EGEnv *, eobject);
typedef void (*GeckoAppShell_setSurfaceView_t)(EGEnv *, eobject);
typedef void (*GeckoAppShell_onResume_t)(EGEnv *);
typedef void (*GeckoAppShell_onLowMemory_t)(EGEnv *);
typedef void (*GeckoAppShell_callObserver_t)(EGEnv *, const char*, const char*, const char*);
typedef void (*GeckoAppShell_removeObserver_t)(EGEnv *jenv, const char*);
typedef void (*GeckoAppShell_loadGeckoLibsNative_t)(EGEnv *jenv, const char*);
typedef void (*GeckoAppShell_loadSQLiteLibsNative_t)(EGEnv *jenv, const char*);
typedef void (*GeckoAppShell_loadNSSLibsNative_t)(EGEnv *jenv, const char*);
typedef void (*GeckoAppShell_onChangeNetworkLinkStatus_t)(EGEnv *, const char*);
typedef void (*GeckoAppShell_reportJavaCrash_t)(EGEnv *, const char*);
typedef void (*GeckoAppShell_nativeRun_t)(EGEnv *, void* , const char*);


GeckoAppShell_loadLibs_t g_fLoadLibs = NULL;
GeckoAppShell_putenv_t g_fPutenv = NULL;
GeckoAppShell_nativeInit_t g_fNativeInit = NULL;
GeckoAppShell_notifyGeckoOfEvent_t g_fNotifyGeckoOfEvent = NULL;
GeckoAppShell_setSurfaceView_t g_fSetSurfaceView = NULL;
GeckoAppShell_onResume_t g_fOnResume = NULL;
GeckoAppShell_onLowMemory_t g_fOnLOwMemory = NULL;
GeckoAppShell_callObserver_t g_fCallObserver = NULL;
GeckoAppShell_removeObserver_t g_fRemoveObserver = NULL;
GeckoAppShell_loadGeckoLibsNative_t g_fLoadGeckoLibsNative = NULL;
GeckoAppShell_loadSQLiteLibsNative_t g_fLoadSQLiteLibsNative = NULL;
GeckoAppShell_loadNSSLibsNative_t g_fLoadNSSLibsNative = NULL;
GeckoAppShell_onChangeNetworkLinkStatus_t g_fONChangeNetworkLInkStatus = NULL;
GeckoAppShell_reportJavaCrash_t g_fReportJavaCrash = NULL;
GeckoAppShell_nativeRun_t g_fNativeRun = NULL;
void* g_fGeckoStart = NULL;

typedef void (*GeckoAppShell_ProcessNextNativeEvent)();

GeckoAppShell_ProcessNextNativeEvent g_fProcessNextNativeEvent = NULL;


typedef void (*GeckoAppShell_NotifyBatteryChange)(EGEnv *,double, bool, double);

GeckoAppShell_NotifyBatteryChange g_fNotifyBatteryChange = NULL;


typedef void (*GeckoAppShell_NotifySmsReceived)(EGEnv *,const char*, const char*, int);

GeckoAppShell_NotifySmsReceived g_fNotifySmsReceived = NULL;


typedef int (*GeckoAppShell_SaveMessageInSentbox)(EGEnv *,const char*, const char*, int);

GeckoAppShell_SaveMessageInSentbox g_fSaveMessageInSentbox = NULL;


typedef void (*GeckoAppShell_NotifySmsSent)(EGEnv *,int, const char*, const char*, int, int, int);

GeckoAppShell_NotifySmsSent g_fNotifySmsSent = NULL;


typedef void (*GeckoAppShell_NotifySmsDelivered)(EGEnv *,int, const char*, const char*, int);

GeckoAppShell_NotifySmsDelivered g_fNotifySmsDelivered = NULL;


typedef void (*GeckoAppShell_NotifySmsSendFailed)(EGEnv *,int, int, long long);

GeckoAppShell_NotifySmsSendFailed g_fNotifySmsSendFailed = NULL;


typedef void (*GeckoAppShell_NotifyGetSms)(EGEnv *,int, const char*, const char*, const char*, int, int, int);

GeckoAppShell_NotifyGetSms g_fNotifyGetSms = NULL;


typedef void (*GeckoAppShell_NotifyGetSmsFailed)(EGEnv *,int, int, int);

GeckoAppShell_NotifyGetSmsFailed g_fNotifyGetSmsFailed = NULL;


typedef void (*GeckoAppShell_NotifySmsDeleted)(EGEnv *,bool, int, int);

GeckoAppShell_NotifySmsDeleted g_fNotifySmsDeleted = NULL;


typedef void (*GeckoAppShell_NotifySmsDeleteFailed)(EGEnv *,int, int, int);

GeckoAppShell_NotifySmsDeleteFailed g_fNotifySmsDeleteFailed = NULL;


typedef void (*GeckoAppShell_NotifyNoMessageInList)(EGEnv *, int, int);

GeckoAppShell_NotifyNoMessageInList g_fNotifyNoMessageInList = NULL;


typedef void (*GeckoAppShell_NotifyListCreated)(EGEnv *, int ,int ,const char* ,const char* ,const char* ,int ,int ,int );

GeckoAppShell_NotifyListCreated g_fNotifyListCreated = NULL;


typedef void (*GeckoAppShell_NotifyGotNextMessage)(EGEnv *, int, const char*, const char* aSender, const char* aBody, int aTimestamp, int aRequestId, int aProcessId);

GeckoAppShell_NotifyGotNextMessage g_fNotifyGotNextMessage = NULL;


typedef void (*GeckoAppShell_NotifyReadingMessageListFailed)(EGEnv *, int ,int ,int );

GeckoAppShell_NotifyReadingMessageListFailed g_fNotifyReadingMessageListFailed = NULL;



GeckoAppShell::LooperThread::LooperThread()
    : Thread()
{
}

ECode GeckoAppShell::LooperThread::Run()
{
    /*Looper::Prepare();
    //try {
    AutoPtr<IHandler> pHandler;
    ECode ec = CHandler::New((IHandler**)&pHandler);
    if (FAILED(ec)) return ec;
    mHandlerQueue.Put(pHandler);
    //} catch (InterruptedException ie) {}
    Looper::Loop();*/

    return NOERROR;
}

PInterface GeckoAppShell::LooperThread::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IThread) {
        return this;
    }

    return NULL;
}

Object* GeckoAppShell::LooperThread::GetSelfLock()
{
    return &mSync;
}

UInt32 GeckoAppShell::LooperThread::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 GeckoAppShell::LooperThread::Release()
{
    return ElRefBase::Release();
}

ECode GeckoAppShell::LooperThread::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [in] */ InterfaceID* pIID)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::CheckAccess()
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::CountStackFrames(
    /* [out] */ Int32* number)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::Destroy()
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::GetId(
    /* [out] */ Int64* id)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::GetName(
    /* [out] */ String* name)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::GetPriority(
    /* [out] */ Int32* priority)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::GetState(
    /* [out] */ ThreadState* state)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::GetThreadGroup(
    /* [out] */ IThreadGroup** group)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::Interrupt()
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::IsAlive(
    /* [out] */ Boolean* isAlive)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::IsDaemon(
    /* [out] */ Boolean* isDaemon)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::IsInterrupted(
    /* [out] */ Boolean* isInterrupted)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::Join()
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::Join(
    /* [in] */ Int64 millis)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::Join(
    /* [in] */ Int64 millis,
    /* [in] */ Int32 nanos)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::Resume()
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::SetDaemon(
    /* [in] */ Boolean isDaemon)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::SetName(
    /* [in] */ const String& threadName)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::SetPriority(
    /* [in] */ Int32 priority)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::Start()
{
    return Thread::Start();
}

ECode GeckoAppShell::LooperThread::Stop()
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::Suspend()
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::LooperThread::Wait(
    /* [in] */ Int64 time,
    /* [in] */ Int32 frac)
{
    return Thread::Wait(time, frac);
}

UInt32 GeckoAppShell::IMEStateUpdater::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 GeckoAppShell::IMEStateUpdater::Release()
{
    return ElRefBase::Release();
}

PInterface GeckoAppShell::IMEStateUpdater::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IRunnable) {
        return this;
    }
//    else if (riid == EIID_ITimerTask) {
//        return this;
//    }

    return NULL;
}

ECode GeckoAppShell::IMEStateUpdater::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IRunnable;
    }
    else {
        return E_INVALID_ARGUMENT;
    }

    return NOERROR;
}

ECode GeckoAppShell::IMEStateUpdater::Run()
{
    mClassLock.Lock();
    instance.Clear();
    mClassLock.Unlock();

    AutoPtr<IContext> context;
    FAIL_RETURN(GeckoApp::sSurfaceView->GetContext((IContext**)&context));
    AutoPtr<IInputMethodManager> imm;
    FAIL_RETURN(context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&imm));

    if (imm == NULL) {
        return E_NO_INTERFACE;
    }

    if (mReset) {
        imm->RestartInput((IView*)GeckoApp::sSurfaceView->mSurfaceView.Get());
    }

    if (!mEnable) {
        return NOERROR;
    }

    Boolean ret;
    Int32 state = GeckoApp::sSurfaceView->mIMEState;
    if (state != CGeckoSurfaceView::IME_STATE_DISABLED &&
        state != CGeckoSurfaceView::IME_STATE_PLUGIN) {
        FAIL_RETURN(imm->ShowSoftInput(
            (IView*)GeckoApp::sSurfaceView->mSurfaceView.Get(), 0, &ret));
    }
    else {
        AutoPtr<IBinder> binder;
        FAIL_RETURN((GeckoApp::sSurfaceView->GetWindowToken((IBinder**)&binder)));
        FAIL_RETURN(imm->HideSoftInputFromWindow(binder, 0, &ret));
    }

    return NOERROR;
}

ECode GeckoAppShell::IMEStateUpdater::Cancel(
    /* [out] */ Boolean* value)
{
//    return TimerTask::Cancel(value);
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::IMEStateUpdater::ScheduledExecutionTime(
    /* [out] */ Int64* value)
{
//    return TimerTask::ScheduledExecutionTime(value);
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::IMEStateUpdater::GetWhen(
        /* [out] */ Int64* when)
{
//    return TimerTask::GetWhen(when);
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::IMEStateUpdater::SetWhen(
        /* [in] */ Int64 when)
{
//    return TimerTask::SetWhen(when);
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::IMEStateUpdater::IsScheduled(
        /* [out] */ Boolean* scheduled)
{
//    return TimerTask::IsScheduled(scheduled);
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::IMEStateUpdater::IsCancelled(
        /* [out] */ Boolean* cancelled)
{
//    return TimerTask::IsCancelled(cancelled);
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::IMEStateUpdater::GetPeriod(
        /* [out] */ Int64* period)
{
//    return TimerTask::GetPeriod(period);
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::IMEStateUpdater::SetPeriod(
        /* [in] */ Int64 period)
{
//    return TimerTask::SetPeriod(period);
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::IMEStateUpdater::IsFixedRate(
        /* [out] */ Boolean* fixed)
{
//    return TimerTask::IsFixedRate(fixed);
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::IMEStateUpdater::SetFixedRate(
        /* [in] */ Boolean fixed)
{
//    return TimerTask::SetFixedRate(fixed);
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::IMEStateUpdater::SetScheduledTime(
            /* [in] */ Int64 time)
{
//    return TimerTask::SetScheduledTime(time);
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::IMEStateUpdater::Lock()
{
    mLock.Lock();
    return NOERROR;
}

ECode GeckoAppShell::IMEStateUpdater::Unlock()
{
    mLock.Unlock();
    return NOERROR;
}

Void GeckoAppShell::IMEStateUpdater::EnableIME()
{
    AutoLock lock(&mClassLock);
    GetInstance()->mEnable = TRUE;
}

Void GeckoAppShell::IMEStateUpdater::ResetIME()
{
    AutoLock lock(&mClassLock);
    GetInstance()->mReset = TRUE;
}

AutoPtr<GeckoAppShell::IMEStateUpdater> GeckoAppShell::IMEStateUpdater::GetInstance()
{
    if (instance == NULL) {
        AutoPtr<IThread> thread;
        Thread::Attach((IThread**)&thread);
        instance = new GeckoAppShell::IMEStateUpdater();
        assert(instance);
//        GeckoAppShell::mIMETimer->Schedule(instance, 500);
        thread->Detach();
    }
    return instance;
}

Object* GeckoAppShell::IMEStateUpdater::GetSelfLock()
{
    return &mLock;
}

ECode GeckoAppShell::GetMainHandler(
    /* [out] */ IHandler** ppHandler)
{
    if (!ppHandler) return E_INVALID_ARGUMENT;

    if (GeckoApp::sAppContext->mMainHandler != NULL) {
        *ppHandler = GeckoApp::sAppContext->mMainHandler.Get();
        (*ppHandler)->AddRef();
    }
    else {
        *ppHandler = NULL;
    }
    return NOERROR;
}

ECode GeckoAppShell::GetHandler(
    /* [out] */ IHandler** ppHandler)
{
    if (!ppHandler) return E_INVALID_ARGUMENT;

    if (!sHandler) {
    /*    LooperThread* pLt = new LooperThread();
        if (!pLt) return E_OUT_OF_MEMORY;
        pLt->Start();
        //try {
        //sHandler = pLt->mHandlerQueue.Take();
        //} catch (InterruptedException ie) {}
        delete pLt;*/
    }
    if (sHandler) {
        *ppHandler = sHandler.Get();
        sHandler->AddRef();
    }
    else {
        *ppHandler = NULL;
    }
    return NOERROR;
}

ECode GeckoAppShell::GetCacheDir(
    /* [out] */ IFile** ppFile)
{
    if (!ppFile) return E_INVALID_ARGUMENT;

    if (!sCacheFile){
        //ECode ec = GeckoApp::sAppContext->GetCacheDir((IFile**)&sCacheFile);
        //if (FAILED(ec)) return ec;
        return E_NOT_IMPLEMENTED;
    }
    *ppFile = sCacheFile.Get();
    sCacheFile->AddRef();
    return NOERROR;
}

ECode GeckoAppShell::GetFreeSpace(
    /* [out] */ Int32* pFreeSpace)
{
    //try {
        if (sFreeSpace == -1) {
            AutoPtr<IFile> cacheDir;
            ECode ec = GetCacheDir((IFile**)&cacheDir);
            if (FAILED(ec)) {
                printf("GeckoAppShell Unable to get cache dir\n");
                *pFreeSpace = -1;
                return NOERROR;
            }
            String path;
            ec = cacheDir->GetPath(&path);
            if (FAILED(ec)) return ec;

            assert(0);
            /*AutoPtr<IStatFs> pCacheStats;
            ec = CStatFs::New(path, (IStatFs**)&pCacheStats);
            if (FAILED(ec)) return ec;
            Int32 freeBlocks = 0, blockSize = 0;
            pCacheStats->GetFreeBlocks(&freeBlocks);
            pCacheStats->GetBlockSize(&blockSize);
            sFreeSpace = freeBlocks * blockSize;*/
        }
    //} catch (Exception e) {
    //    Log.e("GeckoAppShell", "exception while stating cache dir: ", e);
    //}
    *pFreeSpace = sFreeSpace;
    return NOERROR;
}

ECode GeckoAppShell::MoveFile(
    /* [in] */ IFile* pInFile,
    /* [in] */ IFile* pOutFile,
    /* [out] */ Boolean* pResult)
{
    if (!pResult) return E_INVALID_ARGUMENT;

    //Log.i("GeckoAppShell", "moving " + inFile + " to " + outFile);
    AutoPtr<IFile> pTempOutFile = pOutFile;
    Boolean isDirectory;
    ECode ec = pTempOutFile->IsDirectory(&isDirectory);
    if (FAILED(ec)) return ec;
    if (isDirectory) {
        String name;
        ec = pInFile->GetName(&name);
        if (FAILED(ec)) return ec;
        pTempOutFile.Clear();
        ec = CFile::New(pOutFile, name, (IFile**)&pTempOutFile);
        if (FAILED(ec)) return ec;
    }

    //try {
        Boolean succeeded = FALSE;
        ec = pInFile->RenameTo(pTempOutFile, &succeeded);
        if (FAILED(ec)) return ec;

        if (succeeded) {
            *pResult = TRUE;
            return NOERROR;
        }
    //} catch (SecurityException se) {
    //    Log.w("GeckoAppShell", "error trying to rename file", se);
    //}
    //try {
        Int64 lastModified;
        ec = pInFile->LastModified(&lastModified);
        if (FAILED(ec)) return ec;

        ec = pTempOutFile->CreateNewFile(&succeeded);
        if (FAILED(ec)) return ec;

        // so copy it instead
        AutoPtr<IFileInputStream> pIFileInputStream;
        AutoPtr<IFileOutputStream> pIFileOutputStream;
        AutoPtr<IFileChannel> pInChannel;
        AutoPtr<IFileChannel> pOutChannel;
        ec = CFileInputStream::New(pInFile, (IFileInputStream**)&pIFileInputStream);
        if (FAILED(ec)) goto exit;
//        ec = pIFileInputStream->GetChannel((IFileChannel**)&pInChannel);
//        if (FAILED(ec)) goto exit;

        ec = CFileOutputStream::New(pTempOutFile, (IFileOutputStream**)&pIFileOutputStream);
        if (FAILED(ec)) goto exit;

//        ec = pIFileOutputStream->GetChannel((IFileChannel**)&pInChannel);
//        if (FAILED(ec)) goto exit;

        Int64 size, transferred;
        ec = pInChannel->GetSize(&size);
        if (FAILED(ec)) goto exit;

        ec = pInChannel->TransferTo(0, size,
            (IWritableByteChannel*)pOutChannel->Probe(EIID_IWritableByteChannel),
             &transferred);
        if (FAILED(ec)) goto exit;

        ec = pTempOutFile->SetLastModified(lastModified, &succeeded);
        if (FAILED(ec)) goto exit;

        if (transferred == size) {
            ec = pInFile->Delete(&succeeded);
        }
        else {
            *pResult = FALSE;
            return NOERROR;
        }

    /*} catch (Exception e) {
        Log.e("GeckoAppShell", "exception while moving file: ", e);
        try {
            outFile.delete();
        } catch (SecurityException se) {
            Log.w("GeckoAppShell", "error trying to delete file", se);
        }
        return false;
    }*/
exit:
    if (FAILED(ec)) {
        pTempOutFile->Delete(&succeeded);
        *pResult = FALSE;
    }
    else {
        *pResult = TRUE;
    }
    return ec;
}

ECode GeckoAppShell::MoveDir(
    /* [in] */ IFile* pFrom,
    /* [in] */ IFile* pTo,
    /* [out] */ Boolean* pResult)
{
    if (!pResult) return E_INVALID_ARGUMENT;

    //try {
        Boolean succeeded;
        FAIL_RETURN(pTo->Mkdirs(&succeeded));
        FAIL_RETURN(pFrom->RenameTo(pTo, &succeeded));
        if (succeeded) {
            *pResult = TRUE;
            return NOERROR;
        }
    //} catch (SecurityException se) {
    //    Log.w("GeckoAppShell", "error trying to rename file", se);
    //}
    AutoPtr< ArrayOf<IFile*> > pContainer;
    FAIL_RETURN(pFrom->ListFiles((ArrayOf<IFile*>**)&pContainer));

    Boolean retVal = TRUE;
    //try {

        AutoPtr<IFile> pIFile;
        AutoPtr<IFile> pDest;
        String name;
        Boolean isDirectory;
        ECode ec = NOERROR;
        for (Int32 i = 0 ; i < pContainer->GetLength(); ++i) {
            pIFile.Clear();
            pDest.Clear();
            pIFile = (*pContainer)[i];
            pIFile->GetName(&name);
            FAIL_RETURN(CFile::New(pTo, name, (IFile**)&pDest));
            pIFile->IsDirectory(&isDirectory);
            Boolean succeeded;
            if (isDirectory) {
                ec = MoveDir(pIFile, pDest, &succeeded);
            }
            else {
                ec = MoveFile(pIFile, pDest, &succeeded);
            }
            retVal = succeeded ? retVal : FALSE;
            FAIL_RETURN(ec);
        }
        pFrom->Delete(&succeeded);
    //} catch(Exception e) {
    //    Log.e("GeckoAppShell", "error trying to move file", e);
    //}
    *pResult = retVal;
    return NOERROR;
}

ECode GeckoAppShell::LoadGeckoLibs(
    /* [in] */ const String& apkName)
{
    AutoPtr<GeckoApp> geckoApp = GeckoApp::sAppContext;
    String homeDir("/data/data/com.elastos.runtime/elastos/Launcher");
    String resPath("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.apk");
//    AutoPtr<IApplication> pIApplication;
//    FAIL_RETURN(geckoApp->GetApplication((IApplication**)&pIApplication));
//    FAIL_RETURN(pIApplication->GetPackageResourcePath(&resPath));

    Boolean exist = FALSE;
    Boolean result;
    ECode ec;
    if (Build::VERSION::SDK_INT < 8 || resPath.StartWith("/data") || resPath.StartWith("/system")) {
        AutoPtr<IFile> pHome;
        //ec = geckoApp->GetFilesDir((IFile**)&pHome);
        //if (FAILED(ec)) return ec;
        ec = pHome->GetPath(&homeDir);
        if (FAILED(ec)) return ec;

        // handle the application being moved to phone from sdcard
        AutoPtr<IFile> pProfileDir;
        AutoPtr<IFile> pOldHome;
        FAIL_RETURN(CFile::New(homeDir, String("mozilla"), (IFile**)&pProfileDir));

        String oldHomePath("/data/data/com.elastos.runtime/elastos/Launcher");
        String packageName;
        geckoApp->GetPackageName(&packageName);
        oldHomePath += packageName + "/mozilla";
        FAIL_RETURN(CFile::New(oldHomePath, (IFile**)&pOldHome));
        FAIL_RETURN(pOldHome->Exists(&exist));
        if (exist) {
            MoveDir(pOldHome, pProfileDir, &result);
        }

        if (Build::VERSION::SDK_INT >= 8) {
            AutoPtr<IFile> pExtHome;
            AutoPtr<IFile> pExtProf;
            /*ec = geckoApp->GetExternalFilesDir(NULL, (IFile**)&pExtHome);
            if (FAILED(ec)) return ec;*/
            FAIL_RETURN(CFile::New(pExtHome, String("mozilla"), (IFile**)&pExtProf));
            FAIL_RETURN(pExtProf->Exists(&exist));
            if (exist) {
                MoveDir(pExtProf, pProfileDir, &result);
            }
        }
    }
    else {
        AutoPtr<IFile> pHome;
        //ec = geckoApp->GetExternalFilesDir(NULL, (IFile**)&pHome);
        //if (FAILED(ec)) return ec;
        //ec = pHome->GetPath(&homeDir);
        //if (FAILED(ec)) return ec;

        // handle the application being moved to phone from sdcard
        AutoPtr<IFile> pProfileDir;
        AutoPtr<IFile> pOldHome;
        FAIL_RETURN(CFile::New(homeDir, String("mozilla"), (IFile**)&pProfileDir));

        String oldHomePath("/data/data/com.elastos.runtime/elastos/Launcher");
        String packageName;
        geckoApp->GetPackageName(&packageName);
        oldHomePath += packageName + "/mozilla";
        FAIL_RETURN(CFile::New(oldHomePath, (IFile**)&pOldHome));
        FAIL_RETURN(pOldHome->Exists(&exist));
        if (exist) {
            MoveDir(pOldHome, pProfileDir, &result);
        }

        AutoPtr<IFile> pIntHome;
        AutoPtr<IFile> pIntProf;
        /*ec = geckoApp->GetFilesDir((IFile**)&pIntHome);
        if (FAILED(ec))return ec;*/
        FAIL_RETURN(CFile::New(pIntHome, String("mozilla"), (IFile**)&pIntProf));
        FAIL_RETURN(pIntProf->Exists(&exist));
        if (exist) {
            MoveDir(pIntProf, pProfileDir, &result);
        }
    }
    AutoPtr<List<String> > dirs = NULL;//= GeckoApp::sAppContext->GetPluginDirectories();
    if (dirs != NULL) {
        StringBuilder pluginSearchPath;
        for (UInt32 i = 0; i < dirs->GetSize(); i++) {
            printf("GeckoPlugins dir: %s", (*dirs)[i].string());
            pluginSearchPath += (*dirs)[i];
            pluginSearchPath += ":";
        }
        GeckoAppShell::PutEnv(String("MOZ_PLUGIN_PATH=")+ pluginSearchPath.ToString());
    }

    String home("HOME=");
    home.Append(homeDir);
    String greHome("GRE_HOME=");
    String grePath;
    GeckoApp::sGREDir->GetPath(&grePath);
    greHome.Append(grePath);
    GeckoAppShell::PutEnv(home);
    GeckoAppShell::PutEnv(greHome);

    /*AutoPtr<IIntent> pIIntent = geckoApp->GetIntent();
    assert(pIIntent);
    String env;
    ec = pIIntent->GetStringExtra(String("env0"), &env);
    if (FAILED(ec)) return ec;

    printf("GeckoApp env0: %s\n", env);
    for (Int32 c = 1; env.IsNullOrEmpty(); c++) {
        GeckoAppShell::PutEnv(env);
        String temp("env");
        temp += StringUtils::ToString(c);
        ec = pIIntent->GetStringExtra(temp, &env);
        if (FAILED(ec)) return ec;

        printf("GeckoApp %s : %s\n", temp.string(), env.string());
    }

    AutoPtr<IFile> pIFile;
    ec = geckoApp->GetDir("tmp", Context_MODE_WORLD_READABLE |
                             Context_MODE_WORLD_WRITEABLE, (IFile**)&pIFile);
    if (FAILED(ec)) return ec;
    ec = pIFile->Exists(&exist);
    if (FAILED(ec)) return ec;
    if (!exist) {
        Boolean succeeded;
        pIFile->Mkdirs(&succeeded);
    }*/

    String tmpPath;
    //pIFile->GetPath(&tmpPath);
    //pIFile->Release();
    //String tmpDir("TMPDIR=");
    //tmpDir.Append(tmpPath);
    //GeckoAppShell::PutEnv(tmpDir);

    //ec = Environment::GetDownloadCacheDirectory(&pIFile);
    //if (FAILED(ec)) return ec;
    //pIFile->GetPath(&tmpPath);
    String extStorage("EXTERNAL_STORAGE=");
    extStorage.Append(tmpPath);
    GeckoAppShell::PutEnv(extStorage);

    AutoPtr<IFile> pCacheFile;
    FAIL_RETURN(GetCacheDir((IFile**)&pCacheFile));
    pCacheFile->GetPath(&tmpPath);
    String cachePath("MOZ_LINKER_CACHE=");
    cachePath.Append(tmpPath);
    GeckoAppShell::PutEnv(cachePath);

    // gingerbread introduces File.getUsableSpace(). We should use that.
    Int32 freeSpace;
    FAIL_RETURN(GetFreeSpace(&freeSpace));

    //try {
        AutoPtr<IFile> pDownloadDir;
        AutoPtr<IFile> pUpdatesDir;
        if (Build::VERSION::SDK_INT >= 8) {
//            pDownloadDir = Environment::GetExternalStoragePublicDirectory(
//                    Environment::DIRECTORY_DOWNLOADS);
            //ec = geckoApp->GetExternalFilesDir(Environment::DIRECTORY_DOWNLOADS, &pUpdatesDir);
            //if (FAILED(ec)) return ec;
        }
        else {
            String path;
//            AutoPtr<IFile> pIFile = Environment::GetExternalStorageDirectory();
//            assert(pIFile);
//            pIFile->GetPath(&path);

            FAIL_RETURN(CFile::New(path, String("download"), (IFile**)&pDownloadDir));
            pUpdatesDir = pDownloadDir;
        }
        pDownloadDir->GetPath(&tmpPath);
        String downloadDir("DOWNLOADS_DIRECTORY=");
        downloadDir.Append(tmpPath);
        GeckoAppShell::PutEnv(downloadDir);

        pUpdatesDir->GetPath(&tmpPath);
        String updateDir("UPDATES_DIRECTORY=");
        updateDir.Append(tmpPath);
        GeckoAppShell::PutEnv(updateDir);
    //}
    //catch (Exception e) {
    //    Log.i("GeckoApp", "No download directory has been found: " + e);
    //}

    PutLocaleEnv();

    String action;
    //pIIntent->GetAction(&action);
    if (action.Equals(GeckoApp_ACTION_DEBUG)) {
        // remove any previously extracted libs since we're apparently low
        AutoPtr< ArrayOf<IFile*> > pContainer;
        FAIL_RETURN(pCacheFile->ListFiles((ArrayOf<IFile*>**)&pContainer));

        AutoPtr<IFile> pLibFile;
        String name;
        Boolean succeeded;
        for (Int32 i = 0; i < pContainer->GetLength(); ++i) {
            pLibFile.Clear();
            pLibFile = (*pContainer)[i];
            FAIL_RETURN(pLibFile->GetName(&name));
            if (name.EndWith(".so")) {
                pLibFile->Delete(&succeeded);
            }
        }
    }
    LoadSQLiteLibsNative(apkName);
    LoadNSSLibsNative(apkName);
    LoadGeckoLibsNative(apkName);
    return NOERROR;
}

ECode GeckoAppShell::RunGecko(
    /* [in] */ const String& apkPath,
    /* [in] */ const String& args,
    /* [in] */ const String& url)
{
    // run gecko -- it will spawn its own thread
    GeckoAppShell::NativeInit();

    // Tell Gecko where the target surface view is for rendering
    GeckoAppShell::SetSurfaceView(GeckoApp::sSurfaceView);

    // First argument is the .apk path
    StringBuilder combinedArgs(apkPath);
    combinedArgs += " -greomni ";
    combinedArgs += apkPath;
    if (!args.IsNull()) {
        combinedArgs += " ";
        combinedArgs += args;
    }
    if (!url.IsNull()) {
        combinedArgs += " ";
        combinedArgs += url;
    }

    // and go
    GeckoAppShell::NativeRun(combinedArgs.ToString());
    return NOERROR;
}

ECode GeckoAppShell::SendEventToGecko(
    /* [in] */ GeckoEvent* event)
{
    if (GeckoApp::CheckLaunchState(GeckoApp::LaunchState_GeckoRunning)) {
        NotifyGeckoOfEvent(event);
    }
    else {
        gPendingEvents.PushBack(event);
    }
    return NOERROR;
}

ECode GeckoAppShell::SendEventToGeckoSync(
    /* [in] */ GeckoEvent* event)
{
    SendEventToGecko(event);
    GeckoEventSync();
    return NOERROR;
}

//The Gecko-side API: API methods that Gecko calls
ECode GeckoAppShell::ScheduleRedraw()
{
    // Redraw everything
    return ScheduleRedraw(0, -1, -1, -1, -1);
}

ECode GeckoAppShell::ScheduleRedraw(
    /* [in] */ Int32 nativeWindow,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    AutoPtr<GeckoEvent> event;

    if (x == -1) {
        event = new GeckoEvent(GeckoEvent::DRAW, NULL);
    }
    else {
        AutoPtr<IRect> rect;
        ASSERT_SUCCEEDED(CRect::New(x, y, w, h, (IRect**)&rect));
        event = new GeckoEvent(GeckoEvent::DRAW, rect);
    }
    assert(event != NULL);

    event->mNativeWindow = nativeWindow;

    SendEventToGecko(event);
    return NOERROR;
}

ECode GeckoAppShell::NotifyIME(
    /* [in] */ Int32 type,
    /* [in] */ Int32 state)
{
    if (!GeckoApp::sSurfaceView) {
        return NOERROR;
    }
    ECode ec = NOERROR;

    AutoPtr<IContext> context;
    ec = GeckoApp::sSurfaceView->GetContext((IContext**)&context);
    if (FAILED(ec)) return ec;
    AutoPtr<IInputMethodManager> imm;
    ec = context->GetSystemService(
        IContext::INPUT_METHOD_SERVICE, (IInterface**)&imm);
    if (FAILED(ec)) return ec;

    switch (type) {
    case NOTIFY_IME_RESETINPUTSTATE:
    {
        // Composition event is already fired from widget.
        // So reset IME flags.
        GeckoApp::sSurfaceView->mInputConnection->Reset();

        // Don't use IMEStateUpdater for reset.
        // Because IME may not work showSoftInput()
        // after calling restartInput() immediately.
        // So we have to call showSoftInput() delay.
        if (imm == NULL) {
            // no way to reset IME status directly
            IMEStateUpdater::ResetIME();
        }
        else {
            ec = imm->RestartInput((IView*)GeckoApp::sSurfaceView->mSurfaceView.Get());
        }

        // keep current enabled state
        IMEStateUpdater::EnableIME();
        break;
    }
    case NOTIFY_IME_CANCELCOMPOSITION:
        IMEStateUpdater::ResetIME();
        break;

    case NOTIFY_IME_FOCUSCHANGE:
        IMEStateUpdater::ResetIME();
        break;
    }
    return NOERROR;
}

ECode GeckoAppShell::NotifyIMEEnabled(
    /* [in] */ Int32 state,
    /* [in] */ const String& typeHint,
    /* [in] */ const String& actionHint,
    /* [in] */ Boolean landscapeFS)
{
    if (!GeckoApp::sSurfaceView) {
        return NOERROR;
    }

    /* When IME is 'disabled', IME processing is disabled.
       In addition, the IME UI is hidden */
    GeckoApp::sSurfaceView->mIMEState = (CGeckoSurfaceView::IME_STATE)state;
    GeckoApp::sSurfaceView->mIMETypeHint = typeHint;
    GeckoApp::sSurfaceView->mIMEActionHint = actionHint;
    GeckoApp::sSurfaceView->mIMELandscapeFS = landscapeFS;
    IMEStateUpdater::EnableIME();
    return NOERROR;
}

ECode GeckoAppShell::NotifyIMEChange(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 newEnd)
{
    if (!GeckoApp::sSurfaceView ||
        !GeckoApp::sSurfaceView->mInputConnection) {
        return NOERROR;
    }

    AutoPtr<IContext> pIContext;
    ECode ec = GeckoApp::sSurfaceView->GetContext((IContext**)&pIContext);
    if (FAILED(ec)) return ec;
    AutoPtr<IInputMethodManager> pImm;
    ec = pIContext->GetSystemService(
        IContext::INPUT_METHOD_SERVICE, (IInterface**)&pImm);
    if (FAILED(ec)) return ec;

    if (newEnd < 0) {
        ec = GeckoApp::sSurfaceView->mInputConnection->NotifySelectionChange(
            pImm, start, end);
    }
    else {
        ec = GeckoApp::sSurfaceView->mInputConnection->NotifyTextChange(
            pImm, text, start, end, newEnd);
    }
    return ec;
}

ECode GeckoAppShell::NotifyScreenShot(
    /* [in] */ IByteBuffer *data,
    /* [in] */ Int32 tabId,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return NOERROR;
}

// Block the current thread until the Gecko event loop is caught up
ECode GeckoAppShell::GeckoEventSync()
{
    PRINT_FILE_LINE_EX("GeckoAppShell::GeckoEventSync()");
    AutoLock lock(mGeckoAppShell);

    //ECode ec = CCountDownLatch::New(1, (ICountDownLatch**)&sGeckoPendingAcks);
    //if (FAILED(ec)) return ec;

    GeckoAppShell::SendEventToGecko(
        new GeckoEvent(GeckoEvent::GECKO_EVENT_SYNC));
    /*Int32 count;
    ec = sGeckoPendingAcks->GetCount(&count);
    if (FAILED(ec)) {
        sGeckoPendingAcks.Clear();
        return ec;
    }
    while (count != 0) {
        //try {
            sGeckoPendingAcks->Await();
        //} catch (InterruptedException e) {}
        ec = sGeckoPendingAcks->GetCount(&count);
        if (FAILED(ec)) {
            sGeckoPendingAcks.Clear();
            return ec;
        }
    }
    sGeckoPendingAcks.Clear();*/
    return NOERROR;
}

// Signal the Java thread that it's time to wake up
ECode GeckoAppShell::AcknowledgeEventSync()
{
    assert(0);
    /*ICountDownLatch* pTmp = sGeckoPendingAcks;
    if (pTmp != NULL) {
        pTmp->CountDown();
    }*/
    return NOERROR;
}

ECode GeckoAppShell::EnableDeviceMotion(
    /* [in] */ Boolean enable)
{
   /*SensorManager sm = (SensorManager)
        GeckoApp.surfaceView.getContext().getSystemService(Context.SENSOR_SERVICE);

    if (gAccelerometerSensor == NULL || gOrientationSensor == NULL) {
        gAccelerometerSensor = sm.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        gOrientationSensor   = sm.getDefaultSensor(Sensor.TYPE_ORIENTATION);
    }

    if (enable) {
        if (gAccelerometerSensor != NULL)
            sm.registerListener(GeckoApp.surfaceView, gAccelerometerSensor, SensorManager.SENSOR_DELAY_GAME);
        if (gOrientationSensor != NULL)
            sm.registerListener(GeckoApp.surfaceView, gOrientationSensor,   SensorManager.SENSOR_DELAY_GAME);
    } else {
        if (gAccelerometerSensor != NULL)
            sm.unregisterListener(GeckoApp.surfaceView, gAccelerometerSensor);
        if (gOrientationSensor != NULL)
            sm.unregisterListener(GeckoApp.surfaceView, gOrientationSensor);
    }*/
    return NOERROR;
}

class LocationRunnable : public GeckoRunnable
{
public:
    LocationRunnable(Boolean enable)
        : GeckoRunnable(NULL)
        , mEnableLocation(enable)
    {
    }

    ECode Run()
    {
        CGeckoSurfaceView* view = GeckoApp::sSurfaceView.Get();
        AutoPtr<IContext> pIContext;
        ECode ec = view->GetContext((IContext**)&pIContext);
        if (FAILED(ec)) return ec;
        AutoPtr<ILocationManager> pLm;
        ec = pIContext->GetSystemService(
                    IContext::LOCATION_SERVICE, (IInterface**)&pLm);
        if (FAILED(ec)) return ec;

        if (mEnableLocation) {
            AutoPtr<ICriteria> pCrit;
            ec = CCriteria::New((ICriteria**)&pCrit);
            if (FAILED(ec)) return ec;

            pCrit->SetAccuracy(ICriteria::Criteria_ACCURACY_FINE);
            String provider;
            ec = pLm->GetBestProvider(pCrit, TRUE, &provider);
            if (FAILED(ec)) return ec;

            if (provider.IsNull()) {
                return NOERROR;
            }

            AutoPtr<ILocation> pLoc;
            ec = pLm->GetLastKnownLocation(provider, (ILocation**)&pLoc);
            if (FAILED(ec)) return ec;
            if (pLoc) {
                assert(0);
                //view->OnLocationChanged(pLoc);
            }
            assert(0);
            //pLm->RequestLocationUpdates(provider, 100, (Float).5, view, pLooper);
        }
        else {
            assert(0);
            //pLm->RemoveUpdates(view);
        }
        return NOERROR;
    }
private:
    Boolean mEnableLocation;
};

ECode GeckoAppShell::EnableLocation(
    /* [in] */ Boolean enable)
{
    AutoPtr<IHandler> handler;
    ECode ec = GetHandler((IHandler**)&handler);
    if (FAILED(ec)) return ec;

    AutoPtr<LocationRunnable> runnable = new LocationRunnable(enable);
    if (!runnable) return E_OUT_OF_MEMORY;

    Boolean result;
    return handler->Post(runnable, &result);
}

#define SENSOR_ORIENTATION 1
#define SENSOR_ACCELERATION 2
#define SENSOR_PROXIMITY 3

ECode GeckoAppShell::EnableSensor(
    /* [in] */ Int32 aSensortype)
{
    //AutoPtr<ISensorManager> sm;
    AutoPtr<IContext> context;
    ASSERT_SUCCEEDED(GeckoApp::sSurfaceView->GetContext((IContext**)&context));
    //ASSERT_SUCCEEDED(context->GetSystemService(Context_SENSOR_SERVICE,(ISensorManager**)sm));

    switch(aSensortype) {
    case SENSOR_PROXIMITY:
        //if(sGProximitySensor == NULL)
            //ASSERT_SUCCEEDED(sm->GetDefaultSensor(Sensor.TYPE_PROXIMITY,(ISensor**)sGProximitySensor));
        //ASSERT_SUCCEEDED(sm->RegisterListener(GeckoApp::sSurfaceView, sGProximitySensor,
        //                    SensorManager_SENSOR_DELAY_GAME));
        break;
    }
    return NOERROR;
}

ECode GeckoAppShell::DisableSensor(
    /* [in] */ Int32 aSensortype)
{
    //AutoPtr<ISensorManager> sm;
    AutoPtr<IContext> context;
    ASSERT_SUCCEEDED(GeckoApp::sSurfaceView->GetContext((IContext**)&context));
    //ASSERT_SUCCEEDED(context->GetSystemService(Context_SENSOR_SERVICE,(ISensorManager**)sm));

    switch(aSensortype) {
    case SENSOR_PROXIMITY:
        //ASSERT_SUCCEEDED(sm->UnregisterListener(GeckoApp::sSurfaceView, sProximitySensor));
        break;
    }
    return NOERROR;
}

ECode GeckoAppShell::MoveTaskToBack()
{
    //Boolean result;
    //return GeckoApp::sAppContext->MoveTaskToBack(TRUE, &result);
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::ReturnIMEQueryResult(
    /* [in] */ const String& result,
    /* [in] */ Int32 selectionStart,
    /* [in] */ Int32 selectionLength)
{
    GeckoApp::sSurfaceView->mInputConnection->SetSelectionStart(selectionStart);
    GeckoApp::sSurfaceView->mInputConnection->SetSelectionLength(selectionLength);
    //try {
        GeckoApp::sSurfaceView->mInputConnection->PutResult(result);
    //} catch (InterruptedException e) {
    //}
    return NOERROR;
}

ECode GeckoAppShell::OnAppShellReady()
{
    PRINT_FILE_LINE_EX("GeckoAppShell::OnAppShellReady()");
    // mLaunchState can only be Launched at this point
    GeckoApp::SetLaunchState(GeckoApp::LaunchState_GeckoRunning);
    SendPendingEventsToGecko();
    return NOERROR;
}

ECode GeckoAppShell::OnXreExit()
{
    // mLaunchState can only be Launched or GeckoRunning at this point
    GeckoApp::SetLaunchState(GeckoApp::LaunchState_GeckoExiting);
    printf(GeckoAppShell_LOG_FILE_NAME " XRE exited\n");
    if (gRestartScheduled) {
        GeckoApp::sAppContext->DoRestart();
    } else {
        printf(GeckoAppShell_LOG_FILE_NAME " we're done, good bye\n");
        // GeckoApp::sAppContext->Finish();
    }

    // System::Exit(0);
    return NOERROR;
}

ECode GeckoAppShell::ScheduleRestart()
{
    printf(GeckoAppShell_LOG_FILE_NAME " scheduling restart\n");
    gRestartScheduled = TRUE;
    return NOERROR;
}

// "Installs" an application by creating a shortcut
ECode GeckoAppShell::CreateShortcut(
        /* [in] */ const String& aTitle,
        /* [in] */ const String& aURI,
        /* [in] */ const String& aIconData,
        /* [in] */ const String& aType)
{
    printf(GeckoAppShell_LOG_FILE_NAME " installWebApplication for %s [%s] > %s\n",
                     aURI.string(), aTitle.string(), aType.string());

    // TODO:
    //
    return NOERROR;

    // the intent to be launched by the shortcut
    AutoPtr<IIntent> pShortcutIntent;
    ECode ec = CIntent::New(String("org.mozilla.fennec.WEBAPP"), (IIntent**)&pShortcutIntent);
    if (FAILED(ec)) return ec;

    if (aType.EqualsIgnoreCase("webapp")) {
        pShortcutIntent->SetAction(String("org.mozilla.gecko.WEBAPP"));
        pShortcutIntent->PutExtra(String("args"), String("--webapp=") + aURI);
    } else {
        pShortcutIntent->SetAction(String("org.mozilla.gecko.BOOKMARK"));
        pShortcutIntent->PutExtra(String("args"),  String("--url=") + aURI);
    }

    String packageName;
    GeckoApp::sAppContext->GetPackageName(&packageName);
    packageName.Append(".App");
    ec = pShortcutIntent->SetClassName(GeckoApp::sAppContext, packageName);
    if (FAILED(ec)) return ec;

    String args("--webapp=");
    args.Append(aURI);
    ec = pShortcutIntent->PutExtra(String("args"), args);
    if (FAILED(ec)) return ec;

    AutoPtr<IIntent> pIIntent;
    ec = CIntent::New((IIntent**)&pIIntent);
    if (FAILED(ec)) return ec;

    //ec = pIIntent->PutExtra(Intent_EXTRA_SHORTCUT_INTENT, pShortcutIntent);
    if (FAILED(ec)) return ec;
    //ec = pIIntent->PutExtra(Intent_EXTRA_SHORTCUT_NAME, aTitle);
    //if (FAILED(ec)) return ec;

//    AutoPtr<IBase64> base64;
//    ec = CBase64::AcquireSingleton((IBase64**)&base64);
//    if (FAILED(ec)) return ec;
    AutoPtr< ArrayOf<Byte> > raw;
//    ec = base64->Decode(aIconData.Substring(22), Base64_DEFAULT, (ArrayOf<Byte>**)&raw);
//    if (FAILED(ec)) return ec;

    AutoPtr<IBitmapFactory> pIBitmapFactory;
    ec = CBitmapFactory::AcquireSingleton((IBitmapFactory**)&pIBitmapFactory);
    if (FAILED(ec)) return ec;

    AutoPtr<IBitmap> pIBitmap;
    ec = pIBitmapFactory->DecodeByteArray(*raw, 0, raw->GetLength(), (IBitmap**)&pIBitmap);
    if (FAILED(ec)) return ec;

    //ec = pIIntent->PutExtra(Intent_EXTRA_SHORTCUT_ICON, pIBitmap);
    //if (FAILED(ec)) return ec;
    ec = pIIntent->SetAction(String("com.android.launcher.action.INSTALL_SHORTCUT"));
    if (FAILED(ec)) return ec;

    ec = GeckoApp::sAppContext->SendBroadcast(pIIntent);
    return ec;
}

ECode GeckoAppShell::GetHandlersForMimeType(
    /* [in] */ const String& aMimeType,
    /* [in] */ const String& aAction,
    /* [out, callee] */ ArrayOf<String>** ppHandlers)
{
    if (!ppHandlers) return E_INVALID_ARGUMENT;

    AutoPtr<IIntent> pIIntent;
    ECode ec = GetIntentForActionString(aAction, (IIntent**)&pIIntent);
    if (FAILED(ec)) return ec;

    if (!aMimeType.IsNullOrEmpty()) {
        pIIntent->SetType(aMimeType);
    }
    ec = GetHandlersForIntent(pIIntent, ppHandlers);
    return ec;
}

ECode GeckoAppShell::GetHandlersForURL(
    /* [in] */ const String& aURL,
    /* [in] */ const String& aAction,
    /* [out, callee] */ ArrayOf<String>** ppHandlers)
{
    if (!ppHandlers) return E_INVALID_ARGUMENT;

    // aURL may contain the whole URL or just the protocol
    ECode ec;
    AutoPtr<IUri> pUri;

    if (aURL.IndexOf(':') >= 0) {
        assert(0);
        //ec = Uri::Parse(aURL, &pUri);
        if (FAILED(ec)) return ec;
    }
    else {
        AutoPtr<IUriBuilder> pBuilder;
//        ec = CUriBuilder::New((IUriBuilder**)&pBuilder);
//        if (FAILED(ec)) return ec;

        ec = pBuilder->Scheme(aURL);
        if (FAILED(ec)) return ec;

        ec =  pBuilder->Build((IUri**)&pUri);
        if (FAILED(ec)) return ec;
    }

    AutoPtr<IIntent> pIIntent;
    ec = GetIntentForActionString(aAction, (IIntent**)&pIIntent);
    if (FAILED(ec)) return ec;
    pIIntent->SetData(pUri);

    ec = GetHandlersForIntent(pIIntent, ppHandlers);
    return ec;
}

ECode GeckoAppShell::GetHandlersForIntent(
    /* [in] */ IIntent* pIntent,
    /* [out, callee] */ ArrayOf<String>** ppHandlers)
{
    if (!ppHandlers) return E_INVALID_ARGUMENT;

    AutoPtr<IContext> pIContext;
    ECode ec = GeckoApp::sSurfaceView->GetContext((IContext**)&pIContext);
    if (FAILED(ec)) return ec;

    AutoPtr<IPackageManager> pPm;
    ec = pIContext->GetPackageManager((IPackageManager**)&pPm);
    if (FAILED(ec)) return ec;

    /*AutoPtr<IObjectContainer> list;
    ec = pPm->QueryIntentActivities(pIntent, 0, (IObjectContainer**)&list);
    if (FAILED(ec)) return ec;

    Int32 size = 0;
    list->GetObjectCount(&size);
    Int32 numAttr = 4;
    ArrayOf<String>* ret = ArrayOf<String>::Alloc(size * numAttr);
    if (!ret) {
        return E_OUT_OF_MEMORY;
    }

    AutoPtr<IObjectEnumerator> objEnu;
    list->GetObjectEnumerator((IObjectEnumerator**)&objEnu);
    Boolean isSucceeded;
    objEnu->MoveNext(&isSucceeded);

    AutoPtr<ICharSequence> labelSeq;
    Boolean isDefault;
    String lable;
    Int32 index = 0;
    while (isSucceeded) {
        AutoPtr<IResolveInfo> resolveInfo;
        objEnu->Current((IInterface**)&resolveInfo);
        ec = resolveInfo->LoadLabel(pPm, (ICharSequence**)&labelSeq);
        if (FAILED(ec)){
            ArrayOf<String>::Free(ret);
            return ec;
        }
        labelSeq->ToString(&lable);

        (*ret)[index * numAttr] = lable;
        resolveInfo->IsDefault(&isDefault);
        if (isDefault) {
            (*ret)[index * numAttr + 1] = "default";
        }
        else {
            (*ret)[index * numAttr + 1] = "";
        }

        AutoPtr<IActivityInfo> activityInfo;
        ec = resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        if (FAILED(ec)){
            ArrayOf<String>::Free(ret);
            return ec;
        }
        AutoPtr<IApplicationInfo> applicationInfo;
        ec = activityInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        if (FAILED(ec)){
            ArrayOf<String>::Free(ret);
            return ec;
        }

        applicationInfo->GetCapsuleName(&(*ret)[index * numAttr + 2]);
        activityInfo->GetName(&(*ret)[index * numAttr + 3]);
        objEnu->MoveNext(&isSucceeded);
    }

    *ppHandlers = ret;*/
    return NOERROR;
}

ECode GeckoAppShell::GetIntentForActionString(
    /* [in] */ const String& aAction,
    /* [out] */ IIntent** ppIntent)
{
    if (!ppIntent) return E_INVALID_ARGUMENT;

    // Default to the view action if no other action as been specified.
    ECode ec;
    if (!aAction.IsNullOrEmpty()) {
        ec = CIntent::New(aAction, ppIntent);
    }
    else {
        ec = CIntent::New(String(IIntent::ACTION_VIEW), ppIntent);
    }
    return ec;
}

ECode GeckoAppShell::GetExtensionFromMimeType(
    /* [in] */ const String& aMimeType,
    /* [out] */ String* ext)
{
    if (!ext) return E_INVALID_ARGUMENT;

    AutoPtr<IMimeTypeMap> mimeType;
    ECode ec = CMimeTypeMap::New((IMimeTypeMap**)&mimeType);
    if (FAILED(ec)) return ec;
    return mimeType->GetExtensionFromMimeType(aMimeType, ext);
}

ECode GeckoAppShell::GetMimeTypeFromExtensions(
    /* [in] */ const String& aFileExt,
    /* [out] */ String* mimeType)
{
    if (!mimeType) return E_INVALID_ARGUMENT;

    AutoPtr<IMimeTypeMap> pMtm;
    ECode ec = CMimeTypeMap::New((IMimeTypeMap**)&pMtm);
    if (FAILED(ec)) return ec;
    StringTokenizer* st = new StringTokenizer(aFileExt, "., ");
    String type;
    String subType;
    while (st->HasMoreTokens()) {
        String ext = st->NextToken();
        String mt;
        pMtm->GetMimeTypeFromExtension(ext, &mt);
        if (mt.IsNull())
            continue;
        Int32 slash = mt.IndexOf('/');
        String tmpType = mt.Substring(0, slash);
        if (!tmpType.EqualsIgnoreCase(type)) {
            if (type.IsNull()) {
                type = tmpType;
            }
            else {
                type = "*";
            }
        }

        String tmpSubType = mt.Substring(slash + 1);
        if (!tmpSubType.EqualsIgnoreCase(subType)) {
            if (subType.IsNull()) {
                subType = tmpSubType;
            }
            else {
                subType = "*";
            }
        }
    }
    if (type.IsNull())
        type = "*";
    if (subType.IsNull())
        subType = "*";
    String result = type + "/" + subType;
    *mimeType = result;
    delete st;
    return NOERROR;
}

ECode GeckoAppShell::OpenUriExternal(
    /* [in] */ const String& aUriSpec,
    /* [in] */ const String& aMimeType,
    /* [in] */ const String& aPackageName,
    /* [in] */ const String& aClassName,
    /* [in] */ const String& aAction,
    /* [in] */ const String& aTitle)
{
    AutoPtr<IIntent> pIIntent;
    ECode ec = GetIntentForActionString(aAction, (IIntent**)&pIIntent);
    if (FAILED(ec)) return ec;

    if (1) {//aAction.EqualsIgnoreCase(Intent_ACTION_SEND)) {
        ec = pIIntent->PutExtra(String(IIntent::EXTRA_TEXT), aUriSpec);
        if (FAILED(ec)) return ec;
        ec = pIIntent->PutExtra(String(IIntent::EXTRA_SUBJECT), aTitle);
        if (FAILED(ec)) return ec;
        if (!aMimeType.IsNullOrEmpty()) {
            ec = pIIntent->SetType(aMimeType);
            if (FAILED(ec)) return ec;
        }
    }
    else if (!aMimeType.IsNullOrEmpty()) {
        AutoPtr<IUri> pIUri;
//        ec = Uri::Parse(aUriSpec, (IUri**)&pIUri);
//        if (FAILED(ec)) return ec;

        ec = pIIntent->SetDataAndType(pIUri, aMimeType);
        if (FAILED(ec)) return ec;
    }
    else {
        String scheme;
        AutoPtr<IUri> pIUri;
//        ec = Uri::Parse(aUriSpec, (IUri**)&pIUri);
//        if (FAILED(ec)) return ec;

        ec = pIUri->GetScheme(&scheme);
        if (FAILED(ec)) return ec;

        if (scheme.Equals("vnd.youtube")) {
            // Special case youtube to fallback to our own player
            ArrayOf<String>* handlers;
            GetHandlersForURL(aUriSpec, aAction, &handlers);
            if (handlers->GetLength() == 0) {
                CIntent::New(String(VideoPlayer_VIDEO_ACTION), (IIntent**)&pIIntent);
                String packageName;
                GeckoApp::sAppContext->GetPackageName(&packageName);
                pIIntent->SetClassName(packageName,
                                    String("org.mozilla.gecko.VideoPlayer"));
                pIIntent->SetData(pIUri);
                GeckoApp::sAppContext->StartActivity(pIIntent);
                return TRUE;
            }
        }

        if (scheme.Equals("sms")) {
            // Have a apecial handling for the SMS, as the message body
            // is not extracted from the URI automatically
            String query;
            ec = pIUri->GetEncodedQuery(&query);
            if (FAILED(ec)) return ec;
            if (!query.IsNullOrEmpty()) {
                StringTokenizer fields(query.string(), "&");
                Boolean foundBody = FALSE;
                String resultQuery("");
                while (fields.HasMoreTokens()) {
                    String field = fields.NextToken();
                    if (field.GetLength() > 5 && field.Substring(0, 5).Equals("body=")) {
                        String body;
                        //Uri::Decode(field.Substring(5), &body);
                        pIIntent->PutExtra(String("sms_body"), body);
                        foundBody = TRUE;
                    }
                    else {
                        String temp("&");
                        temp.Append(field);
                        resultQuery.Append(resultQuery.GetLength() > 0 ? temp : field);
                    }
                }
                if (foundBody) {
                    // Put the query without the body field back into the URI
                    String prefix(aUriSpec.Substring(0, aUriSpec.IndexOf('?')));
                    String temp;
                    temp.Append(prefix);
                    temp.Append("?");
                    temp.Append(resultQuery);
//                    ec = Uri::Parse(resultQuery.GetLength() > 0 ? temp : prefix, (IUri**)&pIUri);
//                    if (FAILED(ec)) return ec;
                }
            }
        }
        ec = pIIntent->SetData(pIUri);
        if (FAILED(ec)) return ec;
    }

    if (aPackageName.GetLength() > 0 && aClassName.GetLength() > 0) {
        ec = pIIntent->SetClassName(aPackageName, aClassName);
        if (FAILED(ec)) return ec;
    }

    ec = pIIntent->SetFlags(IIntent::FLAG_ACTIVITY_CLEAR_TOP);
    if (FAILED(ec)) return ec;
    //try {
        AutoPtr<IContext> pIContext;
        ec = GeckoApp::sSurfaceView->GetContext((IContext**)&pIContext);
        if (FAILED(ec)) return ec;

        ec = pIContext->StartActivity(pIIntent);
        if (FAILED(ec)) return ec;

    //} catch(ActivityNotFoundException e) {
    //    return false;
    //}
    return ec;
}

class ClipboardQueueRunnable : public GeckoRunnable
{
public:
    ClipboardQueueRunnable()
        : GeckoRunnable(NULL)
    {
    }

    ECode Run()
    {
        AutoPtr<IContext> pContext;
        ECode ec = GeckoApp::sSurfaceView->GetContext((IContext**)&pContext);
        if (FAILED(ec)) return ec;
        String text;
        if (Build::VERSION::SDK_INT >= 11) {
            AutoPtr<IClipboardManager> pCm;
            ec = pContext->GetSystemService(IContext::CLIPBOARD_SERVICE, (IInterface**)&pCm);
            if (FAILED(ec)) return ec;
            /*
            Boolean hasPrimaryClip;
            pCm->HasPrimaryClip(&hasPrimaryClip);
            if (hasPrimaryClip) {
                AutoPtr<IClipData> clip;
                pCm->GetPrimaryClip((IClipData**)&clip);
                if (clip != NULL) {
                    AutoPtr<IClipDataItem> item;
                    clip->GetItemAt(0, (IClipDataItem**)&item);
                    text = item->CoerceToText(context);
                }
            }*/
        } else {
            AutoPtr<IClipboardManager> pCm;
            ec = pContext->GetSystemService(IContext::CLIPBOARD_SERVICE, (IInterface**)&pCm);
            if (FAILED(ec)) return ec;
            Boolean hasText;
            pCm->HasText(&hasText);

            if (hasText)
            {
                AutoPtr<ICharSequence> testSeq;
                pCm->GetText((ICharSequence**)&testSeq);
                testSeq->ToString(&text);
            }
        }

        pthread_mutex_lock(&GeckoAppShell::mQueryMutex);
        GeckoAppShell::sClipboardText = text;
        pthread_mutex_unlock(&GeckoAppShell::mQueryMutex);
        pthread_cond_signal(&GeckoAppShell::mQueryResult);
        //try {
            //sClipboardQueue.Put(text != NULL ? test : "");
        //} catch (InterruptedException ie) {}
        delete this;
        return NOERROR;
    }
};

// On some devices, access to the clipboard service needs to happen
// on a thread with a looper, so dispatch this to our looper thread
// Note: the main looper won't work because it may be blocked on the
// gecko thread, which is most likely this thread
ECode GeckoAppShell::GetClipboardText(
    /* [out] */ String* text)
{
    if (!text) return E_INVALID_ARGUMENT;

    AutoPtr<IHandler> handler;
    ECode ec = GetHandler((IHandler**)&handler);
    if (FAILED(ec)) return ec;

    AutoPtr<ClipboardQueueRunnable> runnable = new ClipboardQueueRunnable();
    if (!runnable) return E_OUT_OF_MEMORY;

    pthread_mutex_lock(&mQueryMutex);
    Boolean result;
    handler->Post(runnable, &result);
    pthread_cond_wait(&mQueryResult, &mQueryMutex);

    /*try {
        String ret = sClipboardQueue.take();
        return ret == "" ? null : ret;
    } catch (InterruptedException ie) {}
    return NULL;*/
    *text = sClipboardText;
    pthread_mutex_unlock(&mQueryMutex);
    return NOERROR;
}

class ClipboardSetRunnable : public GeckoRunnable
{
public:
    ClipboardSetRunnable(
        /* [in] */ const String& text)
        : GeckoRunnable(NULL)
    {
        mClipeBoardText = text;
    }

    ECode Run()
    {
        AutoPtr<IContext> pContext;
        ECode ec = GeckoApp::sSurfaceView->GetContext((IContext**)&pContext);
        if (FAILED(ec)) return ec;

        if(Build::VERSION::SDK_INT >= 11) {
            AutoPtr<IClipboardManager> pCm;
            ec = pContext->GetSystemService(IContext::CLIPBOARD_SERVICE, (IInterface**)&pCm);
            if (FAILED(ec)) return ec;
            AutoPtr<ICharSequence> text;
            //ClipData::NewPlainText("Text", mClipeBoardText, (ICharSequence**)&text)
            //pCm->SetPrimaryClip(text);
        }
        else
        {
            AutoPtr<IClipboardManager> pCm;
            ec = pContext->GetSystemService(IContext::CLIPBOARD_SERVICE, (IInterface**)&pCm);
            if (FAILED(ec)) return ec;
            AutoPtr<ICharSequence> text;
            ec = CStringWrapper::New(mClipeBoardText, (ICharSequence**)&text);
            if (FAILED(ec)) return ec;
            pCm->SetText(text);
        }

        delete this;
        return NOERROR;
    }
private:
    String mClipeBoardText;
};

ECode GeckoAppShell::SetClipboardText(
    /* [in] */ const String& text)
{
    AutoPtr<IHandler> handler;
    ECode ec = GetHandler((IHandler**)&handler);
    if (FAILED(ec)) return ec;

    AutoPtr<ClipboardSetRunnable> runnable = new ClipboardSetRunnable(text);
    if (!runnable) return E_OUT_OF_MEMORY;

    Boolean result;
    return handler->Post(runnable, &result);
}

ECode GeckoAppShell::ShowAlertNotification(
    /* [in] */ const String& aImageUrl,
    /* [in] */ const String& aAlertTitle,
    /* [in] */ const String& aAlertText,
    /* [in] */ const String& aAlertCookie,
    /* [in] */ const String& aAlertName)
{
    printf(GeckoAppShell_LOG_FILE_NAME " GeckoAppShell.showAlertNotification - image = '"
        "%s'\n- title = '%s'\n- text = '%s'\n- cookie = '%s'\n- name = '%s'",
         aImageUrl.string(), aAlertTitle.string(), aAlertText.string(),
         aAlertCookie.string(), aAlertName.string());

    // TODO:
    //
    return NOERROR;

    Int32 icon = 0x7f020004; //R.drawable.icon; // Just use the app icon by default

    String packageName;
    AutoPtr<IUri> pImageUri;
    AutoPtr<IUri> pDataUri;
    AutoPtr<IAlertNotification> pNotification;
    ECode ec;
//    ec = Uri::Parse(aImageUrl, (IUri**)&pImageUri);
//    if (FAILED(ec)) return ec;
    String scheme;
    ec = pImageUri->GetScheme(&scheme);
    if (FAILED(ec)) return ec;

    if (scheme.Equals("drawable")) {
        String resource;
        ec = pImageUri->GetSchemeSpecificPart(&resource);
        if (FAILED(ec)) return ec;

        resource = resource.Substring(resource.LastIndexOf('/') + 1);
        /*try {
            Class drawableClass = R.drawable.class;
            Field f = drawableClass.getField(resource);
            icon = f.getInt(NULL);
        } catch (Exception e) {} // just means the resource doesn't exist*/
        pImageUri.Clear();
    }

    Int32 notificationID = aAlertName.GetHashCode();

    // Remove the old notification with the same ID, if any
    RemoveNotification(notificationID);

    Int64 timeMillis = System::GetCurrentTimeMillis();

    ec = CAlertNotification::New(GeckoApp::sAppContext, notificationID,
                     icon, aAlertTitle, aAlertText, timeMillis, (IAlertNotification**)&pNotification);
    if (FAILED(ec)) return ec;

    // The intent to launch when the user clicks the expanded notification
    AutoPtr<IIntent> pNotificationIntent;
    ec = CIntent::New(String(GeckoApp_ACTION_ALERT_CLICK), (IIntent**)&pNotificationIntent);
    if (FAILED(ec)) return ec;

    ec = GeckoApp::sAppContext->GetPackageName(&packageName);
    if (FAILED(ec)) return ec;
    packageName.Append(".NotificationHandler");
    ec = pNotificationIntent->SetClassName(
                    GeckoApp::sAppContext, packageName);
    if (FAILED(ec)) return ec;

    // Put the strings into the intent as an URI "alert:<name>#<cookie>"
//    ec = Uri::FromParts(String("alert"), aAlertName, aAlertCookie, (IUri**)&pDataUri);
//    if (FAILED(ec)) return ec;
    ec = pNotificationIntent->SetData(pDataUri);
    if (FAILED(ec)) return ec;

    AutoPtr<IPendingIntentHelper> pendingIntentHelper;
//    ec = CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntentHelper);
//    if (FAILED(ec)) return ec;
    AutoPtr<IPendingIntent> pContentIntent;
    ec = pendingIntentHelper->GetBroadcast(GeckoApp::sAppContext,
                        0, pNotificationIntent, 0, (IPendingIntent**)&pContentIntent);
    if (FAILED(ec)) return ec;

    /*ec = pNotification->SetLatestEventInfo(GeckoApp::sAppContext,
                             aAlertTitle, aAlertText, pContentIntent);
    if (FAILED(ec)) return ec;*/
    ec = pNotification->SetCustomIcon(pImageUri);
    if (FAILED(ec)) return ec;

    // The intent to execute when the status entry is deleted by the user with the "Clear All Notifications" button
    AutoPtr<IIntent> pClearNotificationIntent;
    ec = CIntent::New(String(GeckoApp_ACTION_ALERT_CLEAR), (IIntent**)&pClearNotificationIntent);
    if (FAILED(ec)) return ec;

    GeckoApp::sAppContext->GetPackageName(&packageName);
    packageName += ".NotificationHandler";
    pClearNotificationIntent->SetClassName(GeckoApp::sAppContext, packageName);
    pClearNotificationIntent->SetData(pDataUri);

    AutoPtr<IPendingIntent> pDeleteIntent;
    ec = pendingIntentHelper->GetBroadcast(GeckoApp::sAppContext,
                         0, pClearNotificationIntent, 0, (IPendingIntent**)&pDeleteIntent);
    if (FAILED(ec)) return ec;
    ec = pNotification->SetDeleteIntent(pDeleteIntent);

    mAlertNotifications[notificationID] = pNotification;

    pNotification->Show();

    printf("GeckoAppJava Created notification ID %d\n", notificationID);
    return ec;
}

ECode GeckoAppShell::AlertsProgressListener_OnProgress(
    /* [in] */ const String& aAlertName,
    /* [in] */ Int32 aProgress,
    /* [in] */ Int32 aProgressMax,
    /* [in] */ const String& aAlertText)
{
    printf(GeckoAppShell_LOG_FILE_NAME " GeckoAppShell.AlertsProgressListener_OnProgress - "
        " text = '%s'\n- name = '%s'\n- progress = '%d'\n- progressMax = '%d'",
         aAlertName.string(), aAlertText.string(), aProgress, aProgressMax);

    Int32 notificationID = aAlertName.GetHashCode();
    HashMap<Int32, AutoPtr<IAlertNotification> >::Iterator it =
                        mAlertNotifications.Find(notificationID);
    if (it == mAlertNotifications.End()) return NOERROR;
    AutoPtr<IAlertNotification> pNotification = it->mSecond;
    if (pNotification) {
        pNotification->UpdateProgress(aAlertText, aProgress, aProgressMax);
    }

    if (aProgress == aProgressMax) {
        // Hide the notification at 100%
        RemoveObserver(aAlertName);
        RemoveNotification(notificationID);
    }

    return NOERROR;
}

ECode GeckoAppShell::AlertsProgressListener_OnCancel(
    /* [in] */ const String& aAlertName)
{
    printf(GeckoAppShell_LOG_FILE_NAME " GeckoAppShell.alertsProgressListener_OnCancel('%s')\n",
         aAlertName.string());

    // TODO:
    //
    // RemoveObserver(aAlertName);

    // Int32 notificationID = aAlertName.GetHashCode();
    // RemoveNotification(notificationID);
    return NOERROR;
}

ECode GeckoAppShell::HandleNotification(
    /* [in] */ const String& aAction,
    /* [in] */ const String& aAlertName,
    /* [in] */ const String& aAlertCookie)
{
    Int32 notificationID = aAlertName.GetHashCode();

    if (aAction.Equals(GeckoApp_ACTION_ALERT_CLICK)) {
        printf(GeckoAppShell_LOG_FILE_NAME " GeckoAppShell.handleNotification: callObserver(alertclickcallback)\n");
        CallObserver(aAlertName, String("alertclickcallback"), aAlertCookie);
        HashMap<Int32, AutoPtr<IAlertNotification> >::Iterator it =
                    mAlertNotifications.Find(notificationID);
        if (it == mAlertNotifications.End()) return NOERROR;
        AutoPtr<IAlertNotification> pNotification = it->mSecond;
        if (pNotification) {
            Boolean isProgressStyle;
            pNotification->IsProgressStyle(&isProgressStyle);
            // When clicked, keep the notification, if it displays a progress
            if (isProgressStyle) return NOERROR;
        }
    }

    CallObserver(aAlertName, String("alertfinished"), aAlertCookie);

    RemoveObserver(aAlertName);

    RemoveNotification(notificationID);
    return NOERROR;
}

ECode GeckoAppShell::RemoveNotification(
    /* [in] */ Int32 notificationID)
{
    mAlertNotifications.Erase(notificationID);

    AutoPtr<INotificationManager> pNotificationManager;
    ECode ec = GeckoApp::sAppContext->GetSystemService(
                            IContext::NOTIFICATION_SERVICE,
                            (IInterface**)&pNotificationManager);
    if (FAILED(ec)) return ec;
    ec = pNotificationManager->Cancel(notificationID);
    return ec;
}

ECode GeckoAppShell::GetDpi(
    /* [out] */ Int32* dpi)
{
    VALIDATE_NOT_NULL(dpi);

    /*AutoPtr<IDisplayMetrics> metrics;
    ASSERT_SUCCEEDED(CDisplayMetrics::New((IDisplayMetrics**)&metrics));

    AutoPtr<IWindowManager> wm;
    FAIL_RETURN(GeckoApp::sAppContext->GetWindowManager((IWindowManager**)&wm));

    AutoPtr<IDisplay> display;
    FAIL_RETURN(wm->GetDefaultDisplay((IDisplay**)&display));
    FAIL_RETURN(display->GetMetrics(metrics));

    //ec = pMetrics->GetDensityDpi(pDpi);*/
    *dpi = 160;
    return NOERROR;
}

ECode GeckoAppShell::SetFullScreen(
    /* [in] */ Boolean fullscreen)
{
    GeckoApp::sFullscreen = fullscreen;

    // force a reconfiguration to hide/show the system bar
    // GeckoApp::sAppContext->SetRequestedOrientation(
    //                 IActivityInfo::SCREEN_ORIENTATION_PORTRAIT);
    // GeckoApp::sAppContext->SetRequestedOrientation(
    //                 IActivityInfo::SCREEN_ORIENTATION_LANDSCAPE);
    // GeckoApp::sAppContext->SetRequestedOrientation(
    //                 IActivityInfo::SCREEN_ORIENTATION_USER);

    return NOERROR;
}

ECode GeckoAppShell::ShowFilePicker(
    /* [in] */ const String& aFilters,
    /* [out] */ String* pPicker)
{
    if (!pPicker) return E_INVALID_ARGUMENT;

    String mimeType;
    ECode ec = GetMimeTypeFromExtensions(aFilters, &mimeType);
    if (FAILED(ec)) return ec;

    return GeckoApp::sAppContext->ShowFilePicker(mimeType, pPicker);
}

ECode GeckoAppShell::PerformHapticFeedback(
    /* [in] */ Boolean aIsLongPress)
{
    Boolean result;
    return GeckoApp::sSurfaceView->PerformHapticFeedback(aIsLongPress ?
                                  IHapticFeedbackConstants::LONG_PRESS :
                                  IHapticFeedbackConstants::VIRTUAL_KEY, &result);
}

ECode GeckoAppShell::Vibrator(
    /* [in] */ IVibrator** vibrator)
{
    AutoPtr<IContext> context;
    ASSERT_SUCCEEDED(GeckoApp::sSurfaceView->GetContext((IContext**)&context));
    ASSERT_SUCCEEDED(context->GetSystemService(IContext::VIBRATOR_SERVICE,(IInterface**) vibrator));
    return NOERROR;
}

ECode GeckoAppShell::Vibrate(
    /* [in] */ Int64 milliseconds)
{
    AutoPtr<IVibrator> vibrator;
    ASSERT_SUCCEEDED(Vibrator((IVibrator**)&vibrator));
    ASSERT_SUCCEEDED(vibrator->Vibrate(milliseconds));
    return NOERROR;
}

ECode GeckoAppShell::Vibrate(
    /* [in] */ ArrayOf<Int64>& pattern,
    /* [in] */ Int32 repeat)
{
    AutoPtr<IVibrator> vibrator;
    ASSERT_SUCCEEDED(Vibrator((IVibrator**)&vibrator));
    ASSERT_SUCCEEDED(vibrator->Vibrate(pattern, repeat));
    return NOERROR;
}

ECode GeckoAppShell::CancelVibrate()
{
    AutoPtr<IVibrator> vibrator;
    ASSERT_SUCCEEDED(Vibrator((IVibrator**)&vibrator));
    return vibrator->Cancel();
}

ECode GeckoAppShell::ShowInputMethodPicker()
{
    AutoPtr<IContext> pIContext;
    ECode ec = GeckoApp::sSurfaceView->GetContext((IContext**)&pIContext);
    if (FAILED(ec)) return ec;

    AutoPtr<IInputMethodManager> pImm;
    ec = pIContext->GetSystemService(
            IContext::INPUT_METHOD_SERVICE, (IInterface**)&pImm);
    if (FAILED(ec)) return ec;

    ec = pImm->ShowInputMethodPicker();
    return ec;
}

ECode GeckoAppShell::HideProgressDialog()
{
    CGeckoSurfaceView::sShowingSplashScreen = FALSE;
    return NOERROR;
}

class SetScreenRunnable : public GeckoRunnable
{
public:
    SetScreenRunnable(
        /* [in] */ Boolean on)
        : GeckoRunnable(NULL)
        , mScreenOn(on)
    {
    }

    ECode Run()
    {
        GeckoApp::sSurfaceView->SetKeepScreenOn(mScreenOn);
        return NOERROR;
    }
private:
    Boolean mScreenOn;
};

ECode GeckoAppShell::SetKeepScreenOn(
    /* [in] */ Boolean on)
{
    SetScreenRunnable* pRunnable = new SetScreenRunnable(on);
    if (!pRunnable) return E_OUT_OF_MEMORY;
    //return GeckoApp::sAppContext->RunOnUiThread(IRunnable::Probe(pRunnable));
    return NOERROR;
}

ECode GeckoAppShell::IsNetworkLinkUp(
    /* [out] */ Boolean* pIsLinkUp)
{
    // if (!pIsLinkUp) return E_INVALID_ARGUMENT;

    // AutoPtr<IConnectivityManager> pCm;
    // ECode ec = GeckoApp::sAppContext->GetSystemService(
    //             IContext::CONNECTIVITY_SERVICE, (IInterface**)&pCm);
    // if (FAILED(ec)) return ec;

    // AutoPtr<INetworkInfo> pINetworkInfo;
    // ec = pCm->GetActiveNetworkInfo((INetworkInfo**)&pINetworkInfo);
    // if (FAILED(ec)) return ec;

    // if (!pINetworkInfo) {
    //     *pIsLinkUp = FALSE;
    //     return NOERROR;
    // }

    // Boolean isConnected;
    // ec = pINetworkInfo->IsConnected(&isConnected);
    // if (FAILED(ec)) return ec;

    // if (!isConnected) {
    //     *pIsLinkUp = FALSE;
    // }
    // else {
    //     *pIsLinkUp = TRUE;
    // }
    // return NOERROR;
    *pIsLinkUp = TRUE;
    return NOERROR;
}

ECode GeckoAppShell::IsNetworkLinkKnown(
    /* [out] */ Boolean* pIsLinkKnown)
{
    // if (!pIsLinkKnown) return E_INVALID_ARGUMENT;

    // AutoPtr<IConnectivityManager> pCm;
    // ECode ec = GeckoApp::sAppContext->GetSystemService(
    //             IContext::CONNECTIVITY_SERVICE, (IInterface**)&pCm);
    // if (FAILED(ec)) return ec;

    // AutoPtr<INetworkInfo> pINetworkInfo;
    // ec = pCm->GetActiveNetworkInfo((INetworkInfo**)&pINetworkInfo);
    // if (FAILED(ec)) return ec;

    // if (!pINetworkInfo) {
    //     *pIsLinkKnown = FALSE;
    // }
    // else {
    //     *pIsLinkKnown = TRUE;
    // }
    // return NOERROR;
    *pIsLinkKnown = TRUE;
    return NOERROR;
}

ECode GeckoAppShell::SetSelectedLocale(
    /* [in] */ const String& localeCode)
{
//    SharedPreferences settings =
//        GeckoApp.mAppContext.getPreferences(Activity.MODE_PRIVATE);
//    settings.edit().putString(GeckoApp.mAppContext.getPackageName() + ".locale",
//                              localeCode).commit();

    AutoPtr<ILocale> pLocale;
    Int32 index;
    if ((index = localeCode.IndexOf('-')) != -1 ||
            (index = localeCode.IndexOf('_')) != -1) {
        String langCode = localeCode.Substring(0, index);
        String countryCode = localeCode.Substring(index + 1);
        FAIL_RETURN(CLocale::New(langCode, countryCode, (ILocale**)&pLocale));
    } else {
        FAIL_RETURN(CLocale::New(localeCode, (ILocale**)&pLocale));
    }

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    helper->SetDefault(pLocale);

    AutoPtr<IContext> pIContext;
    AutoPtr<IResources> pRes;
    AutoPtr<IConfiguration> pConfig;

    FAIL_RETURN(GeckoApp::sAppContext->GetBaseContext((IContext**)&pIContext));
    FAIL_RETURN(pIContext->GetResources((IResources**)&pRes));
    FAIL_RETURN(pRes->GetConfiguration((IConfiguration**)&pConfig));
    FAIL_RETURN(pConfig->SetLocale(pLocale));

    AutoPtr<IDisplayMetrics> pIDM;
    FAIL_RETURN(pRes->GetDisplayMetrics((IDisplayMetrics**)&pIDM));
    FAIL_RETURN(pRes->UpdateConfiguration(pConfig, pIDM));
    return NOERROR;
}

ECode GeckoAppShell::GetSystemColors(
    /* [out] */ ArrayOf<Int32>** colors)
{
    if (!colors) return E_INVALID_ARGUMENT;

    // attrsAppearance[] must correspond to AndroidSystemColors structure in android/AndroidBridge.h
    AutoPtr<ArrayOf<Int32> > attrsAppearance = ArrayOf<Int32>::Alloc(12);
    (*attrsAppearance)[0] = R::attr::textColor;
    (*attrsAppearance)[1] = R::attr::textColorPrimary;
    (*attrsAppearance)[2] = R::attr::textColorPrimaryInverse;
    (*attrsAppearance)[3] = R::attr::textColorSecondary;
    (*attrsAppearance)[4] = R::attr::textColorSecondaryInverse;
    (*attrsAppearance)[5] = R::attr::textColorTertiary;
    (*attrsAppearance)[6] = R::attr::textColorTertiaryInverse;
    (*attrsAppearance)[7] = R::attr::textColorHighlight;
    (*attrsAppearance)[8] = R::attr::colorForeground;
    (*attrsAppearance)[9] = R::attr::colorBackground;
    (*attrsAppearance)[10] = R::attr::panelColorForeground;
    (*attrsAppearance)[11] = R::attr::panelColorBackground;

    AutoPtr<IContextThemeWrapper> contextThemeWrapper;
    ASSERT_SUCCEEDED(CContextThemeWrapper::New(GeckoApp::sAppContext,
            R::style::TextAppearance, (IContextThemeWrapper**)&contextThemeWrapper));

    AutoPtr<IResourcesTheme> theme;
    ASSERT_SUCCEEDED(contextThemeWrapper->GetTheme((IResourcesTheme**)&theme));

    AutoPtr<ITypedArray> appearance;
    ASSERT_SUCCEEDED(theme->ObtainStyledAttributes(attrsAppearance, (ITypedArray**)&appearance));

    AutoPtr< ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(attrsAppearance->GetLength());
    if (appearance != NULL) {
        Int32 count;
        appearance->GetIndexCount(&count);
        for (Int32 i = 0; i < count; i++) {
            Int32 idx, color;
            appearance->GetIndex(i, &idx);
            appearance->GetColor(idx, 0, &color);
            assert(idx >= 0 && idx <= 11);
            (*result)[idx] = color;
        }
        appearance->Recycle();
    }

    *colors = result;
    REFCOUNT_ADD(*colors);
    return NOERROR;
}

ECode GeckoAppShell::PutChildInBackground()
{
    StringBuilder fileName;
    fileName += "/proc/";
    //fileName += Process::MyPid();
    fileName += "/cgroup";
    AutoPtr<IFile> cgroupFile;
    //AutoPtr<IFileReader> reader;
    AutoPtr<IBufferedReader> br;
    ECode ec = CFile::New(fileName.ToString(), (IFile**)&cgroupFile);
    if (FAILED(ec)) return ec;

    //CFileReader::New(cgroupFile, (IFileReader**)&reader);
    //CBufferedReader::New(reader, (IBufferedReader**)&br);
    String line;
    br->ReadLine(&line);
    StringTokenizer cpuLine(line.string(), "/");
    ICloseable::Probe(br)->Close();
    cpuLine.NextToken();
    String backgroundGroup(cpuLine.GetCount() == 2 ? cpuLine.NextToken() : String(""));

    class PutChildInBackgroundGeckoProcessesVisitor:public GeckoAppShell::GeckoProcessesVisitor
    {

    private:
        String mBackgroundGroup;
    public:
        PutChildInBackgroundGeckoProcessesVisitor(String backgroundGroup){
            mBackgroundGroup = backgroundGroup;
        }

        virtual Boolean Callback(Int32 pid) {
            if (1){//pid != Process::MyPid()) {
                AutoPtr<IFile> outFile;
                AutoPtr<IFileOutputStream> fos;
                ECode ec;
                ec = CFile::New(String("/dev/cpuctl/") + mBackgroundGroup +"/tasks", (IFile**)&outFile);
                if (FAILED(ec)) {goto Exception;}
                ec = CFileOutputStream::New(outFile, (IFileOutputStream**)&fos);
                if (FAILED(ec)) {goto Exception;}
                //ec = fos->Write(new Integer(pid).toString().getBytes());
                if (FAILED(ec)) {goto Exception;}
                ec = fos->Close();
                if (FAILED(ec)) {goto Exception;}
                return TRUE;
Exception:
                printf("error putting child in the background");

            }
            return TRUE;
        }

    };

    PutChildInBackgroundGeckoProcessesVisitor* visitor = new PutChildInBackgroundGeckoProcessesVisitor(backgroundGroup);
    EnumerateGeckoProcesses(visitor);
    delete visitor;
    return NOERROR;
}

ECode GeckoAppShell::PutChildInForeground()
{
    class PutChildInForegroundGeckoProcessesVisitor:public GeckoAppShell::GeckoProcessesVisitor
    {
    public:
        virtual Boolean Callback(
            /* [in] */ Int32 pid)
        {
            if (1) {//pid != Process::MyPid()) {
                AutoPtr<IFile> outFile;
                AutoPtr<IFileOutputStream> fos;
                ECode ec = CFile::New(
                    String("/dev/cpuctl/tasks"), (IFile**)&outFile);
                if (FAILED(ec)) {goto Exception;}
                ec = CFileOutputStream::New(outFile, (IFileOutputStream**)&fos);
                if (FAILED(ec)) {goto Exception;}
                //ec = fos->Write(new Integer(pid).toString().getBytes());
                if (FAILED(ec)) {goto Exception;}
                ec = fos->Close();
                if (FAILED(ec)) {goto Exception;}
                return TRUE;
Exception:
                printf("error putting child in the foreground");
            }
            return TRUE;
        }
    };
    PutChildInForegroundGeckoProcessesVisitor* visitor = new PutChildInForegroundGeckoProcessesVisitor();
    EnumerateGeckoProcesses(visitor);
    delete visitor;
    return NOERROR;
}

class GeckoProcessVisitorChild : public GeckoAppShell::GeckoProcessesVisitor
{
public:
    Boolean Callback(
        /* [in] */ Int32 pid) {
        //if (pid != Process::MyPid()) {
        //   Process::KillProcess(pid);
        //}
        return TRUE;
    }
};

ECode GeckoAppShell::KillAnyZombies()
{
    GeckoProcessVisitorChild* pVisitor = new GeckoProcessVisitorChild();
    if (!pVisitor) return E_OUT_OF_MEMORY;

    ECode ec = EnumerateGeckoProcesses(pVisitor);
    delete pVisitor;
    return ec;
}

class GeckoPidCallback : public GeckoAppShell::GeckoProcessesVisitor
{
public:
    GeckoPidCallback()
        : otherPidExist(FALSE)
    {
    }

    Boolean Callback(
        /* [in] */ Int32 pid) {
        /*if (pid != Process::MyPid()) {
           otherPidExist = TRUE;
           return FALSE;
        }*/
        return TRUE;
    }

    Boolean otherPidExist;
};

ECode GeckoAppShell::CheckForGeckoProcs(
    /* [out] */ Boolean* pResult)
{
    if (!pResult) return E_INVALID_ARGUMENT;

    GeckoPidCallback* pVisitor = new GeckoPidCallback();
    if (!pVisitor) return E_OUT_OF_MEMORY;

    ECode ec = EnumerateGeckoProcesses(pVisitor);
    if (FAILED(ec)) {
        delete pVisitor;
        return ec;
    }

    *pResult = pVisitor->otherPidExist;
    delete pVisitor;
    return NOERROR;
}

ECode GeckoAppShell::WaitForAnotherGeckoProc()
{
    Int32 countDown = 40;
    Boolean result;
    CheckForGeckoProcs(&result);
    while (!result &&  --countDown > 0) {
        //try {
        //pThread = Thread::GetCurrentThread();
        Thread::Sleep(100);
        //} catch (InterruptedException ie) {}
        CheckForGeckoProcs(&result);
    }
    return NOERROR;
}

ECode GeckoAppShell::ScanMedia(
    /* [in] */ const String& aFile,
    /* [in] */ const String& aMimeType)
{
    /*Context context = GeckoApp.surfaceView.getContext();
    GeckoMediaScannerClient client = new GeckoMediaScannerClient(context, aFile, aMimeType);*/
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::GetIconForExtension(
    /* [in] */ String& aExt,
    /* [in] */ Int32 iconSize,
    /* [out] */ ArrayOf<Byte>** ppIcons)
{

    //try {
        if (!ppIcons) return E_INVALID_ARGUMENT;

        if (iconSize <= 0) {
            iconSize = 16;
        }

        if (aExt.GetLength() > 1 && aExt.GetChar(0) == '.') {
            aExt = aExt.Substring(1);
        }

        AutoPtr<IContext> pIContext;
        ECode ec = GeckoApp::sSurfaceView->GetContext((IContext**)&pIContext);
        if (FAILED(ec)) return ec;

        /*AutoPtr<IPackageManager> pPm;
        ec = pIContext->GetCapsuleManager((IPackageManager**)&pPm);
        if (FAILED(ec)) return ec;

        AutoPtr<IDrawable> pIcon;
        ec = GetDrawableForExtension(pPm, aExt, (IDrawable**)&pIcon);
        if (FAILED(ec)) return ec;

        if (!pIcon) {
            // Use a generic icon
            ec = pPm->GetDefaultActivityIcon(&pIcon);
            if (FAILED(ec)) return ec;
        }

        IBitmapDrawable* pIBDrawable = IBitmapDrawable::Probe(pIcon);
        if (!pIBDrawable) return E_NO_INTERFACE;

        AutoPtr<IBitmap> pIBitmap;
        ec = pIBDrawable->GetBitmap((IBitmap**)&pIBitmap);
        if (FAILED(ec)) return ec;

        Int32 width = 0, height = 0;
        pIBitmap->GetWidth(&width);
        pIBitmap->GetHeight(&height);
        if (width != iconSize || height != iconSize) {
            AutoPtr<IBitmapFactory> pBFactory;
            ec = CBitmapFactory::AcquireSingleton((IBitmapFactory**)&pBFactory);
            if (FAILED(ec)) return ec;

            AutoPtr<IBitmap> pTemp;
            //ec = CBitmap::CreateScaledBitmap(
            //        pBFactory, pIBitmap, iconSize, iconSize, TRUE, (IBitmap**)&pTemp);
            if (FAILED(ec)) return ec;
            pIBitmap = pTemp;
        }

        AutoPtr<IByteBufferHelper> pByteBufferHelper;
        ec = CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&pByteBufferHelper);
        if (FAILED(ec)) return ec;

        AutoPtr<IByteBuffer> pBuf;
        ec = pByteBufferHelper->Allocate(iconSize * iconSize * 4, (IByteBuffer**)&pBuf);
        if (FAILED(ec)) return ec;
        ec = pIBitmap->CopyPixelsToBuffer(pBuf);
        if (FAILED(ec)) return ec;

        ec = pBuf->Array(ppIcons);
        return ec;*/
    //}
    //catch (Exception e) {
    //    Log.i("GeckoAppShell", "getIconForExtension error: ",  e);
    //    return NULL;
    //}
    return NOERROR;
}

// extern "C" {
//     void Java_org_mozilla_gecko_GeckoAppShell_notifyGeckoOfEvent(void*, void*, void*);
// }

// Tell the Gecko event loop that an event is available.
Void GeckoAppShell::NotifyGeckoOfEvent(
    /* [in] */ GeckoEvent* event)
{
    g_fNotifyGeckoOfEvent(gEGEnv, (eobject)event);
}

/* The Android-side API: API methods that Android calls */

// Initialization methods
Void GeckoAppShell::NativeInit()
{
    InitEGEnv();

    // The package data lib directory isn't placed in ld.so's
    // search path, so we have to manually load libraries that
    // libxul will depend on.  Not ideal.
    void* handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libmozglue.so", RTLD_LAZY);
    //g_fPutenv = (GeckoAppShell_putenv_t)dlsym(handle, "GeckoAppShell_putenv");
    g_fNativeRun = (GeckoAppShell_nativeRun_t)dlsym(handle, "GeckoAppShell_nativeRun");
    g_fLoadSQLiteLibsNative = (GeckoAppShell_loadSQLiteLibsNative_t)dlsym(handle, "GeckoAppShell_loadSQLiteLibsNative");
    g_fLoadGeckoLibsNative = (GeckoAppShell_loadGeckoLibsNative_t)dlsym(handle, "GeckoAppShell_loadGeckoLibsNative");
    g_fLoadNSSLibsNative = (GeckoAppShell_loadNSSLibsNative_t)dlsym(handle, "GeckoAppShell_loadNSSLibsNative");

    String apkName("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.apk");

    handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libmozalloc.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("failed: %s\n", dlerror());
    }

    handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libmozsqlite3.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("failed: %s\n", dlerror());
    }

    handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libnspr4.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("failed: %s\n", dlerror());
    }
    handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libplc4.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("failed: %s\n", dlerror());
    }
    handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libplds4.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("failed: %s\n", dlerror());
    }
    handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libnssutil3.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("failed: %s\n", dlerror());
    }
    handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libnss3.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("failed: %s\n", dlerror());
    }
    handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libsmime3.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("failed: %s\n", dlerror());
    }
    handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libssl3.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("failed: %s\n", dlerror());
    }
    handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libxul.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("failed: %s\n", dlerror());
    }
    assert(handle != NULL);

    g_fGeckoStart = (void*)dlsym(handle, "GeckoStart");
    g_fNativeInit = (GeckoAppShell_nativeInit_t)dlsym(handle, "GeckoAppShell_nativeInit");

    g_fNotifyGeckoOfEvent = (GeckoAppShell_notifyGeckoOfEvent_t)dlsym(
                                handle, "GeckoAppShell_notifyGeckoOfEvent");
    g_fSetSurfaceView = (GeckoAppShell_setSurfaceView_t)dlsym(handle, "GeckoAppShell_setSurfaceView");
    g_fOnResume = (GeckoAppShell_onResume_t)dlsym(handle, "GeckoAppShell_onResume");
    g_fOnLOwMemory = (GeckoAppShell_onLowMemory_t)dlsym(handle, "GeckoAppShell_onLowMemory");
    g_fCallObserver = (GeckoAppShell_callObserver_t)dlsym(handle, "GeckoAppShell_callObserver");
    g_fRemoveObserver = (GeckoAppShell_removeObserver_t)dlsym(handle, "GeckoAppShell_removeObserver");
    g_fONChangeNetworkLInkStatus = (GeckoAppShell_onChangeNetworkLinkStatus_t)dlsym(
                        handle, "GeckoAppShell_onChangeNetworkLinkStatus");
    g_fReportJavaCrash = (GeckoAppShell_reportJavaCrash_t)dlsym(handle, "GeckoAppShell_reportJavaCrash");


    handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libxpcom.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("failed: %s\n", dlerror());
    }
    handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libnssckbi.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("failed: %s\n", dlerror());
    }
    handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libfreebl3.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("failed: %s\n", dlerror());
    }
    handle = dlopen("/data/data/com.elastos.runtime/elastos/Launcher/libsoftokn3.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("failed: %s\n", dlerror());
    }

    g_fNativeInit(gEGEnv, NULL);
    String home("HOME=/data/data/com.elastos.runtime/elastos/Launcher");
    // home.Append(homeDir);
    String greHome("GRE_HOME=/data/data/com.elastos.runtime/elastos/Launcher");
    // String grePath;
    // GeckoApp::sGREDir->GetPath(&grePath);
    // greHome.Append(grePath);
    GeckoAppShell::PutEnv(home);
    GeckoAppShell::PutEnv(greHome);
    String tmpPath;
    //pIFile->GetPath(&tmpPath);
    //pIFile->Release();
    //String tmpDir("TMPDIR=");
    //tmpDir.Append(tmpPath);
    //GeckoAppShell::PutEnv(tmpDir);

    //ec = Environment::GetDownloadCacheDirectory(&pIFile);
    //if (FAILED(ec)) return ec;
    //pIFile->GetPath(&tmpPath);
    //pIFile->Release();
    String extStorage("EXTERNAL_STORAGE=/data/data/com.elastos.runtime/elastos/Launcher/tmp");
    // extStorage.Append(tmpPath);
    GeckoAppShell::PutEnv(extStorage);


    String cachePath("CACHE_PATH=/data/data/com.elastos.runtime/elastos/Launcher/cache");
    // cachePath.Append(tmpPath);
    GeckoAppShell::PutEnv(cachePath);

    String downloadDir("DOWNLOADS_DIRECTORY=/data/data/com.elastos.runtime/elastos/Launcher/download");
    // downloadDir.Append(tmpPath);
    GeckoAppShell::PutEnv(downloadDir);

    String updateDir("UPDATES_DIRECTORY=/data/data/com.elastos.runtime/elastos/Launcher/update");
    // updateDir.Append(tmpPath);
    GeckoAppShell::PutEnv(updateDir);

    PutLocaleEnv();
}

Void GeckoAppShell::NativeRun(
    /* [in] */ const String& args)
{
    printf(" > Run args [%s], tid: %d\n", args.string(), gettid());
    g_fNativeRun(gEGEnv, g_fGeckoStart, (char*)args.string());
}

// helper methods
Void GeckoAppShell::SetSurfaceView(
    /* [in] */ CGeckoSurfaceView* surfaceView)
{
    g_fSetSurfaceView(gEGEnv, (void*)surfaceView);
}

Void GeckoAppShell::PutEnv(
    /* [in] */ const String& map)
{
    //g_fPutenv(gEGEnv, map.string());
    putenv(map.string());
}

Void GeckoAppShell::OnResume()
{
    return g_fOnResume(gEGEnv);
}

Void GeckoAppShell::OnLowMemory()
{
    return g_fOnLOwMemory(gEGEnv);
}

Void GeckoAppShell::CallObserver(
    /* [in] */ const String& observerKey,
    /* [in] */ const String& topic,
    /* [in] */ const String& data)
{
    g_fCallObserver(gEGEnv, observerKey.string(), topic.string(), data.string());
}

Void GeckoAppShell::RemoveObserver(
    /* [in] */ const String& observerKey)
{
    g_fRemoveObserver(gEGEnv, observerKey.string());
}

Void GeckoAppShell::LoadGeckoLibsNative(
    /* [in] */ const String& apkName)
{
    g_fLoadGeckoLibsNative(gEGEnv, apkName.string());
}

Void GeckoAppShell::LoadSQLiteLibsNative(
    /* [in] */ const String& apkName)
{
    g_fLoadSQLiteLibsNative(gEGEnv, apkName.string());
}

Void GeckoAppShell::LoadNSSLibsNative(
    /* [in] */ const String& apkName)
{
    g_fLoadNSSLibsNative(gEGEnv, apkName.string());
}

Void GeckoAppShell::OnChangeNetworkLinkStatus(
    /* [in] */ const String& status)
{
    g_fONChangeNetworkLInkStatus(gEGEnv, status.string());
}

Void GeckoAppShell::ReportJavaCrash(
    /* [in] */ const String& stack)
{
    g_fReportJavaCrash(gEGEnv, stack.string());
}

Void GeckoAppShell::ProcessNextNativeEvent()
{
    g_fProcessNextNativeEvent();
}

Void GeckoAppShell::NotifyBatteryChange(
    /* [in] */ Double aLevel,
    /* [in] */ Boolean aCharging,
    /* [in] */ Double aRemainingTime)
{
    g_fNotifyBatteryChange(gEGEnv, aLevel, aCharging, aRemainingTime);
}

Void GeckoAppShell::NotifySmsReceived(
    /* [in] */ const String& aSender,
    /* [in] */ const String& aBody,
    /* [in] */ Int32 aTimestamp)
{
    g_fNotifySmsReceived(gEGEnv, aSender, aBody, aTimestamp);
}

Int32  GeckoAppShell::SaveMessageInSentbox(
    /* [in] */ const String& aReceiver,
    /* [in] */ const String& aBody,
    /* [in] */ Int64 aTimestamp)
{
    Int32 result = g_fSaveMessageInSentbox(gEGEnv, aReceiver, aBody, aTimestamp);
    return result;
}

Void GeckoAppShell::NotifySmsSent(
    /* [in] */ Int32 aId, const String& aReceiver,
    /* [in] */ const String& aBody,
    /* [in] */ Int32 aTimestamp,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int32 aProcessId)
{
    g_fNotifySmsSent(gEGEnv, aId, aReceiver, aBody,
        aTimestamp, aRequestId, aProcessId);
}

Void GeckoAppShell::NotifySmsDelivered(
    /* [in] */ Int32 aId,
    /* [in] */ const String& aReceiver,
    /* [in] */ const String& aBody,
    /* [in] */ Int32 aTimestamp)
{
    g_fNotifySmsDelivered(gEGEnv, aId, aReceiver, aBody, aTimestamp);
}

Void GeckoAppShell::NotifySmsSendFailed(
    /* [in] */ Int32 aError,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int64 aProcessId)
{
    g_fNotifySmsSendFailed(gEGEnv, aError, aRequestId, aProcessId);
}

Void GeckoAppShell::NotifyGetSms(
    /* [in] */ Int32 aId,
    /* [in] */ const String& aReceiver,
    /* [in] */ const String& aSender,
    /* [in] */ const String& aBody,
    /* [in] */ Int32 aTimestamp,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int32 aProcessId)
{
    g_fNotifyGetSms(gEGEnv, aId, aReceiver, aSender, aBody,
        aTimestamp, aRequestId, aProcessId);
}

Void GeckoAppShell::NotifyGetSmsFailed(
    /* [in] */ Int32 aError,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int32 aProcessId)
{
    g_fNotifyGetSmsFailed(gEGEnv, aError, aRequestId, aProcessId);
}

Void GeckoAppShell::NotifySmsDeleted(
        /* [in] */ Boolean aDeleted,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int32 aProcessId)
{
    g_fNotifySmsDeleted(gEGEnv, aDeleted, aRequestId, aProcessId);
}

Void GeckoAppShell::NotifySmsDeleteFailed(
    /* [in] */ Int32 aError,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int32 aProcessId)
{
    g_fNotifySmsDeleteFailed(gEGEnv, aError, aRequestId, aProcessId);
}

Void GeckoAppShell::NotifyNoMessageInList(
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int32 aProcessId)
{
    g_fNotifyNoMessageInList(gEGEnv, aRequestId, aProcessId);
}

Void GeckoAppShell::NotifyListCreated(
    /* [in] */ Int32 aListId,
    /* [in] */ Int32 aMessageId,
    /* [in] */ const String& aReceiver,
    /* [in] */ const String& aSender,
    /* [in] */ const String& aBody,
    /* [in] */ Int32 aTimestamp,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int32 aProcessId)
{
    g_fNotifyListCreated(gEGEnv, aListId, aMessageId, aReceiver,
        aSender, aBody, aTimestamp, aRequestId, aProcessId);
}

Void GeckoAppShell::NotifyGotNextMessage(
    /* [in] */ Int32 aMessageId,
    /* [in] */ const String& aReceiver,
    /* [in] */ const String& aSender,
    /* [in] */ const String& aBody,
    /* [in] */ Int32 aTimestamp,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int32 aProcessId)
{
    g_fNotifyGotNextMessage(gEGEnv, aMessageId, aReceiver,
        aSender, aBody, aTimestamp, aRequestId, aProcessId);
}

Void GeckoAppShell::NotifyReadingMessageListFailed(
    /* [in] */ Int32 aError,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int32 aProcessId)
{
    g_fNotifyReadingMessageListFailed(gEGEnv, aError, aRequestId, aProcessId);
}

ECode GeckoAppShell::PutLocaleEnv()
{
    AutoPtr<ILocaleHelper> pLocaleHelper;
    ECode ec = CLocaleHelper::AcquireSingleton((ILocaleHelper**)&pLocaleHelper);
    if (FAILED(ec)) return ec;
    AutoPtr<ILocale> pLocale;
    ec = pLocaleHelper->GetDefault((ILocale**)&pLocale);
    if (FAILED(ec)) return ec;
    String langEnv("LANG=");
    //langEnv += pLocale->ToString();
    GeckoAppShell::PutEnv(langEnv);

   /* GeckoAppShell::PutEnv("LANG=" + Locale.getDefault().toString());
    NumberFormat nf = NumberFormat.getInstance();
    if (nf instanceof DecimalFormat) {
        DecimalFormat df = (DecimalFormat)nf;
        DecimalFormatSymbols dfs = df.getDecimalFormatSymbols();

        GeckoAppShell::PutEnv("LOCALE_DECIMAL_POINT=" + dfs.getDecimalSeparator());
        GeckoAppShell::PutEnv("LOCALE_THOUSANDS_SEP=" + dfs.getGroupingSeparator());
        GeckoAppShell::PutEnv("LOCALE_GROUPING=" + (char)df.getGroupingSize());
    }*/
    return NOERROR;
}

ECode GeckoAppShell::SendPendingEventsToGecko()
{
    //try {
        List< AutoPtr<GeckoEvent> >::Iterator it = gPendingEvents.Begin();
        while (it != gPendingEvents.End()) {
            GeckoEvent* event = *it;
            NotifyGeckoOfEvent(event);
            it = gPendingEvents.Erase(it);
        }
    //} catch (NoSuchElementException e) {}
    return NOERROR;
}

ECode GeckoAppShell::EnumerateGeckoProcesses(
    /* [in] */ GeckoProcessesVisitor* pVisiter)
{
    /*Int32 tokenSoFar, uid, pid;
    Boolean keepGoing;
    String token;
    //try {
        // run ps and parse its output
        AutoPtr<IRuntime> pIRuntime;
        ECode ec = Runtime::GetRuntime((IRuntime**)&pIRuntime);
        if (FAILED(ec)) return ec;
        AutoPtr<IProcess> pPs;
        ec = pIRuntime->Exec("ps", (IProcess**)&pPs);
        if (FAILED(ec)) return ec;

        AutoPtr<IInputStream> pInS;
        ec = pPs->GetInputStream((IInputStream**)&pInS);
        if (FAILED(ec)) return ec;
        AutoPtr<IInputStreamReader> pInSReader;
        ec = CInputStreamReader::New(pInS, (IInputStreamReader**)&pInSReader);
        if (FAILED(ec)) return ec;
        AutoPtr<IBufferedReader> pIn;
        ec = CBufferedReader::New(pInSReader, 2048, (IBufferedReader**)&pIn);
        if (FAILED(ec)) return ec;

        String headerOutput;
        ec = pIn->ReadLine(&headerOutput);
        if (FAILED(ec)) goto exit;

        // figure out the column offsets.  We only care about the pid and user fields
        if (sPidColumn == -1 || sUserColumn == -1) {
            StringTokenizer st(headerOutput.string());

            tokenSoFar = 0;
            while(st.HasMoreTokens()) {
                next = st.NextToken();
                if (next.EqualsIgnoreCase("PID")) {
                    sPidColumn = tokenSoFar;
                }
                else if (next.EqualsIgnoreCase("USER")) {
                    sUserColumn = tokenSoFar;
                }
                tokenSoFar++;
            }
        }

        // alright, the rest are process entries.
        String psOutput;
        while (SUCCEEDED(pIn->ReadLine(&psOutput);) && !psOutput.IsNull()) {
            StringTokenizer split(psOutput.string(), "\\s+");
            //String[] split = psOutput.split("\\s+");
            Int32 length = split.GetCount();
            if (length <= sPidColumn || length <= sUserColumn) {
                continue;
            }
            String uidName, pidName,last;
            for (Int32 i = 0; i < length; i++) {
                String token = split.NextToken();
                if (i == sUserColumn) {
                    uidName = token;
                }
                else if (i == sPidColumn) {
                    pidName = token;
                }
                else if (i == length - 1) {
                    last = token;
                }
            }
            uid = Process::GetUidForName(uidName);
            if (uid == Process::MyUid() && !last.EqualsIgnoreCase("ps")) {
                pid = StringUtils::ParseInt32(pidName);
                keepGoing = pVisiter->Callback(pid);
                if (!keepGoing) {
                    break;
                }
            }
        }
    //}
   // catch (Exception e) {
   //     Log.i("GeckoAppShell", "finding procs throws ",  e);
   // }

exit:
    pIn->Close();
    return ec;*/
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::GetDrawableForExtension(
    /* [in] */ IPackageManager* pPm,
    /* [in] */ const String& aExt,
    /* [out] */ IDrawable** ppDrawable)
{
    AutoPtr<IIntent> pIIntent;
    ECode ec = CIntent::New((IIntent**)&pIIntent);//Intent_ACTION_VIEW, &pIIntent);
    if (FAILED(ec)) return ec;

    AutoPtr<IMimeTypeMap> pMtm;
    ec = CMimeTypeMap::New((IMimeTypeMap**)&pMtm);
    if (FAILED(ec)) return ec;

    String mimeType;
    ec = pMtm->GetMimeTypeFromExtension(aExt, &mimeType);
    if (FAILED(ec)) return ec;

    if (!mimeType.IsNullOrEmpty()) {
        pIIntent->SetType(mimeType);
    }
    else {
        *ppDrawable = NULL;
        return NOERROR;
    }

    AutoPtr<IObjectContainer> list;
    ec = pPm->QueryIntentActivities(pIIntent, 0, (IObjectContainer**)&list);
    if (FAILED(ec)) return ec;

    Int32 size = 0;
    list->GetObjectCount(&size);
    if (size == 0) {
        *ppDrawable = NULL;
        return NOERROR;
    }

    AutoPtr<IObjectEnumerator> objEnu;
    list->GetObjectEnumerator((IObjectEnumerator**)&objEnu);
    Boolean isSucceeded;
    objEnu->MoveNext(&isSucceeded);
    AutoPtr<IResolveInfo> pResolveInfo ;
    if (isSucceeded) {
        objEnu->Current((IInterface**)&pResolveInfo);
        if (!pResolveInfo) {
            *ppDrawable = NULL;
            return NOERROR;
        }
    }

    AutoPtr<IActivityInfo> pIActivityInfo;
    ec = pResolveInfo->GetActivityInfo((IActivityInfo**)&pIActivityInfo);
    if (FAILED(ec)) return ec;

    ec = pIActivityInfo->LoadIcon(pPm, ppDrawable);
    return ec;
}


ECode GeckoAppShell::GetShowPasswordSetting(
    /* [out] */ Boolean*  isShow)
{
    *isShow = TRUE;
    ECode ec;
    Int32 showPassword;
    AutoPtr<IContentResolver> resolver;
    ec = GeckoApp::sAppContext->GetContentResolver((IContentResolver**)&resolver);
    if (FAILED(ec)) return NOERROR;

//    AutoPtr<ISettingsSystem> system;
//    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&system);

//    ec = system->GetInt32(resolver, String(ISettingsSystem::TEXT_SHOW_PASSWORD), 1, &showPassword);

    if (FAILED(ec)) return NOERROR;
    *isShow = (showPassword > 0);

    return NOERROR;
}

ECode GeckoAppShell::GetAccessibilityEnabled(
    /* [out] */ Boolean*  isEnabled)
{
    AutoPtr<IContext> pIContext;
    ECode ec = GeckoApp::sSurfaceView->GetContext((IContext**)&pIContext);
    if (FAILED(ec)) return ec;

    AutoPtr<IAccessibilityManager> accessibilityManager;
    ec = pIContext->GetSystemService(
                    IContext::ACCESSIBILITY_SERVICE,(IInterface**)&accessibilityManager);
    if (FAILED(ec)) return ec;

    assert(0);
    //ec = accessibilityManager->IsEnabled(isEnabled);
    return ec;
}

class AddPluginViewRunnable : public GeckoRunnable
{
public:
    AddPluginViewRunnable(
        IView* view,
        Double x, Double y,
        Double w, Double h)
        : GeckoRunnable(NULL),view(view),
        x(x),y(y),w(w),h(h)
    {}

    ECode Run()
    {
        AutoPtr<IAbsoluteLayoutLayoutParams> lp;
        ASSERT_SUCCEEDED(CAbsoluteLayoutLayoutParams::New(
            (Int32)w, (Int32)h,
            (Int32)x, (Int32)y,
            (IAbsoluteLayoutLayoutParams**)&lp));

        Int32 index;
        GeckoApp::sMainLayout->IndexOfChild(view,&index);
        if ( index == -1) {
            view->SetWillNotDraw(TRUE);
            GeckoApp::sMainLayout->AddView(view, (IViewGroupLayoutParams*)lp);
        }
        else
        {
            assert(0);
            return E_NOT_IMPLEMENTED;
            //return GeckoApp::sMainLayout->UpdateViewLayout(view, (IViewGroupLayoutParams*)lp);
        }
        delete this;
        return NOERROR;
    }

protected:
    AutoPtr<IView> view;
    Double x;
    Double y;
    Double w;
    Double h;
};

ECode GeckoAppShell::AddPluginView(
    /* [in] */ IView* view,
    /* [in] */ Double x,
    /* [in] */ Double y,
    /* [in] */ Double w,
    /* [in] */ Double h)
{
    //printf("GeckoAppShell "  "addPluginView:%s @ x:%f y:%f w:%f h:%f",
    //    view->ToString(), x, y, w, h);

    AutoPtr<IHandler> handler;
    ECode ec = GetHandler((IHandler**)&handler);
    if (FAILED(ec)) return ec;

    AutoPtr<AddPluginViewRunnable> runnable = new AddPluginViewRunnable(view, x, y, w, h);
    if (!runnable) return E_OUT_OF_MEMORY;

    Boolean result;
    return handler->Post(runnable, &result);
}


class RemovePluginViewRunnable : public GeckoRunnable
{
public:
    RemovePluginViewRunnable(
        IView* view)
        : GeckoRunnable(NULL),view(view)
    {}

    ECode Run()
    {
        assert(0);
        //GeckoApp::sMainLayout->RemoveView(view);
        //return NOERROR;
        delete this;
        return E_NOT_IMPLEMENTED;
    }

protected:
    AutoPtr<IView> view;
};

ECode GeckoAppShell::RemovePluginView(
    /* [in] */ IView* view)
{
    AutoPtr<IHandler> handler;
    ECode ec = GetHandler((IHandler**)&handler);
    if (FAILED(ec)) return ec;

    AutoPtr<RemovePluginViewRunnable> runnable = new RemovePluginViewRunnable(view);
    if (!runnable) return E_OUT_OF_MEMORY;

    Boolean result;
    return handler->Post(runnable, &result);
}

/*
public static Class<?> loadPluginClass(String className, String libName) {
    Log.i("GeckoAppShell", "in loadPluginClass... attempting to access className, then libName.....");
    Log.i("GeckoAppShell", "className: " + className);
    Log.i("GeckoAppShell", "libName: " + libName);

    try {
        String[] split = libName.split("/");
        String packageName = split[split.length - 3];
        Log.i("GeckoAppShell", "load \"" + className + "\" from \"" + packageName +
              "\" for \"" + libName + "\"");
        Context pluginContext =
            GeckoApp.mAppContext.createPackageContext(packageName,
                                                      Context.CONTEXT_INCLUDE_CODE |
                                                  Context.CONTEXT_IGNORE_SECURITY);
        ClassLoader pluginCL = pluginContext.getClassLoader();
        return pluginCL.loadClass(className);
    } catch (java.lang.ClassNotFoundException cnfe) {
        Log.i("GeckoAppShell", "class not found", cnfe);
    } catch (android.content.pm.PackageManager.NameNotFoundException nnfe) {
        Log.i("GeckoAppShell", "package not found", nnfe);
    }
    Log.e("GeckoAppShell", "couldn't find class");
    return NULL;
}
*/

ECode GeckoAppShell::GetSurfaceInfo(
        /* [in] */ ISurfaceView* sview,
        /* [out] */ SurfaceInfo** info)
{
    //printf("GeckoAppShell" "getSurfaceInfo %s\n",sview->ToString());
    if (sview == NULL)
        return E_INVALID_ARGUMENT;

    Int32 format = -1;
/*
    try {
        Field privateFormatField = SurfaceView.class.getDeclaredField("mFormat");
        privateFormatField.setAccessible(true);
        format = privateFormatField.getInt(sview);
    } catch (Exception e) {
        Log.i("GeckoAppShell", "mFormat is not a field of sview: ", e);
    }
*/
/*
    Int32 n = 0;
    if (format == ElPixelFormat:RGB_565) {
        n = 2;
    } else if (format == ElPixelFormat::RGBA_8888) {
        n = 4;
    } else {
        printf("GeckoAppShell"  "Unknown pixel format: %s\n", format);
        return NULL;
    }
*/
    *info = new SurfaceInfo();

    AutoPtr<ISurfaceHolder> holder;
    sview->GetHolder((ISurfaceHolder**)&holder);
    AutoPtr<IRect> r;
    holder->GetSurfaceFrame((IRect**)&r);

    Int32 right;
    r->GetRight(&right);
    (*info)->width = right;

    Int32 bottom;
    r->GetBottom(&bottom);
    (*info)->height = bottom;

    (*info)->format = format;

    return NOERROR;
}

/*
Class GeckoAppShell::GetSurfaceInfoClass() {
    Log.i("GeckoAppShell", "class name: " + SurfaceInfo.class.getName());
    return SurfaceInfo.class;
}
*/

typedef void (*GeckoAppShell_executeNextRunnable_t)(EGEnv *jenv);

GeckoAppShell_executeNextRunnable_t g_fExecuteNextRunnable = NULL;

Void GeckoAppShell::ExecuteNextRunnable()
{
    return g_fExecuteNextRunnable(gEGEnv);
}


class PostToJavaThreadRunnable : public GeckoRunnable
{
public:
    PostToJavaThreadRunnable    ()
        : GeckoRunnable(NULL)
    {}

    ECode Run()
    {
        printf("GeckoShell" "run GeckoRunnableCallback");
        GeckoAppShell::ExecuteNextRunnable();
        return  NOERROR;
    }
};

ECode GeckoAppShell::PostToJavaThread(
    /* [in] */ Boolean mainThread)
{
    assert(0);
    printf("GeckoShell"  "post to %s java thread\n",(mainThread ? "main " : ""));
    AutoPtr<IHandler> pHandler;
    ECode ec = GetMainHandler((IHandler**)&pHandler);
    if (FAILED(ec)) return ec;

    AutoPtr<PostToJavaThreadRunnable> pRunnable = new PostToJavaThreadRunnable();

    //return pHandler->Post(IRunnable::Probe(pRunnable));
    return NOERROR;
}


typedef Void (*GeckoAppShell_cameraCallbackBridge_t)(EGEnv *jenv, ArrayOf<Byte>* data);

GeckoAppShell_cameraCallbackBridge_t g_fCameraCallbackBridge = NULL;

ECode GeckoAppShell::CameraCallbackBridge(
    /* [in] */ ArrayOf<Byte>* data)
{
    g_fCameraCallbackBridge(gEGEnv, data);
    return NOERROR;
}

Int32 GeckoAppShell::sKPreferedFps = 25;

ArrayOf<Byte>* GeckoAppShell::sCameraBuffer = NULL;


class InitCameraThreadRunnable : public GeckoRunnable
{
public:
    InitCameraThreadRunnable()
        : GeckoRunnable(NULL)
    {}

    ECode Run()
    {
        printf("GeckoShell" "run GeckoRunnableCallback");
        GeckoApp::sAppContext->EnableCameraView();
        return NOERROR;
    }
};

ECode GeckoAppShell::InitCamera(
    /* [in] */ const String& aContentType,
    /* [in] */ Int32 aCamera,
    /* [in] */ Int32 aWidth,
    /* [in] */ Int32 aHeight,
    /* [out] */ ArrayOf<Int32>** result)
{
    AutoPtr<IThread> thread;
    thread = Thread::GetCurrentThread();
    Int64 id;
    thread->GetId(&id);
    printf(GeckoAppShell_LOG_FILE_NAME "initCamera(%s, %dx%d) on thread %d\n", (const char *)aContentType, aWidth, aHeight, id);

    IHandler* pHandler;
    ECode ec = GetMainHandler(&pHandler);
    if (FAILED(ec)) return ec;

    AutoPtr<InitCameraThreadRunnable> pRunnable = new InitCameraThreadRunnable();

    if (!pRunnable) {
       pHandler->Release();
       return E_OUT_OF_MEMORY;
    }

    //return pHandler->Post(IRunnable::Probe(pRunnable));

    // [0] = 0|1 (failure/success)
    // [1] = width
    // [2] = height
    // [3] = fps
    *result = ArrayOf<Int32>::Alloc(4);
    (*(*result))[0] = 0;

    if (Build::VERSION::SDK_INT >= 9) {
        Int32 num;
        //CCamera::GetNumberOfCameras(&num);
        if (num == 0)
            return NOERROR;
    }

    AutoPtr<IObjectContainer> pRateContainer;
    AutoPtr<IObjectEnumerator> pRateEnum;
    AutoPtr<IObjectContainer> pSizeContainer;
    AutoPtr<IObjectEnumerator> pSizeEnum;
    AutoPtr<ISurfaceHolder> holder;


    // no front/back camera before API level 9
    if (Build::VERSION::SDK_INT >= 9) {
        //ec = CCamera::Open(aCamera, (ICamera**)sCamera);
        if (FAILED(ec)) {goto out;}
    }
    else {
        //ec = CCamera::Open((ICamera**)sCamera);
        if (FAILED(ec)) {goto out;}
    }


    //AutoPtr<IParameters> params;
    //ASSERT_SUCCEEDED(sCamera->GetParameters((IParameters**)&params));
    //ec = params->SetPreviewFormat(ImageFormat_NV21);
    if (FAILED(ec)) {goto out;}

    // use the preview fps closest to 25 fps.
    Int32 fpsDelta;
    fpsDelta = 1000;

    //ec = params->GetSupportedPreviewFrameRates(&pRateContainer);
    if (FAILED(ec)) {goto SetPFR;}

    ec = pRateContainer->GetObjectEnumerator((IObjectEnumerator**)&pRateEnum);
    if (FAILED(ec)) {goto SetPFR;}

    Boolean succeeded;
    ec = pRateEnum->MoveNext(&succeeded);
    if (FAILED(ec)) {goto SetPFR;}

    while (SUCCEEDED(ec) && succeeded) {
        Int32  nFps;
        FAIL_RETURN(pRateEnum->Current((IInterface**)&nFps));
        Int32 delta = Math::Abs(nFps - sKPreferedFps);

        if (delta < fpsDelta) {
            fpsDelta = Math::Abs(nFps - sKPreferedFps);
            //params->SetPreviewFrameRate(nFps);
        }

        ec = pRateEnum->MoveNext(&succeeded);
        if (FAILED(ec)) {goto SetPFR;}
    }

SetPFR:
    if (FAILED(ec)) {
        //params->SetPreviewFrameRate(sKPreferedFps);
    };

    // set up the closest preview size available


    //FAIL_RETURN(params->GetSupportedPreviewSizes(&pSizeContainer));


    FAIL_RETURN(pSizeContainer->GetObjectEnumerator((IObjectEnumerator**    )&pSizeEnum));

    Boolean sizeSucceeded;
    FAIL_RETURN(pSizeEnum->MoveNext(&sizeSucceeded));

    Int32 sizeDelta;
    sizeDelta = 10000000;
    Int32 bufferSize;
    bufferSize = 0;
    while (SUCCEEDED(ec) && sizeSucceeded) {
        /*
        AutoPtr<ISize> size;
        FAIL_RETURN(pSizeEnum->Current((IInterface*)&size));

        Int32 width;
        size->GetWidth(&with);
        Int32 height;
        size->GetHeight(&height);

        if (Math::Abs(with * height - aWidth * aHeight) < sizeDelta) {
            sizeDelta = Math::Abs(width * height - aWidth * aHeight);
            //params->SetPreviewSize(width, height);
            bufferSize = width * height;
        }

        FAIL_RETURN(pSizeEnum->MoveNext(&succeeded));
        */
    }


    GeckoApp::sCameraView->GetHolder((ISurfaceHolder**)&holder);
    //ec = sCamera->SetPreviewDisplay(holder);
    if (FAILED(ec)) {
        printf(GeckoAppShell_LOG_FILE_NAME "Error setPreviewDisplay\n");
    };

    //sCamera->SetParameters(params);

    sCameraBuffer = ArrayOf<Byte>::Alloc((bufferSize * 12) / 8);
    //sCamera->AddCallbackBuffer(sCameraBuffer);
/*
    sCamera->SetPreviewCallbackWithBuffer(new android.hardware.Camera.PreviewCallback() {
        public void onPreviewFrame(byte[] data, android.hardware.Camera camera) {
            cameraCallbackBridge(data);
            if (sCamera != NULL)
                sCamera.addCallbackBuffer(sCameraBuffer);
        }
    });
*/
    //sCamera->StartPreview();
    //sCamera->GetParameters((IParameters**)&params);
    //AutoPtr<ISize> size;
    //params->GetPreviewSize((ISize**)&size);
    Int32 width;
    //size->GetWidth(&with);
    Int32 height;
    //size->GetHeight(&height);
    Int32 rate;
    //params->GetPreviewFrameRate(&rate);
    Int32 format;
    //params->GetPreviewFormat(&format);
    printf(GeckoAppShell_LOG_FILE_NAME "Camera: %dx%d @ %dfps. format is %d\n", width, height, rate, format);
    (*(*result))[0] = 1;
    (*(*result))[1] = width;
    (*(*result))[2] = height;
    (*(*result))[3] = rate;

    printf(GeckoAppShell_LOG_FILE_NAME "Camera preview started");
    return NOERROR;
out:
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        printf(GeckoAppShell_LOG_FILE_NAME "initCamera RuntimeException ");
        (*(*result))[0] = (*(*result))[1] = (*(*result))[2] = (*(*result))[3] = 0;
        return NOERROR;
    }

    return ec;
}

class CloseCameraRunnable : public GeckoRunnable
{
public:
    CloseCameraRunnable()
        : GeckoRunnable(NULL)
    {}

    ECode Run()
    {
        GeckoApp::sAppContext->DisableCameraView();
        return NOERROR;
    }
};

ECode GeckoAppShell::CloseCamera()
{
    AutoLock lock(&sSyncCloseCameraMet);
    AutoPtr<IThread> thread = Thread::GetCurrentThread();
    Int64 id;
    thread->GetId(&id);
    printf(GeckoAppShell_LOG_FILE_NAME "closeCamera() on thread ");
    AutoPtr<IHandler> pHandler;
    ECode ec = GetMainHandler((IHandler**)&pHandler);
    if (FAILED(ec)) return ec;

    AutoPtr<CloseCameraRunnable> pRunnable = new CloseCameraRunnable();

    //pHandler->Post(IRunnable::Probe(pRunnable));
    if (sCamera != NULL) {
        //sCamera->StopPreview();
        sCamera->Release();
        sCamera = NULL;
        if (sCameraBuffer != NULL) {
            ArrayOf<Byte>::Free(sCameraBuffer);
            sCameraBuffer = NULL;
        }
    }

    return NOERROR;
}

class FireAndWaitForTracerEventRunnable : public GeckoRunnable
{
public:
    FireAndWaitForTracerEventRunnable()
        : GeckoRunnable(NULL)
    {}

    ECode Run()
    {
        AutoPtr<IDate> date;
        ASSERT_SUCCEEDED(CDate::New((IDate**)&date));
        // ECode ec = sTracerQueue->Put(date);
        // if (FAILED(ec)) {
        //     printf(GeckoAppShell_LOG_FILE_NAME "exception firing tracer");
        // }
        return NOERROR;
    }
};

ECode GeckoAppShell::FireAndWaitForTracerEvent()
{
    assert(0);
    AutoPtr<IHandler> pHandler;
    ECode ec = GetMainHandler((IHandler**)&pHandler);
    if (FAILED(ec)) return ec;

    AutoPtr<FireAndWaitForTracerEventRunnable> pRunnable = new FireAndWaitForTracerEventRunnable();


    //pHandler->Post(IRunnable::Probe(pRunnable));

    //ec = sTracerQueue->Take();
    if (FAILED(ec)) {
        printf(GeckoAppShell_LOG_FILE_NAME "exception firing tracer");
    };
    return NOERROR;
}

// unused
ECode GeckoAppShell::CheckUriVisited(
    /* [in] */ const String& uri)
{
    return NOERROR;
}

// unused
ECode GeckoAppShell::MarkUriVisited(
    /* [in] */ const String& uri)
{
    return NOERROR;
}

/*
 * Battery API related methods.
 */
ECode GeckoAppShell::EnableBatteryNotifications()
{
    CGeckoBatteryManager::EnableNotifications();
    return NOERROR;
}

ECode GeckoAppShell::HandleGeckoMessage(
    /* [in] */ const String& message,
    /* [out] */ String* result)
{
    PRINT_FILE_LINE_EX("HandleGeckoMessage %s", message.string());
/*    ECode ec = NOERROR;
    json_t *json;
    json_error_t error;
    json_t * geckoObject;
    json_t * typeObject;

    json = json_loads(message.string(), 0, &error);
    if(!json) { return E_UNEXPECTED; }
    geckoObject = json_object_get(json, "gecko");
    typeObject = json_object_get(geckoObject, "type");
    String type(json_string_value(typeObject));

    if (type.Equals("Gecko:Ready")) {
        ec = OnAppShellReady();
    }

    if (FAILED(ec)) {
        printf(GeckoAppShell_LOG_FILE_NAME "handleGeckoMessage throws %d" , ec);
    };

    *result = "";
    json_decref(json);*/
    OnAppShellReady();
    return  NOERROR;
}

ECode GeckoAppShell::DisableBatteryNotifications()
{
    CGeckoBatteryManager::DisableNotifications();
    return NOERROR;
}

ECode GeckoAppShell::GetCurrentBatteryInformation(
    /* [out] */ ArrayOf<double>** info )
{
    *info = CGeckoBatteryManager::GetCurrentInformation();
    return NOERROR;
}

/*
 * WebSMS related methods.
 */
ECode GeckoAppShell::GetNumberOfMessagesForText(
    /* [in] */ const String& aText,
    /* [out] */ Int32* num)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::SendMessage(
    /* [in] */ const String& aNumber,
    /* [in] */ const String& aMessage,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int64 aProcessId)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::SaveSentMessage(
    /* [in] */ ICharSequence* aRecipient,
    /* [in] */ ICharSequence* aBody,
    /* [in] */ IInteger32* aDate,
    /* [in] */ Int32* errorCode)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::GetMessage(
    /* [in] */ Int32 aMessageId,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int64 aProcessId)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::DeleteMessage(
    /* [in] */ Int32 aMessageId,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int64 aProcessId)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::CreateMessageList(
    /* [in] */ Int64 aStartDate,
    /* [in] */ Int64 aEndDate,
    /* [in] */ ArrayOf<String>& aNumbers,
    /* [in] */ Int32 aNumbersCount,
    /* [in] */ Int32 aDeliveryState,
    /* [in] */ Boolean aReverse,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int64 aProcessId)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::GetNextMessageInList(
    /* [in] */ Int32 aListId,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int64 aProcessId)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::ClearMessageList(
    /* [in] */ Int32 aListId)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoAppShell::IsTablet(
    /* [out] */ Boolean *isTablet)
{
    if (Build::VERSION::SDK_INT > Build::VERSION_CODES::GINGERBREAD) {
        AutoPtr<IResources> res;
        AutoPtr<IConfiguration> config;
        FAIL_RETURN(GeckoApp::sAppContext->GetResources((IResources**)&res));
        FAIL_RETURN(res->GetConfiguration((IConfiguration**)&config));
        // xlarge is defined by android as screens larger than 960dp x 720dp
        // and should include most devices ~7in and up.
        // http://developer.android.com/guide/practices/screens_support.html
        Int32 layout;
        config->GetScreenLayout(&layout);
        if ((layout & IConfiguration::SCREENLAYOUT_SIZE_MASK) >= IConfiguration::SCREENLAYOUT_SIZE_XLARGE) {
            *isTablet = TRUE;
            return NOERROR;
        }
    }
    *isTablet = FALSE;
    return NOERROR;
}

ECode GeckoAppShell::EmitGeckoAccessibilityEvent(
    /* [in] */ Int32 eventType,
    /* [in] */ ArrayOf<String>& textList,
    /* [in] */ const String& description,
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean checked,
    /* [in] */ Boolean password)
{
    return NOERROR;
}

ECode GeckoAppShell::GetCurrentNetworkInformation(
    /* [out] */ ArrayOf<Double>** info)
{
    AutoPtr<CGeckoNetworkManager> mgr
     = CGeckoNetworkManager::GetInstance();
    return mgr->GetCurrentInformation(info);
}

ECode GeckoAppShell::EnableNetworkNotifications()
{
    AutoPtr<CGeckoNetworkManager> mgr
        = CGeckoNetworkManager::GetInstance();
    return mgr->EnableNotifications();
}

ECode GeckoAppShell::DisableNetworkNotifications()
{
    AutoPtr<CGeckoNetworkManager> mgr
        = CGeckoNetworkManager::GetInstance();
    return mgr->DisableNotifications();
}

// This is only used in Native Fennec.
ECode GeckoAppShell::SetPreventPanning(
    /* [in] */ Boolean aPreventPanning)
{
    return NOERROR;
}

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
