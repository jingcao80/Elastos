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

#ifndef __ELASTOS_DROID_SERVER_TV_TVINPUTMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_TV_TVINPUTMANAGERSERVICE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/display/PersistentDataStore.h"
#include "elastos/droid/server/tv/PersistentDataStore.h"
#include "elastos/droid/server/tv/TvInputHardwareManager.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Media.h>
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/content/PackageMonitor.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::Media::Tv::IITvInputSessionCallback;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Media::Tv::IITvInputClient;
using Elastos::Droid::Media::Tv::IITvInputHardware;
using Elastos::Droid::Media::Tv::IITvInputHardwareCallback;
using Elastos::Droid::Media::Tv::IITvInputManager;
using Elastos::Droid::Media::Tv::IITvInputManagerCallback;
using Elastos::Droid::Media::Tv::IITvInputService;
using Elastos::Droid::Media::Tv::IITvInputServiceCallback;
using Elastos::Droid::Media::Tv::IITvInputSession;
using Elastos::Droid::Media::Tv::ITvInputHardwareInfo;
using Elastos::Droid::Media::Tv::ITvInputInfo;
using Elastos::Droid::Media::Tv::ITvStreamConfig;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Server::Display::PersistentDataStore;
using Elastos::Droid::Server::SystemService;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::ISurface;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Tv {

/** This class provides a system service that manages television inputs. */
class TvInputManagerService
    : public SystemService
{
private:
    class BinderService
        : public Object
        , public IITvInputManager
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        BinderService(
            /* [in] */ TvInputManagerService* host);

        // @Override
        CARAPI GetTvInputList(
            /* [in] */ Int32 userId,
            /* [out] */ IList** result);

        // @Override
        CARAPI GetTvInputInfo(
            /* [in] */ const String& inputId,
            /* [in] */ Int32 userId,
            /* [out] */ ITvInputInfo** result);

        // @Override
        CARAPI GetTvContentRatingSystemList(
            /* [in] */ Int32 userId,
            /* [out] */ IList** result);

        // @Override
        CARAPI RegisterCallback(
            /* [in] */ IITvInputManagerCallback* callback,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI UnregisterCallback(
            /* [in] */ IITvInputManagerCallback* callback,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI IsParentalControlsEnabled(
            /* [in] */ Int32 userId,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI SetParentalControlsEnabled(
            /* [in] */ Boolean enabled,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI IsRatingBlocked(
            /* [in] */ const String& rating,
            /* [in] */ Int32 userId,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetBlockedRatings(
            /* [in] */ Int32 userId,
            /* [out] */ IList** result);

        // @Override
        CARAPI AddBlockedRating(
            /* [in] */ const String& rating,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI RemoveBlockedRating(
            /* [in] */ const String& rating,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI CreateSession(
            /* [in] */ IITvInputClient* client,
            /* [in] */ const String& inputId,
            /* [in] */ Int32 seq,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI ReleaseSession(
            /* [in] */ IBinder* sessionToken,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI SetMainSession(
            /* [in] */ IBinder* sessionToken,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI SetSurface(
            /* [in] */ IBinder* sessionToken,
            /* [in] */ ISurface* surface,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI DispatchSurfaceChanged(
            /* [in] */ IBinder* sessionToken,
            /* [in] */ Int32 format,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI SetVolume(
            /* [in] */ IBinder* sessionToken,
            /* [in] */ Float volume,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI Tune(
            /* [in] */ IBinder* sessionToken,
            /* [in] */ IUri* channelUri,
            /* [in] */ IBundle* params,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI RequestUnblockContent(
            /* [in] */ IBinder* sessionToken,
            /* [in] */ const String& unblockedRating,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI SetCaptionEnabled(
            /* [in] */ IBinder* sessionToken,
            /* [in] */ Boolean enabled,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI SelectTrack(
            /* [in] */ IBinder* sessionToken,
            /* [in] */ Int32 type,
            /* [in] */ const String& trackId,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI SendAppPrivateCommand(
            /* [in] */ IBinder* sessionToken,
            /* [in] */ const String& command,
            /* [in] */ IBundle* data,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI CreateOverlayView(
            /* [in] */ IBinder* sessionToken,
            /* [in] */ IBinder* windowToken,
            /* [in] */ IRect* frame,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI RelayoutOverlayView(
            /* [in] */ IBinder* sessionToken,
            /* [in] */ IRect* frame,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI RemoveOverlayView(
            /* [in] */ IBinder* sessionToken,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI GetHardwareList(
            /* [out] */ IList** result);

        // @Override
        CARAPI AcquireTvInputHardware(
            /* [in] */ Int32 deviceId,
            /* [in] */ IITvInputHardwareCallback* callback,
            /* [in] */ ITvInputInfo* info,
            /* [in] */ Int32 userId,
            /* [out] */ IITvInputHardware** result);

        // @Override
        CARAPI ReleaseTvInputHardware(
            /* [in] */ Int32 deviceId,
            /* [in] */ IITvInputHardware* hardware,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI GetAvailableTvStreamConfigList(
            /* [in] */ const String& inputId,
            /* [in] */ Int32 userId,
            /* [out] */ IList** result);

        // @Override
        CARAPI CaptureFrame(
            /* [in] */ const String& inputId,
            /* [in] */ ISurface* surface,
            /* [in] */ ITvStreamConfig* config,
            /* [in] */ Int32 userId,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI IsSingleSessionActive(
            /* [in] */ Int32 userId,
            /* [out] */ Boolean* result);

        // @Override
        // @SuppressWarnings("resource")
        CARAPI Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* writer,
            /* [in] */ ArrayOf<String>* args);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        CARAPI EnsureParentalControlsPermission();

    private:
        TvInputManagerService* mHost;
    };

    class ClientState;
    class SessionState;
    class InputServiceConnection;
    class SessionCallback;
    class HardwareListener;
    class InnerSub_ProxyDeathRecipient;
    class InnerSub_PackageMonitor;
    class UserState
        : public Object
    {
        friend class BinderService;
        friend class ClientState;
        friend class SessionState;
        friend class InputServiceConnection;
        friend class SessionCallback;
        friend class HardwareListener;
        friend class InnerSub_ProxyDeathRecipient;
        friend class InnerSub_PackageMonitor;
        friend class TvInputManagerService;

    public:
        UserState(
            /* [in] */ IContext* context,
            /* [in] */ Int32 userId);

    private:
        // The token of a "main" TV input session.
        AutoPtr<IBinder> mMainSessionToken;

        // service.
        AutoPtr<PersistentDataStore> mPersistentDataStore;

        // A mapping from the TV input id to its TvInputState.
        AutoPtr<IMap> mInputMap;

        // A set of all TV input packages.
        AutoPtr<ISet> mPackageSet;

        // A list of all TV content rating systems defined.
        AutoPtr<IList> mContentRatingSystemList;

        // A mapping from the token of a client to its state.
        AutoPtr<IMap> mClientStateMap;

        // A mapping from the name of a TV input service to its state.
        AutoPtr<IMap> mServiceStateMap;

        // A mapping from the token of a TV input session to its state.
        AutoPtr<IMap> mSessionStateMap;

        // A set of callbacks.
        AutoPtr<ISet> mCallbackSet;
    };

    class ClientState
        : public Object
        , public IProxyDeathRecipient
    {
        friend class BinderService;
        friend class SessionCallback;
        friend class TvInputManagerService;

    public:
        CAR_INTERFACE_DECL()

        ClientState(
            /* [in] */ TvInputManagerService* host);

        CARAPI constructor(
            /* [in] */ IBinder* clientToken,
            /* [in] */ Int32 userId);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI ProxyDied();

    private:
        TvInputManagerService* mHost;

        AutoPtr<IList> mSessionTokens;

        AutoPtr<IBinder> mClientToken;

        Int32 mUserId;
    };

    class ServiceCallback;
    class ServiceState
        : public Object
    {
        friend class BinderService;
        friend class InputServiceConnection;
        friend class ServiceCallback;
        friend class HardwareListener;
        friend class TvInputManagerService;

    private:
        ServiceState(
            /* [in] */ TvInputManagerService* host,
            /* [in] */ IComponentName* component,
            /* [in] */ Int32 userId);

    private:
        TvInputManagerService* mHost;

        AutoPtr<IList> mSessionTokens;

        AutoPtr<IServiceConnection> mConnection;

        AutoPtr<IComponentName> mComponent;

        Boolean mIsHardware;

        AutoPtr<IList> mInputList;

        AutoPtr<IITvInputService> mService;

        AutoPtr<ServiceCallback> mCallback;

        Boolean mBound;

        Boolean mReconnecting;
    };

    class TvInputState
        : public Object
    {
        friend class BinderService;
        friend class InputServiceConnection;
        friend class TvInputManagerService;

    public:
        TvInputState();

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    private:
        // A TvInputInfo object which represents the TV input.
        AutoPtr<ITvInputInfo> mInfo;

        // The state of TV input. Connected by default.
        Int32 mState;
    };

    class SessionState
        : public Object
        , public IProxyDeathRecipient
    {
        friend class BinderService;
        friend class SessionCallback;
        friend class TvInputManagerService;

    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI ProxyDied();

    private:
        SessionState(
            /* [in] */ TvInputManagerService* host,
            /* [in] */ IBinder* sessionToken,
            /* [in] */ ITvInputInfo* info,
            /* [in] */ IITvInputClient* client,
            /* [in] */ Int32 seq,
            /* [in] */ Int32 callingUid,
            /* [in] */ Int32 userId);

    private:
        TvInputManagerService* mHost;

        AutoPtr<ITvInputInfo> mInfo;

        AutoPtr<IITvInputClient> mClient;

        Int32 mSeq;

        Int32 mCallingUid;

        Int32 mUserId;

        AutoPtr<IBinder> mSessionToken;

        AutoPtr<IITvInputSession> mSession;

        AutoPtr<IUri> mLogUri;

        // Not null if this session represents an external device connected to a hardware TV input.
        AutoPtr<IBinder> mHardwareSessionToken;
    };

    class InputServiceConnection
        : public Object
        , public IServiceConnection
    {
        friend class ServiceState;

    public:
        CAR_INTERFACE_DECL()

        InputServiceConnection();

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* component,
            /* [in] */ IBinder* service);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* component);

    private:
        InputServiceConnection(
            /* [in] */ TvInputManagerService* host,
            /* [in] */ IComponentName* component,
            /* [in] */ Int32 userId);

    private:
        TvInputManagerService* mHost;

        AutoPtr<IComponentName> mComponent;

        Int32 mUserId;
    };

    class ServiceCallback
        : public Object
        , public IITvInputServiceCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        ServiceCallback(
            /* [in] */ TvInputManagerService* host);

        CARAPI constructor(
            /* [in] */ IComponentName* component,
            /* [in] */ Int32 userId);

        // @Override
        CARAPI AddHardwareTvInput(
            /* [in] */ Int32 deviceId,
            /* [in] */ ITvInputInfo* inputInfo);

        // @Override
        CARAPI AddHdmiTvInput(
            /* [in] */ Int32 id,
            /* [in] */ ITvInputInfo* inputInfo);

        // @Override
        CARAPI RemoveTvInput(
            /* [in] */ const String& inputId);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        CARAPI EnsureHardwarePermission();

        CARAPI EnsureValidInput(
            /* [in] */ ITvInputInfo* inputInfo);

        CARAPI AddTvInputLocked(
            /* [in] */ ITvInputInfo* inputInfo);

    private:
        TvInputManagerService* mHost;

        AutoPtr<IComponentName> mComponent;

        Int32 mUserId;
    };

    class SessionCallback
        : public Object
        , public IITvInputSessionCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        SessionCallback(
            /* [in] */ TvInputManagerService* host);

        CARAPI constructor(
            /* [in] */ SessionState* sessionState,
            /* [in] */ ArrayOf<IInputChannel*>* channels);

        // @Override
        CARAPI OnSessionCreated(
            /* [in] */ IITvInputSession* session,
            /* [in] */ IBinder* harewareSessionToken);

        // @Override
        CARAPI OnChannelRetuned(
            /* [in] */ IUri* channelUri);

        // @Override
        CARAPI OnTracksChanged(
            /* [in] */ IList* tracks);

        // @Override
        CARAPI OnTrackSelected(
            /* [in] */ Int32 type,
            /* [in] */ const String& trackId);

        // @Override
        CARAPI OnVideoAvailable();

        // @Override
        CARAPI OnVideoUnavailable(
            /* [in] */ Int32 reason);

        // @Override
        CARAPI OnContentAllowed();

        // @Override
        CARAPI OnContentBlocked(
            /* [in] */ const String& rating);

        // @Override
        CARAPI OnLayoutSurface(
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom);

        // @Override
        CARAPI OnSessionEvent(
            /* [in] */ const String& eventType,
            /* [in] */ IBundle* eventArgs);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        CARAPI AddSessionTokenToClientStateLocked(
            /* [in] */ IITvInputSession* session,
            /* [out] */ Boolean* result);

    private:
        TvInputManagerService* mHost;

        AutoPtr<SessionState> mSessionState;

        AutoPtr<ArrayOf<IInputChannel*> > mChannels;
    };

    class WatchLogHandler
        : public Handler
    {
        friend class BinderService;
        friend class TvInputManagerService;

    public:
        TO_STRING_IMPL("TvInputManagerService::WatchLogHandler")

        CARAPI constructor(
            /* [in] */ IContentResolver* contentResolver,
            /* [in] */ ILooper* looper);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(String) EncodeTuneParams(
            /* [in] */ IBundle* tuneParams);

        CARAPI_(String) ReplaceEscapeCharacters(
            /* [in] */ const String& src);

    private:
        // system service.
        static const Int32 MSG_LOG_WATCH_START;

        static const Int32 MSG_LOG_WATCH_END;

        AutoPtr<IContentResolver> mContentResolver;
    };

    class HardwareListener
        : public Object
        , public ITvInputHardwareManagerListener
    {
    public:
        CAR_INTERFACE_DECL()

        HardwareListener(
            /* [in] */ TvInputManagerService* host);

        // @Override
        CARAPI OnStateChanged(
            /* [in] */ const String& inputId,
            /* [in] */ Int32 state);

        // @Override
        CARAPI OnHardwareDeviceAdded(
            /* [in] */ ITvInputHardwareInfo* info);

        // @Override
        CARAPI OnHardwareDeviceRemoved(
            /* [in] */ ITvInputHardwareInfo* info);

        // @Override
        CARAPI OnHdmiDeviceAdded(
            /* [in] */ IHdmiDeviceInfo* deviceInfo);

        // @Override
        CARAPI OnHdmiDeviceRemoved(
            /* [in] */ IHdmiDeviceInfo* deviceInfo);

        // @Override
        CARAPI OnHdmiDeviceUpdated(
            /* [in] */ const String& inputId,
            /* [in] */ IHdmiDeviceInfo* deviceInfo);

    private:
        TvInputManagerService* mHost;
    };

    class InnerSub_ProxyDeathRecipient
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_ProxyDeathRecipient(
            /* [in] */ IITvInputManagerCallback* callback,
            /* [in] */ IObject* lock,
            /* [in] */ UserState* userState);

        // @Override
        CARAPI ProxyDied();

    private:
        AutoPtr<IITvInputManagerCallback> mCallback;
        AutoPtr<IObject> mLock;
        AutoPtr<UserState> mUserState;
    };

    class InnerSub_PackageMonitor
        : public PackageMonitor
    {
    public:
        InnerSub_PackageMonitor(
            /* [in] */ TvInputManagerService* host);

        // @Override
        CARAPI OnPackageUpdateFinished(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        // @Override
        CARAPI OnPackagesAvailable(
            /* [in] */ ArrayOf<String>* packages);

        // @Override
        CARAPI OnPackagesUnavailable(
            /* [in] */ ArrayOf<String>* packages);

        // @Override
        CARAPI OnSomePackagesChanged();

        // @Override
        CARAPI OnPackageRemoved(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

    private:
        CARAPI BuildTvInputList(
            /* [in] */ ArrayOf<String>* packages);

    private:
        TvInputManagerService* mHost;
    };

    class InnerSub_BroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("TvInputManagerService::InnerSub_BroadcastReceiver")

        InnerSub_BroadcastReceiver(
            /* [in] */ TvInputManagerService* host);

        // @Override
        ECode OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        TvInputManagerService* mHost;
    };

public:
    TvInputManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

private:
    CARAPI RegisterBroadcastReceivers();

    static CARAPI_(Boolean) HasHardwarePermission(
        /* [in] */ IPackageManager* pm,
        /* [in] */ IComponentName* component);

    CARAPI BuildTvInputListLocked(
        /* [in] */ Int32 userId,
        /* [in] */ ArrayOf<String>* updatedPackages);

    CARAPI BuildTvContentRatingSystemListLocked(
        /* [in] */ Int32 userId);

    CARAPI SwitchUser(
        /* [in] */ Int32 userId);

    CARAPI RemoveUser(
        /* [in] */ Int32 userId);

    CARAPI GetUserStateLocked(
        /* [in] */ Int32 userId,
        /* [out] */ UserState** result);

    CARAPI GetServiceStateLocked(
        /* [in] */ IComponentName* component,
        /* [in] */ Int32 userId,
        /* [out] */ ServiceState** result);

    CARAPI GetSessionStateLocked(
        /* [in] */ IBinder* sessionToken,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 userId,
        /* [out] */ SessionState** result);

    CARAPI GetSessionLocked(
        /* [in] */ IBinder* sessionToken,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 userId,
        /* [out] */ IITvInputSession** result);

    CARAPI GetSessionLocked(
        /* [in] */ SessionState* sessionState,
        /* [out] */ IITvInputSession** result);

    CARAPI ResolveCallingUserId(
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 requestedUserId,
        /* [in] */ const String& methodName,
        /* [out] */ Int32* result);

    static CARAPI_(Boolean) ShouldMaintainConnection(
        /* [in] */ ServiceState* serviceState);

    CARAPI UpdateServiceConnectionLocked(
        /* [in] */ IComponentName* component,
        /* [in] */ Int32 userId);

    CARAPI AbortPendingCreateSessionRequestsLocked(
        /* [in] */ ServiceState* serviceState,
        /* [in] */ const String& inputId,
        /* [in] */ Int32 userId);

    CARAPI CreateSessionInternalLocked(
        /* [in] */ IITvInputService* service,
        /* [in] */ IBinder* sessionToken,
        /* [in] */ Int32 userId);

    CARAPI SendSessionTokenToClientLocked(
        /* [in] */ IITvInputClient* client,
        /* [in] */ const String& inputId,
        /* [in] */ IBinder* sessionToken,
        /* [in] */ IInputChannel* channel,
        /* [in] */ Int32 seq);

    CARAPI ReleaseSessionLocked(
        /* [in] */ IBinder* sessionToken,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 userId);

    CARAPI RemoveSessionStateLocked(
        /* [in] */ IBinder* sessionToken,
        /* [in] */ Int32 userId);

    CARAPI SetMainLocked(
        /* [in] */ IBinder* sessionToken,
        /* [in] */ Boolean isMain,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 userId);

    CARAPI NotifyInputAddedLocked(
        /* [in] */ UserState* userState,
        /* [in] */ const String& inputId);

    CARAPI NotifyInputRemovedLocked(
        /* [in] */ UserState* userState,
        /* [in] */ const String& inputId);

    CARAPI NotifyInputUpdatedLocked(
        /* [in] */ UserState* userState,
        /* [in] */ const String& inputId);

    CARAPI NotifyInputStateChangedLocked(
        /* [in] */ UserState* userState,
        /* [in] */ const String& inputId,
        /* [in] */ Int32 state,
        /* [in] */ IITvInputManagerCallback* targetCallback);

    CARAPI SetStateLocked(
        /* [in] */ const String& inputId,
        /* [in] */ Int32 state,
        /* [in] */ Int32 userId);

private:
    static const Boolean DEBUG;

    static const String TAG;

    AutoPtr<IContext> mContext;

    AutoPtr<TvInputHardwareManager> mTvInputHardwareManager;

    AutoPtr<IContentResolver> mContentResolver;

    // A global lock.
    AutoPtr<IObject> mLock;

    // ID of the current user.
    Int32 mCurrentUserId;

    // A map from user id to UserState.
    AutoPtr<ISparseArray> mUserStates;

    AutoPtr<WatchLogHandler> mWatchLogHandler;
};

} // namespace Tv
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TV_TVINPUTMANAGERSERVICE_H__
