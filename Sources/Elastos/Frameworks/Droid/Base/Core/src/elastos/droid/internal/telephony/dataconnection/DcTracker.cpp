/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 *
 * Not a Contribution.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.dataconnection;

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Net::INetworkConfig;
using Elastos::Droid::Net::INetworkFactory;
using Elastos::Droid::Net::INetworkRequest;
using Elastos::Droid::Net::INetworkUtils;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::Cdma::ICdmaCellLocation;
using Elastos::Droid::Telephony::Gsm::IGsmCellLocation;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IEventLog;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::Cdma::ICDMALTEPhone;
using Elastos::Droid::Internal::Telephony::Cdma::ICDMAPhone;
using Elastos::Droid::Internal::Telephony::Dataconnection::ICdmaApnProfileTracker;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IDctConstants;
using Elastos::Droid::Internal::Telephony::IEventLogTags;
using Elastos::Droid::Internal::Telephony::IITelephony;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::Gsm::IGSMPhone;
using Elastos::Droid::Internal::Telephony::Cdma::ICDMAPhone;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSubscriptionSourceManager;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IRuimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::IArrayUtils;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Nio::IByteBuffer;
using Elastos::Nio::IByteOrder;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IObjects;
using Elastos::Lang::IStringBuilder;

using Elastos::Droid::Provider::ISettings;

using Elastos::Droid::Internal::Telephony::IServiceStateTracker;
/**
 * {@hide}
 */
public class DcTracker extends DcTrackerBase {
    protected final String LOG_TAG;

    /**
     * List of messages that are waiting to be posted, when data call disconnect
     * is complete
     */
    private ArrayList<Message> mDisconnectAllCompleteMsgList = new ArrayList<Message>();

    private RegistrantList mAllDataDisconnectedRegistrants = new RegistrantList();

    protected Int32 mDisconnectPendingCount = 0;

    /**
     * Handles changes to the APN db.
     */
    private class ApnChangeObserver extends ContentObserver {
        public ApnChangeObserver () {
            Super(mDataConnectionTracker);
        }

        //@Override
        CARAPI OnChange(Boolean selfChange) {
            SendMessage(ObtainMessage(DctConstants.EVENT_APN_CHANGED));
        }
    }

    //***** Instance Variables

    private Boolean mReregisterOnReconnectFailure = FALSE;


    //***** Constants

    // Used by puppetmaster/*/radio_stress.py
    private static const String PUPPET_MASTER_RADIO_STRESS_TEST = "gsm.defaultpdpcontext.active";

    private static const Int32 POLL_PDP_MILLIS = 5 * 1000;

    private static const Int32 PROVISIONING_SPINNER_TIMEOUT_MILLIS = 120 * 1000;

    static const Uri PREFERAPN_NO_UPDATE_URI =
                        Uri->Parse("content://telephony/carriers/preferapn_no_update");
    static const String APN_ID = "apn_id";

    /**
     * Property that can be used to set the IP version for CDMA
     */
    private static const String PROPERTY_CDMA_IPPROTOCOL = SystemProperties->Get(
            "persist.telephony.cdma.protocol", "IP");

    /**
     * Property that can be used to set the IP version for CDMA when roaming
     */
    private static const String PROPERTY_CDMA_ROAMING_IPPROTOCOL = SystemProperties->Get(
            "persist.telephony.cdma.rproto", "IP");

    /*
     * Flag that indicates that Out Of Service is considered as data call disconnect
     */
    protected Boolean mOosIsDisconnect = SystemProperties->GetBoolean(
            PhoneBase.PROPERTY_OOS_IS_DISCONNECT, TRUE);

    private Boolean mCanSetPreferApn = FALSE;

    private AtomicBoolean mAttached = new AtomicBoolean(FALSE);

    /** Watches for changes to the APN db. */
    private ApnChangeObserver mApnObserver;

    private final String mProvisionActionName;
    private BroadcastReceiver mProvisionBroadcastReceiver;
    private ProgressDialog mProvisioningSpinner;

    /** Used to send us NetworkRequests from ConnectivityService.  Remeber it so we can
     * unregister on dispose. */
    private Messenger mNetworkFactoryMessenger;
    private NetworkFactory mNetworkFactory;
    private NetworkCapabilities mNetworkFilter;
    private CdmaSubscriptionSourceManager mCdmaSsm;

    public Boolean mImsRegistrationState = FALSE;
    private ApnContext mWaitCleanUpApnContext = NULL;
    private Boolean mDeregistrationAlarmState = FALSE;
    private PendingIntent mImsDeregistrationDelayIntent = NULL;

    /* IWLAN and WWAN co-exist flag */
    private Boolean mWwanIwlanCoexistFlag = FALSE;
    private Int64 mSubId;


    private BroadcastReceiver defaultDdsBroadcastReceiver = new BroadcastReceiver() {
        CARAPI OnReceive(Context context, Intent intent) {
            mSubId = mPhone->GetSubId();
            Log("got ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED, new DDS = "
                    + intent->GetLongExtra(PhoneConstants.SUBSCRIPTION_KEY,
                            SubscriptionManager.INVALID_SUB_ID));
            UpdateSubIdAndCapability();

            If (mSubId == SubscriptionController->GetInstance()->GetDefaultDataSubId()) {
                Log("Dct is default-DDS now, process any pending MMS requests");
            }
        }
    };

    private BroadcastReceiver subInfoBroadcastReceiver = new BroadcastReceiver() {
        CARAPI OnReceive(Context context, Intent intent) {
            mSubId = mPhone->GetSubId();
            Log("got ACTION_SUBINFO_RECORD_UPDATED, mySubId = " + mSubId);
            UpdateSubIdAndCapability();
        }
    };

    private CdmaApnProfileTracker mOmhApt;

    //***** Constructor
    public DcTracker(PhoneBase p) {
        Super(p);
        If (p->GetPhoneType() == PhoneConstants.PHONE_TYPE_GSM) {
            LOG_TAG = "GsmDCT";
        } else If (p->GetPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
            LOG_TAG = "CdmaDCT";
        } else {
            LOG_TAG = "DCT";
            Loge("unexpected phone type [" + p->GetPhoneType() + "]");
        }
        If (DBG) Log(LOG_TAG + ".constructor");

        If (p->GetPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
            final Boolean fetchApnFromOmhCard = p->GetContext()->GetResources().
                    GetBoolean(R.bool.config_fetch_apn_from_omh_card);
            Log(LOG_TAG + " fetchApnFromOmhCard: " + fetchApnFromOmhCard);
            If (fetchApnFromOmhCard) {
                mOmhApt = new CdmaApnProfileTracker((CDMAPhone)p);
                mOmhApt->RegisterForModemProfileReady(this,
                        DctConstants.EVENT_MODEM_DATA_PROFILE_READY, NULL);
            }
        }

        mDataConnectionTracker = this;
        Update();
        mApnObserver = new ApnChangeObserver();
        p->GetContext()->GetContentResolver().RegisterContentObserver(
                Telephony.Carriers.CONTENT_URI, TRUE, mApnObserver);

        InitApnContexts();

        For (ApnContext apnContext : mApnContexts->Values()) {
            // Register the reconnect and restart actions.
            IntentFilter filter = new IntentFilter();
            filter->AddAction(INTENT_RECONNECT_ALARM + '.' + apnContext->GetApnType());
            filter->AddAction(INTENT_RESTART_TRYSETUP_ALARM + '.' + apnContext->GetApnType());
            mPhone->GetContext()->RegisterReceiver(mIntentReceiver, filter, NULL, mPhone);
        }

        ConnectivityManager cm = (ConnectivityManager)p->GetContext()->GetSystemService(
                Context.CONNECTIVITY_SERVICE);

        mNetworkFilter = new NetworkCapabilities();
        mNetworkFilter->AddTransportType(NetworkCapabilities.TRANSPORT_CELLULAR);
        mNetworkFilter->AddCapability(NetworkCapabilities.NET_CAPABILITY_MMS);
        mNetworkFilter->AddCapability(NetworkCapabilities.NET_CAPABILITY_SUPL);
        mNetworkFilter->AddCapability(NetworkCapabilities.NET_CAPABILITY_DUN);
        mNetworkFilter->AddCapability(NetworkCapabilities.NET_CAPABILITY_FOTA);
        mNetworkFilter->AddCapability(NetworkCapabilities.NET_CAPABILITY_IMS);
        mNetworkFilter->AddCapability(NetworkCapabilities.NET_CAPABILITY_CBS);
        mNetworkFilter->AddCapability(NetworkCapabilities.NET_CAPABILITY_IA);
        mNetworkFilter->AddCapability(NetworkCapabilities.NET_CAPABILITY_RCS);
        mNetworkFilter->AddCapability(NetworkCapabilities.NET_CAPABILITY_XCAP);
        mNetworkFilter->AddCapability(NetworkCapabilities.NET_CAPABILITY_EIMS);
        mNetworkFilter->AddCapability(NetworkCapabilities.NET_CAPABILITY_NOT_RESTRICTED);
        //Dont add INTERNET capability, only defaultDataSubscription provides INTERNET.

        mNetworkFactory = new TelephonyNetworkFactory(this->GetLooper(), p->GetContext(),
                "TelephonyNetworkFactory", mNetworkFilter, mPhone);
        mNetworkFactory->SetScoreFilter(-1);
        mNetworkFactoryMessenger = new Messenger(mNetworkFactory);
        cm->RegisterNetworkFactory(mNetworkFactoryMessenger, "Telephony");

        // Add Emergency APN to APN setting list by default to support EPDN in sim absent cases
        InitEmergencyApnSetting();
        AddEmergencyApnSetting();

        mProvisionActionName = "com.android.internal.telephony.PROVISION" + p->GetPhoneId();
        mPhone->GetContext()->RegisterReceiver(subInfoBroadcastReceiver,
                new IntentFilter(TelephonyIntents.ACTION_SUBINFO_RECORD_UPDATED));

        mPhone->GetContext()->RegisterReceiver(defaultDdsBroadcastReceiver,
                new IntentFilter(TelephonyIntents.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED));

    }

    private void ProcessPendingNetworkRequests(NetworkRequest n) {
        ((TelephonyNetworkFactory)mNetworkFactory).ProcessPendingNetworkRequests(n);
    }

    private void UpdateSubIdAndCapability() {
        ((TelephonyNetworkFactory)mNetworkFactory).UpdateNetworkCapability(mSubId);
    }

    private void ReleaseAllNetworkRequests() {
        ((TelephonyNetworkFactory)mNetworkFactory).ReleaseAllNetworkRequests();
    }

    protected void RegisterForAllEvents() {
        mPhone.mCi->RegisterForAvailable(this, DctConstants.EVENT_RADIO_AVAILABLE, NULL);
        mPhone.mCi->RegisterForOffOrNotAvailable(this,
               DctConstants.EVENT_RADIO_OFF_OR_NOT_AVAILABLE, NULL);
        mPhone.mCi->RegisterForWwanIwlanCoexistence(this,
                DctConstants.EVENT_GET_WWAN_IWLAN_COEXISTENCE_DONE, NULL);
        mPhone.mCi->RegisterForDataNetworkStateChanged(this,
               DctConstants.EVENT_DATA_STATE_CHANGED, NULL);
        mPhone->GetCallTracker().RegisterForVoiceCallEnded (this,
               DctConstants.EVENT_VOICE_CALL_ENDED, NULL);
        mPhone->GetCallTracker().RegisterForVoiceCallStarted (this,
               DctConstants.EVENT_VOICE_CALL_STARTED, NULL);
        mPhone->GetServiceStateTracker()->RegisterForDataConnectionAttached(this,
               DctConstants.EVENT_DATA_CONNECTION_ATTACHED, NULL);
        mPhone->GetServiceStateTracker()->RegisterForDataConnectionDetached(this,
               DctConstants.EVENT_DATA_CONNECTION_DETACHED, NULL);
        mPhone->GetServiceStateTracker()->RegisterForRoamingOn(this,
               DctConstants.EVENT_ROAMING_ON, NULL);
        mPhone->GetServiceStateTracker()->RegisterForRoamingOff(this,
               DctConstants.EVENT_ROAMING_OFF, NULL);
        mPhone->GetServiceStateTracker()->RegisterForPsRestrictedEnabled(this,
                DctConstants.EVENT_PS_RESTRICT_ENABLED, NULL);
        mPhone->GetServiceStateTracker()->RegisterForPsRestrictedDisabled(this,
                DctConstants.EVENT_PS_RESTRICT_DISABLED, NULL);
     //   SubscriptionManager->RegisterForDdsSwitch(this,
     //          DctConstants.EVENT_CLEAN_UP_ALL_CONNECTIONS, NULL);
        mPhone->GetServiceStateTracker()->RegisterForDataRegStateOrRatChanged(this,
                DctConstants.EVENT_DATA_RAT_CHANGED, NULL);
        If (mPhone->GetPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
            mCdmaSsm = CdmaSubscriptionSourceManager->GetInstance(
                    mPhone->GetContext(), mPhone.mCi, this,
                    DctConstants.EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, NULL);
            // CdmaSsm doesn't send this event whenever you register - fake it ourselves
            SendMessage(ObtainMessage(DctConstants.EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED));
        }
    }
    //@Override
    CARAPI Dispose() {
        If (DBG) Log("GsmDCT.dispose");

        ReleaseAllNetworkRequests();

        If (mProvisionBroadcastReceiver != NULL) {
            mPhone->GetContext()->UnregisterReceiver(mProvisionBroadcastReceiver);
            mProvisionBroadcastReceiver = NULL;
        }
        If (mProvisioningSpinner != NULL) {
            mProvisioningSpinner->Dismiss();
            mProvisioningSpinner = NULL;
        }

        ConnectivityManager cm = (ConnectivityManager)mPhone->GetContext()->GetSystemService(
                Context.CONNECTIVITY_SERVICE);
        cm->UnregisterNetworkFactory(mNetworkFactoryMessenger);

        mPhone->GetContext()->UnregisterReceiver(defaultDdsBroadcastReceiver);
        mPhone->GetContext()->UnregisterReceiver(subInfoBroadcastReceiver);

        mNetworkFactoryMessenger = NULL;

        CleanUpAllConnections(TRUE, NULL);

        super->Dispose();

        mPhone->GetContext()->GetContentResolver().UnregisterContentObserver(mApnObserver);
        mApnContexts->Clear();
        mPrioritySortedApnContexts->Clear();

        If (mCdmaSsm != NULL) {
            mCdmaSsm->Dispose(this);
        }

        If (mOmhApt != NULL) {
            mOmhApt->UnregisterForModemProfileReady(this);
        }

        DestroyDataConnections();
    }
    protected void UnregisterForAllEvents() {
         //Unregister for all events
        mPhone.mCi->UnregisterForAvailable(this);
        mPhone.mCi->UnregisterForOffOrNotAvailable(this);
        IccRecords r = mIccRecords->Get();
        If (r != NULL) {
            r->UnregisterForRecordsLoaded(this);
            mIccRecords->Set(NULL);
        }
        mPhone.mCi->UnregisterForDataNetworkStateChanged(this);
        mPhone->GetCallTracker()->UnregisterForVoiceCallEnded(this);
        mPhone->GetCallTracker()->UnregisterForVoiceCallStarted(this);
        mPhone->GetServiceStateTracker()->UnregisterForDataConnectionAttached(this);
        mPhone->GetServiceStateTracker()->UnregisterForDataConnectionDetached(this);
        mPhone->GetServiceStateTracker()->UnregisterForRoamingOn(this);
        mPhone->GetServiceStateTracker()->UnregisterForRoamingOff(this);
        mPhone->GetServiceStateTracker()->UnregisterForPsRestrictedEnabled(this);
        mPhone->GetServiceStateTracker()->UnregisterForPsRestrictedDisabled(this);
        //SubscriptionManager->UnregisterForDdsSwitch(this);
    }

    private class TelephonyNetworkFactory extends NetworkFactory {
        private PhoneBase mPhone;
        private NetworkCapabilities mNetworkCapabilities;

        //Thread safety not required as Int64 as list operation are done by single thread.
        private SparseArray<NetworkRequest> mDdsRequests = new SparseArray<NetworkRequest>();

        public TelephonyNetworkFactory(Looper l, Context c, String TAG,
                NetworkCapabilities nc, PhoneBase phone) {
            Super(l, c, TAG, nc);
            mPhone = phone;
            mNetworkCapabilities = nc;
        }

        CARAPI ProcessPendingNetworkRequests(NetworkRequest n) {
            For (Int32 i = 0; i < mDdsRequests->Size(); i++) {
                NetworkRequest nr = mDdsRequests->ValueAt(i);
                If (nr->Equals(n)) {
                    Log("Found pending request in ddsRequest list = " + nr);
                    ApnContext apnContext = ApnContextForNetworkRequest(nr);
                    If (apnContext != NULL) {
                        Log("Activating APN=" + apnContext);
                        apnContext->IncRefCount();
                    }
                }
            }
        }

        private void RegisterOnDemandDdsCallback() {
            SubscriptionController subController = SubscriptionController->GetInstance();

            subController->RegisterForOnDemandDdsLockNotification(mPhone->GetSubId(),
                    new SubscriptionController->OnDemandDdsLockNotifier() {
                        CARAPI NotifyOnDemandDdsLockGranted(NetworkRequest n) {
                            Log("Got the tempDds lock for the request = " + n);
                            ProcessPendingNetworkRequests(n);
                        }
                    });
        }

        CARAPI UpdateNetworkCapability(Int64 subId) {
            Log("update networkCapabilites for subId = " + subId);

            mNetworkCapabilities->SetNetworkSpecifier(""+subId);
            If ((subId > 0 && SubscriptionController->GetInstance().
                    GetSubState(subId) == SubscriptionManager.ACTIVE) &&
                    (subId == SubscriptionController->GetInstance()->GetDefaultDataSubId())) {
                Log("INTERNET capability is with subId = " + subId);
                //Only defaultDataSub provides INTERNET.
                mNetworkCapabilities->AddCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
            } else {
                Log("INTERNET capability is removed from subId = " + subId);
                mNetworkCapabilities->RemoveCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);

            }
            SetScoreFilter(50);
            RegisterOnDemandDdsCallback();

            Log("Ready to handle network requests");
        }

        //@Override
        protected void NeedNetworkFor(NetworkRequest networkRequest, Int32 score) {
            // figure out the apn type and enable it
            If (DBG) Log("Cellular needs Network for " + networkRequest);
            SubscriptionController subController = SubscriptionController->GetInstance();
            Log("subController = " + subController);

            Int64 currentDds = subController->GetDefaultDataSubId();
            Int64 subId = mPhone->GetSubId();
            Int64 requestedSpecifier = subController->GetSubIdFromNetworkRequest(networkRequest);

            Log("CurrentDds = " + currentDds);
            Log("mySubId = " + subId);
            Log("Requested networkSpecifier = " + requestedSpecifier);
            Log("my networkSpecifier = " + mNetworkCapabilities->GetNetworkSpecifier());

            If (subId < 0) {
                Log("Can't handle any network request now, subId not ready.");
                return;
            }

            // For clients that do not send subId in NetworkCapabilities,
            // Connectivity will send to all network factories. Accept only
            // when requestedSpecifier is same as current factory's subId
            If (requestedSpecifier != subId) {
                Log("requestedSpecifier is not same as mysubId. Bail out.");
                return;
            }

            If (currentDds != requestedSpecifier) {
                Log("This request would result in DDS switch");
                Log("Requested DDS switch to subId = " + requestedSpecifier);

                //Queue this request and initiate temp DDS switch.
                //Once the DDS switch is done we will revist the pending requests.
                mDdsRequests->Put(networkRequest.requestId, networkRequest);
                RequestOnDemandDataSubscriptionLock(networkRequest);

                return;
            } else {
                If(IsNetworkRequestForInternet(networkRequest)) {
                    Log("Activating internet request on subId = " + subId);
                    ApnContext apnContext = ApnContextForNetworkRequest(networkRequest);
                    If (apnContext != NULL) {
                        Log("Activating APN=" + apnContext);
                        apnContext->IncRefCount();
                    }
                } else {
                    If(IsValidRequest(networkRequest)) {
                        //non-default APN requests for this subscription.
                        mDdsRequests->Put(networkRequest.requestId, networkRequest);
                        RequestOnDemandDataSubscriptionLock(networkRequest);
                    } else {
                        Log("Bogus request req = " + networkRequest);
                    }
                }
            }
        }

        private Boolean IsValidRequest(NetworkRequest n) {
            Int32[] types = n.networkCapabilities->GetCapabilities();
            Return (types.length > 0);
        }

        private Boolean IsNetworkRequestForInternet(NetworkRequest n) {
            Boolean flag = n.networkCapabilities.hasCapability
                (NetworkCapabilities.NET_CAPABILITY_INTERNET);
            Log("Is the request for Internet = " + flag);
            return flag;
        }

        private void RequestOnDemandDataSubscriptionLock(NetworkRequest n) {
            If(!IsNetworkRequestForInternet(n)) {
                //Request tempDDS lock only for non-default PDP requests
                SubscriptionController subController = SubscriptionController->GetInstance();
                Log("requestOnDemandDataSubscriptionLock for request = " + n);
                subController->StartOnDemandDataSubscriptionRequest(n);
            }
        }

        private void RemoveRequestFromList(SparseArray<NetworkRequest> list, NetworkRequest n) {
            NetworkRequest nr = list->Get(n.requestId);
            If (nr != NULL) {
                Log("Removing request = " + nr);
                list->Remove(n.requestId);
                ApnContext apnContext = ApnContextForNetworkRequest(n);
                If (apnContext != NULL) {
                    Log("Deactivating APN=" + apnContext);
                    apnContext->DecRefCount();
                }
            }
        }

        private void RemoveRequestIfFound(NetworkRequest n) {
            Log("Release the request from dds queue, if found");
            RemoveRequestFromList(mDdsRequests, n);

            If(!IsNetworkRequestForInternet(n)) {
                SubscriptionController subController = SubscriptionController->GetInstance();
                subController->StopOnDemandDataSubscriptionRequest(n);
            } else {
                // Internet requests are not queued in DDS list. So deactivate here explicitly.
                ApnContext apnContext = ApnContextForNetworkRequest(n);
                If (apnContext != NULL) {
                    Log("Deactivating APN=" + apnContext);
                    apnContext->DecRefCount();
                }
            }
        }

        //@Override
        protected void ReleaseNetworkFor(NetworkRequest networkRequest) {
            If (DBG) Log("Cellular releasing Network for " + networkRequest);
            RemoveRequestIfFound(networkRequest);
        }

        CARAPI ReleaseAllNetworkRequests() {
            Log("releaseAllNetworkRequests");
            SubscriptionController subController = SubscriptionController->GetInstance();
            For (Int32 i = 0; i < mDdsRequests->Size(); i++) {
                NetworkRequest nr = mDdsRequests->ValueAt(i);
                If (nr != NULL) {
                    Log("Removing request = " + nr);
                    subController->StopOnDemandDataSubscriptionRequest(nr);
                    mDdsRequests->Remove(nr.requestId);
                }
            }
        }

        //@Override
        protected void Log(String s) {
            Logger::D("TelephonyNetworkFactory" + mPhone->GetSubId(), s);
        }
    }

    private ApnContext ApnContextForNetworkRequest(NetworkRequest nr) {
        NetworkCapabilities nc = nr.networkCapabilities;
        // for now, ignore the bandwidth stuff
        If (nc->GetTransportTypes().length > 0 &&
                nc->HasTransport(NetworkCapabilities.TRANSPORT_CELLULAR) == FALSE) {
            return NULL;
        }

        // in the near term just do 1-1 matches.
        // TODO - actually try to match the set of capabilities
        Int32 type = -1;
        String name = NULL;

        Boolean error = FALSE;
        If (nc->HasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)) {
            If (name != NULL) error = TRUE;
            name = PhoneConstants.APN_TYPE_DEFAULT;
            type = ConnectivityManager.TYPE_MOBILE;
        }
        If (nc->HasCapability(NetworkCapabilities.NET_CAPABILITY_MMS)) {
            If (name != NULL) error = TRUE;
            name = PhoneConstants.APN_TYPE_MMS;
            type = ConnectivityManager.TYPE_MOBILE_MMS;
        }
        If (nc->HasCapability(NetworkCapabilities.NET_CAPABILITY_SUPL)) {
            If (name != NULL) error = TRUE;
            name = PhoneConstants.APN_TYPE_SUPL;
            type = ConnectivityManager.TYPE_MOBILE_SUPL;
        }
        If (nc->HasCapability(NetworkCapabilities.NET_CAPABILITY_DUN)) {
            If (name != NULL) error = TRUE;
            name = PhoneConstants.APN_TYPE_DUN;
            type = ConnectivityManager.TYPE_MOBILE_DUN;
        }
        If (nc->HasCapability(NetworkCapabilities.NET_CAPABILITY_FOTA)) {
            If (name != NULL) error = TRUE;
            name = PhoneConstants.APN_TYPE_FOTA;
            type = ConnectivityManager.TYPE_MOBILE_FOTA;
        }
        If (nc->HasCapability(NetworkCapabilities.NET_CAPABILITY_IMS)) {
            If (name != NULL) error = TRUE;
            name = PhoneConstants.APN_TYPE_IMS;
            type = ConnectivityManager.TYPE_MOBILE_IMS;
        }
        If (nc->HasCapability(NetworkCapabilities.NET_CAPABILITY_CBS)) {
            If (name != NULL) error = TRUE;
            name = PhoneConstants.APN_TYPE_CBS;
            type = ConnectivityManager.TYPE_MOBILE_CBS;
        }
        If (nc->HasCapability(NetworkCapabilities.NET_CAPABILITY_IA)) {
            If (name != NULL) error = TRUE;
            name = PhoneConstants.APN_TYPE_IA;
            type = ConnectivityManager.TYPE_MOBILE_IA;
        }
        If (nc->HasCapability(NetworkCapabilities.NET_CAPABILITY_RCS)) {
            If (name != NULL) error = TRUE;
            name = NULL;
            Loge("RCS APN type not yet supported");
        }
        If (nc->HasCapability(NetworkCapabilities.NET_CAPABILITY_XCAP)) {
            If (name != NULL) error = TRUE;
            name = NULL;
            Loge("XCAP APN type not yet supported");
        }
        If (nc->HasCapability(NetworkCapabilities.NET_CAPABILITY_EIMS)) {
            If (name != NULL) error = TRUE;
            name = NULL;
            Loge("EIMS APN type not yet supported");
        }
        If (error) {
            Loge("Multiple apn types specified in request - result is unspecified!");
        }
        If (type == -1 || name == NULL) {
            Loge("Unsupported NetworkRequest in Telephony: " + nr);
            return NULL;
        }
        ApnContext apnContext = mApnContexts->Get(name);
        If (apnContext == NULL) {
            Loge("Request for unsupported mobile type: " + type);
        }
        return apnContext;
    }

    // Turn telephony radio on or off.
    private void SetRadio(Boolean on) {
        final ITelephony phone = ITelephony.Stub->AsInterface(ServiceManager->CheckService("phone"));
        try {
            phone->SetRadio(on);
        } Catch (Exception e) {
            // Ignore.
        }
    }

    // Class to handle Intent dispatched with user selects the "Sign-in to network"
    // notification.
    private class ProvisionNotificationBroadcastReceiver extends BroadcastReceiver {
        private final String mNetworkOperator;
        // Mobile provisioning URL.  Valid while provisioning notification is up.
        // Set prior to notification being posted as URL contains ICCID which
        // disappears when radio is Off (which is the case when notification is up).
        private final String mProvisionUrl;

        public ProvisionNotificationBroadcastReceiver(String provisionUrl, String networkOperator) {
            mNetworkOperator = networkOperator;
            mProvisionUrl = provisionUrl;
        }

        private void SetEnableFailFastMobileData(Int32 enabled) {
            SendMessage(ObtainMessage(DctConstants.CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA, enabled, 0));
        }

        private void EnableMobileProvisioning() {
            final Message msg = ObtainMessage(DctConstants.CMD_ENABLE_MOBILE_PROVISIONING);
            msg->SetData(Bundle->ForPair(DctConstants.PROVISIONING_URL_KEY, mProvisionUrl));
            SendMessage(msg);
        }

        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            // Turning back on the radio can take time on the order of a minute, so show user a
            // spinner so they know something is going on.
            mProvisioningSpinner = new ProgressDialog(context);
            mProvisioningSpinner->SetTitle(mNetworkOperator);
            mProvisioningSpinner->SetMessage(
                    // TODO: Don't borrow "Connecting..." i18n string; give Telephony a version.
                    context->GetText(R::string::media_route_status_connecting));
            mProvisioningSpinner->SetIndeterminate(TRUE);
            mProvisioningSpinner->SetCancelable(TRUE);
            // Allow non-Activity Service Context to create a View.
            mProvisioningSpinner->GetWindow()->SetType(
                    WindowManager.LayoutParams.TYPE_KEYGUARD_DIALOG);
            mProvisioningSpinner->Show();
            // After timeout, hide spinner so user can at least use their device.
            // TODO: Indicate to user that it is taking an unusually Int64 time to connect?
            SendMessageDelayed(ObtainMessage(DctConstants.CMD_CLEAR_PROVISIONING_SPINNER,
                    mProvisioningSpinner), PROVISIONING_SPINNER_TIMEOUT_MILLIS);
            // This code is almost identical to the old
            // ConnectivityService.handleMobileProvisioningAction code.
            SetRadio(TRUE);
            SetEnableFailFastMobileData(DctConstants.ENABLED);
            EnableMobileProvisioning();
        }
    }

    //@Override
    public Boolean IsApnTypeActive(String type) {
        ApnContext apnContext = mApnContexts->Get(type);
        If (apnContext == NULL) return FALSE;

        Return (apnContext->GetDcAc() != NULL);
    }

    //@Override
    public Boolean IsOnDemandDataPossible(String apnType) {
        /*
         * Check if APN enabled
         * Check if MobileData is ON
         * Check if MobileData UI override present
         */

        Boolean flag = FALSE;
        ApnContext apnContext = mApnContexts->Get(apnType);
        If (apnContext == NULL) {
            return FALSE;
        }
        Boolean apnContextIsEnabled = apnContext->IsEnabled();

        DctConstants.State apnContextState = apnContext->GetState();
        Boolean apnTypePossible = !(apnContextIsEnabled &&
                (apnContextState == DctConstants.State.FAILED));

        Boolean userDataEnabled = mUserDataEnabled;

        If (PhoneConstants.APN_TYPE_MMS->Equals(apnType)) {
            Boolean mobileDataOffOveride = mPhone->GetContext()->GetResources().
                GetBoolean(R.bool.config_enable_mms_with_mobile_data_off);
            Log("isOnDemandDataPossible MobileDataEnabled override = " + mobileDataOffOveride);

            userDataEnabled = (mUserDataEnabled || mobileDataOffOveride);
        }

        flag = apnTypePossible && userDataEnabled;
        Log("isOnDemandDataPossible, possible =" + flag + ", apnContext = " + apnContext);
        return flag;
    }

    //@Override
    public Boolean IsDataPossible(String apnType) {
        ApnContext apnContext = mApnContexts->Get(apnType);
        If (apnContext == NULL) {
            return FALSE;
        }
        Boolean apnContextIsEnabled = apnContext->IsEnabled();
        DctConstants.State apnContextState = apnContext->GetState();
        Boolean apnTypePossible = !(apnContextIsEnabled &&
                (apnContextState == DctConstants.State.FAILED));
        Boolean isEmergencyApn = apnContext->GetApnType()->Equals(PhoneConstants.APN_TYPE_EMERGENCY);
        // Set the emergency APN availability status as TRUE irrespective of conditions checked in
        // IsDataAllowed() like IN_SERVICE, MOBILE DATA status etc.
        Boolean dataAllowed = isEmergencyApn || IsDataAllowed();
        Boolean possible = dataAllowed && apnTypePossible;

        If ((apnContext->GetApnType()->Equals(PhoneConstants.APN_TYPE_DEFAULT)
                    || apnContext->GetApnType()->Equals(PhoneConstants.APN_TYPE_IA))
                && (mPhone->GetServiceState()->GetRilDataRadioTechnology()
                == ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN)
                && (!mWwanIwlanCoexistFlag)) {
            Log("Default data call activation not possible in iwlan.");
            possible = FALSE;
        }

        If (VDBG) {
            Log(String->Format("IsDataPossible(%s): possible=%b isDataAllowed=%b " +
                    "apnTypePossible=%b apnContextisEnabled=%b ApnContextState()=%s",
                    apnType, possible, dataAllowed, apnTypePossible,
                    apnContextIsEnabled, apnContextState));
        }
        return possible;
    }

    //@Override
    protected void Finalize() {
        If(DBG) Log("finalize");
    }

    protected void SupplyMessenger() {
       // Supply the data connection tracker messenger only if
       // this is corresponding to the current DDS.
       If (!IsActiveDataSubscription()) {
           return;
       }

        ConnectivityManager cm = (ConnectivityManager)mPhone->GetContext()->GetSystemService(
                Context.CONNECTIVITY_SERVICE);
        cm->SupplyMessenger(ConnectivityManager.TYPE_MOBILE, new Messenger(this));
        cm->SupplyMessenger(ConnectivityManager.TYPE_MOBILE_MMS, new Messenger(this));
        cm->SupplyMessenger(ConnectivityManager.TYPE_MOBILE_SUPL, new Messenger(this));
        cm->SupplyMessenger(ConnectivityManager.TYPE_MOBILE_DUN, new Messenger(this));
        cm->SupplyMessenger(ConnectivityManager.TYPE_MOBILE_HIPRI, new Messenger(this));
        cm->SupplyMessenger(ConnectivityManager.TYPE_MOBILE_FOTA, new Messenger(this));
        cm->SupplyMessenger(ConnectivityManager.TYPE_MOBILE_IMS, new Messenger(this));
        cm->SupplyMessenger(ConnectivityManager.TYPE_MOBILE_CBS, new Messenger(this));
        cm->SupplyMessenger(ConnectivityManager.TYPE_MOBILE_EMERGENCY, new Messenger(this));
    }

    private ApnContext AddApnContext(String type, NetworkConfig networkConfig) {
        ApnContext apnContext = new ApnContext(mPhone->GetContext(), type, LOG_TAG, networkConfig,
                this);
        mApnContexts->Put(type, apnContext);
        mPrioritySortedApnContexts->Add(apnContext);
        return apnContext;
    }

    protected void InitApnContexts() {
        Log("initApnContexts: E");
        // Load device network attributes from resources
        String[] networkConfigStrings = mPhone->GetContext()->GetResources().GetStringArray(
                R.array.networkAttributes);
        For (String networkConfigString : networkConfigStrings) {
            NetworkConfig networkConfig = new NetworkConfig(networkConfigString);
            ApnContext apnContext = NULL;

            Switch (networkConfig.type) {
            case ConnectivityManager.TYPE_MOBILE:
                apnContext = AddApnContext(PhoneConstants.APN_TYPE_DEFAULT, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_MMS:
                apnContext = AddApnContext(PhoneConstants.APN_TYPE_MMS, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_SUPL:
                apnContext = AddApnContext(PhoneConstants.APN_TYPE_SUPL, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_DUN:
                apnContext = AddApnContext(PhoneConstants.APN_TYPE_DUN, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_HIPRI:
                apnContext = AddApnContext(PhoneConstants.APN_TYPE_HIPRI, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_FOTA:
                apnContext = AddApnContext(PhoneConstants.APN_TYPE_FOTA, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_IMS:
                apnContext = AddApnContext(PhoneConstants.APN_TYPE_IMS, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_CBS:
                apnContext = AddApnContext(PhoneConstants.APN_TYPE_CBS, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_IA:
                apnContext = AddApnContext(PhoneConstants.APN_TYPE_IA, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_EMERGENCY:
                apnContext = AddApnContext(PhoneConstants.APN_TYPE_EMERGENCY, networkConfig);
                break;
            default:
                Log("initApnContexts: skipping unknown type=" + networkConfig.type);
                continue;
            }
            Log("initApnContexts: apnContext=" + apnContext);
        }
        Log("initApnContexts: X mApnContexts=" + mApnContexts);
    }

    //@Override
    public LinkProperties GetLinkProperties(String apnType) {
        ApnContext apnContext = mApnContexts->Get(apnType);
        If (apnContext != NULL) {
            DcAsyncChannel dcac = apnContext->GetDcAc();
            If (dcac != NULL) {
                If (DBG) Log("return link properites for " + apnType);
                return dcac->GetLinkPropertiesSync();
            }
        }
        If (DBG) Log("return new LinkProperties");
        return new LinkProperties();
    }

    //@Override
    public NetworkCapabilities GetNetworkCapabilities(String apnType) {
        ApnContext apnContext = mApnContexts->Get(apnType);
        If (apnContext!=NULL) {
            DcAsyncChannel dataConnectionAc = apnContext->GetDcAc();
            If (dataConnectionAc != NULL) {
                If (DBG) {
                    Log("get active pdp is not NULL, return NetworkCapabilities for " + apnType);
                }
                return dataConnectionAc->GetNetworkCapabilitiesSync();
            }
        }
        If (DBG) Log("return new NetworkCapabilities");
        return new NetworkCapabilities();
    }

    //@Override
    // Return all active apn types
    public String[] GetActiveApnTypes() {
        If (DBG) Log("get all active apn types");
        ArrayList<String> result = new ArrayList<String>();

        For (ApnContext apnContext : mApnContexts->Values()) {
            If (mAttached->Get() && apnContext->IsReady()) {
                result->Add(apnContext->GetApnType());
            }
        }

        return result->ToArray(new String[0]);
    }

    //@Override
    // Return active apn of specific apn type
    public String GetActiveApnString(String apnType) {
        If (VDBG) Log( "get active apn string for type:" + apnType);
        ApnContext apnContext = mApnContexts->Get(apnType);
        If (apnContext != NULL) {
            ApnSetting apnSetting = apnContext->GetApnSetting();
            If (apnSetting != NULL) {
                return apnSetting.apn;
            }
        }
        return NULL;
    }

    //@Override
    public Boolean IsApnTypeEnabled(String apnType) {
        ApnContext apnContext = mApnContexts->Get(apnType);
        If (apnContext == NULL) {
            return FALSE;
        }
        return apnContext->IsEnabled();
    }

    //@Override
    protected void SetState(DctConstants.State s) {
        If (DBG) Log("setState should not be used in GSM" + s);
    }

    // Return state of specific apn type
    //@Override
    public DctConstants.State GetState(String apnType) {
        ApnContext apnContext = mApnContexts->Get(apnType);
        If (apnContext != NULL) {
            return apnContext->GetState();
        }
        return DctConstants.State.FAILED;
    }

    // Return if apn type is a provisioning apn.
    //@Override
    protected Boolean IsProvisioningApn(String apnType) {
        ApnContext apnContext = mApnContexts->Get(apnType);
        If (apnContext != NULL) {
            return apnContext->IsProvisioningApn();
        }
        return FALSE;
    }

    // Return state of overall
    //@Override
    public DctConstants.State GetOverallState() {
        Boolean isConnecting = FALSE;
        Boolean isFailed = TRUE; // All enabled Apns should be FAILED.
        Boolean isAnyEnabled = FALSE;

        For (ApnContext apnContext : mApnContexts->Values()) {
            If (apnContext->IsEnabled()) {
                isAnyEnabled = TRUE;
                Switch (apnContext->GetState()) {
                case CONNECTED:
                case DISCONNECTING:
                    If (DBG) Log("overall state is CONNECTED");
                    return DctConstants.State.CONNECTED;
                case RETRYING:
                case CONNECTING:
                    isConnecting = TRUE;
                    isFailed = FALSE;
                    break;
                case IDLE:
                case SCANNING:
                    isFailed = FALSE;
                    break;
                default:
                    isAnyEnabled = TRUE;
                    break;
                }
            }
        }

        If (!isAnyEnabled) { // Nothing enabled. return IDLE.
            If (DBG) Log( "overall state is IDLE");
            return DctConstants.State.IDLE;
        }

        If (isConnecting) {
            If (DBG) Log( "overall state is CONNECTING");
            return DctConstants.State.CONNECTING;
        } else If (!isFailed) {
            If (DBG) Log( "overall state is IDLE");
            return DctConstants.State.IDLE;
        } else {
            If (DBG) Log( "overall state is FAILED");
            return DctConstants.State.FAILED;
        }
    }

    //@Override
    protected Boolean IsApnTypeAvailable(String type) {
        If (type->Equals(PhoneConstants.APN_TYPE_DUN) && FetchDunApn() != NULL) {
            return TRUE;
        }

        If (mAllApnSettings != NULL) {
            For (ApnSetting apn : mAllApnSettings) {
                If (apn->CanHandleType(type)) {
                    return TRUE;
                }
            }
        }
        return FALSE;
    }

    /**
     * Report on whether data connectivity is enabled for any APN.
     * @return {@code FALSE} if data connectivity has been explicitly disabled,
     * {@code TRUE} otherwise.
     */
    //@Override
    public Boolean GetAnyDataEnabled() {
        Synchronized (mDataEnabledLock) {
            If (!(mInternalDataEnabled && mUserDataEnabled && sPolicyDataEnabled)) return FALSE;
            For (ApnContext apnContext : mApnContexts->Values()) {
                // Make sure we don't have a context that is going down
                // and is explicitly disabled.
                If (IsDataAllowed(apnContext)) {
                    return TRUE;
                }
            }
            return FALSE;
        }
    }

    public Boolean GetAnyDataEnabled(Boolean checkUserDataEnabled) {
        Synchronized (mDataEnabledLock) {
            If (!(mInternalDataEnabled && (!checkUserDataEnabled || mUserDataEnabled)
                        && (!checkUserDataEnabled || sPolicyDataEnabled)))
                return FALSE;

            For (ApnContext apnContext : mApnContexts->Values()) {
                // Make sure we dont have a context that going down
                // and is explicitly disabled.
                If (IsDataAllowed(apnContext)) {
                    return TRUE;
                }
            }
            return FALSE;
        }
    }

    private Boolean IsDataAllowed(ApnContext apnContext) {
        //If RAT is iwlan then dont allow default/IA PDP at all.
        //Rest of APN types can be evaluated for remaining conditions.
        If ((apnContext->GetApnType()->Equals(PhoneConstants.APN_TYPE_DEFAULT)
                    || apnContext->GetApnType()->Equals(PhoneConstants.APN_TYPE_IA))
                && (mPhone->GetServiceState()->GetRilDataRadioTechnology()
                    == ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN)
                && (!mWwanIwlanCoexistFlag)) {
            Log("Default data call activation not allowed in iwlan.");
            return FALSE;
        } else {
            return apnContext->IsReady() && IsDataAllowed();
        }
    }

    //****** Called from ServiceStateTracker
    /**
     * Invoked when ServiceStateTracker observes a transition from GPRS
     * attach to detach.
     */
    protected void OnDataConnectionDetached() {
        /*
         * We presently believe it is unnecessary to tear down the PDP context
         * when GPRS detaches, but we should stop the network polling.
         */
        If (DBG) Log ("onDataConnectionDetached: stop polling and notify detached");
        StopNetStatPoll();
        StopDataStallAlarm();
        NotifyDataConnection(Phone.REASON_DATA_DETACHED);
        mAttached->Set(FALSE);
    }

    private void OnDataConnectionAttached() {
        If (DBG) Log("onDataConnectionAttached");
        mAttached->Set(TRUE);
        If (GetOverallState() == DctConstants.State.CONNECTED) {
            If (DBG) Log("onDataConnectionAttached: start polling notify attached");
            StartNetStatPoll();
            StartDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
            NotifyDataConnection(Phone.REASON_DATA_ATTACHED);
        } else {
            // update APN availability so that APN can be enabled.
            NotifyOffApnsOfAvailability(Phone.REASON_DATA_ATTACHED);
        }
        If (mAutoAttachOnCreationConfig) {
            mAutoAttachOnCreation = TRUE;
        }
        SetupDataOnConnectableApns(Phone.REASON_DATA_ATTACHED);
    }

    //@Override
    protected Boolean IsDataAllowed() {
        final Boolean internalDataEnabled;
        Synchronized (mDataEnabledLock) {
            internalDataEnabled = mInternalDataEnabled;
        }

        Boolean attachedState = mAttached->Get();
        Boolean desiredPowerState = mPhone->GetServiceStateTracker()->GetDesiredPowerState();
        Int32 radioTech = mPhone->GetServiceState()->GetRilDataRadioTechnology();
        If (radioTech == ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN
                && desiredPowerState == FALSE) {
            desiredPowerState = TRUE;
            attachedState = TRUE;
        }

        IccRecords r = mIccRecords->Get();
        Boolean recordsLoaded = (r != NULL) ? r->GetRecordsLoaded() : FALSE;
        Boolean subscriptionFromNv = IsNvSubscription();

        Boolean allowed =
                    (attachedState || mAutoAttachOnCreation) &&
                    (subscriptionFromNv || recordsLoaded) &&
                    (mPhone->GetState() == PhoneConstants.State.IDLE ||
                     mPhone->GetServiceStateTracker()->IsConcurrentVoiceAndDataAllowed()) &&
                    internalDataEnabled &&
                    (!mPhone->GetServiceState()->GetRoaming() || GetDataOnRoamingEnabled()) &&
                    !mIsPsRestricted &&
                    desiredPowerState;
        If (!allowed && DBG) {
            String reason = "";
            If (!(attachedState || mAutoAttachOnCreation)) {
                reason += " - Attached= " + attachedState;
            }
            If (!(subscriptionFromNv || recordsLoaded)) {
                reason += " - SIM not loaded and not NV subscription";
            }
            If (mPhone->GetState() != PhoneConstants.State.IDLE &&
                    !mPhone->GetServiceStateTracker()->IsConcurrentVoiceAndDataAllowed()) {
                reason += " - PhoneState= " + mPhone->GetState();
                reason += " - Concurrent voice and data not allowed";
            }
            If (!internalDataEnabled) reason += " - mInternalDataEnabled= FALSE";
            If (mPhone->GetServiceState()->GetRoaming() && !GetDataOnRoamingEnabled()) {
                reason += " - Roaming and data roaming not enabled";
            }
            If (mIsPsRestricted) reason += " - mIsPsRestricted= TRUE";
            If (!desiredPowerState) reason += " - desiredPowerState= FALSE";
            If (DBG) Log("isDataAllowed: not allowed due to" + reason);
        }
        return allowed;
    }

    private void SetupDataOnConnectableApns(String reason) {
        If (DBG) Log("setupDataOnConnectableApns: " + reason);

        For (ApnContext apnContext : mPrioritySortedApnContexts) {
            If (DBG) Log("setupDataOnConnectableApns: apnContext " + apnContext);
            If (apnContext->GetState() == DctConstants.State.FAILED) {
                apnContext->SetState(DctConstants.State.IDLE);
            }
            If (apnContext->IsConnectable()) {
                Log("setupDataOnConnectableApns: IsConnectable() call trySetupData");
                apnContext->SetReason(reason);
                TrySetupData(apnContext);
            }
        }
    }

    private Boolean TrySetupData(ApnContext apnContext) {
        If (DBG) {
            Log("trySetupData for type:" + apnContext->GetApnType() +
                    " due to " + apnContext->GetReason() + " apnContext=" + apnContext);
            Log("trySetupData with mIsPsRestricted=" + mIsPsRestricted);
        }

        If (mPhone->GetSimulatedRadioControl() != NULL) {
            // Assume data is connected on the simulator
            // FIXME  this can be improved
            apnContext->SetState(DctConstants.State.CONNECTED);
            mPhone->NotifyDataConnection(apnContext->GetReason(), apnContext->GetApnType());

            Log("trySetupData: X We're on the simulator; assuming connected retValue=TRUE");
            return TRUE;
        }

        // Allow SETUP_DATA request for E-APN to be completed during emergency call
        // and MOBILE DATA On/Off cases as well.
        Boolean isEmergencyApn = apnContext->GetApnType()->Equals(PhoneConstants.APN_TYPE_EMERGENCY);
        Boolean desiredPowerState = mPhone->GetServiceStateTracker()->GetDesiredPowerState();
        Boolean checkUserDataEnabled =
                    !(apnContext->GetApnType()->Equals(PhoneConstants.APN_TYPE_IMS));

        // If set the special property, enable mms data even if mobile data is turned off.
        If (apnContext->GetApnType()->Equals(PhoneConstants.APN_TYPE_MMS)) {
            checkUserDataEnabled = checkUserDataEnabled && !(mPhone->GetContext()->GetResources().
                    GetBoolean(R.bool.config_enable_mms_with_mobile_data_off));
        }

        If (apnContext->IsConnectable() && (isEmergencyApn ||
                (IsDataAllowed(apnContext) &&
                GetAnyDataEnabled(checkUserDataEnabled) && !IsEmergency()))) {
            If (apnContext->GetState() == DctConstants.State.FAILED) {
                If (DBG) Log("trySetupData: make a FAILED ApnContext IDLE so its reusable");
                apnContext->SetState(DctConstants.State.IDLE);
            }
            Int32 radioTech = mPhone->GetServiceState()->GetRilDataRadioTechnology();
            If (apnContext->GetState() == DctConstants.State.IDLE) {

                ArrayList<ApnSetting> waitingApns = BuildWaitingApns(apnContext->GetApnType(),
                        radioTech);
                If (waitingApns->IsEmpty()) {
                    NotifyNoData(DcFailCause.MISSING_UNKNOWN_APN, apnContext);
                    NotifyOffApnsOfAvailability(apnContext->GetReason());
                    If (DBG) Log("trySetupData: X No APN found retValue=FALSE");
                    return FALSE;
                } else {
                    apnContext->SetWaitingApns(waitingApns);
                    If (DBG) {
                        Log ("trySetupData: Create from mAllApnSettings : "
                                    + ApnListToString(mAllApnSettings));
                    }
                }
            }

            If (DBG) {
                If (apnContext->GetWaitingApns() == NULL) {
                    Log("trySetupData: call setupData, waitingApns : NULL");
                }
                else {
                    Log("trySetupData: call setupData, waitingApns : "
                            + ApnListToString(apnContext->GetWaitingApns()));
                }
            }
            Boolean retValue = SetupData(apnContext, radioTech);
            NotifyOffApnsOfAvailability(apnContext->GetReason());

            If (DBG) Log("trySetupData: X retValue=" + retValue);
            return retValue;
        } else {
            If (!apnContext->GetApnType()->Equals(PhoneConstants.APN_TYPE_DEFAULT)
                    && apnContext->IsConnectable()) {
                mPhone->NotifyDataConnectionFailed(apnContext->GetReason(), apnContext->GetApnType());
            }
            NotifyOffApnsOfAvailability(apnContext->GetReason());
            If (DBG) Log ("trySetupData: X apnContext not 'ready' retValue=FALSE");
            return FALSE;
        }
    }

    //@Override
    // Disabled apn's still need avail/unavail notificiations - send them out
    protected void NotifyOffApnsOfAvailability(String reason) {
        For (ApnContext apnContext : mApnContexts->Values()) {
            If ((!mAttached->Get() && mOosIsDisconnect) || !apnContext->IsReady()) {
                If (VDBG) Log("notifyOffApnOfAvailability type:" + apnContext->GetApnType());
                mPhone->NotifyDataConnection(reason != NULL ? reason : apnContext->GetReason(),
                                            apnContext->GetApnType(),
                                            PhoneConstants.DataState.DISCONNECTED);
            } else {
                If (VDBG) {
                    Log("notifyOffApnsOfAvailability skipped apn due to attached && isReady " +
                            apnContext->ToString());
                }
            }
        }
    }

    /**
     * If tearDown is TRUE, this only tears down a CONNECTED session. Presently,
     * there is no mechanism for abandoning an CONNECTING session,
     * but would likely involve cancelling pending async requests or
     * setting a flag or new state to ignore them when they came in
     * @param tearDown TRUE if the underlying DataConnection should be
     * disconnected.
     * @param reason reason for the clean up.
     * @return Boolean - TRUE if we did cleanup any connections, FALSE if they
     *                   were already all disconnected.
     */
    protected Boolean CleanUpAllConnections(Boolean tearDown, String reason) {
        If (DBG) Log("cleanUpAllConnections: tearDown=" + tearDown + " reason=" + reason);
        Boolean didDisconnect = FALSE;
        Boolean specificdisable = FALSE;

        If (!TextUtils->IsEmpty(reason)) {
            specificdisable = reason->Equals(Phone.REASON_DATA_SPECIFIC_DISABLED);
        }

        For (ApnContext apnContext : mApnContexts->Values()) {
            If (apnContext->IsDisconnected() == FALSE) didDisconnect = TRUE;
            If (specificdisable) {
                If (!apnContext->GetApnType()->Equals(PhoneConstants.APN_TYPE_IMS)) {
                    If (DBG) Log("ApnConextType: " + apnContext->GetApnType());
                    apnContext->SetReason(reason);
                    CleanUpConnection(tearDown, apnContext);
                }
            } else {
                // TODO - only do cleanup if not disconnected
                apnContext->SetReason(reason);
                CleanUpConnection(tearDown, apnContext);
            }
        }

        StopNetStatPoll();
        StopDataStallAlarm();

        // TODO: Do we need mRequestedApnType?
        mRequestedApnType = PhoneConstants.APN_TYPE_DEFAULT;

        Log("cleanUpConnection: mDisconnectPendingCount = " + mDisconnectPendingCount);
        If (tearDown && mDisconnectPendingCount == 0) {
            NotifyDataDisconnectComplete();
            NotifyAllDataDisconnected();
        }

        return didDisconnect;
    }

    /**
     * Cleanup all connections.
     *
     * TODO: Cleanup only a specified connection passed as a parameter.
     *       Also, make sure when you clean up a conn, if it is last apply
     *       logic as though it is cleanupAllConnections
     *
     * @param cause for the clean up.
     */

    //@Override
    protected void OnCleanUpAllConnections(String cause) {
        CleanUpAllConnections(TRUE, cause);
    }

    protected void CleanUpConnection(Boolean tearDown, ApnContext apnContext) {

        If (apnContext == NULL) {
            If (DBG) Log("cleanUpConnection: apn context is NULL");
            return;
        }

        DcAsyncChannel dcac = apnContext->GetDcAc();
        If (DBG) {
            Log("cleanUpConnection: E tearDown=" + tearDown + " reason=" + apnContext->GetReason() +
                    " apnContext=" + apnContext);
        }
        If (tearDown) {
            If (apnContext->IsDisconnected()) {
                // The request is tearDown and but ApnContext is not connected.
                // If apnContext is not enabled anymore, break the linkage to the DCAC/DC.
                apnContext->SetState(DctConstants.State.IDLE);
                If (!apnContext->IsReady()) {
                    If (dcac != NULL) {
                        dcac->TearDown(apnContext, "", NULL);
                    }
                    apnContext->SetDataConnectionAc(NULL);
                }
            } else {
                // Connection is still there. Try to clean up.
                If (dcac != NULL) {
                    If (apnContext->GetState() != DctConstants.State.DISCONNECTING) {
                        Boolean disconnectAll = FALSE;
                        If (PhoneConstants.APN_TYPE_DUN->Equals(apnContext->GetApnType())) {
                            // CAF_MSIM is this below condition required.
                            // If (PhoneConstants.APN_TYPE_DUN->Equals(PhoneConstants.APN_TYPE_DEFAULT)) {
                            If (TeardownForDun()) {
                                If (DBG) Log("tearing down dedicated DUN connection");
                                // we need to tear it down - we brought it up just for dun and
                                // other people are camped on it and now dun is done.  We need
                                // to stop using it and let the normal apn list get used to find
                                // connections for the remaining desired connections
                                disconnectAll = TRUE;
                            }
                        }
                        If (DBG) {
                            Log("cleanUpConnection: tearing down" + (disconnectAll ? " all" :""));
                        }
                        Message msg = ObtainMessage(DctConstants.EVENT_DISCONNECT_DONE, apnContext);
                        If (disconnectAll) {
                            apnContext->GetDcAc()->TearDownAll(apnContext->GetReason(), msg);
                        } else {
                            apnContext->GetDcAc()
                                .TearDown(apnContext, apnContext->GetReason(), msg);
                        }
                        apnContext->SetState(DctConstants.State.DISCONNECTING);
                        mDisconnectPendingCount++;
                    }
                } else {
                    // apn is connected but no reference to dcac.
                    // Should not be happen, but reset the state in case.
                    apnContext->SetState(DctConstants.State.IDLE);
                    mPhone->NotifyDataConnection(apnContext->GetReason(),
                                                apnContext->GetApnType());
                }
            }
        } else {
            // force clean up the data connection.
            If (dcac != NULL) dcac->ReqReset();
            apnContext->SetState(DctConstants.State.IDLE);
            mPhone->NotifyDataConnection(apnContext->GetReason(), apnContext->GetApnType());
            apnContext->SetDataConnectionAc(NULL);
        }

        If (mOmhApt != NULL) {
            mOmhApt->ClearActiveApnProfile();
        }

        // Make sure reconnection alarm is cleaned up if there is no ApnContext
        // associated to the connection.
        If (dcac != NULL) {
            CancelReconnectAlarm(apnContext);
        }

        SetupDataForSinglePdnArbitration(apnContext->GetReason());

        If (DBG) {
            Log("cleanUpConnection: X tearDown=" + tearDown + " reason=" + apnContext->GetReason() +
                    " apnContext=" + apnContext + " dcac=" + apnContext->GetDcAc());
        }
    }

    protected void SetupDataForSinglePdnArbitration(String reason) {
        // In single pdn case, if a higher priority call which was scheduled for retry gets
        // cleaned up due to say apn disabled, we need to try setup data on connectable apns
        // as there won't be any EVENT_DISCONNECT_DONE call back.
        If(DBG) {
            Log("setupDataForSinglePdn: reason = " + reason
                    + " isDisconnected = " + IsDisconnected());
        }
        If (IsOnlySingleDcAllowed(mPhone->GetServiceState()->GetRilDataRadioTechnology())
                && IsDisconnected()
                && !Phone.REASON_SINGLE_PDN_ARBITRATION->Equals(reason)) {
            SetupDataOnConnectableApns(Phone.REASON_SINGLE_PDN_ARBITRATION);
        }
    }

    /**
     * Determine if DUN connection is special and we need to teardown on start/stop
     */
    private Boolean TeardownForDun() {
        // CDMA always needs to do this the profile id is correct
        final Int32 rilRat = mPhone->GetServiceState()->GetRilDataRadioTechnology();
        If (ServiceState->IsCdma(rilRat)) return TRUE;

        Return (FetchDunApn() != NULL);
    }

    /**
     * Cancels the alarm associated with apnContext.
     *
     * @param apnContext on which the alarm should be stopped.
     */
    private void CancelReconnectAlarm(ApnContext apnContext) {
        If (apnContext == NULL) return;

        PendingIntent intent = apnContext->GetReconnectIntent();

        If (intent != NULL) {
                AlarmManager am =
                    (AlarmManager) mPhone->GetContext()->GetSystemService(Context.ALARM_SERVICE);
                am->Cancel(intent);
                apnContext->SetReconnectIntent(NULL);
        }
    }

    /**
     * @param types comma delimited list of APN types
     * @return array of APN types
     */
    private String[] ParseTypes(String types) {
        String[] result;
        // If unset, set to DEFAULT.
        If (types == NULL || types->Equals("")) {
            result = new String[1];
            result[0] = PhoneConstants.APN_TYPE_ALL;
        } else {
            result = types->Split(",");
        }
        return result;
    }

    private Boolean ImsiMatches(String imsiDB, String imsiSIM) {
        // Note: imsiDB value has digit number or 'x' character for seperating USIM information
        // for MVNO operator. And then digit number is matched at same order and 'x' character
        // could replace by any digit number.
        // ex) if imsiDB inserted '310260x10xxxxxx' for GG Operator,
        //     that means first 6 digits, 8th and 9th digit
        //     should be set in USIM for GG Operator.
        Int32 len = imsiDB->Length();
        Int32 idxCompare = 0;

        If (len <= 0) return FALSE;
        If (len > imsiSIM->Length()) return FALSE;

        For (Int32 idx=0; idx<len; idx++) {
            Char32 c = imsiDB->CharAt(idx);
            If ((c == 'x') || (c == 'X') || (c == imsiSIM->CharAt(idx))) {
                continue;
            } else {
                return FALSE;
            }
        }
        return TRUE;
    }

    //@Override
    protected Boolean MvnoMatches(IccRecords r, String mvnoType, String mvnoMatchData) {
        If (mvnoType->EqualsIgnoreCase("spn")) {
            If ((r->GetServiceProviderName() != NULL) &&
                    r->GetServiceProviderName()->EqualsIgnoreCase(mvnoMatchData)) {
                return TRUE;
            }
        } else If (mvnoType->EqualsIgnoreCase("imsi")) {
            String imsiSIM = r->GetIMSI();
            If ((imsiSIM != NULL) && ImsiMatches(mvnoMatchData, imsiSIM)) {
                return TRUE;
            }
        } else If (mvnoType->EqualsIgnoreCase("gid")) {
            String gid1 = r->GetGid1();
            Int32 mvno_match_data_length = mvnoMatchData->Length();
            If ((gid1 != NULL) && (gid1->Length() >= mvno_match_data_length) &&
                    gid1->Substring(0, mvno_match_data_length).EqualsIgnoreCase(mvnoMatchData)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    //@Override
    protected Boolean IsPermanentFail(DcFailCause dcFailCause) {
        Return (dcFailCause->IsPermanentFail() &&
                (mAttached->Get() == FALSE || dcFailCause != DcFailCause.SIGNAL_LOST));
    }

    private ApnSetting MakeApnSetting(Cursor cursor) {
        String[] types = ParseTypes(
                cursor->GetString(cursor->GetColumnIndexOrThrow(Telephony.Carriers.TYPE)));
        ApnSetting apn = new ApnSetting(
                cursor->GetInt(cursor->GetColumnIndexOrThrow(Telephony.Carriers._ID)),
                cursor->GetString(cursor->GetColumnIndexOrThrow(Telephony.Carriers.NUMERIC)),
                cursor->GetString(cursor->GetColumnIndexOrThrow(Telephony.Carriers.NAME)),
                cursor->GetString(cursor->GetColumnIndexOrThrow(Telephony.Carriers.APN)),
                NetworkUtils->TrimV4AddrZeros(
                        cursor->GetString(
                        cursor->GetColumnIndexOrThrow(Telephony.Carriers.PROXY))),
                cursor->GetString(cursor->GetColumnIndexOrThrow(Telephony.Carriers.PORT)),
                NetworkUtils->TrimV4AddrZeros(
                        cursor->GetString(
                        cursor->GetColumnIndexOrThrow(Telephony.Carriers.MMSC))),
                NetworkUtils->TrimV4AddrZeros(
                        cursor->GetString(
                        cursor->GetColumnIndexOrThrow(Telephony.Carriers.MMSPROXY))),
                cursor->GetString(cursor->GetColumnIndexOrThrow(Telephony.Carriers.MMSPORT)),
                cursor->GetString(cursor->GetColumnIndexOrThrow(Telephony.Carriers.USER)),
                cursor->GetString(cursor->GetColumnIndexOrThrow(Telephony.Carriers.PASSWORD)),
                cursor->GetInt(cursor->GetColumnIndexOrThrow(Telephony.Carriers.AUTH_TYPE)),
                types,
                cursor->GetString(cursor->GetColumnIndexOrThrow(Telephony.Carriers.PROTOCOL)),
                cursor->GetString(cursor->GetColumnIndexOrThrow(
                        Telephony.Carriers.ROAMING_PROTOCOL)),
                cursor->GetInt(cursor->GetColumnIndexOrThrow(
                        Telephony.Carriers.CARRIER_ENABLED)) == 1,
                cursor->GetInt(cursor->GetColumnIndexOrThrow(Telephony.Carriers.BEARER)),
                cursor->GetInt(cursor->GetColumnIndexOrThrow(Telephony.Carriers.PROFILE_ID)),
                cursor->GetInt(cursor->GetColumnIndexOrThrow(
                        Telephony.Carriers.MODEM_COGNITIVE)) == 1,
                cursor->GetInt(cursor->GetColumnIndexOrThrow(Telephony.Carriers.MAX_CONNS)),
                cursor->GetInt(cursor->GetColumnIndexOrThrow(
                        Telephony.Carriers.WAIT_TIME)),
                cursor->GetInt(cursor->GetColumnIndexOrThrow(Telephony.Carriers.MAX_CONNS_TIME)),
                cursor->GetInt(cursor->GetColumnIndexOrThrow(Telephony.Carriers.MTU)),
                cursor->GetString(cursor->GetColumnIndexOrThrow(Telephony.Carriers.MVNO_TYPE)),
                cursor->GetString(cursor->GetColumnIndexOrThrow(Telephony.Carriers.MVNO_MATCH_DATA)));
        return apn;
    }

    private ArrayList<ApnSetting> CreateApnList(Cursor cursor) {
        ArrayList<ApnSetting> mnoApns = new ArrayList<ApnSetting>();
        ArrayList<ApnSetting> mvnoApns = new ArrayList<ApnSetting>();
        IccRecords r = mIccRecords->Get();

        If (cursor->MoveToFirst()) {
            do {
                ApnSetting apn = MakeApnSetting(cursor);
                If (apn == NULL) {
                    continue;
                }

                If (apn->HasMvnoParams()) {
                    If (r != NULL && MvnoMatches(r, apn.mvnoType, apn.mvnoMatchData)) {
                        mvnoApns->Add(apn);
                    }
                } else {
                    mnoApns->Add(apn);
                }
            } While (cursor->MoveToNext());
        }

        ArrayList<ApnSetting> result = mvnoApns->IsEmpty() ? mnoApns : mvnoApns;
        If (DBG) Log("createApnList: X result=" + result);
        return result;
    }

    private Boolean DataConnectionNotInUse(DcAsyncChannel dcac) {
        If (DBG) Log("dataConnectionNotInUse: check if dcac is inuse dcac=" + dcac);
        For (ApnContext apnContext : mApnContexts->Values()) {
            If (apnContext->GetDcAc() == dcac) {
                If (DBG) Log("dataConnectionNotInUse: in use by apnContext=" + apnContext);
                return FALSE;
            }
        }
        // TODO: Fix retry handling so free DataConnections have empty apnlists.
        // Probably move retry handling into DataConnections and reduce complexity
        // of DCT.
        If (DBG) Log("dataConnectionNotInUse: tearDownAll");
        dcac->TearDownAll("No connection", NULL);
        If (DBG) Log("dataConnectionNotInUse: not in use return TRUE");
        return TRUE;
    }

    private DcAsyncChannel FindFreeDataConnection() {
        For (DcAsyncChannel dcac : mDataConnectionAcHashMap->Values()) {
            If (dcac->IsInactiveSync() && DataConnectionNotInUse(dcac)) {
                If (DBG) {
                    Log("findFreeDataConnection: found free DataConnection=" +
                        " dcac=" + dcac);
                }
                return dcac;
            }
        }
        Log("findFreeDataConnection: NO free DataConnection");
        return NULL;
    }

    private Boolean SetupData(ApnContext apnContext, Int32 radioTech) {
        If (DBG) Log("setupData: apnContext=" + apnContext);
        ApnSetting apnSetting;
        DcAsyncChannel dcac = NULL;

        apnSetting = apnContext->GetNextWaitingApn();
        If (apnSetting == NULL) {
            If (DBG) Log("setupData: return for no apn found!");
            return FALSE;
        }

        Int32 profileId = apnSetting.profileId;
        If (profileId == 0) {
            profileId = GetApnProfileID(apnContext->GetApnType());
        }

        // On CDMA, if we're explicitly asking for DUN, we need have
        // a dun-profiled connection so we can't share an existing one
        // On GSM/LTE we can share existing apn connections provided they support
        // this type.
        If (apnContext->GetApnType() != PhoneConstants.APN_TYPE_DUN ||
                TeardownForDun() == FALSE) {
            dcac = CheckForCompatibleConnectedApnContext(apnContext);
            If (dcac != NULL) {
                // Get the dcacApnSetting for the connection we want to share.
                ApnSetting dcacApnSetting = dcac->GetApnSettingSync();
                If (dcacApnSetting != NULL) {
                    // Setting is good, so use it.
                    apnSetting = dcacApnSetting;
                }
            }
        }
        If (dcac == NULL) {
            If (IsOnlySingleDcAllowed(radioTech)) {
                If (IsHigherPriorityApnContextActive(apnContext)) {
                    If (DBG) {
                        Log("setupData: Higher priority ApnContext active.  Ignoring call");
                    }
                    return FALSE;
                }

                // Only lower priority calls left.  Disconnect them all in this single PDP case
                // so that we can bring up the requested higher priority Call (once we receive
                // repsonse for deactivate request for the calls we are about to disconnect
                If (CleanUpAllConnections(TRUE, Phone.REASON_SINGLE_PDN_ARBITRATION)) {
                    // If any call actually requested to be disconnected, means we can't
                    // bring up this connection yet as we need to wait for those data calls
                    // to be disconnected.
                    If (DBG) Log("setupData: Some calls are disconnecting first.  Wait and retry");
                    return FALSE;
                }

                // No other calls are active, so proceed
                If (DBG) Log("setupData: Single pdp. Continue setting up data call.");
            }

            dcac = FindFreeDataConnection();

            If (dcac == NULL) {
                dcac = CreateDataConnection();
            }

            If (dcac == NULL) {
                If (DBG) Log("setupData: No free DataConnection and couldn't create one, WEIRD");
                return FALSE;
            }
        }
        If (DBG) Log("setupData: dcac=" + dcac + " apnSetting=" + apnSetting);

        apnContext->SetDataConnectionAc(dcac);
        apnContext->SetApnSetting(apnSetting);
        apnContext->SetState(DctConstants.State.CONNECTING);
        mPhone->NotifyDataConnection(apnContext->GetReason(), apnContext->GetApnType());

        Message msg = ObtainMessage();
        msg.what = DctConstants.EVENT_DATA_SETUP_COMPLETE;
        msg.obj = apnContext;
        dcac->BringUp(apnContext, GetInitialMaxRetry(), profileId, radioTech, mAutoAttachOnCreation,
                msg);

        If (DBG) Log("setupData: initing!");
        return TRUE;
    }

    /**
     * Handles changes to the APN database.
     */
    private void OnApnChanged() {
        If (DBG) Log("onApnChanged: tryRestartDataConnections");
        TryRestartDataConnections(TRUE, Phone.REASON_APN_CHANGED);
    }

    private void TryRestartDataConnections(Boolean isCleanupNeeded, String reason) {
        DctConstants.State overallState = GetOverallState();
        Boolean isDisconnected = (overallState == DctConstants.State.IDLE ||
                overallState == DctConstants.State.FAILED);

        If (mPhone instanceof GSMPhone) {
            // The "current" may no longer be valid.  MMS depends on this to send properly. TBD
            ((GSMPhone)mPhone).UpdateCurrentCarrierInProvider();
        }

        // TODO: It'd be nice to only do this if the changed Entrie(s)
        // match the current operator.
        If (DBG) Log("tryRestartDataConnections: createAllApnList and cleanUpAllConnections");
        CreateAllApnList();
        SetInitialAttachApn();
        If (isCleanupNeeded) {
            CleanUpAllConnections(!isDisconnected, reason);
        }
        // If the state is already connected don't setup data now.
        If (isDisconnected) {
            SetupDataOnConnectableApns(reason);
        }
    }

    private void OnWwanIwlanCoexistenceDone(AsyncResult ar) {
        If (ar.exception != NULL) {
            Log("onWwanIwlanCoexistenceDone: error = " + ar.exception);
        } else {
            Byte[] array = (Byte[])ar.result;
            Log("onWwanIwlanCoexistenceDone, payload hexdump = "
                    + IccUtils.BytesToHexString (array));
            ByteBuffer oemHookResponse = ByteBuffer->Wrap(array);
            oemHookResponse->Order(ByteOrder->NativeOrder());
            Int32 resp = oemHookResponse->Get();
            Log("onWwanIwlanCoexistenceDone: resp = " + resp);

            Boolean tempStatus = (resp > 0)? TRUE : FALSE;

            If (mWwanIwlanCoexistFlag == tempStatus) {
                Log("onWwanIwlanCoexistenceDone: no change in status, ignore.");
                return;
            }
            mWwanIwlanCoexistFlag = tempStatus;

            If (mPhone->GetServiceState()->GetRilDataRadioTechnology()
                    == ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN) {
                Log("notifyDataConnection IWLAN_AVAILABLE");
                NotifyDataConnection(Phone.REASON_IWLAN_AVAILABLE);
            }

        }
    }

    private void OnModemApnProfileReady() {
        If (mState == DctConstants.State.FAILED) {
            CleanUpAllConnections(FALSE, Phone.REASON_PS_RESTRICT_ENABLED);
        }
        If (DBG) Log("OMH: OnModemApnProfileReady(): Setting up data call");
        TryRestartDataConnections(FALSE, Phone.REASON_SIM_LOADED);
    }

    /**
     * @param cid Connection id provided from RIL.
     * @return DataConnectionAc associated with specified cid.
     */
    private DcAsyncChannel FindDataConnectionAcByCid(Int32 cid) {
        For (DcAsyncChannel dcac : mDataConnectionAcHashMap->Values()) {
            If (dcac->GetCidSync() == cid) {
                return dcac;
            }
        }
        return NULL;
    }

    // TODO: For multiple Active APNs not exactly sure how to do this.
    //@Override
    protected void GotoIdleAndNotifyDataConnection(String reason) {
        If (DBG) Log("gotoIdleAndNotifyDataConnection: reason=" + reason);
        NotifyDataConnection(reason);
        mActiveApn = NULL;
    }

    /**
     * "Active" here means ApnContext IsEnabled() and not in FAILED state
     * @param apnContext to compare with
     * @return TRUE if higher priority active apn found
     */
    private Boolean IsHigherPriorityApnContextActive(ApnContext apnContext) {
        For (ApnContext otherContext : mPrioritySortedApnContexts) {
            If (apnContext->GetApnType()->EqualsIgnoreCase(otherContext->GetApnType())) return FALSE;
            If (otherContext->IsEnabled() && otherContext->GetState() != DctConstants.State.FAILED) {
                return TRUE;
            }
        }
        return FALSE;
    }

    /**
     * Reports if we support multiple connections or not.
     * This is a combination of factors, based on carrier and RAT.
     * @param rilRadioTech the RIL Radio Tech currently in use
     * @return TRUE if only single DataConnection is allowed
     */
    private Boolean IsOnlySingleDcAllowed(Int32 rilRadioTech) {
        Int32[] singleDcRats = mPhone->GetContext()->GetResources().GetIntArray(
                R.array.config_onlySingleDcAllowed);
        Boolean onlySingleDcAllowed = FALSE;
        If (Build.IS_DEBUGGABLE &&
                SystemProperties->GetBoolean("persist.telephony.test.singleDc", FALSE)) {
            onlySingleDcAllowed = TRUE;
        }
        If (singleDcRats != NULL) {
            For (Int32 i=0; i < singleDcRats.length && onlySingleDcAllowed == FALSE; i++) {
                If (rilRadioTech == singleDcRats[i]) onlySingleDcAllowed = TRUE;
            }
        }

        If (DBG) Log("IsOnlySingleDcAllowed(" + rilRadioTech + "): " + onlySingleDcAllowed);
        return onlySingleDcAllowed;
    }

    //@Override
    protected void RestartRadio() {
        If (DBG) Log("restartRadio: ************TURN OFF RADIO**************");
        CleanUpAllConnections(TRUE, Phone.REASON_RADIO_TURNED_OFF);
        mPhone->GetServiceStateTracker()->PowerOffRadioSafely(this);
        /* Note: no need to call SetRadioPower(TRUE).  Assuming the desired
         * radio power state is still ON (as tracked by ServiceStateTracker),
         * ServiceStateTracker will call setRadioPower when it receives the
         * RADIO_STATE_CHANGED notification for the power off.  And if the
         * desired power state has changed in the interim, we don't want to
         * override it with an unconditional power on.
         */

        Int32 reset = Integer->ParseInt(SystemProperties->Get("net.ppp.reset-by-timeout", "0"));
        SystemProperties->Set("net.ppp.reset-by-timeout", String->ValueOf(reset+1));
    }

    /**
     * Return TRUE if data connection need to be setup after disconnected due to
     * reason.
     *
     * @param reason the reason why data is disconnected
     * @return TRUE if try setup data connection is need for this reason
     */
    private Boolean RetryAfterDisconnected(ApnContext apnContext) {
        Boolean retry = TRUE;
        String reason = apnContext->GetReason();

        If ( Phone.REASON_RADIO_TURNED_OFF->Equals(reason) ||
                (IsOnlySingleDcAllowed(mPhone->GetServiceState()->GetRilDataRadioTechnology())
                 && IsHigherPriorityApnContextActive(apnContext))) {
            retry = FALSE;
        }
        return retry;
    }

    private void StartAlarmForReconnect(Int32 delay, ApnContext apnContext) {
        String apnType = apnContext->GetApnType();

        Intent intent = new Intent(INTENT_RECONNECT_ALARM + "." + apnType);
        intent->PutExtra(INTENT_RECONNECT_ALARM_EXTRA_REASON, apnContext->GetReason());
        intent->PutExtra(INTENT_RECONNECT_ALARM_EXTRA_TYPE, apnType);

        // Get current sub id.
        Int64 subId = SubscriptionManager->GetDefaultDataSubId();
        intent->PutExtra(PhoneConstants.SUBSCRIPTION_KEY, subId);

        If (DBG) {
            Log("startAlarmForReconnect: delay=" + delay + " action=" + intent->GetAction()
                    + " apn=" + apnContext);
        }

        PendingIntent alarmIntent = PendingIntent.GetBroadcast (mPhone->GetContext(), 0,
                                        intent, PendingIIntent::FLAG_UPDATE_CURRENT);
        apnContext->SetReconnectIntent(alarmIntent);
        mAlarmManager->Set(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                SystemClock->ElapsedRealtime() + delay, alarmIntent);
    }

    private void StartAlarmForRestartTrySetup(Int32 delay, ApnContext apnContext) {
        String apnType = apnContext->GetApnType();
        Intent intent = new Intent(INTENT_RESTART_TRYSETUP_ALARM + "." + apnType);
        intent->PutExtra(INTENT_RESTART_TRYSETUP_ALARM_EXTRA_TYPE, apnType);

        If (DBG) {
            Log("startAlarmForRestartTrySetup: delay=" + delay + " action=" + intent->GetAction()
                    + " apn=" + apnContext);
        }
        PendingIntent alarmIntent = PendingIntent.GetBroadcast (mPhone->GetContext(), 0,
                                        intent, PendingIIntent::FLAG_UPDATE_CURRENT);
        apnContext->SetReconnectIntent(alarmIntent);
        mAlarmManager->Set(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                SystemClock->ElapsedRealtime() + delay, alarmIntent);
    }

    private void NotifyNoData(DcFailCause lastFailCauseCode,
                              ApnContext apnContext) {
        If (DBG) Log( "notifyNoData: type=" + apnContext->GetApnType());
        If (IsPermanentFail(lastFailCauseCode)
            && (!apnContext->GetApnType()->Equals(PhoneConstants.APN_TYPE_DEFAULT))) {
            mPhone->NotifyDataConnectionFailed(apnContext->GetReason(), apnContext->GetApnType());
        }
    }

    private void OnRecordsLoaded() {
        If (mOmhApt != NULL) {
            Log("OMH: OnRecordsLoaded(): calling LoadProfiles()");
            /* query for data profiles stored in the modem */
            mOmhApt->LoadProfiles();
            If (mPhone.mCi->GetRadioState()->IsOn()) {
                If (DBG) Log("onRecordsLoaded: notifying data availability");
                NotifyOffApnsOfAvailability(Phone.REASON_SIM_LOADED);
            }
        } else {
            If (DBG) Log("onRecordsLoaded: createAllApnList");
            If (mPhone.mCi->GetRadioState()->IsOn()) {
                If (DBG) Log("onRecordsLoaded: notifying data availability");
                NotifyOffApnsOfAvailability(Phone.REASON_SIM_LOADED);
            }
            TryRestartDataConnections(FALSE, Phone.REASON_SIM_LOADED);
       }
    }

    private void OnNvReady() {
        If (DBG) Log("onNvReady");
        CreateAllApnList();
        SetupDataOnConnectableApns(Phone.REASON_NV_READY);
    }

    //@Override
    protected void OnSetDependencyMet(String apnType, Boolean met) {
        // don't allow users to tweak hipri to work around default dependency not met
        If (PhoneConstants.APN_TYPE_HIPRI->Equals(apnType)) return;

        ApnContext apnContext = mApnContexts->Get(apnType);
        If (apnContext == NULL) {
            Loge("onSetDependencyMet: ApnContext not found in OnSetDependencyMet(" +
                    apnType + ", " + met + ")");
            return;
        }
        ApplyNewState(apnContext, apnContext->IsEnabled(), met);
        If (PhoneConstants.APN_TYPE_DEFAULT->Equals(apnType)) {
            // tie actions on default to similar actions on HIPRI regarding dependencyMet
            apnContext = mApnContexts->Get(PhoneConstants.APN_TYPE_HIPRI);
            If (apnContext != NULL) ApplyNewState(apnContext, apnContext->IsEnabled(), met);
        }
    }

    private void ApplyNewState(ApnContext apnContext, Boolean enabled, Boolean met) {
        Boolean cleanup = FALSE;
        Boolean trySetup = FALSE;
        If (DBG) {
            Log("ApplyNewState(" + apnContext->GetApnType() + ", " + enabled +
                    "(" + apnContext->IsEnabled() + "), " + met + "(" +
                    apnContext->GetDependencyMet() +"))");
        }
        If (apnContext->IsReady()) {
            cleanup = TRUE;
            If (enabled && met) {
                DctConstants.State state = apnContext->GetState();
                Switch(state) {
                    case CONNECTING:
                    case SCANNING:
                    case CONNECTED:
                    case DISCONNECTING:
                        // We're "READY" and active so just return
                        If (DBG) Log("applyNewState: 'ready' so return");
                        return;
                    case IDLE:
                        // fall through: this is unexpected but if it happens cleanup and try setup
                    case FAILED:
                    case RETRYING: {
                        // We're "READY" but not active so Disconnect (cleanup = TRUE) and
                        // Connect (trySetup = TRUE) to be sure we retry the connection.
                        trySetup = TRUE;
                        apnContext->SetReason(Phone.REASON_DATA_ENABLED);
                        break;
                    }
                }
            } else If (met) {
                apnContext->SetReason(Phone.REASON_DATA_DISABLED);
                // If ConnectivityService has disabled this network, stop trying to bring
                // it up, but do not tear it down - ConnectivityService will do that
                // directly by talking with the DataConnection.
                //
                // This doesn't apply to DUN, however.  Those connections have special
                // requirements from carriers and we need stop using them when the dun
                // request goes away.  This applies to both CDMA and GSM because they both
                // can declare the DUN APN sharable by default traffic, thus still satisfying
                // those requests and not torn down organically.
                If ((apnContext->GetApnType() == PhoneConstants.APN_TYPE_DUN && TeardownForDun()) ||
                        (apnContext->GetApnType() == PhoneConstants.APN_TYPE_MMS)) {
                    cleanup = TRUE;
                } else {
                    cleanup = FALSE;
                }
            } else {
                apnContext->SetReason(Phone.REASON_DATA_DEPENDENCY_UNMET);
            }
        } else {
            If (enabled && met) {
                If (apnContext->IsEnabled()) {
                    apnContext->SetReason(Phone.REASON_DATA_DEPENDENCY_MET);
                } else {
                    apnContext->SetReason(Phone.REASON_DATA_ENABLED);
                }
                If (apnContext->GetState() == DctConstants.State.FAILED) {
                    apnContext->SetState(DctConstants.State.IDLE);
                }
                trySetup = TRUE;
            }
        }
        apnContext->SetEnabled(enabled);
        apnContext->SetDependencyMet(met);
        If (cleanup) CleanUpConnection(TRUE, apnContext);
        If (trySetup) TrySetupData(apnContext);
    }

    private DcAsyncChannel CheckForCompatibleConnectedApnContext(ApnContext apnContext) {
        String apnType = apnContext->GetApnType();
        ApnSetting dunSetting = NULL;

        If (PhoneConstants.APN_TYPE_DUN->Equals(apnType)) {
            dunSetting = FetchDunApn();
        }
        If (DBG) {
            Log("checkForCompatibleConnectedApnContext: apnContext=" + apnContext );
        }

        DcAsyncChannel potentialDcac = NULL;
        ApnContext potentialApnCtx = NULL;
        For (ApnContext curApnCtx : mApnContexts->Values()) {
            DcAsyncChannel curDcac = curApnCtx->GetDcAc();
            Log("curDcac: " + curDcac);
            If (curDcac != NULL) {
                ApnSetting apnSetting = curApnCtx->GetApnSetting();
                Log("apnSetting: " + apnSetting);
                If (dunSetting != NULL) {
                    If (dunSetting->Equals(apnSetting)) {
                        Switch (curApnCtx->GetState()) {
                            case CONNECTED:
                                If (DBG) {
                                    Log("checkForCompatibleConnectedApnContext:"
                                            + " found dun conn=" + curDcac
                                            + " curApnCtx=" + curApnCtx);
                                }
                                return curDcac;
                            case RETRYING:
                            case CONNECTING:
                                potentialDcac = curDcac;
                                potentialApnCtx = curApnCtx;
                                break;
                            case DISCONNECTING:
                                //Update for DISCONNECTING only if there is no other potential match
                                If (potentialDcac == NULL) {
                                    potentialDcac = curDcac;
                                    potentialApnCtx = curApnCtx;
                                }
                            default:
                                // Not connected, potential unchanged
                                break;
                        }
                    }
                } else If (apnSetting != NULL && apnSetting->CanHandleType(apnType)) {
                    Switch (curApnCtx->GetState()) {
                        case CONNECTED:
                            If (DBG) {
                                Log("checkForCompatibleConnectedApnContext:"
                                        + " found canHandle conn=" + curDcac
                                        + " curApnCtx=" + curApnCtx);
                            }
                            return curDcac;
                        case RETRYING:
                        case CONNECTING:
                            potentialDcac = curDcac;
                            potentialApnCtx = curApnCtx;
                            break;
                        case DISCONNECTING:
                            // Update for DISCONNECTING only if there is no other potential match
                            If (potentialDcac == NULL) {
                                potentialDcac = curDcac;
                                potentialApnCtx = curApnCtx;
                            }
                        default:
                            // Not connected, potential unchanged
                            break;
                    }
                }
            } else {
                If (VDBG) {
                    Log("checkForCompatibleConnectedApnContext: not conn curApnCtx=" + curApnCtx);
                }
            }
        }
        If (potentialDcac != NULL) {
            If (DBG) {
                Log("checkForCompatibleConnectedApnContext: found potential conn=" + potentialDcac
                        + " curApnCtx=" + potentialApnCtx);
            }
            return potentialDcac;
        }

        If (DBG) Log("checkForCompatibleConnectedApnContext: NO conn apnContext=" + apnContext);
        return NULL;
    }

    //@Override
    protected void OnEnableApn(Int32 apnId, Int32 enabled) {
        ApnContext apnContext = mApnContexts->Get(ApnIdToType(apnId));
        If (apnContext == NULL) {
            Loge("OnEnableApn(" + apnId + ", " + enabled + "): NO ApnContext");
            return;
        }
        // TODO change our retry manager to use the appropriate numbers for the new APN
        If (DBG) Log("onEnableApn: apnContext=" + apnContext + " call applyNewState");
        ApplyNewState(apnContext, enabled == DctConstants.ENABLED, apnContext->GetDependencyMet());
    }

    //@Override
    // TODO: We shouldnt need this.
    protected Boolean OnTrySetupData(String reason) {
        If (DBG) Log("onTrySetupData: reason=" + reason);
        SetupDataOnConnectableApns(reason);
        return TRUE;
    }

    protected Boolean OnTrySetupData(ApnContext apnContext) {
        If (DBG) Log("onTrySetupData: apnContext=" + apnContext);
        return TrySetupData(apnContext);
    }

    //@Override
    protected void OnRoamingOff() {
        If (DBG) Log("onRoamingOff");

        If (mUserDataEnabled == FALSE) return;

        If (GetDataOnRoamingEnabled() == FALSE) {
            NotifyOffApnsOfAvailability(Phone.REASON_ROAMING_OFF);
            SetupDataOnConnectableApns(Phone.REASON_ROAMING_OFF);
        } else {
            NotifyDataConnection(Phone.REASON_ROAMING_OFF);
        }
    }

    //@Override
    protected void OnRoamingOn() {
        If (mUserDataEnabled == FALSE) return;

        If (GetDataOnRoamingEnabled()) {
            If (DBG) Log("onRoamingOn: setup data on roaming");
            SetupDataOnConnectableApns(Phone.REASON_ROAMING_ON);
            NotifyDataConnection(Phone.REASON_ROAMING_ON);
        } else {
            If (DBG) Log("onRoamingOn: Tear down data connection on roaming.");
            CleanUpAllConnections(TRUE, Phone.REASON_ROAMING_ON);
            NotifyOffApnsOfAvailability(Phone.REASON_ROAMING_ON);
        }
    }

    //@Override
    protected void OnRadioAvailable() {
        If (DBG) Log("onRadioAvailable");
        If (mPhone->GetSimulatedRadioControl() != NULL) {
            // Assume data is connected on the simulator
            // FIXME  this can be improved
            // SetState(DctConstants.State.CONNECTED);
            NotifyDataConnection(NULL);

            Log("onRadioAvailable: We're on the simulator; assuming data is connected");
        }

        IccRecords r = mIccRecords->Get();
        If (r != NULL && r->GetRecordsLoaded()) {
            NotifyOffApnsOfAvailability(NULL);
        }

        If (GetOverallState() != DctConstants.State.IDLE) {
            CleanUpConnection(TRUE, NULL);
        }
    }

    //@Override
    protected void OnRadioOffOrNotAvailable() {
        // Make sure our reconnect delay starts at the initial value
        // next time the radio comes on

        mReregisterOnReconnectFailure = FALSE;

        If (mPhone->GetSimulatedRadioControl() != NULL) {
            // Assume data is connected on the simulator
            // FIXME  this can be improved
            Log("We're on the simulator; assuming radio off is meaningless");
        } else {
            If (DBG) Log("onRadioOffOrNotAvailable: is off and clean up all connections");
            CleanUpAllConnections(FALSE, Phone.REASON_RADIO_TURNED_OFF);
        }
        NotifyOffApnsOfAvailability(NULL);
    }

    //@Override
    protected void CompleteConnection(ApnContext apnContext) {
        Boolean isProvApn = apnContext->IsProvisioningApn();

        If (DBG) Log("completeConnection: successful, notify the world apnContext=" + apnContext);

        If (mIsProvisioning && !TextUtils->IsEmpty(mProvisioningUrl)) {
            If (DBG) {
                Log("completeConnection: MOBILE_PROVISIONING_ACTION url="
                        + mProvisioningUrl);
            }
            Intent newIntent = Intent->MakeMainSelectorActivity(IIntent::ACTION_MAIN,
                    Intent.CATEGORY_APP_BROWSER);
            newIntent->SetData(Uri->Parse(mProvisioningUrl));
            newIntent->SetFlags(IIntent::FLAG_ACTIVITY_BROUGHT_TO_FRONT |
                    IIntent::FLAG_ACTIVITY_NEW_TASK);
            try {
                mPhone->GetContext()->StartActivity(newIntent);
            } Catch (ActivityNotFoundException e) {
                Loge("completeConnection: startActivityAsUser failed" + e);
            }
        }
        mIsProvisioning = FALSE;
        mProvisioningUrl = NULL;
        If (mProvisioningSpinner != NULL) {
            SendMessage(ObtainMessage(DctConstants.CMD_CLEAR_PROVISIONING_SPINNER,
                    mProvisioningSpinner));
        }

        mPhone->NotifyDataConnection(apnContext->GetReason(), apnContext->GetApnType());
        StartNetStatPoll();
        StartDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
    }

    /**
     * A SETUP (aka bringUp) has completed, possibly with an error. If
     * there is an error this method will call {@link #onDataSetupCompleteError}.
     */
    //@Override
    protected void OnDataSetupComplete(AsyncResult ar) {

        DcFailCause cause = DcFailCause.UNKNOWN;
        Boolean handleError = FALSE;
        ApnContext apnContext = NULL;

        If(ar.userObj instanceof ApnContext){
            apnContext = (ApnContext)ar.userObj;
        } else {
            throw new RuntimeException("onDataSetupComplete: No apnContext");
        }

        If (ar.exception == NULL) {
            DcAsyncChannel dcac = apnContext->GetDcAc();

            If (RADIO_TESTS) {
                // Note: To change radio.test.onDSC.NULL.dcac from command line you need to
                // adb root and adb remount and from the command line you can only change the
                // value to 1 once. To change it a second time you can reboot or execute
                // adb shell stop and then adb shell start. The command line to set the value is:
                // adb shell sqlite3 /data/data/com.android.providers.settings/databases/settings.db "insert into System (name,value) Values ('radio.test.onDSC.NULL.dcac', '1');"
                ContentResolver cr = mPhone->GetContext()->GetContentResolver();
                String radioTestProperty = "radio.test.onDSC.NULL.dcac";
                If (Settings.System->GetInt(cr, radioTestProperty, 0) == 1) {
                    Log("onDataSetupComplete: " + radioTestProperty +
                            " is TRUE, set dcac to NULL and reset property to FALSE");
                    dcac = NULL;
                    Settings.System->PutInt(cr, radioTestProperty, 0);
                    Log("onDataSetupComplete: " + radioTestProperty + "=" +
                            Settings.System->GetInt(mPhone->GetContext()->GetContentResolver(),
                                    radioTestProperty, -1));
                }
            }
            If (dcac == NULL) {
                Log("onDataSetupComplete: no connection to DC, handle as error");
                cause = DcFailCause.CONNECTION_TO_DATACONNECTIONAC_BROKEN;
                handleError = TRUE;
            } else {
                ApnSetting apn = apnContext->GetApnSetting();
                If (DBG) {
                    Log("onDataSetupComplete: success apn=" + (apn == NULL ? "unknown" : apn.apn));
                }
                If (apn != NULL && apn.proxy != NULL && apn.proxy->Length() != 0) {
                    try {
                        String port = apn.port;
                        If (TextUtils->IsEmpty(port)) port = "8080";
                        ProxyInfo proxy = new ProxyInfo(apn.proxy,
                                Integer->ParseInt(port), NULL);
                        dcac->SetLinkPropertiesHttpProxySync(proxy);
                    } Catch (NumberFormatException e) {
                        Loge("onDataSetupComplete: NumberFormatException making ProxyProperties (" +
                                apn.port + "): " + e);
                    }
                }

                // everything is setup
                If(TextUtils->Equals(apnContext->GetApnType(),PhoneConstants.APN_TYPE_DEFAULT)) {
                    SystemProperties->Set(PUPPET_MASTER_RADIO_STRESS_TEST, "TRUE");
                    If (mCanSetPreferApn && mPreferredApn == NULL) {
                        If (DBG) Log("onDataSetupComplete: PREFERED APN is NULL");
                        mPreferredApn = apn;
                        If (mPreferredApn != NULL) {
                            SetPreferredApn(mPreferredApn.id);
                        }
                    }
                } else {
                    SystemProperties->Set(PUPPET_MASTER_RADIO_STRESS_TEST, "FALSE");
                }

                // A connection is setup
                apnContext->SetState(DctConstants.State.CONNECTED);
                Boolean isProvApn = apnContext->IsProvisioningApn();
                final ConnectivityManager cm = ConnectivityManager->From(mPhone->GetContext());
                If (mProvisionBroadcastReceiver != NULL) {
                    mPhone->GetContext()->UnregisterReceiver(mProvisionBroadcastReceiver);
                    mProvisionBroadcastReceiver = NULL;
                }
                If ((!isProvApn) || mIsProvisioning) {
                    // Hide any provisioning notification.
                    cm->SetProvisioningNotificationVisible(FALSE, ConnectivityManager.TYPE_MOBILE,
                            mProvisionActionName);
                    // Complete the connection normally notifying the world we're connected.
                    // We do this if this isn't a special provisioning apn or if we've been
                    // told its time to provision.
                    CompleteConnection(apnContext);
                } else {
                    // This is a provisioning APN that we're reporting as connected. Later
                    // when the user desires to upgrade this to a "default" connection,
                    // mIsProvisioning == TRUE, we'll go through the code path above.
                    // mIsProvisioning becomes TRUE when CMD_ENABLE_MOBILE_PROVISIONING
                    // is sent to the DCT.
                    If (DBG) {
                        Log("onDataSetupComplete: successful, BUT send connected to prov apn as"
                                + " mIsProvisioning:" + mIsProvisioning + " == FALSE"
                                + " && (isProvisioningApn:" + isProvApn + " == TRUE");
                    }

                    // While radio is up, grab provisioning URL.  The URL contains ICCID which
                    // disappears when radio is off.
                    mProvisionBroadcastReceiver = new ProvisionNotificationBroadcastReceiver(
                            cm->GetMobileProvisioningUrl(),
                            TelephonyManager->GetDefault()->GetNetworkOperatorName());
                    mPhone->GetContext()->RegisterReceiver(mProvisionBroadcastReceiver,
                            new IntentFilter(mProvisionActionName));
                    // Put up user notification that sign-in is required.
                    cm->SetProvisioningNotificationVisible(TRUE, ConnectivityManager.TYPE_MOBILE,
                            mProvisionActionName);
                    // Turn off radio to save battery and avoid wasting carrier resources.
                    // The network isn't usable and network validation will just fail anyhow.
                    SetRadio(FALSE);

                    Intent intent = new Intent(
                            TelephonyIntents.ACTION_DATA_CONNECTION_CONNECTED_TO_PROVISIONING_APN);
                    intent->PutExtra(PhoneConstants.DATA_APN_KEY, apnContext->GetApnSetting().apn);
                    intent->PutExtra(PhoneConstants.DATA_APN_TYPE_KEY, apnContext->GetApnType());

                    String apnType = apnContext->GetApnType();
                    LinkProperties linkProperties = GetLinkProperties(apnType);
                    If (linkProperties != NULL) {
                        intent->PutExtra(PhoneConstants.DATA_LINK_PROPERTIES_KEY, linkProperties);
                        String iface = linkProperties->GetInterfaceName();
                        If (iface != NULL) {
                            intent->PutExtra(PhoneConstants.DATA_IFACE_NAME_KEY, iface);
                        }
                    }
                    NetworkCapabilities networkCapabilities = GetNetworkCapabilities(apnType);
                    If (networkCapabilities != NULL) {
                        intent->PutExtra(PhoneConstants.DATA_NETWORK_CAPABILITIES_KEY,
                                networkCapabilities);
                    }

                    mPhone->GetContext()->SendBroadcastAsUser(intent, UserHandle.ALL);
                }
                If (DBG) {
                    Log("onDataSetupComplete: SETUP complete type=" + apnContext->GetApnType()
                        + ", reason:" + apnContext->GetReason());
                }
            }
        } else {
            cause = (DcFailCause) (ar.result);
            If (DBG) {
                ApnSetting apn = apnContext->GetApnSetting();
                Log(String->Format("onDataSetupComplete: error apn=%s cause=%s",
                        (apn == NULL ? "unknown" : apn.apn), cause));
            }

            If (cause == NULL) {
                cause = DcFailCause.UNKNOWN;
            }

            If (cause->IsEventLoggable()) {
                // Log this failure to the Event Logs.
                Int32 cid = GetCellLocationId();
                EventLog->WriteEvent(EventLogTags.PDP_SETUP_FAIL,
                        cause->Ordinal(), cid, TelephonyManager->GetDefault()->GetNetworkType());
            }
            ApnSetting apn = apnContext->GetApnSetting();
            mPhone->NotifyPreciseDataConnectionFailed(apnContext->GetReason(),
                    apnContext->GetApnType(), apn != NULL ? apn.apn : "unknown", cause->ToString());

            // Count permanent failures and remove the APN we just tried
            If (IsPermanentFail(cause)) apnContext->DecWaitingApnsPermFailCount();

            apnContext->RemoveWaitingApn(apnContext->GetApnSetting());
            If (DBG) {
                If (apnContext->GetWaitingApns() == NULL){
                    Log(String->Format("onDataSetupComplete: WaitingApns.size = NULL" +
                            " WaitingApnsPermFailureCountDown=%d",
                            apnContext->GetWaitingApnsPermFailCount()));
                }
                else {
                    Log(String->Format("onDataSetupComplete: WaitingApns.size=%d" +
                            " WaitingApnsPermFailureCountDown=%d",
                            apnContext->GetWaitingApns()->Size(),
                            apnContext->GetWaitingApnsPermFailCount()));
                }
            }
            handleError = TRUE;
        }

        If (handleError) {
            OnDataSetupCompleteError(ar);
        }

        /* If flag is set to FALSE after SETUP_DATA_CALL is invoked, we need
         * to clean data connections.
         */
        If (!mInternalDataEnabled) {
            CleanUpAllConnections(NULL);
        }

    }

    /**
     * @return number of milli-seconds to delay between trying apns'
     */
    private Int32 GetApnDelay(String reason) {
        If (mFailFast || Phone.REASON_NW_TYPE_CHANGED->Equals(reason) ||
                Phone.REASON_APN_CHANGED->Equals(reason)) {
            return SystemProperties->GetInt("persist.radio.apn_ff_delay",
                    APN_FAIL_FAST_DELAY_DEFAULT_MILLIS);
        } else {
            return SystemProperties->GetInt("persist.radio.apn_delay", APN_DELAY_DEFAULT_MILLIS);
        }
    }

    /**
     * Error has occurred during the SETUP {aka bringUP} request and the DCT
     * should either try the next waiting APN or start over from the
     * beginning if the list is empty. Between each SETUP request there will
     * be a delay defined by {@link #GetApnDelay()}.
     */
    //@Override
    protected void OnDataSetupCompleteError(AsyncResult ar) {
        String reason = "";
        ApnContext apnContext = NULL;

        If(ar.userObj instanceof ApnContext){
            apnContext = (ApnContext)ar.userObj;
        } else {
            throw new RuntimeException("onDataSetupCompleteError: No apnContext");
        }

        // See if there are more APN's to try
        If (apnContext->GetWaitingApns() == NULL || apnContext->GetWaitingApns()->IsEmpty()) {
            apnContext->SetState(DctConstants.State.FAILED);
            mPhone->NotifyDataConnection(Phone.REASON_APN_FAILED, apnContext->GetApnType());

            apnContext->SetDataConnectionAc(NULL);

            If (apnContext->GetWaitingApnsPermFailCount() == 0) {
                If (DBG) {
                    Log("onDataSetupCompleteError: All APN's had permanent failures, stop retrying");
                }
            } else {
                Int32 delay = GetApnDelay(Phone.REASON_APN_FAILED);
                If (DBG) {
                    Log("onDataSetupCompleteError: Not all APN's had permanent failures delay="
                            + delay);
                }
                StartAlarmForRestartTrySetup(delay, apnContext);
            }
        } else {
            If (DBG) Log("onDataSetupCompleteError: Try next APN");
            apnContext->SetState(DctConstants.State.SCANNING);
            // Wait a bit before trying the next APN, so that
            // we're not tying up the RIL command channel
            StartAlarmForReconnect(GetApnDelay(Phone.REASON_APN_FAILED), apnContext);
        }
    }

    /**
     * Called when EVENT_DISCONNECT_DONE is received.
     */
    //@Override
    protected void OnDisconnectDone(Int32 connId, AsyncResult ar) {
        ApnContext apnContext = NULL;

        If (ar.userObj instanceof ApnContext) {
            apnContext = (ApnContext) ar.userObj;
        } else {
            Loge("onDisconnectDone: Invalid ar in onDisconnectDone, ignore");
            return;
        }

        If(DBG) Log("onDisconnectDone: EVENT_DISCONNECT_DONE apnContext=" + apnContext);
        apnContext->SetState(DctConstants.State.IDLE);

        mPhone->NotifyDataConnection(apnContext->GetReason(), apnContext->GetApnType());

        // if all data connection are gone, check whether Airplane mode request was
        // pending.
        If (IsDisconnected()) {
            If (mPhone->GetServiceStateTracker()->ProcessPendingRadioPowerOffAfterDataOff()) {
                If(DBG) Log("onDisconnectDone: radio will be turned off, no retries");
                // Radio will be turned off. No need to retry data setup
                apnContext->SetApnSetting(NULL);
                apnContext->SetDataConnectionAc(NULL);

                // Need to notify disconnect as well, in the case of switching Airplane mode.
                // Otherwise, it would cause 30s delayed to turn on Airplane mode.
                If (mDisconnectPendingCount > 0)
                    mDisconnectPendingCount--;

                If (mDisconnectPendingCount == 0) {
                    NotifyDataDisconnectComplete();
                    NotifyAllDataDisconnected();
                }
                return;
            }
        }

        // If APN is still enabled, try to bring it back up automatically
        If (mAttached->Get() && apnContext->IsReady() && RetryAfterDisconnected(apnContext)) {
            SystemProperties->Set(PUPPET_MASTER_RADIO_STRESS_TEST, "FALSE");
            // Wait a bit before trying the next APN, so that
            // we're not tying up the RIL command channel.
            // This also helps in any external dependency to turn off the context.
            If(DBG) Log("onDisconnectDone: attached, ready and retry after disconnect");
            StartAlarmForReconnect(GetApnDelay(apnContext->GetReason()), apnContext);
        } else {
            Boolean restartRadioAfterProvisioning = mPhone->GetContext()->GetResources().GetBoolean(
                    R.bool.config_restartRadioAfterProvisioning);

            If (apnContext->IsProvisioningApn() && restartRadioAfterProvisioning) {
                Log("onDisconnectDone: restartRadio after provisioning");
                RestartRadio();
            }
            apnContext->SetApnSetting(NULL);
            apnContext->SetDataConnectionAc(NULL);
            If (IsOnlySingleDcAllowed(mPhone->GetServiceState()->GetRilDataRadioTechnology())) {
                If(DBG) Log("onDisconnectDone: isOnlySigneDcAllowed TRUE so setup single apn");
                SetupDataOnConnectableApns(Phone.REASON_SINGLE_PDN_ARBITRATION);
            } else {
                If(DBG) Log("onDisconnectDone: not retrying");
            }
        }

        If (mDisconnectPendingCount > 0)
            mDisconnectPendingCount--;

        If (mDisconnectPendingCount == 0) {
            NotifyDataDisconnectComplete();
            NotifyAllDataDisconnected();
        }

    }

    /**
     * Called when EVENT_DISCONNECT_DC_RETRYING is received.
     */
    //@Override
    protected void OnDisconnectDcRetrying(Int32 connId, AsyncResult ar) {
        // We could just do this in DC!!!
        ApnContext apnContext = NULL;

        If (ar.userObj instanceof ApnContext) {
            apnContext = (ApnContext) ar.userObj;
        } else {
            Loge("onDisconnectDcRetrying: Invalid ar in onDisconnectDone, ignore");
            return;
        }

        apnContext->SetState(DctConstants.State.RETRYING);
        If(DBG) Log("onDisconnectDcRetrying: apnContext=" + apnContext);

        mPhone->NotifyDataConnection(apnContext->GetReason(), apnContext->GetApnType());
    }


    //@Override
    protected void OnVoiceCallStarted() {
        If (DBG) Log("onVoiceCallStarted");
        mInVoiceCall = TRUE;
        If (IsConnected() && ! mPhone->GetServiceStateTracker()->IsConcurrentVoiceAndDataAllowed()) {
            If (DBG) Log("onVoiceCallStarted stop polling");
            StopNetStatPoll();
            StopDataStallAlarm();
            NotifyDataConnection(Phone.REASON_VOICE_CALL_STARTED);
        }
    }

    //@Override
    protected void OnVoiceCallEnded() {
        If (DBG) Log("onVoiceCallEnded");
        mInVoiceCall = FALSE;
        If (IsConnected()) {
            If (!mPhone->GetServiceStateTracker()->IsConcurrentVoiceAndDataAllowed()) {
                StartNetStatPoll();
                StartDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
                NotifyDataConnection(Phone.REASON_VOICE_CALL_ENDED);
            } else {
                // clean slate after call end.
                ResetPollStats();
            }
        }
        // reset reconnect timer
        SetupDataOnConnectableApns(Phone.REASON_VOICE_CALL_ENDED);
    }

    //@Override
    protected void OnCleanUpConnection(Boolean tearDown, Int32 apnId, String reason) {
        If (DBG) Log("onCleanUpConnection");
        ApnContext apnContext = mApnContexts->Get(ApnIdToType(apnId));
        If (apnContext != NULL) {
            apnContext->SetReason(reason);
            CleanUpConnection(tearDown, apnContext);
        }
    }

    //@Override
    protected Boolean IsConnected() {
        For (ApnContext apnContext : mApnContexts->Values()) {
            If (apnContext->GetState() == DctConstants.State.CONNECTED) {
                // At least one context is connected, return TRUE
                return TRUE;
            }
        }
        // There are not any contexts connected, return FALSE
        return FALSE;
    }

    //@Override
    public Boolean IsDisconnected() {
        For (ApnContext apnContext : mApnContexts->Values()) {
            If (!apnContext->IsDisconnected()) {
                // At least one context was not disconnected return FALSE
                return FALSE;
            }
        }
        // All contexts were disconnected so return TRUE
        return TRUE;
    }

    //@Override
    protected void NotifyDataConnection(String reason) {
        If (DBG) Log("notifyDataConnection: reason=" + reason);
        For (ApnContext apnContext : mApnContexts->Values()) {
            If ((mAttached->Get() || !mOosIsDisconnect) && apnContext->IsReady()) {
                If (DBG) Log("notifyDataConnection: type:" + apnContext->GetApnType());
                mPhone->NotifyDataConnection(reason != NULL ? reason : apnContext->GetReason(),
                        apnContext->GetApnType());
            }
        }
        NotifyOffApnsOfAvailability(reason);
    }

    private Boolean IsNvSubscription() {
        Int32 radioTech = mPhone->GetServiceState()->GetRilVoiceRadioTechnology();
        If (mCdmaSsm == NULL) {
            return FALSE;
        }
        If (UiccController->GetFamilyFromRadioTechnology(radioTech) == UiccController.APP_FAM_3GPP2
                && mCdmaSsm->GetCdmaSubscriptionSource() ==
                        CdmaSubscriptionSourceManager.SUBSCRIPTION_FROM_NV) {
            return TRUE;
        }
        return FALSE;
    }

    /**
     * Returns mccmnc for data call either from cdma_home_operator or from IccRecords
     * @return operator numeric
     */
    private String GetOperatorNumeric() {
        String result;
        If (IsNvSubscription()) {
            result = SystemProperties->Get(CDMAPhone.PROPERTY_CDMA_HOME_OPERATOR_NUMERIC);
            Log("getOperatorNumberic - returning from NV: " + result);
        } else {
            IccRecords r = mIccRecords->Get();
            result = (r != NULL) ? r->GetOperatorNumeric() : "";
            Log("getOperatorNumberic - returning from card: " + result);
        }
        If (result == NULL) result = "";
        return result;
    }

    /**
     * Based on the operator numeric, create a list for all possible
     * Data Connections and setup the preferredApn.
     */
    private void CreateAllApnList() {
        mAllApnSettings->Clear();
        String operator = GetOperatorNumeric();
        Int32 radioTech = mPhone->GetServiceState()->GetRilDataRadioTechnology();

        If (mOmhApt != NULL && (ServiceState->IsCdma(radioTech) &&
                ServiceState.RIL_RADIO_TECHNOLOGY_EHRPD != radioTech)) {
            ArrayList<ApnSetting> mOmhApnsList = new ArrayList<ApnSetting>();
            mOmhApnsList = mOmhApt->GetOmhApnProfilesList();
            If (!mOmhApnsList->IsEmpty()) {
                If (DBG) Log("createAllApnList: Copy Omh profiles");
                mAllApnSettings->AddAll(mOmhApnsList);
            }
        }

        If (mAllApnSettings->IsEmpty()) {
            If (operator != NULL && !operator->IsEmpty()) {
                String selection = "numeric = '" + operator + "'";
                // query only enabled apn.
                // carrier_enabled : 1 means enabled apn, 0 disabled apn.
                selection += " and carrier_enabled = 1";
                If (DBG) Log("createAllApnList: selection=" + selection);

                Cursor cursor = mPhone->GetContext()->GetContentResolver().Query(
                        Telephony.Carriers.CONTENT_URI, NULL, selection, NULL, NULL);

                If (cursor != NULL) {
                    If (cursor->GetCount() > 0) {
                        mAllApnSettings = CreateApnList(cursor);
                    }
                    cursor->Close();
                }
            }
        }

        DedupeApnSettings();

        If (mAllApnSettings->IsEmpty() && IsDummyProfileNeeded()) {
            AddDummyApnSettings(operator);
        }

        AddEmergencyApnSetting();

        If (mAllApnSettings->IsEmpty()) {
            If (DBG) Log("createAllApnList: No APN found for carrier: " + operator);
            mPreferredApn = NULL;
            // TODO: What is the right behavior?
            //NotifyNoData(DataConnection.FailCause.MISSING_UNKNOWN_APN);
        } else {
            mPreferredApn = GetPreferredApn();
            If (mPreferredApn != NULL && !mPreferredApn.numeric->Equals(operator)) {
                mPreferredApn = NULL;
                SetPreferredApn(-1);
            }
            If (DBG) Log("createAllApnList: mPreferredApn=" + mPreferredApn);
        }
        If (DBG) Log("createAllApnList: X mAllApnSettings=" + mAllApnSettings);

        SetDataProfilesAsNeeded();
    }

    private void DedupeApnSettings() {
        ArrayList<ApnSetting> resultApns = new ArrayList<ApnSetting>();

        // coalesce APNs if they are similar enough to prevent
        // us from bringing up two data calls with the same interface
        Int32 i = 0;
        While (i < mAllApnSettings->Size() - 1) {
            ApnSetting first = mAllApnSettings->Get(i);
            ApnSetting second = NULL;
            Int32 j = i + 1;
            While (j < mAllApnSettings->Size()) {
                second = mAllApnSettings->Get(j);
                If (ApnsSimilar(first, second)) {
                    ApnSetting newApn = MergeApns(first, second);
                    mAllApnSettings->Set(i, newApn);
                    first = newApn;
                    mAllApnSettings->Remove(j);
                } else {
                    j++;
                }
            }
            i++;
        }
    }

    //check whether the types of two APN Same (even only one type of each APN is same)
    private Boolean ApnTypeSameAny(ApnSetting first, ApnSetting second) {
        If(VDBG) {
            StringBuilder apnType1 = new StringBuilder(first.apn + ": ");
            For(Int32 index1 = 0; index1 < first.types.length; index1++) {
                apnType1->Append(first.types[index1]);
                apnType1->Append(",");
            }

            StringBuilder apnType2 = new StringBuilder(second.apn + ": ");
            For(Int32 index1 = 0; index1 < second.types.length; index1++) {
                apnType2->Append(second.types[index1]);
                apnType2->Append(",");
            }
            Log("APN1: is " + apnType1);
            Log("APN2: is " + apnType2);
        }

        For(Int32 index1 = 0; index1 < first.types.length; index1++) {
            For(Int32 index2 = 0; index2 < second.types.length; index2++) {
                If(first.types[index1].Equals(PhoneConstants.APN_TYPE_ALL) ||
                        second.types[index2].Equals(PhoneConstants.APN_TYPE_ALL) ||
                        first.types[index1].Equals(second.types[index2])) {
                    If(VDBG)Log("apnTypeSameAny: return TRUE");
                    return TRUE;
                }
            }
        }

        If(VDBG)Log("apnTypeSameAny: return FALSE");
        return FALSE;
    }

    // Check if neither mention DUN and are substantially similar
    private Boolean ApnsSimilar(ApnSetting first, ApnSetting second) {
        Return (first->CanHandleType(PhoneConstants.APN_TYPE_DUN) == FALSE &&
                second->CanHandleType(PhoneConstants.APN_TYPE_DUN) == FALSE &&
                Objects->Equals(first.apn, second.apn) &&
                !ApnTypeSameAny(first, second) &&
                XorEquals(first.proxy, second.proxy) &&
                XorEquals(first.port, second.port) &&
                first.carrierEnabled == second.carrierEnabled &&
                first.bearer == second.bearer &&
                first.profileId == second.profileId &&
                Objects->Equals(first.mvnoType, second.mvnoType) &&
                Objects->Equals(first.mvnoMatchData, second.mvnoMatchData) &&
                XorEquals(first.mmsc, second.mmsc) &&
                XorEquals(first.mmsProxy, second.mmsProxy) &&
                XorEquals(first.mmsPort, second.mmsPort));
    }

    // equal or one is not specified
    private Boolean XorEquals(String first, String second) {
        Return (Objects->Equals(first, second) ||
                TextUtils->IsEmpty(first) ||
                TextUtils->IsEmpty(second));
    }

    private ApnSetting MergeApns(ApnSetting dest, ApnSetting src) {
        ArrayList<String> resultTypes = new ArrayList<String>();
        resultTypes->AddAll(Arrays->AsList(dest.types));
        For (String srcType : src.types) {
            If (resultTypes->Contains(srcType) == FALSE) resultTypes->Add(srcType);
        }
        String mmsc = (TextUtils->IsEmpty(dest.mmsc) ? src.mmsc : dest.mmsc);
        String mmsProxy = (TextUtils->IsEmpty(dest.mmsProxy) ? src.mmsProxy : dest.mmsProxy);
        String mmsPort = (TextUtils->IsEmpty(dest.mmsPort) ? src.mmsPort : dest.mmsPort);
        String proxy = (TextUtils->IsEmpty(dest.proxy) ? src.proxy : dest.proxy);
        String port = (TextUtils->IsEmpty(dest.port) ? src.port : dest.port);
        String protocol = src.protocol->Equals("IPV4V6") ? src.protocol : dest.protocol;
        String roamingProtocol = src.roamingProtocol->Equals("IPV4V6") ? src.roamingProtocol :
                dest.roamingProtocol;

        return new ApnSetting(dest.id, dest.numeric, dest.carrier, dest.apn,
                proxy, port, mmsc, mmsProxy, mmsPort, dest.user, dest.password,
                dest.authType, resultTypes->ToArray(new String[0]), protocol,
                roamingProtocol, dest.carrierEnabled, dest.bearer, dest.profileId,
                (dest.modemCognitive || src.modemCognitive), dest.maxConns, dest.waitTime,
                dest.maxConnsTime, dest.mtu, dest.mvnoType, dest.mvnoMatchData);
    }

    private Boolean IsDummyProfileNeeded() {
        Int32 radioTech = mPhone->GetServiceState()->GetRilDataRadioTechnology();
        Int32 radioTechFam = UiccController->GetFamilyFromRadioTechnology(radioTech);
        IccRecords r = mIccRecords->Get();
        If (DBG) Log("isDummyProfileNeeded: radioTechFam = " + radioTechFam);
        // If uicc app family based on data rat is unknown,
        // check if records selected is RuimRecords.
        Return (radioTechFam == UiccController.APP_FAM_3GPP2 ||
                ((radioTechFam == UiccController.APP_FAM_UNKNOWN) &&
                (r != NULL) && (r instanceof RuimRecords)));
    }

    private void AddDummyApnSettings(String operator) {
        // Create dummy data profiles.
        If (DBG) Log("createAllApnList: Creating dummy apn for cdma operator:" + operator);
        String[] defaultApnTypes = {
                PhoneConstants.APN_TYPE_DEFAULT,
                PhoneConstants.APN_TYPE_MMS,
                PhoneConstants.APN_TYPE_SUPL,
                PhoneConstants.APN_TYPE_HIPRI,
                PhoneConstants.APN_TYPE_FOTA,
                PhoneConstants.APN_TYPE_IMS,
                PhoneConstants.APN_TYPE_CBS};
        String[] dunApnTypes = {
                PhoneConstants.APN_TYPE_DUN};

        ApnSetting apn = new ApnSetting(DctConstants.APN_DEFAULT_ID, operator, NULL, NULL,
                NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                RILConstants.SETUP_DATA_AUTH_PAP_CHAP, defaultApnTypes,
                PROPERTY_CDMA_IPPROTOCOL, PROPERTY_CDMA_ROAMING_IPPROTOCOL, TRUE, 0,
                0, FALSE, 0, 0, 0, PhoneConstants.UNSET_MTU, "", "");
        mAllApnSettings->Add(apn);
        apn = new ApnSetting(DctConstants.APN_DUN_ID, operator, NULL, NULL,
                NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                RILConstants.SETUP_DATA_AUTH_PAP_CHAP, dunApnTypes,
                PROPERTY_CDMA_IPPROTOCOL, PROPERTY_CDMA_ROAMING_IPPROTOCOL, TRUE, 0,
                0, FALSE, 0, 0, 0, PhoneConstants.UNSET_MTU, "", "");
        mAllApnSettings->Add(apn);
    }

    /** Return the DC AsyncChannel for the new data connection */
    private DcAsyncChannel CreateDataConnection() {
        If (DBG) Log("createDataConnection E");

        Int32 id = mUniqueIdGenerator->GetAndIncrement();
        DataConnection conn = DataConnection->MakeDataConnection(mPhone, id,
                                                this, mDcTesterFailBringUpAll, mDcc);
        mDataConnections->Put(id, conn);
        DcAsyncChannel dcac = new DcAsyncChannel(conn, LOG_TAG);
        Int32 status = dcac->FullyConnectSync(mPhone->GetContext(), this, conn->GetHandler());
        If (status == AsyncChannel.STATUS_SUCCESSFUL) {
            mDataConnectionAcHashMap->Put(dcac->GetDataConnectionIdSync(), dcac);
        } else {
            Loge("createDataConnection: Could not connect to dcac=" + dcac + " status=" + status);
        }

        If (DBG) Log("CreateDataConnection() X id=" + id + " dc=" + conn);
        return dcac;
    }

    private void DestroyDataConnections() {
        If(mDataConnections != NULL) {
            If (DBG) Log("destroyDataConnections: clear mDataConnectionList");
            mDataConnections->Clear();
        } else {
            If (DBG) Log("destroyDataConnections: mDataConnecitonList is empty, ignore");
        }
    }

    /**
     * Build a list of APNs to be used to create PDP's.
     *
     * @param requestedApnType
     * @return waitingApns list to be used to create PDP
     *          error when waitingApns->IsEmpty()
     */
    private ArrayList<ApnSetting> BuildWaitingApns(String requestedApnType, Int32 radioTech) {
        If (DBG) Log("buildWaitingApns: E requestedApnType=" + requestedApnType);
        ArrayList<ApnSetting> apnList = new ArrayList<ApnSetting>();

        If (requestedApnType->Equals(PhoneConstants.APN_TYPE_DUN)) {
            ApnSetting dun = FetchDunApn();
            If (dun != NULL) {
                apnList->Add(dun);
                If (DBG) Log("buildWaitingApns: X added APN_TYPE_DUN apnList=" + apnList);
                return apnList;
            }
        }

        String operator = GetOperatorNumeric();
        // This is a workaround for a Bug (7305641) where we don't failover to other
        // suitable APNs if our preferred APN fails.  On prepaid ATT sims we need to
        // failover to a provisioning APN, but once we've used their default data
        // connection we are locked to it for life.  This change allows ATT devices
        // to say they don't want to use preferred at all.
        Boolean usePreferred = TRUE;
        try {
            usePreferred = ! mPhone->GetContext()->GetResources().GetBoolean(com.android.
                    internal.R.bool.config_dontPreferApn);
        } Catch (Resources.NotFoundException e) {
            If (DBG) Log("buildWaitingApns: usePreferred NotFoundException set to TRUE");
            usePreferred = TRUE;
        }
        If (usePreferred) {
            mPreferredApn = GetPreferredApn();
        }
        If (DBG) {
            Log("buildWaitingApns: usePreferred=" + usePreferred
                    + " canSetPreferApn=" + mCanSetPreferApn
                    + " mPreferredApn=" + mPreferredApn
                    + " operator=" + operator + " radioTech=" + radioTech);
        }

        If (usePreferred && mCanSetPreferApn && mPreferredApn != NULL &&
                mPreferredApn->CanHandleType(requestedApnType)) {
            If (DBG) {
                Log("buildWaitingApns: Preferred APN:" + operator + ":"
                        + mPreferredApn.numeric + ":" + mPreferredApn);
            }
            If (mPreferredApn.numeric != NULL && mPreferredApn.numeric->Equals(operator)) {
                If (mPreferredApn.bearer == 0 || mPreferredApn.bearer == radioTech) {
                    apnList->Add(mPreferredApn);
                    If (DBG) Log("buildWaitingApns: X added preferred apnList=" + apnList);
                    return apnList;
                } else {
                    If (DBG) Log("buildWaitingApns: no preferred APN");
                    SetPreferredApn(-1);
                    mPreferredApn = NULL;
                }
            } else {
                If (DBG) Log("buildWaitingApns: no preferred APN");
                SetPreferredApn(-1);
                mPreferredApn = NULL;
            }
        }
        If (mAllApnSettings != NULL && !mAllApnSettings->IsEmpty()) {
            If (DBG) Log("buildWaitingApns: mAllApnSettings=" + mAllApnSettings);
            For (ApnSetting apn : mAllApnSettings) {
                If (DBG) Log("buildWaitingApns: apn=" + apn);
                If (apn->CanHandleType(requestedApnType)) {
                    If (apn.bearer == 0 || apn.bearer == radioTech) {
                        If (DBG) Log("buildWaitingApns: adding apn=" + apn->ToString());
                        apnList->Add(apn);
                    } else {
                        If (DBG) {
                            Log("buildWaitingApns: bearer:" + apn.bearer + " != "
                                    + "radioTech:" + radioTech);
                        }
                    }
                } else {
                    If (DBG) {
                        Log("buildWaitingApns: couldn't handle requesedApnType="
                                + requestedApnType);
                    }
                }
            }
        } else {
            Loge("mAllApnSettings is NULL!");
        }
        If (DBG) Log("buildWaitingApns: X apnList=" + apnList);
        return apnList;
    }

    private String ApnListToString (ArrayList<ApnSetting> apns) {
        If (apns == NULL) {
            Log("apnListToString: apns = NULL.");
            return "";
        }
        else {
            StringBuilder result = new StringBuilder();
            For (Int32 i = 0, size = apns->Size(); i < size; i++) {
                result->Append('[')
                      .Append(apns->Get(i).ToString())
                      .Append(']');
            }
            return result->ToString();
        }
    }

    private void SetPreferredApn(Int32 pos) {
        If (!mCanSetPreferApn) {
            Log("setPreferredApn: X !canSEtPreferApn");
            return;
        }

        Log("setPreferredApn: delete");
        ContentResolver resolver = mPhone->GetContext()->GetContentResolver();
        resolver->Delete(GetUri(PREFERAPN_NO_UPDATE_URI), NULL, NULL);

        If (pos >= 0) {
            Log("setPreferredApn: insert");
            ContentValues values = new ContentValues();
            values->Put(APN_ID, pos);
            resolver->Insert(GetUri(PREFERAPN_NO_UPDATE_URI), values);
        }
    }

    private ApnSetting GetPreferredApn() {
        If (mAllApnSettings == NULL || mAllApnSettings->IsEmpty()) {
            Log("getPreferredApn: mAllApnSettings is " + ((mAllApnSettings == NULL)?"NULL":"empty"));
            return NULL;
        }

        Cursor cursor = mPhone->GetContext()->GetContentResolver().Query(
                GetUri(PREFERAPN_NO_UPDATE_URI), new String[] { "_id", "name", "apn" },
                NULL, NULL, Telephony.Carriers.DEFAULT_SORT_ORDER);

        If (cursor != NULL) {
            mCanSetPreferApn = TRUE;
        } else {
            mCanSetPreferApn = FALSE;
        }
        Log("getPreferredApn: mRequestedApnType=" + mRequestedApnType + " cursor=" + cursor
                + " cursor.count=" + ((cursor != NULL) ? cursor->GetCount() : 0));

        If (mCanSetPreferApn && cursor->GetCount() > 0) {
            Int32 pos;
            cursor->MoveToFirst();
            pos = cursor->GetInt(cursor->GetColumnIndexOrThrow(Telephony.Carriers._ID));
            For(ApnSetting p : mAllApnSettings) {
                Log("getPreferredApn: apnSetting=" + p);
                If (p.id == pos && p->CanHandleType(mRequestedApnType)) {
                    Log("getPreferredApn: X found apnSetting" + p);
                    cursor->Close();
                    return p;
                }
            }
        }

        If (cursor != NULL) {
            cursor->Close();
        }

        Log("getPreferredApn: X not found");
        return NULL;
    }

    //@Override
    CARAPI HandleMessage (Message msg) {
        If (DBG) Log("handleMessage msg=" + msg);

        If (!mPhone.mIsTheCurrentActivePhone || mIsDisposed) {
            Loge("handleMessage: Ignore GSM msgs since GSM phone is inactive");
            return;
        }

        Switch (msg.what) {
            case DctConstants.EVENT_RECORDS_LOADED:
                OnRecordsLoaded();
                break;

            case DctConstants.EVENT_DATA_CONNECTION_DETACHED:
                OnDataConnectionDetached();
                break;

            case DctConstants.EVENT_DATA_CONNECTION_ATTACHED:
                OnDataConnectionAttached();
                break;

            case DctConstants.EVENT_DO_RECOVERY:
                DoRecovery();
                break;

            case DctConstants.EVENT_APN_CHANGED:
                OnApnChanged();
                break;

            case DctConstants.EVENT_PS_RESTRICT_ENABLED:
                /**
                 * We don't need to explicitly to tear down the PDP context
                 * when PS restricted is enabled. The base band will deactive
                 * PDP context and notify us with PDP_CONTEXT_CHANGED.
                 * But we should stop the network polling and prevent reset PDP.
                 */
                If (DBG) Log("EVENT_PS_RESTRICT_ENABLED " + mIsPsRestricted);
                StopNetStatPoll();
                StopDataStallAlarm();
                mIsPsRestricted = TRUE;
                break;

            case DctConstants.EVENT_PS_RESTRICT_DISABLED:
                /**
                 * When PS restrict is removed, we need setup PDP connection if
                 * PDP connection is down.
                 */
                If (DBG) Log("EVENT_PS_RESTRICT_DISABLED " + mIsPsRestricted);
                mIsPsRestricted  = FALSE;
                If (IsConnected()) {
                    StartNetStatPoll();
                    StartDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
                } else {
                    // TODO: Should all PDN states be checked to fail?
                    If (mState == DctConstants.State.FAILED) {
                        CleanUpAllConnections(FALSE, Phone.REASON_PS_RESTRICT_ENABLED);
                        mReregisterOnReconnectFailure = FALSE;
                    }
                    ApnContext apnContext = mApnContexts->Get(PhoneConstants.APN_TYPE_DEFAULT);
                    If (apnContext != NULL) {
                        apnContext->SetReason(Phone.REASON_PS_RESTRICT_ENABLED);
                        TrySetupData(apnContext);
                    } else {
                        Loge("**** Default ApnContext not found ****");
                        If (Build.IS_DEBUGGABLE) {
                            throw new RuntimeException("Default ApnContext not found");
                        }
                    }
                }
                break;

            case DctConstants.EVENT_TRY_SETUP_DATA:
                If (msg.obj instanceof ApnContext) {
                    OnTrySetupData((ApnContext)msg.obj);
                } else If (msg.obj instanceof String) {
                    OnTrySetupData((String)msg.obj);
                } else {
                    Loge("EVENT_TRY_SETUP request w/o apnContext or String");
                }
                break;

            case DctConstants.EVENT_CLEAN_UP_CONNECTION:
                Boolean tearDown = (msg.arg1 == 0) ? FALSE : TRUE;
                If (DBG) Log("EVENT_CLEAN_UP_CONNECTION tearDown=" + tearDown);
                If (msg.obj instanceof ApnContext) {
                    CleanUpConnection(tearDown, (ApnContext)msg.obj);
                } else {
                    Loge("EVENT_CLEAN_UP_CONNECTION request w/o apn context, call super");
                    super->HandleMessage(msg);
                }
                break;
            case DctConstants.EVENT_SET_INTERNAL_DATA_ENABLE:
                Boolean enabled = (msg.arg1 == DctConstants.ENABLED) ? TRUE : FALSE;
                OnSetInternalDataEnabled(enabled, (Message) msg.obj);
                break;

            case DctConstants.EVENT_CLEAN_UP_ALL_CONNECTIONS:
                Message mCause = ObtainMessage(DctConstants.EVENT_CLEAN_UP_ALL_CONNECTIONS, NULL);
                If ((msg.obj != NULL) && (msg.obj instanceof String)) {
                    mCause.obj = msg.obj;
                }
                super->HandleMessage(mCause);
                break;

            case DctConstants.EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED: // fall thru
            case DctConstants.EVENT_DATA_RAT_CHANGED:
                // When data rat changes we might need to load different
                // set of Apns (example, LTE->1x)
                If (OnUpdateIcc()) {
                    Log("onUpdateIcc: tryRestartDataConnections " + Phone.REASON_NW_TYPE_CHANGED);
                    TryRestartDataConnections(TRUE, Phone.REASON_NW_TYPE_CHANGED);
                } else If (IsNvSubscription()){
                    // If cdma subscription source changed to NV or data rat changed to cdma
                    // (while subscription source was NV) - we need to trigger NV ready
                    OnNvReady();
                }
                break;

            case DctConstants.CMD_CLEAR_PROVISIONING_SPINNER:
                // Check message sender intended to clear the current spinner.
                If (mProvisioningSpinner == msg.obj) {
                    mProvisioningSpinner->Dismiss();
                    mProvisioningSpinner = NULL;
                }
                break;

            case DctConstants.EVENT_GET_WWAN_IWLAN_COEXISTENCE_DONE:
                OnWwanIwlanCoexistenceDone((AsyncResult)msg.obj);
                break;

            case DctConstants.EVENT_MODEM_DATA_PROFILE_READY:
                OnModemApnProfileReady();
                break;

            default:
                // handle the message in the super class DataConnectionTracker
                super->HandleMessage(msg);
                break;
        }
    }

    protected Int32 GetApnProfileID(String apnType) {
        If (TextUtils->Equals(apnType, PhoneConstants.APN_TYPE_IMS)) {
            return RILConstants.DATA_PROFILE_IMS;
        } else If (TextUtils->Equals(apnType, PhoneConstants.APN_TYPE_FOTA)) {
            return RILConstants.DATA_PROFILE_FOTA;
        } else If (TextUtils->Equals(apnType, PhoneConstants.APN_TYPE_CBS)) {
            return RILConstants.DATA_PROFILE_CBS;
        } else If (TextUtils->Equals(apnType, PhoneConstants.APN_TYPE_IA)) {
            return RILConstants.DATA_PROFILE_DEFAULT; // DEFAULT for now
        } else If (TextUtils->Equals(apnType, PhoneConstants.APN_TYPE_DUN)) {
            return RILConstants.DATA_PROFILE_TETHERED;
        } else {
            return RILConstants.DATA_PROFILE_DEFAULT;
        }
    }

    private Int32 GetCellLocationId() {
        Int32 cid = -1;
        CellLocation loc = mPhone->GetCellLocation();

        If (loc != NULL) {
            If (loc instanceof GsmCellLocation) {
                cid = ((GsmCellLocation)loc).GetCid();
            } else If (loc instanceof CdmaCellLocation) {
                cid = ((CdmaCellLocation)loc).GetBaseStationId();
            }
        }
        return cid;
    }

    private IccRecords GetUiccRecords(Int32 appFamily) {
        return mUiccController->GetIccRecords(mPhone->GetPhoneId(), appFamily);
    }


    /**
     * @description This function updates mIccRecords reference to track
     *              currently used IccRecords
     * @return TRUE if IccRecords changed
     */
    //@Override
    protected Boolean OnUpdateIcc() {
        Boolean result = FALSE;
        If (mUiccController == NULL ) {
            Loge("onUpdateIcc: mUiccController is NULL. Error!");
            return FALSE;
        }

        Int32 dataRat = mPhone->GetServiceState()->GetRilDataRadioTechnology();
        Int32 appFamily = UiccController->GetFamilyFromRadioTechnology(dataRat);
        IccRecords newIccRecords = GetUiccRecords(appFamily);
        Log("onUpdateIcc: newIccRecords " + ((newIccRecords != NULL) ?
                newIccRecords->GetClass()->GetName() : NULL));
        If (dataRat == ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN) {
            // Ignore this. This could be due to data not registered
            // We want to ignore RADIO_TECHNOLOGY_UNKNOWN so that we do not tear down data
            // call in case we are out of service.
            return FALSE;
        }

        IccRecords r = mIccRecords->Get();
        If (r != newIccRecords) {
            If (r != NULL) {
                Log("Removing stale icc objects. " + ((r != NULL) ?
                        r->GetClass()->GetName() : NULL));
                r->UnregisterForRecordsLoaded(this);
                mIccRecords->Set(NULL);
            }
            If (newIccRecords != NULL) {
                Log("New records found " + ((newIccRecords != NULL) ?
                        newIccRecords->GetClass()->GetName() : NULL));
                mIccRecords->Set(newIccRecords);
                newIccRecords->RegisterForRecordsLoaded(
                        this, DctConstants.EVENT_RECORDS_LOADED, NULL);
            }
            // Records changed -> return TRUE
            result = TRUE;
        }
        return result;
    }

    // setAsCurrentDataConnectionTracker
    CARAPI Update() {
        Log("update sub = " + mPhone->GetSubId());
        Log("Update(): Active DDS, register for all events now!");
        RegisterForAllEvents();
        OnUpdateIcc();

        mUserDataEnabled = Settings::Global::>GetInt(mPhone->GetContext()->GetContentResolver(),
                Settings::Global::MOBILE_DATA + mPhone->GetPhoneId(), 1) == 1;

        If (mPhone instanceof CDMALTEPhone) {
            ((CDMALTEPhone)mPhone).UpdateCurrentCarrierInProvider();
            SupplyMessenger();
        } else If (mPhone instanceof GSMPhone) {
            ((GSMPhone)mPhone).UpdateCurrentCarrierInProvider();
            SupplyMessenger();
        } else {
            Log("Phone object is not MultiSim. This should not hit!!!!");
        }
    }

    //@Override
    CARAPI CleanUpAllConnections(String cause) {
        CleanUpAllConnections(cause, NULL);
    }

    CARAPI UpdateRecords() {
        OnUpdateIcc();
    }

    CARAPI CleanUpAllConnections(String cause, Message disconnectAllCompleteMsg) {
        Log("cleanUpAllConnections");
        If (disconnectAllCompleteMsg != NULL) {
            mDisconnectAllCompleteMsgList->Add(disconnectAllCompleteMsg);
        }

        Message msg = ObtainMessage(DctConstants.EVENT_CLEAN_UP_ALL_CONNECTIONS);
        msg.obj = cause;
        SendMessage(msg);
    }

    protected void NotifyDataDisconnectComplete() {
        Log("notifyDataDisconnectComplete");
        For (Message m: mDisconnectAllCompleteMsgList) {
            m->SendToTarget();
        }
        mDisconnectAllCompleteMsgList->Clear();
    }


    protected void NotifyAllDataDisconnected() {
        sEnableFailFastRefCounter = 0;
        mFailFast = FALSE;
        mAllDataDisconnectedRegistrants->NotifyRegistrants();
    }

    CARAPI RegisterForAllDataDisconnected(Handler h, Int32 what, Object obj) {
        mAllDataDisconnectedRegistrants->AddUnique(h, what, obj);

        If (IsDisconnected()) {
            Log("notify All Data Disconnected");
            NotifyAllDataDisconnected();
        }
    }

    CARAPI UnregisterForAllDataDisconnected(Handler h) {
        mAllDataDisconnectedRegistrants->Remove(h);
    }


    //@Override
    protected void OnSetInternalDataEnabled(Boolean enable) {
        OnSetInternalDataEnabled(enable, NULL);
    }

    protected void OnSetInternalDataEnabled(Boolean enabled, Message onCompleteMsg) {
        Boolean sendOnComplete = TRUE;

        Synchronized (mDataEnabledLock) {
            mInternalDataEnabled = enabled;
            If (enabled) {
                Log("onSetInternalDataEnabled: changed to enabled, try to setup data call");
                OnTrySetupData(Phone.REASON_DATA_ENABLED);
            } else {
                sendOnComplete = FALSE;
                Log("onSetInternalDataEnabled: changed to disabled, cleanUpAllConnections");
                CleanUpAllConnections(NULL, onCompleteMsg);
            }
        }

        If (sendOnComplete) {
            If (onCompleteMsg != NULL) {
                onCompleteMsg->SendToTarget();
            }
        }
    }

    public Boolean SetInternalDataEnabledFlag(Boolean enable) {
        If (DBG)
            Log("SetInternalDataEnabledFlag(" + enable + ")");

        If (mInternalDataEnabled != enable) {
            mInternalDataEnabled = enable;
        }
        return TRUE;
    }

    //@Override
    public Boolean SetInternalDataEnabled(Boolean enable) {
        return SetInternalDataEnabled(enable, NULL);
    }

    public Boolean SetInternalDataEnabled(Boolean enable, Message onCompleteMsg) {
        If (DBG)
            Log("SetInternalDataEnabled(" + enable + ")");

        Message msg = ObtainMessage(DctConstants.EVENT_SET_INTERNAL_DATA_ENABLE, onCompleteMsg);
        msg.arg1 = (enable ? DctConstants.ENABLED : DctConstants.DISABLED);
        SendMessage(msg);
        return TRUE;
    }

    /** Returns TRUE if this is current DDS. */
    protected Boolean IsActiveDataSubscription() {
        // FIXME This should have code like
        // Return (mPhone->GetSubId() == SubscriptionManager->GetDefaultDataSubId());
        return TRUE;
    }

    CARAPI SetDataAllowed(Boolean enable, Message response) {
         mIsCleanupRequired = !enable;
         mPhone.mCi->SetDataAllowed(enable, response);
         mInternalDataEnabled = enable;
    }

    //@Override
    protected void Log(String s) {
        Rlog->D(LOG_TAG, "[" + mPhone->GetPhoneId() + "]" + s);
    }

    //@Override
    protected void Loge(String s) {
        Rlog->E(LOG_TAG, "[" + mPhone->GetPhoneId() + "]" + s);
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("DataConnectionTracker extends:");
        super->Dump(fd, pw, args);
        pw->Println(" mReregisterOnReconnectFailure=" + mReregisterOnReconnectFailure);
        pw->Println(" canSetPreferApn=" + mCanSetPreferApn);
        pw->Println(" mApnObserver=" + mApnObserver);
        pw->Println(" getOverallState=" + GetOverallState());
        pw->Println(" mDataConnectionAsyncChannels=%s\n" + mDataConnectionAcHashMap);
        pw->Println(" mAttached=" + mAttached->Get());
    }

    //@Override
    public String[] GetPcscfAddress(String apnType) {
        Log("GetPcscfAddress()");
        ApnContext apnContext = NULL;

        If(apnType == NULL){
            Log("apnType is NULL, return NULL");
            return NULL;
        }

        If (TextUtils->Equals(apnType, PhoneConstants.APN_TYPE_EMERGENCY)) {
            apnContext = mApnContexts->Get(PhoneConstants.APN_TYPE_EMERGENCY);
        } else If (TextUtils->Equals(apnType, PhoneConstants.APN_TYPE_IMS)) {
            apnContext = mApnContexts->Get(PhoneConstants.APN_TYPE_IMS);
        } else {
            Log("apnType is invalid, return NULL");
            return NULL;
        }

        If (apnContext == NULL) {
            Log("apnContext is NULL, return NULL");
            return NULL;
        }

        DcAsyncChannel dcac = apnContext->GetDcAc();
        String[] result = NULL;

        If (dcac != NULL) {
            result = dcac->GetPcscfAddr();

            For (Int32 i = 0; i < result.length; i++) {
                Log("Pcscf[" + i + "]: " + result[i]);
            }
            return result;
        }
        return NULL;
    }

    //@Override
    CARAPI SetImsRegistrationState(Boolean registered) {
        Log("setImsRegistrationState - MImsRegistrationState(before): "+ mImsRegistrationState
                + ", Registered(current) : " + registered);

        If (mPhone == NULL) return;

        ServiceStateTracker sst = mPhone->GetServiceStateTracker();
        If (sst == NULL) return;

        sst->SetImsRegistrationState(registered);
    }

    /**
     * Read APN configuration from Telephony.db for Emergency APN
     * All opertors recognize the connection request for EPDN based on APN type
     * PLMN name,APN name are not mandatory parameters
     */
    private void InitEmergencyApnSetting() {
        // Operator Numeric is not available when sim records are not loaded.
        // Query Telephony.db with APN type as EPDN request does not
        // require APN name, plmn and all operators support same APN config.
        // DB will contain only one entry for Emergency APN
        String selection = "type=\"emergency\"";
        Cursor cursor = mPhone->GetContext()->GetContentResolver().Query(
                Telephony.Carriers.CONTENT_URI, NULL, selection, NULL, NULL);

        If (cursor != NULL) {
            If (cursor->GetCount() > 0) {
                If (cursor->MoveToFirst()) {
                    mEmergencyApn = MakeApnSetting(cursor);
                }
            }
            cursor->Close();
        }
    }

    /**
     * Add the Emergency APN settings to APN settings list
     */
    private void AddEmergencyApnSetting() {
        If(mEmergencyApn != NULL) {
            If(mAllApnSettings == NULL) {
                mAllApnSettings = new ArrayList<ApnSetting>();
            } else {
                Boolean hasEmergencyApn = FALSE;
                For (ApnSetting apn : mAllApnSettings) {
                    If (ArrayUtils->Contains(apn.types, PhoneConstants.APN_TYPE_EMERGENCY)) {
                        hasEmergencyApn = TRUE;
                        break;
                    }
                }

                If(hasEmergencyApn == FALSE) {
                    mAllApnSettings->Add(mEmergencyApn);
                } else {
                    Log("addEmergencyApnSetting - E-APN setting is already present");
                }
            }
        }
    }

    private Uri GetUri(Uri uri) {
        return Uri->WithAppendedPath(uri, "/subId/" + mSubId);
    }

}
