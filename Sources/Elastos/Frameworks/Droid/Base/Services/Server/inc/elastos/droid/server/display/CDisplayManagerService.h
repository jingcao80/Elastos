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

#ifndef __ELASTOS_DROID_SERVER_DISPLAY_CDISPLAYMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_CDISPLAYMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_Display_CDisplayManagerService.h"
#include <Elastos.Droid.Hardware.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/display/DisplayAdapter.h"
#include "elastos/droid/server/display/DisplayDevice.h"
#include "elastos/droid/server/display/DisplayPowerController.h"
#include "elastos/droid/server/display/VirtualDisplayAdapter.h"
#include "elastos/droid/server/display/WifiDisplayAdapter.h"
#include "elastos/droid/server/display/LogicalDisplay.h"
#include "elastos/droid/server/display/PersistentDataStore.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::View::IWindowManagerInternal;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Hardware::Input::IInputManagerInternal;
using Elastos::Droid::Hardware::Display::IDisplayTransactionListener;
using Elastos::Droid::Hardware::Display::IDisplayPowerRequest;
using Elastos::Droid::Hardware::Display::IDisplayPowerCallbacks;
using Elastos::Droid::Hardware::Display::IIVirtualDisplayCallback;
using Elastos::Droid::Hardware::Display::IIDisplayManagerCallback;
using Elastos::Droid::Hardware::Display::IIDisplayManager;
using Elastos::Droid::Media::Projection::IIMediaProjection;
using Elastos::Droid::Media::Projection::IIMediaProjectionManager;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Concurrent::ICopyOnWriteArrayList;
using Elastos::Utility::Etl::HashMap;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

/**
 * Manages attached displays.
 * <p>
 * The {@link DisplayManagerService} manages the global lifecycle of displays,
 * decides how to configure logical displays based on the physical display devices currently
 * attached, sends notifications to the system and to applications when the state
 * changes, and so on.
 * </p><p>
 * The display manager service relies on a collection of {@link DisplayAdapter} components,
 * for discovering and configuring physical display devices attached to the system.
 * There are separate display adapters for each manner that devices are attached:
 * one display adapter for built-in local displays, one for simulated non-functional
 * displays when the system is headless, one for simulated overlay displays used for
 * development, one for wifi displays, etc.
 * </p><p>
 * Display adapters are only weakly coupled to the display manager service.
 * Display adapters communicate changes in display device state to the display manager
 * service asynchronously via a {@link DisplayAdapter.Listener} registered
 * by the display manager service.  This separation of concerns is important for
 * two main reasons.  First, it neatly encapsulates the responsibilities of these
 * two classes: display adapters handle individual display devices whereas
 * the display manager service handles the global state.  Second, it eliminates
 * the potential for deadlocks resulting from asynchronous display device discovery.
 * </p>
 *
 * <h3>Synchronization</h3>
 * <p>
 * Because the display manager may be accessed by multiple threads, the synchronization
 * story gets a little complicated.  In particular, the window manager may call into
 * the display manager while holding a surface transaction with the expectation that
 * it can apply changes immediately.  Unfortunately, that means we can't just do
 * everything asynchronously (*grump*).
 * </p><p>
 * To make this work, all of the objects that belong to the display manager must
 * use the same lock.  We call this lock the synchronization root and it has a unique
 * type {@link DisplayManagerService.SyncRoot}.  Methods that require this lock are
 * named with the "Locked" suffix.
 * </p><p>
 * Where things get tricky is that the display manager is not allowed to make
 * any potentially reentrant calls, especially into the window manager.  We generally
 * avoid this by making all potentially reentrant out-calls asynchronous.
 * </p>
 */
CarClass(CDisplayManagerService)
    , public SystemService
{
public:
    /**
     * This is the object that everything in the display manager locks on.
     * We make it an inner class within the {@link DisplayManagerService} to so that it is
     * clear that the object belongs to the display manager service and that it is
     * a unique object with a special purpose.
     */
    class SyncRoot : public Object
    {
    public:
        CARAPI ToString(
            /* [out] */ String* str)
        {
            VALIDATE_NOT_NULL(str)
            *str = "CDisplayManagerService::SyncRoot";
            return NOERROR;
        }
    };

    class BinderService
        : public Object
        , public IIDisplayManager
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ISystemService* displayManagerService);

        CARAPI ToString(
            /* [out] */ String* str);

        /**
         * Returns information about the specified logical display.
         *
         * @param displayId The logical display id.
         * @return The logical display info, or null if the display does not exist.  The
         * returned object must be treated as immutable.
         */
        // @Override // Binder call
        CARAPI GetDisplayInfo(
            /* [in] */ Int32 displayId,
            /* [out] */ IDisplayInfo** info);

        /**
         * Returns the list of all display ids.
         */
        // @Override // Binder call
        CARAPI GetDisplayIds(
            /* [out, callee] */ ArrayOf<Int32>** ids);

        //@Override // Binder call
        CARAPI RegisterCallback(
            /* [in] */ IIDisplayManagerCallback* callback);

        //@Override // Binder call
        CARAPI StartWifiDisplayScan();

        //@Override // Binder call
        CARAPI StopWifiDisplayScan();

        //@Override // Binder call
        CARAPI ConnectWifiDisplay(
            /* [in] */ const String& address);

        //@Override // Binder call
        CARAPI DisconnectWifiDisplay();

        //@Override // Binder call
        CARAPI RenameWifiDisplay(
            /* [in] */ const String& address,
            /* [in] */ const String& alias);

        //@Override // Binder call
        CARAPI ForgetWifiDisplay(
            /* [in] */ const String& address);

        //@Override // Binder call
        CARAPI PauseWifiDisplay();

        //@Override // Binder call
        CARAPI ResumeWifiDisplay();

        //@Override // Binder call
        CARAPI GetWifiDisplayStatus(
            /* [out] */ IWifiDisplayStatus** status);

        //@Override // Binder call
        CARAPI CreateVirtualDisplay(
            /* [in] */ IIVirtualDisplayCallback* callback,
            /* [in] */ IIMediaProjection* projection,
            /* [in] */ const String& packageName,
            /* [in] */ const String& name,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 densityDpi,
            /* [in] */ ISurface* surface,
            /* [in] */ Int32 flags,
            /* [out] */ Int32* result);

        //@Override // Binder call
        CARAPI ResizeVirtualDisplay(
            /* [in] */ IIVirtualDisplayCallback* callback,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 densityDpi);

        //@Override // Binder call
        CARAPI SetVirtualDisplaySurface(
            /* [in] */ IIVirtualDisplayCallback* callback,
            /* [in] */ ISurface* surface);

        //@Override // Binder call
        CARAPI ReleaseVirtualDisplay(
            /* [in] */ IIVirtualDisplayCallback* callback);

        //@Override // Binder call
        CARAPI Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* pw,
            /* [in] */ ArrayOf<String>* args);

    private:
        Boolean ValidatePackageName(
            /* [in] */ Int32 uid,
            /* [in] */ const String& packageName);

        Boolean CanProjectVideo(
            /* [in] */ IIMediaProjection* projection);

        Boolean CanProjectSecureVideo(
            /* [in] */ IIMediaProjection* projection);
    private:
        CDisplayManagerService* mService;
    };

    class LocalService
        : public Object
        , public IDisplayManagerInternal
    {
    public:
        CAR_INTERFACE_DECL()

        LocalService(
            /* [in] */ CDisplayManagerService* host);

        // @Override
        CARAPI InitPowerManagement(
            /* [in] */ IDisplayPowerCallbacks* callbacks,
            /* [in] */ IHandler* handler,
            /* [in] */ ISensorManager* sensorManager);

        // @Override
        CARAPI RequestPowerState(
            /* [in] */ IDisplayPowerRequest* request,
            /* [in] */ Boolean waitForNegativeProximity,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI IsProximitySensorAvailable(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetDisplayInfo(
            /* [in] */ Int32 displayId,
            /* [out] */ IDisplayInfo** result);

        // @Override
        CARAPI RegisterDisplayTransactionListener(
            /* [in] */ IDisplayTransactionListener* listener);

        // @Override
        CARAPI UnregisterDisplayTransactionListener(
            /* [in] */ IDisplayTransactionListener* listener);

        // @Override
        CARAPI SetDisplayInfoOverrideFromWindowManager(
            /* [in] */ Int32 displayId,
            /* [in] */ IDisplayInfo* info);

        // @Override
        CARAPI PerformTraversalInTransactionFromWindowManager();

        // @Override
        CARAPI SetDisplayProperties(
            /* [in] */ Int32 displayId,
            /* [in] */ Boolean hasContent,
            /* [in] */ Float requestedRefreshRate,
            /* [in] */ Boolean inTraversal);
    private:
        CDisplayManagerService* mHost;
    };

    class DisplayManagerHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CDisplayManagerService::DisplayManagerHandler")

        DisplayManagerHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CDisplayManagerService* host);

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        static String CodeToString(
            /* [in] */ Int32 what);
    private:
        CDisplayManagerService* mHost;
    };

    class DisplayAdapterListener
        : public Object
        , public IDisplayAdapterListener
    {
    public:
        CAR_INTERFACE_DECL()

        DisplayAdapterListener(
            /* [in] */ CDisplayManagerService* owner);

        //@Override
        CARAPI OnDisplayDeviceEvent(
            /* [in] */ HANDLE device,
            /* [in] */ Int32 event);

        //@Override
        CARAPI OnTraversalRequested();

    private:
        CDisplayManagerService* mHost;
    };

    class CallbackRecord
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        CallbackRecord(
            /* [in] */ CDisplayManagerService* owner,
            /* [in] */ Int32 pid,
            /* [in] */ IIDisplayManagerCallback* callback);

        ~CallbackRecord();

        //@Override
        CARAPI ProxyDied();

        CARAPI_(void) NotifyDisplayEventAsync(
            /* [in] */ Int32 displayId,
            /* [in] */ Int32 event);
    public:
        Int32 mPid;
        Boolean mWifiDisplayScanRequested;
    private:
        CDisplayManagerService* mHost;
        AutoPtr<IIDisplayManagerCallback> mCallback;
    };

    class DisplayBlanker
        : public Object
        , public IDisplayBlanker
    {
    public:
        CAR_INTERFACE_DECL()

        DisplayBlanker(
            /* [in] */ CDisplayManagerService* host,
            /* [in] */ IDisplayPowerCallbacks* callbacks);

        // @Override
        CARAPI RequestDisplayState(
            /* [in] */ Int32 state);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        CDisplayManagerService* mHost;
        AutoPtr<IDisplayPowerCallbacks> mCallbacks;
    };

public:
    CAR_OBJECT_DECL()

    CDisplayManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

    // TODO: Use dependencies or a boot phase
    CARAPI WindowManagerAndInputReady();

    /**
     * Called when the system is ready to go.
     */
    CARAPI SystemReady(
        /* [in] */ Boolean safeMode,
        /* [in] */ Boolean onlyCore);

private:
    CARAPI RegisterDisplayTransactionListenerInternal(
        /* [in] */ IDisplayTransactionListener* listener);

    CARAPI UnregisterDisplayTransactionListenerInternal(
        /* [in] */ IDisplayTransactionListener* listener);

    CARAPI SetDisplayInfoOverrideFromWindowManagerInternal(
        /* [in] */ Int32 displayId,
        /* [in] */ IDisplayInfo* info);

    CARAPI PerformTraversalInTransactionFromWindowManagerInternal();

    CARAPI RequestGlobalDisplayStateInternal(
        /* [in] */ Int32 state);

    // Binder call
    CARAPI GetDisplayInfoInternal(
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 callingUid,
        /* [out] */ IDisplayInfo** displayInfo);

    // Binder call
    CARAPI GetDisplayIdsInternal(
        /* [in] */ Int32 callingUid,
        /* [out, callee] */ ArrayOf<Int32>** displayIds);

    // Binder call
    CARAPI RegisterCallbackInternal(
        /* [in] */ IIDisplayManagerCallback* callback,
        /* [in] */ Int32 callingUid);

    CARAPI_(void) OnCallbackDied(
        /* [in] */ CallbackRecord* record);

    CARAPI StartWifiDisplayScanInternal(
        /* [in] */ Int32 callingPid);

    CARAPI StartWifiDisplayScanLocked(
        /* [in] */ CallbackRecord* record);

    CARAPI StopWifiDisplayScanInternal(
        /* [in] */ Int32 callingPid);

    CARAPI StopWifiDisplayScanLocked(
        /* [in] */ CallbackRecord* record);

    CARAPI ConnectWifiDisplayInternal(
        /* [in] */ const String& address);

    CARAPI PauseWifiDisplayInternal();

    CARAPI ResumeWifiDisplayInternal();

    CARAPI DisconnectWifiDisplayInternal();

    CARAPI RenameWifiDisplayInternal(
        /* [in] */ const String& address,
        /* [in] */ const String& alias);

    CARAPI ForgetWifiDisplayInternal(
        /* [in] */ const String& address);

    // Binder call
    CARAPI GetWifiDisplayStatusInternal(
        /* [out] */ IWifiDisplayStatus** status);

    Int32 CreateVirtualDisplayInternal(
        /* [in] */ IIVirtualDisplayCallback* callback,
        /* [in] */ IIMediaProjection* projection,
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& packageName,
        /* [in] */ const String& name,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 densityDpi,
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 flags);

    CARAPI ResizeVirtualDisplayInternal(
        /* [in] */ IBinder* appToken,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 densityDpi);

    CARAPI SetVirtualDisplaySurfaceInternal(
        /* [in] */ IBinder* appToken,
        /* [in] */ ISurface* surface);

    CARAPI ReleaseVirtualDisplayInternal(
        /* [in] */ IBinder* appToken);

    CARAPI RegisterDefaultDisplayAdapter();

    CARAPI RegisterAdditionalDisplayAdapters();

    CARAPI RegisterOverlayDisplayAdapterLocked();

    CARAPI RegisterWifiDisplayAdapterLocked();

    CARAPI RegisterVirtualDisplayAdapterLocked();

    CARAPI_(Boolean) ShouldRegisterNonEssentialDisplayAdaptersLocked();

    CARAPI_(void) RegisterDisplayAdapterLocked(
        /* [in] */ DisplayAdapter* adapter);

    CARAPI_(void) HandleDisplayDeviceAdded(
       /* [in] */ DisplayDevice* device);

    CARAPI_(void) HandleDisplayDeviceAddedLocked(
       /* [in] */ DisplayDevice* device);

    CARAPI_(void) HandleDisplayDeviceChanged(
        /* [in] */ DisplayDevice* device);

    CARAPI_(void) HandleDisplayDeviceRemoved(
        /* [in] */ DisplayDevice* device);

    CARAPI_(void) HandleDisplayDeviceRemovedLocked(
        /* [in] */ DisplayDevice* device);

    void UpdateGlobalDisplayStateLocked(
        /* [in] */ IList* workQueue);

    AutoPtr<IRunnable> UpdateDisplayStateLocked(
        /* [in] */ DisplayDevice* device);

    // Adds a new logical display based on the given display device.
    // Sends notifications if needed.
    CARAPI_(void) AddLogicalDisplayLocked(
        /* [in] */ DisplayDevice* device);

    CARAPI_(Int32) AssignDisplayIdLocked(
        /* [in] */ Boolean isDefault);

    CARAPI_(Int32) AssignLayerStackLocked(
        /* [in] */ Int32 displayId);

    // Updates all existing logical displays given the current set of display devices.
    // Removes invalid logical displays.
    // Sends notifications if needed.
    CARAPI_(Boolean) UpdateLogicalDisplaysLocked();

    CARAPI_(void) PerformTraversalInTransactionLocked();

    CARAPI_(void) SetDisplayPropertiesInternal(
        /* [in] */ Int32 displayId,
        /* [in] */ Boolean hasContent,
        /* [in] */ Float requestedRefreshRate,
        /* [in] */ Boolean inTraversal);

    CARAPI_(void) ClearViewportsLocked();

    CARAPI_(void) ConfigureDisplayInTransactionLocked(
        /* [in] */ DisplayDevice* device);

    static CARAPI_(void) SetViewportLocked(
        /* [in] */ IDisplayViewport* viewport,
        /* [in] */ LogicalDisplay* display,
        /* [in] */ DisplayDevice* device);

    CARAPI_(AutoPtr<LogicalDisplay>) FindLogicalDisplayForDeviceLocked(
        /* [in] */ DisplayDevice* device);

    CARAPI_(void) SendDisplayEventLocked(
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 event);

    // Requests that performTraversalsInTransactionFromWindowManager be called at a
    // later time to apply changes to surfaces and displays.
    CARAPI_(void) ScheduleTraversalLocked(
        /* [in] */ Boolean inTraversal);

    AutoPtr<IIMediaProjectionManager> GetProjectionService();

    // Binder call
    CARAPI DumpInternal(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    // Runs on Handler thread.
    // Delivers display event notifications to callbacks.
    CARAPI_(void) DeliverDisplayEvent(
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 event);

public:
    // True if in safe mode.
    // This option may disable certain display adapters.
    Boolean mSafeMode;

    // True if we are in a special boot mode where only core applications and
    // services should be started.  This option may disable certain display adapters.
    Boolean mOnlyCore;

    // All callback records indexed by calling process id.
    HashMap<Int32, AutoPtr<CallbackRecord> > mCallbacks;

private:
    static const String TAG;
    static const Boolean DEBUG;

    // When this system property is set to 0, WFD is forcibly disabled on boot.
    // When this system property is set to 1, WFD is forcibly enabled on boot.
    // Otherwise WFD is enabled according to the value of config_enableWifiDisplay.
    static const String FORCE_WIFI_DISPLAY_ENABLE;

    static const String SYSTEM_HEADLESS;
    static const Int64 WAIT_FOR_DEFAULT_DISPLAY_TIMEOUT = 10000;

    static const Int32 MSG_REGISTER_DEFAULT_DISPLAY_ADAPTER = 1;
    static const Int32 MSG_REGISTER_ADDITIONAL_DISPLAY_ADAPTERS = 2;
    static const Int32 MSG_DELIVER_DISPLAY_EVENT = 3;
    static const Int32 MSG_REQUEST_TRAVERSAL = 4;
    static const Int32 MSG_UPDATE_VIEWPORT = 5;

    AutoPtr<IContext> mContext;
    AutoPtr<DisplayManagerHandler> mHandler;
    AutoPtr<IHandler> mUiHandler;
    AutoPtr<DisplayAdapterListener> mDisplayAdapterListener;
    AutoPtr<IWindowManagerInternal> mWindowManagerInternal;
    AutoPtr<IInputManagerInternal> mInputManagerInternal;
    AutoPtr<IIMediaProjectionManager> mProjectionService;

    // NOTE: This is synchronized on while holding WindowManagerService.mWindowMap so never call
    // into WindowManagerService methods that require mWindowMap while holding this unless you are
    // very very sure that no deadlock can occur.
    AutoPtr<SyncRoot> mSyncRoot;

    // True if the display manager service should pretend there is only one display
    // and only tell applications about the existence of the default logical display.
    // The display manager can still mirror content to secondary displays but applications
    // cannot present unique content on those displays.
    // Used for demonstration purposes only.
    Boolean mSingleDisplayDemoMode;

    // List of all currently registered display adapters.
    List< AutoPtr<DisplayAdapter> > mDisplayAdapters;

    // List of all currently connected display devices.
    List< AutoPtr<DisplayDevice> > mDisplayDevices;

    // List of all logical displays indexed by logical display id.
    HashMap<Int32, AutoPtr<LogicalDisplay> > mLogicalDisplays;
    Int32 mNextNonDefaultDisplayId;

    // List of all display transaction listeners.
    AutoPtr<ICopyOnWriteArrayList> mDisplayTransactionListeners;//new CopyOnWriteArrayList<DisplayTransactionListener>();

    // Display power controller.
    AutoPtr<DisplayPowerController> mDisplayPowerController;

    // The overall display state, independent of changes that might influence one
    // display or another in particular.
    Int32 mGlobalDisplayState;

    // Set to TRUE when there are pending display changes that have yet to be applied
    // to the surface flinger state.
    Boolean mPendingTraversal;

    // The Wifi display adapter, or NULL if not registered.
    AutoPtr<WifiDisplayAdapter> mWifiDisplayAdapter;

    // The number of active wifi display scan requests.
    Int32 mWifiDisplayScanRequestCount;

    // The virtual display adapter, or null if not registered.
    AutoPtr<VirtualDisplayAdapter> mVirtualDisplayAdapter;

    // Viewports of the default display and the display that should receive touch
    // input from an external source.  Used by the input system.
    AutoPtr<IDisplayViewport> mDefaultViewport;
    AutoPtr<IDisplayViewport> mExternalTouchViewport;

    // Persistent data store for all internal settings maintained by the display manager service.
    AutoPtr<PersistentDataStore> mPersistentDataStore;

    // Temporary callback list, used when sending display events to applications.
    // May be used outside of the lock but only on the handler thread.
    List< AutoPtr<CallbackRecord> > mTempCallbacks;

    // Temporary display info, used for comparing display configurations.
    AutoPtr<IDisplayInfo> mTempDisplayInfo;

    // Temporary viewports, used when sending new viewport information to the
    // input system.  May be used outside of the lock but only on the handler thread.
    AutoPtr<IDisplayViewport> mTempDefaultViewport;
    AutoPtr<IDisplayViewport> mTempExternalTouchViewport;

    // Temporary list of deferred work to perform when setting the display state.
    // Only used by requestDisplayState.  The field is self-synchronized and only
    // intended for use inside of the requestGlobalDisplayStateInternal function.
    AutoPtr<IArrayList> mTempDisplayStateWorkQueue;// = new ArrayList<Runnable>();

};

} // namespace Display
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_DISPLAY_CDISPLAYMANAGERSERVICE_H__
