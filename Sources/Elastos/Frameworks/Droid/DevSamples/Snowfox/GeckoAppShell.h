
#ifndef __GECKOAPPSHELL_H__
#define __GECKOAPPSHELL_H__

#ifndef __USE_MALLOC
#define __USE_MALLOC
#endif

#include "GeckoEvent.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/core/Thread.h>
#include "CAlertNotification.h"
#include "CGeckoSurfaceView.h"
#include <elastos/TimerTask.h>
#include "SurfaceInfo.h"
//#include <Elastos.Utility.Locks.h>

#define GeckoAppShell_LOG_FILE_NAME "GeckoAppShell"

using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::IO::IFile;
using Elastos::IO::IByteBuffer;
using Elastos::Utility::ITimer;
using Elastos::Utility::ITimerTask;
using Elastos::Utility::TimerTask;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::View::ISurfaceView;
using Elastos::Droid::Graphics::ICamera;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

class GeckoAppShell
{
private:
    // A looper thread, accessed by GeckoAppShell.getHandler
    class LooperThread : public IThread
                       , public Thread
                       , public ElRefBase
    {
    public:
        LooperThread();

        ECode Run();

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(Object*) GetSelfLock();

        UInt32 AddRef();

        UInt32 Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface* pObject,
            /* [in] */ InterfaceID* pIID);

        CARAPI CheckAccess();

        CARAPI CountStackFrames(
            /* [out] */ Int32* number);

        CARAPI Destroy();

        CARAPI GetId(
            /* [out] */ Int64* id);

        CARAPI GetName(
            /* [out] */ String* name);

        CARAPI GetPriority(
            /* [out] */ Int32* priority);

        CARAPI GetState(
            /* [out] */ ThreadState* state);

        CARAPI GetThreadGroup(
            /* [out] */ IThreadGroup** group);

        CARAPI Interrupt();

        CARAPI IsAlive(
            /* [out] */ Boolean* isAlive);

        CARAPI IsDaemon(
            /* [out] */ Boolean* isDaemon);

        CARAPI IsInterrupted(
            /* [out] */ Boolean* isInterrupted);

        CARAPI Join();

        CARAPI Join(
            /* [in] */ Int64 millis);

        CARAPI Join(
            /* [in] */ Int64 millis,
            /* [in] */ Int32 nanos);

        CARAPI Resume();

        CARAPI SetDaemon(
            /* [in] */ Boolean isDaemon);

        CARAPI SetName(
            /* [in] */ const String& threadName);

        CARAPI SetPriority(
            /* [in] */ Int32 priority);

        CARAPI Start();

        CARAPI Stop();

        CARAPI Suspend();

        CARAPI Wait(
            /* [in] */ Int64 time,
            /* [in] */ Int32 frac);

    public:
         //SynchronousQueue<Handler> mHandlerQueue;
    private:
        Object mSync;
    };

    /*private static class GeckoMediaScannerClient implements MediaScannerConnectionClient {
        private String mFile = "";
        private String mMimeType = "";
        private MediaScannerConnection mScanner = null;

        public GeckoMediaScannerClient(Context aContext, String aFile, String aMimeType) {
            mFile = aFile;
            mMimeType = aMimeType;
            mScanner = new MediaScannerConnection(aContext, this);
            if (mScanner != null)
                mScanner.connect();
        }

        public Void onMediaScannerConnected() {
            mScanner.scanFile(mFile, mMimeType);
        }

        public Void onScanCompleted(String path, Uri uri) {
            if(path.equals(mFile)) {
                mScanner.disconnect();
                mScanner = null;
            }
        }
    }*/

    /* Delay updating IME states (see bug 573800) */
    class IMEStateUpdater
         : public ElRefBase
         , public ITimerTask
         , public TimerTask
    {
    public:
        IMEStateUpdater()
            : mEnable(FALSE)
            , mReset(FALSE)
        {}

        CARAPI_(PInterface) Probe(
           /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
           /* [in] */ IInterface *pObject,
           /* [out] */ InterfaceID *pIID);

        CARAPI Run();

        CARAPI Cancel(
            /* [out] */ Boolean * pValue);

        CARAPI ScheduledExecutionTime(
            /* [out] */ Int64 * pValue);

        CARAPI GetWhen(
            /* [out] */ Int64* when);

        CARAPI SetWhen(
            /* [in] */ Int64 when);

        CARAPI IsScheduled(
            /* [out] */ Boolean* scheduled);

        CARAPI IsCancelled(
            /* [out] */ Boolean* cancelled);

        CARAPI GetPeriod(
            /* [out] */ Int64* period);

        CARAPI SetPeriod(
            /* [in] */ Int64 period);

        CARAPI IsFixedRate(
            /* [out] */ Boolean* fixed);

        CARAPI SetFixedRate(
            /* [in] */ Boolean fixed);

        CARAPI Lock();

        CARAPI Unlock();

        CARAPI SetScheduledTime(
            /* [in] */ Int64 time);

        static Void EnableIME();

        static Void ResetIME();

    private:
        CARAPI_(Object*) GetSelfLock();

        static AutoPtr<GeckoAppShell::IMEStateUpdater> GetInstance();

    private:
        Object mLock;
        static Object mClassLock;
        static AutoPtr<GeckoAppShell::IMEStateUpdater> instance;
        Boolean mEnable;
        Boolean mReset;
    };

public:
    // Get a Handler for the main java thread
    static ECode GetMainHandler(
        /* [out] */ IHandler** ppHandler);

    static ECode GetHandler(
        /* [out] */ IHandler** ppHandler);

    static ECode GetCacheDir(
        /* [out] */ IFile** ppFile);

    static ECode GetFreeSpace(
        /* [out] */ Int32* pFreeSpace);

    static ECode MoveFile(
        /* [in] */ IFile* pInFile,
        /* [in] */ IFile* pOutFile,
        /* [out] */ Boolean* pResult);

    static ECode MoveDir(
        /* [in] */ IFile* pFrom,
        /* [in] */ IFile* pTo,
        /* [out] */ Boolean* pResult);

    static ECode LoadGeckoLibs(
        /* [in] */ const String& apkName);

    static ECode RunGecko(
        /* [in] */ const String& apkPath,
        /* [in] */ const String& args,
        /* [in] */ const String& url);

    static ECode SendEventToGecko(
        /* [in] */ GeckoEvent* event);

    static ECode SendEventToGeckoSync(
        /* [in] */ GeckoEvent* event);

    //The Gecko-side API: API methods that Gecko calls
    static ECode ScheduleRedraw();

    static ECode ScheduleRedraw(
        /* [in] */ Int32 nativeWindow,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    static ECode NotifyIME(
        /* [in] */ Int32 type,
        /* [in] */ Int32 state);

    static ECode NotifyIMEEnabled(
        /* [in] */ Int32 state,
        /* [in] */ const String& typeHint,
        /* [in] */ const String& actionHint,
        /* [in] */ Boolean landscapeFS);

    static ECode NotifyScreenShot(
        /* [in] */ IByteBuffer *data,
        /* [in] */ Int32 tabId,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    static ECode NotifyIMEChange(
        /* [in] */ const String& text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 newEnd);

//    static ECode NotifyScreenShot(
//        /* [in] */ ByteBuffer& data,
//        /* [in] */ Int32 tabId,
//        /* [in] */ Int32 width,
//        /* [in] */ Int32 height);

    // Block the current thread until the Gecko event loop is caught up
    static ECode GeckoEventSync();

    // Signal the Java thread that it's time to wake up
    static ECode AcknowledgeEventSync();

    static ECode EnableDeviceMotion(
        /* [in] */ Boolean enable);

    static ECode EnableLocation(
        /* [in] */ Boolean enable);

    static ECode EnableSensor(
        /* [in] */ Int32 aSensortype);

    static ECode DisableSensor(
        /* [in] */ Int32 aSensortype);

    static ECode MoveTaskToBack();

    static ECode ReturnIMEQueryResult(
        /* [in] */ const String& result,
        /* [in] */ Int32 selectionStart,
        /* [in] */ Int32 selectionLength);

    static ECode OnAppShellReady();

    static ECode OnXreExit();

    static ECode ScheduleRestart();

    // "Installs" an application by creating a shortcut
    static ECode CreateShortcut(
        /* [in] */ const String& aTitle,
        /* [in] */ const String& aURI,
        /* [in] */ const String& aIconData,
        /* [in] */ const String& aType);

    static ECode GetHandlersForMimeType(
        /* [in] */ const String& aMimeType,
        /* [in] */ const String& aAction,
        /* [out, callee] */ ArrayOf<String>** ppHandlers);

    static ECode GetHandlersForURL(
        /* [in] */ const String& aURL,
        /* [in] */ const String& aAction,
        /* [out, callee] */ ArrayOf<String>** ppHandlers);

    static ECode GetHandlersForIntent(
        /* [in] */ IIntent* pIntent,
        /* [out, callee] */ ArrayOf<String>** ppHandlers);

    static ECode GetIntentForActionString(
        /* [in] */ const String& aAction,
        /* [out] */ IIntent** ppIntent);

    static ECode GetExtensionFromMimeType(
        /* [in] */ const String& aMimeType,
        /* [out] */ String* ext);

    static ECode GetMimeTypeFromExtensions(
        /* [in] */ const String& aFileExt,
        /* [out] */ String* mimeType);

    static ECode OpenUriExternal(
        /* [in] */ const String& aUriSpec,
        /* [in] */ const String& aMimeType,
        /* [in] */ const String& aPackageName,
        /* [in] */ const String& aClassName,
        /* [in] */ const String& aAction,
        /* [in] */ const String& aTitle);

    // On some devices, access to the clipboard service needs to happen
    // on a thread with a looper, so dispatch this to our looper thread
    // Note: the main looper won't work because it may be blocked on the
    // gecko thread, which is most likely this thread
    static ECode GetClipboardText(
        /* [out] */ String* text);

    static ECode SetClipboardText(
        /* [in] */ const String& text);

    static ECode ShowAlertNotification(
        /* [in] */ const String& aImageUrl,
        /* [in] */ const String& aAlertTitle,
        /* [in] */ const String& aAlertText,
        /* [in] */ const String& aAlertCookie,
        /* [in] */ const String& aAlertName);

    static ECode AlertsProgressListener_OnProgress(
        /* [in] */ const String& aAlertName,
        /* [in] */ Int32 aProgress,
        /* [in] */ Int32 aProgressMax,
        /* [in] */ const String& aAlertText);

    static ECode AlertsProgressListener_OnCancel(
        /* [in] */ const String& aAlertName);

    static ECode HandleNotification(
        /* [in] */ const String& aAction,
        /* [in] */ const String& aAlertName,
        /* [in] */ const String& aAlertCookie);

    static ECode RemoveNotification(
        /* [in] */ Int32 notificationID);

    static ECode GetDpi(
        /* [out] */ Int32* pDpi);

    static ECode SetFullScreen(
        /* [in] */ Boolean fullscreen);

    static ECode ShowFilePicker(
        /* [in] */ const String& aFilters,
        /* [out] */ String* pPicker);

    static ECode PerformHapticFeedback(
        /* [in] */ Boolean aIsLongPress);

    static ECode Vibrate(Int64 milliseconds);

    static ECode Vibrate(ArrayOf<Int64>& pattern, Int32 repeat);

    static ECode CancelVibrate();

    static ECode ShowInputMethodPicker();

    static ECode HideProgressDialog();

    static ECode SetKeepScreenOn(
        /* [in] */ Boolean on);

    static ECode IsNetworkLinkUp(
        /* [out] */ Boolean* isLinkUp);

    static ECode IsNetworkLinkKnown(
        /* [out] */ Boolean* isLinkKnown);

    static ECode SetSelectedLocale(
        /* [in] */ const String& localeCode);

    static ECode GetSystemColors(
        /* [out] */ ArrayOf<Int32>** ppColors);

    static ECode PutChildInBackground();

    static ECode PutChildInForeground();

    static ECode KillAnyZombies();

    static ECode CheckForGeckoProcs(
        /* [out] */ Boolean* pResult);

    static ECode WaitForAnotherGeckoProc();

    static ECode ScanMedia(
        /* [in] */ const String& aFile,
        /* [in] */ const String& aMimeType);

    static ECode GetIconForExtension(
        /* [in] */ String& aExt,
        /* [in] */ Int32 iconSize,
        /* [out] */ ArrayOf<Byte>** ppIcons);

    static ECode GetShowPasswordSetting(
        /* [out] */ Boolean*  isShow);

    static ECode GetAccessibilityEnabled(
        /* [out] */ Boolean*  isEnabled);

    static ECode AddPluginView(
        /* [in] */ IView* view,
        /* [in] */ Double x,
        /* [in] */ Double y,
        /* [in] */ Double w,
        /* [in] */ Double h);

    static ECode RemovePluginView(
        /* [in] */ IView* view);

    static ECode GetSurfaceInfo(
        /* [in] */ ISurfaceView* sview,
        /* [out] */ SurfaceInfo** info);


//    static Class GetSurfaceInfoClass();

    static Void ExecuteNextRunnable();

    static ECode PostToJavaThread(
        /* [in] */ Boolean mainThread);

    static ECode CameraCallbackBridge(
        /* [in] */ ArrayOf<Byte>* data);

    static ECode InitCamera(
        /* [in] */ const String& aContentType,
        /* [in] */ Int32 aCamera,
        /* [in] */ Int32 aWidth,
        /* [in] */ Int32 aHeight,
        /* [out] */ ArrayOf<Int32>** result);

    static ECode CloseCamera();

    static ECode FireAndWaitForTracerEvent();

    static ECode CheckUriVisited(
        /* [in] */ const String& uri);

    static ECode MarkUriVisited(
        /* [in] */ const String& uri);

    static ECode EnableBatteryNotifications();

    static ECode HandleGeckoMessage(
        /* [in] */ const String& message,
        /* [out] */ String* result);

    static ECode DisableBatteryNotifications();

    static ECode GetCurrentBatteryInformation(
        /* [out] */ ArrayOf<double>** info );

    static ECode GetNumberOfMessagesForText(
        /* [in] */ const String& aText,
        /* [out] */ Int32* num);

    static ECode SendMessage(
        /* [in] */ const String& aNumber,
        /* [in] */ const String& aMessage,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int64 aProcessId);

    static ECode SaveSentMessage(
        /* [in] */ ICharSequence* aRecipient,
        /* [in] */ ICharSequence* aBody,
        /* [in] */ IInteger32* aDate,
        /* [out] */ Int32* errorCode);

    static ECode GetMessage(
        /* [in] */ Int32 aMessageId,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int64 aProcessId);

    static ECode DeleteMessage(
        /* [in] */ Int32 aMessageId,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int64 aProcessId);

    static ECode CreateMessageList(
        /* [in] */ Int64 aStartDate,
        /* [in] */ Int64 aEndDate,
        /* [in] */ ArrayOf<String>& aNumbers,
        /* [in] */ Int32 aNumbersCount,
        /* [in] */ Int32 aDeliveryState,
        /* [in] */ Boolean aReverse,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int64 aProcessId);

    static ECode GetNextMessageInList(
        /* [in] */ Int32 aListId,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int64 aProcessId);

    static ECode ClearMessageList(
            /* [in] */ Int32 aListId);

    static ECode IsTablet(
        /* [in] */ Boolean *isTablet);

    static ECode EmitGeckoAccessibilityEvent(
        /* [in] */ Int32 eventType,
        /* [in] */ ArrayOf<String>& textList,
        /* [in] */ const String& description,
        /* [in] */ Boolean enabled,
        /* [in] */ Boolean checked,
        /* [in] */ Boolean password);

    static ECode GetCurrentNetworkInformation(
        /* [out] */ ArrayOf<Double>** info);

    static ECode EnableNetworkNotifications();

    static ECode DisableNetworkNotifications();

    static ECode SetPreventPanning(
        /* [in] */ Boolean aPreventPanning);

//native function
public:
    // Tell the Gecko event loop that an event is available.
    static Void NotifyGeckoOfEvent(
        /* [in] */ GeckoEvent* event);

    /* The Android-side API: API methods that Android calls */

    // Initialization methods
    static Void NativeInit();

    static Void NativeRun(
        /* [in] */ const String& args);

    // helper methods
    static Void SetSurfaceView(
        /* [in] */ CGeckoSurfaceView* pSurfaceView);

    static Void PutEnv(
        /* [in] */ const String& map);

    static Void OnResume();

    static Void OnLowMemory();

    static Void CallObserver(
        /* [in] */ const String& observerKey,
        /* [in] */ const String& topic,
        /* [in] */ const String& data);

    static Void RemoveObserver(
        /* [in] */ const String& observerKey);

    static Void LoadGeckoLibsNative(
        /* [in] */ const String& apkName);

    static Void LoadSQLiteLibsNative(
        /* [in] */ const String& apkName);

    static Void LoadNSSLibsNative(
        /* [in] */ const String& apkName);

    static Void OnChangeNetworkLinkStatus(
        /* [in] */ const String& status);

    static Void ReportJavaCrash(
        /* [in] */ const String& stack);

    static Void ProcessNextNativeEvent();

    static Void NotifyBatteryChange(
        /* [in] */ Double aLevel,
        /* [in] */ Boolean aCharging,
        /* [in] */ Double aRemainingTime);

    static Void NotifySmsReceived(
        /* [in] */ const String& aSender,
        /* [in] */ const String& aBody,
        /* [in] */ Int32 aTimestamp);

    static Int32  SaveMessageInSentbox(
        /* [in] */ const String& aReceiver,
        /* [in] */ const String& aBody,
        /* [in] */ Int64 aTimestamp);

    static Void NotifySmsSent(
        /* [in] */ Int32 aId,
        /* [in] */ const String& aReceiver,
        /* [in] */ const String& aBody,
        /* [in] */ Int32 aTimestamp,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int32 aProcessId);

    static Void NotifySmsDelivered(
        /* [in] */ Int32 aId,
        /* [in] */ const String& aReceiver,
        /* [in] */ const String& aBody,
        /* [in] */ Int32 aTimestamp);

    static Void NotifySmsSendFailed(
        /* [in] */ Int32 aError,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int64 aProcessId);

    static Void NotifyGetSms(
        /* [in] */ Int32 aId,
        /* [in] */ const String& aReceiver,
        /* [in] */ const String& aSender,
        /* [in] */ const String& aBody,
        /* [in] */ Int32 aTimestamp,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int32 aProcessId);

    static Void NotifyGetSmsFailed(
        /* [in] */ Int32 aError,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int32 aProcessId);

    static Void NotifySmsDeleted(
        /* [in] */ Boolean aDeleted,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int32 aProcessId);

    static Void NotifySmsDeleteFailed(
        /* [in] */ Int32 aError,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int32 aProcessId);

    static Void NotifyNoMessageInList(
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int32 aProcessId);

    static Void NotifyListCreated(
        /* [in] */ Int32 aListId,
        /* [in] */ Int32 aMessageId,
        /* [in] */ const String& aReceiver,
        /* [in] */ const String& aSender,
        /* [in] */ const String& aBody,
        /* [in] */ Int32 aTimestamp,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int32 aProcessId);

    static Void NotifyGotNextMessage(
        /* [in] */ Int32 aMessageId,
        /* [in] */ const String& aReceiver,
        /* [in] */ const String& aSender,
        /* [in] */ const String& aBody,
        /* [in] */ Int32 aTimestamp,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int32 aProcessId);

    static Void NotifyReadingMessageListFailed(
        /* [in] */ Int32 aError,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int32 aProcessId);

class GeckoProcessesVisitor {
    virtual Boolean Callback(
        /* [in] */ Int32 pid) = 0;
};

private:
    // static members only

    static ECode PutLocaleEnv();

    static ECode SendPendingEventsToGecko();

    static ECode EnumerateGeckoProcesses(
        /* [in] */ GeckoProcessesVisitor* pVisiter);

    static ECode GetDrawableForExtension(
        /* [in] */ IPackageManager* pPm,
        /* [in] */ const String& aExt,
        /* [out] */ IDrawable** ppDrawable);

    static ECode Vibrator(
        /* [out] */ IVibrator** vibrator);

    static void CopyFile(const String& srcStr, const String& targetStr);

public:
    static Int32 sPidColumn;
    static Int32 sUserColumn;
    static AutoPtr<ICamera> sCamera;
    static Int32 sKPreferedFps;
    static AutoPtr<ArrayOf<Byte> > sCameraBuffer;
    static String sClipboardText;
    static pthread_cond_t mQueryResult;
    static pthread_mutex_t mQueryMutex;

private:
    static List< AutoPtr<GeckoEvent> > gPendingEvents;

    static Boolean gRestartScheduled;
    static Object sSyncCloseCameraMet;
    //AutoPtr< ISynchronousQueue<IDate> > sTracerQueue;

    static AutoPtr<ITimer> mIMETimer;
    static HashMap<Int32, AutoPtr<IAlertNotification> > mAlertNotifications;

#define NOTIFY_IME_RESETINPUTSTATE      0
#define NOTIFY_IME_SETOPENSTATE         1
#define NOTIFY_IME_CANCELCOMPOSITION    2
#define NOTIFY_IME_FOCUSCHANGE          3

    static const Int32 kLibFreeSpaceBuffer = 20971520L; // 29MB
    static AutoPtr<IFile> sCacheFile;
    static Int32 sFreeSpace;
    static AutoPtr<IHandler> sHandler;
    static GeckoEvent* mLastDrawEvent;
    //static AutoPtr<ICountDownLatch> sGeckoPendingAcks;
    //static AutoPtr<ISensor> gAccelerometerSensor;
    //static AutoPtr<ISensor> gOrientationSensor;
    //static SynchronousQueue<String> sClipboardQueue;

    static Object mGeckoAppShell;

    friend class IMEStateUpdater;
};

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__GECKOAPPSHELL_H__

