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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTRACKER_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/dataconnection/DcTrackerBase.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/utility/AsyncChannel.h>
#include <elastos/droid/internal/utility/State.h>
#include <elastos/droid/net/NetworkFactory.h>
#include <elastos/droid/net/NetworkRequest.h>
#include <elastos/droid/os/Build.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSubscriptionSourceManager;
using Elastos::Droid::Internal::Telephony::IDctConstants;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Utility::AsyncChannel;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Net::INetworkConfig;
using Elastos::Droid::Net::INetworkFactory;
using Elastos::Droid::Net::INetworkRequest;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::NetworkFactory;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Core::Thread;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * {@hide}
 */
class DcTracker
    : public DcTrackerBase
    , public IDcTracker
{
private:
    class SubBroadcastReceiverDefaultDds
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("DcTracker::SubBroadcastReceiverDefaultDds")

        SubBroadcastReceiverDefaultDds(
            /* [in] */ DcTracker* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        DcTracker* mHost;
    };

    class SubBroadcastReceiverSubInfo
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("DcTracker::SubBroadcastReceiverSubInfo")

        SubBroadcastReceiverSubInfo(
            /* [in] */ DcTracker* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        DcTracker* mHost;
    };

    /**
     * Handles changes to the APN db.
     */
    class ApnChangeObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("DcTracker::ApnChangeObserver")

        ApnChangeObserver(
            /* [in] */ DcTracker* host);

        CARAPI constructor();

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        DcTracker* mHost;
    };

    class TelephonyNetworkFactory
        : public NetworkFactory
    {
    public:
        TO_STRING_IMPL("DcTracker::TelephonyNetworkFactory")

        TelephonyNetworkFactory(
            /* [in] */ DcTracker* host);

        CARAPI constructor(
            /* [in] */ ILooper* l,
            /* [in] */ IContext* c,
            /* [in] */ const String& TAG,
            /* [in] */ INetworkCapabilities* nc,
            /* [in] */ IPhoneBase* phone);

        CARAPI ProcessPendingNetworkRequests(
            /* [in] */ INetworkRequest* n);

        CARAPI UpdateNetworkCapability(
            /* [in] */ Int64 subId);

        // @Override
        CARAPI NeedNetworkFor(
            /* [in] */ INetworkRequest* networkRequest,
            /* [in] */ Int32 score);

        // @Override
        CARAPI ReleaseNetworkFor(
            /* [in] */ INetworkRequest* networkRequest);

        CARAPI ReleaseAllNetworkRequests();

        // @Override
        CARAPI Log(
            /* [in] */ const char *fmt, ...);

    private:
        CARAPI RegisterOnDemandDdsCallback();

        CARAPI IsValidRequest(
            /* [in] */ INetworkRequest* n,
            /* [out] */ Boolean* result);

        CARAPI IsNetworkRequestForInternet(
            /* [in] */ INetworkRequest* n,
            /* [out] */ Boolean* result);

        CARAPI RequestOnDemandDataSubscriptionLock(
            /* [in] */ INetworkRequest* n);

        CARAPI RemoveRequestFromList(
            /* [in] */ ISparseArray* list,
            /* [in] */ INetworkRequest* n);

        CARAPI RemoveRequestIfFound(
            /* [in] */ INetworkRequest* n);

    private:
        //Thread safety not required as long as list operation are done by single thread.
        AutoPtr<ISparseArray> mDdsRequests;

        AutoPtr<IPhoneBase> mPhone;

        AutoPtr<INetworkCapabilities> mNetworkCapabilities;

        DcTracker* mHost;
    };

    // Class to handle Intent dispatched with user selects the "Sign-in to network"
    // notification.
    class ProvisionNotificationBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("DcTracker::ProvisionNotificationBroadcastReceiver")

        ProvisionNotificationBroadcastReceiver(
            /* [in] */ DcTracker* host);

        CARAPI constructor(
            /* [in] */ const String& provisionUrl,
            /* [in] */ const String& networkOperator);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CARAPI SetEnableFailFastMobileData(
            /* [in] */ Int32 enabled);

        CARAPI EnableMobileProvisioning();

    private:
        String mNetworkOperator;

        // Mobile provisioning URL.  Valid while provisioning notification is up.
        // Set prior to notification being posted as URL contains ICCID which
        // disappears when radio is off (which is the case when notification is up).
        String mProvisionUrl;

        DcTracker* mHost;
    };

    class OnDemandDdsLockNotifier
        : public Object
        , public ISubscriptionControllerOnDemandDdsLockNotifier
    {
    public:
        CAR_INTERFACE_DECL()

        OnDemandDdsLockNotifier(
            /* [in] */ DcTracker* host);

        CARAPI NotifyOnDemandDdsLockGranted(
            /* [in] */ INetworkRequest* n);

    private:
        DcTracker* mHost;
    };

public:
    TO_STRING_IMPL("DcTracker")

    CAR_INTERFACE_DECL()

    DcTracker();

    //***** Constructor
    CARAPI constructor(
        /* [in] */ IPhoneBase* p);

    CARAPI RegisterForAllEvents();

    // @Override
    CARAPI Dispose();

    CARAPI UnregisterForAllEvents();

    // @Override
    CARAPI IsApnTypeActive(
        /* [in] */ const String& type,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsOnDemandDataPossible(
        /* [in] */ const String& apnType,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsDataPossible(
        /* [in] */ const String& apnType,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI Finalize();

    CARAPI SupplyMessenger();

    CARAPI InitApnContexts();

    // @Override
    CARAPI GetLinkProperties(
        /* [in] */ const String& apnType,
        /* [out] */ ILinkProperties** result);

    // @Override
    CARAPI GetNetworkCapabilities(
        /* [in] */ const String& apnType,
        /* [out] */ INetworkCapabilities** result);

    // Return all active apn types
    CARAPI GetActiveApnTypes(
        /* [out, callee] */ ArrayOf<String>** result);

    // Return active apn of specific apn type
    CARAPI GetActiveApnString(
        /* [in] */ const String& apnType,
        /* [out] */ String* result);

    // @Override
    CARAPI IsApnTypeEnabled(
        /* [in] */ const String& apnType,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetState(
        /* [in] */ DctConstantsState s);

    // Return state of specific apn type
    // @Override
    CARAPI GetState(
        /* [in] */ const String& apnType,
        /* [out] */ DctConstantsState* result);

    // Return if apn type is a provisioning apn.
    // @Override
    CARAPI IsProvisioningApn(
        /* [in] */ const String& apnType,
        /* [out] */ Boolean* result);

    // Return state of overall
    // @Override
    CARAPI GetOverallState(
        /* [out] */ DctConstantsState* result);

    // @Override
    CARAPI IsApnTypeAvailable(
        /* [in] */ const String& type,
        /* [out] */ Boolean* result);

    /**
     * Report on whether data connectivity is enabled for any APN.
     * @return {@code false} if data connectivity has been explicitly disabled,
     * {@code true} otherwise.
     */
    // @Override
    CARAPI GetAnyDataEnabled(
        /* [out] */ Boolean* result);

    CARAPI GetAnyDataEnabled(
        /* [in] */ Boolean checkUserDataEnabled,
        /* [out] */ Boolean* result);

    /**
     * Invoked when ServiceStateTracker observes a transition from GPRS
     * attach to detach.
     */
    CARAPI OnDataConnectionDetached();

    // @Override
    CARAPI IsDataAllowed(
        /* [out] */ Boolean* result);

    // Disabled apn's still need avail/unavail notificiations - send them out
    CARAPI NotifyOffApnsOfAvailability(
        /* [in] */ const String& reason);

    /**
     * If tearDown is true, this only tears down a CONNECTED session. Presently,
     * there is no mechanism for abandoning an CONNECTING session,
     * but would likely involve cancelling pending async requests or
     * setting a flag or new state to ignore them when they came in
     * @param tearDown true if the underlying DataConnection should be
     * disconnected.
     * @param reason reason for the clean up.
     * @return boolean - true if we did cleanup any connections, false if they
     *                   were already all disconnected.
     */
    CARAPI CleanUpAllConnections(
        /* [in] */ Boolean tearDown,
        /* [in] */ const String& reason,
        /* [out] */ Boolean* result);

    /**
     * Cleanup all connections.
     *
     * TODO: Cleanup only a specified connection passed as a parameter.
     *       Also, make sure when you clean up a conn, if it is last apply
     *       logic as though it is cleanupAllConnections
     *
     * @param cause for the clean up.
     */
    // @Override
    CARAPI OnCleanUpAllConnections(
        /* [in] */ const String& cause);

    CARAPI CleanUpConnection(
        /* [in] */ Boolean tearDown,
        /* [in] */ IApnContext* apnContext);

    CARAPI SetupDataForSinglePdnArbitration(
        /* [in] */ const String& reason);

    // @Override
    CARAPI MvnoMatches(
        /* [in] */ IIccRecords* r,
        /* [in] */ const String& mvnoType,
        /* [in] */ const String& mvnoMatchData,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsPermanentFail(
        /* [in] */ IDcFailCause* dcFailCause,
        /* [out] */ Boolean* result);

    // TODO: For multiple Active APNs not exactly sure how to do this.
    // @Override
    CARAPI GotoIdleAndNotifyDataConnection(
        /* [in] */ const String& reason);

    // @Override
    CARAPI RestartRadio();

    // @Override
    CARAPI OnSetDependencyMet(
        /* [in] */ const String& apnType,
        /* [in] */ Boolean met);

    // @Override
    CARAPI OnEnableApn(
        /* [in] */ Int32 apnId,
        /* [in] */ Int32 enabled);

    // TODO: We shouldnt need this.
    CARAPI OnTrySetupData(
        /* [in] */ const String& reason,
        /* [out] */ Boolean* result);

    CARAPI OnTrySetupData(
        /* [in] */ IApnContext* apnContext,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnRoamingOff();

    // @Override
    CARAPI OnRoamingOn();

    // @Override
    CARAPI OnRadioAvailable();

    // @Override
    CARAPI OnRadioOffOrNotAvailable();

    // @Override
    CARAPI CompleteConnection(
        /* [in] */ IApnContext* apnContext);

    /**
     * A SETUP (aka bringUp) has completed, possibly with an error. If
     * there is an error this method will call {@link #onDataSetupCompleteError}.
     */
    // @Override
    CARAPI OnDataSetupComplete(
        /* [in] */ IAsyncResult* ar);

    /**
     * Error has occurred during the SETUP {aka bringUP} request and the DCT
     * should either try the next waiting APN or start over from the
     * beginning if the list is empty. Between each SETUP request there will
     * be a delay defined by {@link #getApnDelay()}.
     */
    // @Override
    CARAPI OnDataSetupCompleteError(
        /* [in] */ IAsyncResult* ar);

    /**
     * Called when EVENT_DISCONNECT_DONE is received.
     */
    // @Override
    CARAPI OnDisconnectDone(
        /* [in] */ Int32 connId,
        /* [in] */ IAsyncResult* ar);

    /**
     * Called when EVENT_DISCONNECT_DC_RETRYING is received.
     */
    // @Override
    CARAPI OnDisconnectDcRetrying(
        /* [in] */ Int32 connId,
        /* [in] */ IAsyncResult* ar);

    // @Override
    CARAPI OnVoiceCallStarted();

    // @Override
    CARAPI OnVoiceCallEnded();

    // @Override
    CARAPI OnCleanUpConnection(
        /* [in] */ Boolean tearDown,
        /* [in] */ Int32 apnId,
        /* [in] */ const String& reason);

    // @Override
    CARAPI IsConnected(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsDisconnected(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI NotifyDataConnection(
        /* [in] */ const String& reason);

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    CARAPI GetApnProfileID(
        /* [in] */ const String& apnType,
        /* [out] */ Int32* result);

    /**
     * @description This function updates mIccRecords reference to track
     *              currently used IccRecords
     * @return true if IccRecords changed
     */
    // @Override
    CARAPI OnUpdateIcc(
        /* [out] */ Boolean* result);

    // setAsCurrentDataConnectionTracker
    CARAPI Update();

    // @Override
    CARAPI CleanUpAllConnections(
        /* [in] */ const String& cause);

    CARAPI UpdateRecords();

    CARAPI CleanUpAllConnections(
        /* [in] */ const String& cause,
        /* [in] */ IMessage* disconnectAllCompleteMsg);

    CARAPI NotifyDataDisconnectComplete();

    CARAPI NotifyAllDataDisconnected();

    CARAPI RegisterForAllDataDisconnected(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForAllDataDisconnected(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI OnSetInternalDataEnabled(
        /* [in] */ Boolean enable);

    CARAPI OnSetInternalDataEnabled(
        /* [in] */ Boolean enabled,
        /* [in] */ IMessage* onCompleteMsg);

    CARAPI SetInternalDataEnabledFlag(
        /* [in] */ Boolean enable,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetInternalDataEnabled(
        /* [in] */ Boolean enable,
        /* [out] */ Boolean* result);

    CARAPI SetInternalDataEnabled(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* onCompleteMsg,
        /* [out] */ Boolean* result);

    /** Returns true if this is current DDS. */
    CARAPI IsActiveDataSubscription(
        /* [out] */ Boolean* result);

    CARAPI SetDataAllowed(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI Log(
        /* [in] */ const char *fmt, ...);

    // @Override
    CARAPI Loge(
        /* [in] */ const char *fmt, ...);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    // @Override
    CARAPI GetPcscfAddress(
        /* [in] */ const String& apnType,
        /* [out, callee] */ ArrayOf<String>** result);

    // @Override
    CARAPI SetImsRegistrationState(
        /* [in] */ Boolean registered);

    CARAPI GetImsRegistrationState(
        /* [out] */ Boolean* result);

private:
    CARAPI ProcessPendingNetworkRequests(
        /* [in] */ INetworkRequest* n);

    CARAPI UpdateSubIdAndCapability();

    CARAPI ReleaseAllNetworkRequests();

    CARAPI ApnContextForNetworkRequest(
        /* [in] */ INetworkRequest* nr,
        /* [out] */ IApnContext** result);

    // Turn telephony radio on or off.
    CARAPI SetRadio(
        /* [in] */ Boolean on);

    CARAPI AddApnContext(
        /* [in] */ const String& type,
        /* [in] */ INetworkConfig* networkConfig,
        /* [out] */ IApnContext** result);

    CARAPI IsDataAllowed(
        /* [in] */ IApnContext* apnContext,
        /* [out] */ Boolean* result);

    CARAPI OnDataConnectionAttached();

    CARAPI SetupDataOnConnectableApns(
        /* [in] */ const String& reason);

    CARAPI TrySetupData(
        /* [in] */ IApnContext* apnContext,
        /* [out] */ Boolean* result);

    /**
     * Determine if DUN connection is special and we need to teardown on start/stop
     */
    CARAPI TeardownForDun(
        /* [out] */ Boolean* result);

    /**
     * Cancels the alarm associated with apnContext.
     *
     * @param apnContext on which the alarm should be stopped.
     */
    CARAPI CancelReconnectAlarm(
        /* [in] */ IApnContext* apnContext);

    /**
     * @param types comma delimited list of APN types
     * @return array of APN types
     */
    CARAPI ParseTypes(
        /* [in] */ const String& types,
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI ImsiMatches(
        /* [in] */ const String& imsiDB,
        /* [in] */ const String& imsiSIM,
        /* [out] */ Boolean* result);

    CARAPI MakeApnSetting(
        /* [in] */ ICursor* cursor,
        /* [out] */ IApnSetting** result);

    CARAPI CreateApnList(
        /* [in] */ ICursor* cursor,
        /* [out] */ IArrayList** result);

    CARAPI DataConnectionNotInUse(
        /* [in] */ IDcAsyncChannel* dcac,
        /* [out] */ Boolean* result);

    CARAPI FindFreeDataConnection(
        /* [out] */ IDcAsyncChannel** result);

    CARAPI SetupData(
        /* [in] */ IApnContext* apnContext,
        /* [in] */ Int32 radioTech,
        /* [out] */ Boolean* result);

    /**
     * Handles changes to the APN database.
     */
    CARAPI OnApnChanged();

    CARAPI TryRestartDataConnections(
        /* [in] */ Boolean isCleanupNeeded,
        /* [in] */ const String& reason);

    CARAPI OnWwanIwlanCoexistenceDone(
        /* [in] */ IAsyncResult* ar);

    CARAPI OnModemApnProfileReady();

    /**
     * @param cid Connection id provided from RIL.
     * @return DataConnectionAc associated with specified cid.
     */
    CARAPI FindDataConnectionAcByCid(
        /* [in] */ Int32 cid,
        /* [out] */ IDcAsyncChannel** result);

    /**
     * "Active" here means ApnContext isEnabled() and not in FAILED state
     * @param apnContext to compare with
     * @return true if higher priority active apn found
     */
    CARAPI IsHigherPriorityApnContextActive(
        /* [in] */ IApnContext* apnContext,
        /* [out] */ Boolean* result);

    /**
     * Reports if we support multiple connections or not.
     * This is a combination of factors, based on carrier and RAT.
     * @param rilRadioTech the RIL Radio Tech currently in use
     * @return true if only single DataConnection is allowed
     */
    CARAPI IsOnlySingleDcAllowed(
        /* [in] */ Int32 rilRadioTech,
        /* [out] */ Boolean* result);

    /**
     * Return true if data connection need to be setup after disconnected due to
     * reason.
     *
     * @param reason the reason why data is disconnected
     * @return true if try setup data connection is need for this reason
     */
    CARAPI RetryAfterDisconnected(
        /* [in] */ IApnContext* apnContext,
        /* [out] */ Boolean* result);

    CARAPI StartAlarmForReconnect(
        /* [in] */ Int32 delay,
        /* [in] */ IApnContext* apnContext);

    CARAPI StartAlarmForRestartTrySetup(
        /* [in] */ Int32 delay,
        /* [in] */ IApnContext* apnContext);

    CARAPI NotifyNoData(
        /* [in] */ IDcFailCause* lastFailCauseCode,
        /* [in] */ IApnContext* apnContext);

    CARAPI OnRecordsLoaded();

    CARAPI OnNvReady();

    CARAPI ApplyNewState(
        /* [in] */ IApnContext* apnContext,
        /* [in] */ Boolean enabled,
        /* [in] */ Boolean met);

    CARAPI CheckForCompatibleConnectedApnContext(
        /* [in] */ IApnContext* apnContext,
        /* [out] */ IDcAsyncChannel** result);

    /**
     * @return number of milli-seconds to delay between trying apns'
     */
    CARAPI GetApnDelay(
        /* [in] */ const String& reason,
        /* [out] */ Int32* result);

    CARAPI IsNvSubscription(
        /* [out] */ Boolean* result);

    /**
     * Returns mccmnc for data call either from cdma_home_operator or from IccRecords
     * @return operator numeric
     */
    CARAPI GetOperatorNumeric(
        /* [out] */ String* result);

    /**
     * Based on the operator numeric, create a list for all possible
     * Data Connections and setup the preferredApn.
     */
    CARAPI CreateAllApnList();

    CARAPI DedupeApnSettings();

    //check whether the types of two APN same (even only one type of each APN is same)
    CARAPI ApnTypeSameAny(
        /* [in] */ IApnSetting* first,
        /* [in] */ IApnSetting* second,
        /* [out] */ Boolean* result);

    // Check if neither mention DUN and are substantially similar
    CARAPI ApnsSimilar(
        /* [in] */ IApnSetting* first,
        /* [in] */ IApnSetting* second,
        /* [out] */ Boolean* result);

    // equal or one is not specified
    CARAPI XorEquals(
        /* [in] */ const String& first,
        /* [in] */ const String& second,
        /* [out] */ Boolean* result);

    CARAPI MergeApns(
        /* [in] */ IApnSetting* dest,
        /* [in] */ IApnSetting* src,
        /* [out] */ IApnSetting** result);

    CARAPI IsDummyProfileNeeded(
        /* [out] */ Boolean* result);

    CARAPI AddDummyApnSettings(
        /* [in] */ const String& op);

    /** Return the DC AsyncChannel for the new data connection */
    CARAPI CreateDataConnection(
        /* [out] */ IDcAsyncChannel** result);

    CARAPI DestroyDataConnections();

    /**
     * Build a list of APNs to be used to create PDP's.
     *
     * @param requestedApnType
     * @return waitingApns list to be used to create PDP
     *          error when waitingApns.isEmpty()
     */
    CARAPI BuildWaitingApns(
        /* [in] */ const String& requestedApnType,
        /* [in] */ Int32 radioTech,
        /* [out] */ IArrayList** result);

    CARAPI ApnListToString(
        /* [in] */ IArrayList* apns,
        /* [out] */ String* result);

    CARAPI SetPreferredApn(
        /* [in] */ Int32 pos);

    CARAPI GetPreferredApn(
        /* [out] */ IApnSetting** result);

    CARAPI GetCellLocationId(
        /* [out] */ Int32* result);

    CARAPI GetUiccRecords(
        /* [in] */ Int32 appFamily,
        /* [out] */ IIccRecords** result);

    /**
     * Read APN configuration from Telephony.db for Emergency APN
     * All opertors recognize the connection request for EPDN based on APN type
     * PLMN name,APN name are not mandatory parameters
     */
    CARAPI InitEmergencyApnSetting();

    /**
     * Add the Emergency APN settings to APN settings list
     */
    CARAPI AddEmergencyApnSetting();

    CARAPI GetUri(
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    static CARAPI_(AutoPtr<IUri>) InitPREFERAPN_NO_UPDATE_URI();

    static CARAPI_(String) InitPROPERTY_CDMA_IPPROTOCOL();

    static CARAPI_(String) InitPROPERTY_CDMA_ROAMING_IPPROTOCOL();

public:
    String LOG__TAG;

    Int32 mDisconnectPendingCount;

    static AutoPtr<IUri> PREFERAPN_NO_UPDATE_URI;

    static const String APN_ID;

    /*
     * Flag that indicates that Out Of Service is considered as data call disconnect
     */
    Boolean mOosIsDisconnect;

private:
    //***** Instance Variables
    Boolean mReregisterOnReconnectFailure;

    //***** Constants

    // Used by puppetmaster/*/radio_stress.py
    static const String PUPPET_MASTER_RADIO_STRESS_TEST;

    static const Int32 POLL_PDP_MILLIS;

    static const Int32 PROVISIONING_SPINNER_TIMEOUT_MILLIS;

    /**
     * Property that can be used to set the IP version for CDMA
     */
    static const String PROPERTY_CDMA_IPPROTOCOL;

    /**
     * Property that can be used to set the IP version for CDMA when roaming
     */
    static const String PROPERTY_CDMA_ROAMING_IPPROTOCOL;

    Boolean mCanSetPreferApn;

    /** Watches for changes to the APN db. */
    AutoPtr<ApnChangeObserver> mApnObserver;

    String mProvisionActionName;

    AutoPtr<IBroadcastReceiver> mProvisionBroadcastReceiver;

    AutoPtr<IProgressDialog> mProvisioningSpinner;

    /** Used to send us NetworkRequests from ConnectivityService.  Remeber it so we can
     * unregister on dispose. */
    AutoPtr<IMessenger> mNetworkFactoryMessenger;

    AutoPtr<INetworkFactory> mNetworkFactory;

    AutoPtr<INetworkCapabilities> mNetworkFilter;

    AutoPtr<ICdmaSubscriptionSourceManager> mCdmaSsm;

    Boolean mImsRegistrationState;

    AutoPtr<IApnContext> mWaitCleanUpApnContext;

    Boolean mDeregistrationAlarmState;

    AutoPtr<IPendingIntent> mImsDeregistrationDelayIntent;

    /* IWLAN and WWAN co-exist flag */
    Boolean mWwanIwlanCoexistFlag;

    Int64 mSubId;

    AutoPtr<ICdmaApnProfileTracker> mOmhApt;

    /**
     * List of messages that are waiting to be posted, when data call disconnect
     * is complete
     */
    AutoPtr<IArrayList> mDisconnectAllCompleteMsgList;

    AutoPtr<IRegistrantList> mAllDataDisconnectedRegistrants;

    AutoPtr<IAtomicBoolean> mAttached;

    AutoPtr<IBroadcastReceiver> mDefaultDdsBroadcastReceiver;

    AutoPtr<IBroadcastReceiver> mSubInfoBroadcastReceiver;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTRACKER_H__
