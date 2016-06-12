
#include "elastos/droid/internal/telephony/dataconnection/DcTracker.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <_Elastos.Droid.Core.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DcTracker::SubBroadcastReceiverDefaultDds
//=============================================================================
DcTracker::SubBroadcastReceiverDefaultDds::SubBroadcastReceiverDefaultDds(
    /* [in] */ DcTracker* host)
    : mHost(host)
{}

ECode DcTracker::SubBroadcastReceiverDefaultDds::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
            mSubId = mPhone.getSubId();
            log("got ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED, new DDS = "
                    + intent.getLongExtra(PhoneConstants.SUBSCRIPTION_KEY,
                            SubscriptionManager.INVALID_SUB_ID));
            updateSubIdAndCapability();
            if (mSubId == SubscriptionController.getInstance().getDefaultDataSubId()) {
                log("Dct is default-DDS now, process any pending MMS requests");
            }

#endif
}

//=============================================================================
// DcTracker::SubBroadcastReceiverSubInfo
//=============================================================================
DcTracker::SubBroadcastReceiverSubInfo::SubBroadcastReceiverSubInfo(
    /* [in] */ DcTracker* host)
    : mHost(host)
{}

ECode DcTracker::SubBroadcastReceiverSubInfo::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
            mSubId = mPhone.getSubId();
            log("got ACTION_SUBINFO_RECORD_UPDATED, mySubId = " + mSubId);
            updateSubIdAndCapability();

#endif
}

//=============================================================================
// DcTracker::ApnChangeObserver
//=============================================================================
DcTracker::ApnChangeObserver::ApnChangeObserver(
    /* [in] */ DcTracker* host)
    : mHost(host)
{}

ECode DcTracker::ApnChangeObserver::constructor()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                super(mDataConnectionTracker);

#endif
}

ECode DcTracker::ApnChangeObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                sendMessage(obtainMessage(DctConstants.EVENT_APN_CHANGED));

#endif
}

//=============================================================================
// DcTracker::TelephonyNetworkFactory
//=============================================================================
DcTracker::TelephonyNetworkFactory::TelephonyNetworkFactory(
    /* [in] */ DcTracker* host)
    : mHost(host)
{
#if 0 // TODO: Translate codes below
    CSparseArray::New((ISparseArray**)&mDdsRequests);
#endif
}

ECode DcTracker::TelephonyNetworkFactory::constructor(
    /* [in] */ ILooper* l,
    /* [in] */ IContext* c,
    /* [in] */ const String& TAG,
    /* [in] */ INetworkCapabilities* nc,
    /* [in] */ IPhoneBase* phone)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                super(l, c, TAG, nc);
                mPhone = phone;
                mNetworkCapabilities = nc;

#endif
}

ECode DcTracker::TelephonyNetworkFactory::ProcessPendingNetworkRequests(
    /* [in] */ INetworkRequest* n)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                for (int i = 0; i < mDdsRequests.size(); i++) {
                    NetworkRequest nr = mDdsRequests.valueAt(i);
                    if (nr.equals(n)) {
                        log("Found pending request in ddsRequest list = " + nr);
                        ApnContext apnContext = apnContextForNetworkRequest(nr);
                        if (apnContext != null) {
                            log("Activating APN=" + apnContext);
                            apnContext.incRefCount();
                        }
                    }
                }

#endif
}

ECode DcTracker::TelephonyNetworkFactory::RegisterOnDemandDdsCallback()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                SubscriptionController subController = SubscriptionController.getInstance();
                subController.registerForOnDemandDdsLockNotification(mPhone.getSubId(),
                        new SubscriptionController.OnDemandDdsLockNotifier() {
                            public void notifyOnDemandDdsLockGranted(NetworkRequest n) {
                                log("Got the tempDds lock for the request = " + n);
                                processPendingNetworkRequests(n);
                            }
                        });

#endif
}

ECode DcTracker::TelephonyNetworkFactory::UpdateNetworkCapability(
    /* [in] */ Int64 subId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                log("update networkCapabilites for subId = " + subId);
                mNetworkCapabilities.setNetworkSpecifier(""+subId);
                if ((subId > 0 && SubscriptionController.getInstance().
                        getSubState(subId) == SubscriptionManager.ACTIVE) &&
                        (subId == SubscriptionController.getInstance().getDefaultDataSubId())) {
                    log("INTERNET capability is with subId = " + subId);
                    //Only defaultDataSub provides INTERNET.
                    mNetworkCapabilities.addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
                } else {
                    log("INTERNET capability is removed from subId = " + subId);
                    mNetworkCapabilities.removeCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
                }
                setScoreFilter(50);
                registerOnDemandDdsCallback();
                log("Ready to handle network requests");

#endif
}

ECode DcTracker::TelephonyNetworkFactory::NeedNetworkFor(
    /* [in] */ INetworkRequest* networkRequest,
    /* [in] */ Int32 score)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                // figure out the apn type and enable it
                if (DBG) log("Cellular needs Network for " + networkRequest);
                SubscriptionController subController = SubscriptionController.getInstance();
                log("subController = " + subController);
                long currentDds = subController.getDefaultDataSubId();
                long subId = mPhone.getSubId();
                long requestedSpecifier = subController.getSubIdFromNetworkRequest(networkRequest);
                log("CurrentDds = " + currentDds);
                log("mySubId = " + subId);
                log("Requested networkSpecifier = " + requestedSpecifier);
                log("my networkSpecifier = " + mNetworkCapabilities.getNetworkSpecifier());
                if (subId < 0) {
                    log("Can't handle any network request now, subId not ready.");
                    return;
                }
                // For clients that do not send subId in NetworkCapabilities,
                // Connectivity will send to all network factories. Accept only
                // when requestedSpecifier is same as current factory's subId
                if (requestedSpecifier != subId) {
                    log("requestedSpecifier is not same as mysubId. Bail out.");
                    return;
                }
                if (currentDds != requestedSpecifier) {
                    log("This request would result in DDS switch");
                    log("Requested DDS switch to subId = " + requestedSpecifier);
                    //Queue this request and initiate temp DDS switch.
                    //Once the DDS switch is done we will revist the pending requests.
                    mDdsRequests.put(networkRequest.requestId, networkRequest);
                    requestOnDemandDataSubscriptionLock(networkRequest);
                    return;
                } else {
                    if(isNetworkRequestForInternet(networkRequest)) {
                        log("Activating internet request on subId = " + subId);
                        ApnContext apnContext = apnContextForNetworkRequest(networkRequest);
                        if (apnContext != null) {
                            log("Activating APN=" + apnContext);
                            apnContext.incRefCount();
                        }
                    } else {
                        if(isValidRequest(networkRequest)) {
                            //non-default APN requests for this subscription.
                            mDdsRequests.put(networkRequest.requestId, networkRequest);
                            requestOnDemandDataSubscriptionLock(networkRequest);
                        } else {
                            log("Bogus request req = " + networkRequest);
                        }
                    }
                }

#endif
}

ECode DcTracker::TelephonyNetworkFactory::IsValidRequest(
    /* [in] */ INetworkRequest* n,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                int[] types = n.networkCapabilities.getCapabilities();
                return (types.length > 0);

#endif
}

ECode DcTracker::TelephonyNetworkFactory::IsNetworkRequestForInternet(
    /* [in] */ INetworkRequest* n,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                boolean flag = n.networkCapabilities.hasCapability
                    (NetworkCapabilities.NET_CAPABILITY_INTERNET);
                log("Is the request for Internet = " + flag);
                return flag;

#endif
}

ECode DcTracker::TelephonyNetworkFactory::RequestOnDemandDataSubscriptionLock(
    /* [in] */ INetworkRequest* n)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                if(!isNetworkRequestForInternet(n)) {
                    //Request tempDDS lock only for non-default PDP requests
                    SubscriptionController subController = SubscriptionController.getInstance();
                    log("requestOnDemandDataSubscriptionLock for request = " + n);
                    subController.startOnDemandDataSubscriptionRequest(n);
                }

#endif
}

ECode DcTracker::TelephonyNetworkFactory::RemoveRequestFromList(
    /* [in] */ ISparseArray* list,
    /* [in] */ INetworkRequest* n)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                NetworkRequest nr = list.get(n.requestId);
                if (nr != null) {
                    log("Removing request = " + nr);
                    list.remove(n.requestId);
                    ApnContext apnContext = apnContextForNetworkRequest(n);
                    if (apnContext != null) {
                        log("Deactivating APN=" + apnContext);
                        apnContext.decRefCount();
                    }
                }

#endif
}

ECode DcTracker::TelephonyNetworkFactory::RemoveRequestIfFound(
    /* [in] */ INetworkRequest* n)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                log("Release the request from dds queue, if found");
                removeRequestFromList(mDdsRequests, n);
                if(!isNetworkRequestForInternet(n)) {
                    SubscriptionController subController = SubscriptionController.getInstance();
                    subController.stopOnDemandDataSubscriptionRequest(n);
                } else {
                    // Internet requests are not queued in DDS list. So deactivate here explicitly.
                    ApnContext apnContext = apnContextForNetworkRequest(n);
                    if (apnContext != null) {
                        log("Deactivating APN=" + apnContext);
                        apnContext.decRefCount();
                    }
                }

#endif
}

ECode DcTracker::TelephonyNetworkFactory::ReleaseNetworkFor(
    /* [in] */ INetworkRequest* networkRequest)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                if (DBG) log("Cellular releasing Network for " + networkRequest);
                removeRequestIfFound(networkRequest);

#endif
}

ECode DcTracker::TelephonyNetworkFactory::ReleaseAllNetworkRequests()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                log("releaseAllNetworkRequests");
                SubscriptionController subController = SubscriptionController.getInstance();
                for (int i = 0; i < mDdsRequests.size(); i++) {
                    NetworkRequest nr = mDdsRequests.valueAt(i);
                    if (nr != null) {
                        log("Removing request = " + nr);
                        subController.stopOnDemandDataSubscriptionRequest(nr);
                        mDdsRequests.remove(nr.requestId);
                    }
                }

#endif
}

ECode DcTracker::TelephonyNetworkFactory::Log(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                Log.d("TelephonyNetworkFactory" + mPhone.getSubId(), s);

#endif
}

//=============================================================================
// DcTracker::ProvisionNotificationBroadcastReceiver
//=============================================================================
DcTracker::ProvisionNotificationBroadcastReceiver::ProvisionNotificationBroadcastReceiver(
    /* [in] */ DcTracker* host)
    : mHost(host)
{}

ECode DcTracker::ProvisionNotificationBroadcastReceiver::constructor(
    /* [in] */ const String& provisionUrl,
    /* [in] */ const String& networkOperator)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                mNetworkOperator = networkOperator;
                mProvisionUrl = provisionUrl;

#endif
}

ECode DcTracker::ProvisionNotificationBroadcastReceiver::SetEnableFailFastMobileData(
    /* [in] */ Int32 enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                sendMessage(obtainMessage(DctConstants.CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA, enabled, 0));

#endif
}

ECode DcTracker::ProvisionNotificationBroadcastReceiver::EnableMobileProvisioning()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                final Message msg = obtainMessage(DctConstants.CMD_ENABLE_MOBILE_PROVISIONING);
                msg.setData(Bundle.forPair(DctConstants.PROVISIONING_URL_KEY, mProvisionUrl));
                sendMessage(msg);

#endif
}

ECode DcTracker::ProvisionNotificationBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                // Turning back on the radio can take time on the order of a minute, so show user a
                // spinner so they know something is going on.
                mProvisioningSpinner = new ProgressDialog(context);
                mProvisioningSpinner.setTitle(mNetworkOperator);
                mProvisioningSpinner.setMessage(
                        // TODO: Don't borrow "Connecting..." i18n string; give Telephony a version.
                        context.getText(com.android.internal.R.string.media_route_status_connecting));
                mProvisioningSpinner.setIndeterminate(true);
                mProvisioningSpinner.setCancelable(true);
                // Allow non-Activity Service Context to create a View.
                mProvisioningSpinner.getWindow().setType(
                        WindowManager.LayoutParams.TYPE_KEYGUARD_DIALOG);
                mProvisioningSpinner.show();
                // After timeout, hide spinner so user can at least use their device.
                // TODO: Indicate to user that it is taking an unusually long time to connect?
                sendMessageDelayed(obtainMessage(DctConstants.CMD_CLEAR_PROVISIONING_SPINNER,
                        mProvisioningSpinner), PROVISIONING_SPINNER_TIMEOUT_MILLIS);
                // This code is almost identical to the old
                // ConnectivityService.handleMobileProvisioningAction code.
                setRadio(true);
                setEnableFailFastMobileData(DctConstants.ENABLED);
                enableMobileProvisioning();

#endif
}

//=============================================================================
// DcTracker
//=============================================================================
CAR_INTERFACE_IMPL(DcTracker, DcTrackerBase, IDcTracker)

const String DcTracker::PUPPET_MASTER_RADIO_STRESS_TEST("gsm.defaultpdpcontext.active");
const Int32 DcTracker::POLL_PDP_MILLIS = 5 * 1000;
const Int32 DcTracker::PROVISIONING_SPINNER_TIMEOUT_MILLIS = 120 * 1000;
AutoPtr<IUri> DcTracker::PREFERAPN_NO_UPDATE_URI = InitPREFERAPN_NO_UPDATE_URI();
const String DcTracker::APN_ID("apn_id");
const String DcTracker::PROPERTY_CDMA_IPPROTOCOL = InitPROPERTY_CDMA_IPPROTOCOL();
const String DcTracker::PROPERTY_CDMA_ROAMING_IPPROTOCOL = InitPROPERTY_CDMA_ROAMING_IPPROTOCOL();

DcTracker::DcTracker()
    : mDisconnectPendingCount(0)
    , mReregisterOnReconnectFailure(false)
    , mCanSetPreferApn(false)
    , mImsRegistrationState(false)
    , mWaitCleanUpApnContext(NULL)
    , mDeregistrationAlarmState(false)
    , mImsDeregistrationDelayIntent(NULL)
    , mWwanIwlanCoexistFlag(false)
{
#if 0 // TODO: Translate codes below
        Boolean mOosIsDisconnect = SystemProperties.getBoolean(
            PhoneBase.PROPERTY_OOS_IS_DISCONNECT, true);
    CArrayList::New((IArrayList**)&mDisconnectAllCompleteMsgList);

    CRegistrantList::New((IRegistrantList**)&mAllDataDisconnectedRegistrants);

    CAtomicBoolean::New(false, (IAtomicBoolean**)&mAttached);
#endif
}

ECode DcTracker::constructor(
    /* [in] */ IPhoneBase* p)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super(p);
        if (p.getPhoneType() == PhoneConstants.PHONE_TYPE_GSM) {
            LOG__TAG = "GsmDCT";
        } else if (p.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
            LOG__TAG = "CdmaDCT";
        } else {
            LOG__TAG = "DCT";
            loge("unexpected phone type [" + p.getPhoneType() + "]");
        }
        if (DBG) log(LOG__TAG + ".constructor");
        if (p.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
            final boolean fetchApnFromOmhCard = p.getContext().getResources().
                    getBoolean(com.android.internal.R.bool.config_fetch_apn_from_omh_card);
            log(LOG__TAG + " fetchApnFromOmhCard: " + fetchApnFromOmhCard);
            if (fetchApnFromOmhCard) {
                mOmhApt = new CdmaApnProfileTracker((CDMAPhone)p);
                mOmhApt.registerForModemProfileReady(this,
                        DctConstants.EVENT_MODEM_DATA_PROFILE_READY, null);
            }
        }
        mDataConnectionTracker = this;
        update();
        mApnObserver = new ApnChangeObserver();
        p.getContext().getContentResolver().registerContentObserver(
                Telephony.Carriers.CONTENT_URI, true, mApnObserver);
        initApnContexts();
        for (ApnContext apnContext : mApnContexts.values()) {
            // Register the reconnect and restart actions.
            IntentFilter filter = new IntentFilter();
            filter.addAction(INTENT_RECONNECT_ALARM + '.' + apnContext.getApnType());
            filter.addAction(INTENT_RESTART_TRYSETUP_ALARM + '.' + apnContext.getApnType());
            mPhone.getContext().registerReceiver(mIntentReceiver, filter, null, mPhone);
        }
        ConnectivityManager cm = (ConnectivityManager)p.getContext().getSystemService(
                Context.CONNECTIVITY_SERVICE);
        mNetworkFilter = new NetworkCapabilities();
        mNetworkFilter.addTransportType(NetworkCapabilities.TRANSPORT_CELLULAR);
        mNetworkFilter.addCapability(NetworkCapabilities.NET_CAPABILITY_MMS);
        mNetworkFilter.addCapability(NetworkCapabilities.NET_CAPABILITY_SUPL);
        mNetworkFilter.addCapability(NetworkCapabilities.NET_CAPABILITY_DUN);
        mNetworkFilter.addCapability(NetworkCapabilities.NET_CAPABILITY_FOTA);
        mNetworkFilter.addCapability(NetworkCapabilities.NET_CAPABILITY_IMS);
        mNetworkFilter.addCapability(NetworkCapabilities.NET_CAPABILITY_CBS);
        mNetworkFilter.addCapability(NetworkCapabilities.NET_CAPABILITY_IA);
        mNetworkFilter.addCapability(NetworkCapabilities.NET_CAPABILITY_RCS);
        mNetworkFilter.addCapability(NetworkCapabilities.NET_CAPABILITY_XCAP);
        mNetworkFilter.addCapability(NetworkCapabilities.NET_CAPABILITY_EIMS);
        mNetworkFilter.addCapability(NetworkCapabilities.NET_CAPABILITY_NOT_RESTRICTED);
        //Dont add INTERNET capability, only defaultDataSubscription provides INTERNET.
        mNetworkFactory = new TelephonyNetworkFactory(this.getLooper(), p.getContext(),
                "TelephonyNetworkFactory", mNetworkFilter, mPhone);
        mNetworkFactory.setScoreFilter(-1);
        mNetworkFactoryMessenger = new Messenger(mNetworkFactory);
        cm.registerNetworkFactory(mNetworkFactoryMessenger, "Telephony");
        // Add Emergency APN to APN setting list by default to support EPDN in sim absent cases
        initEmergencyApnSetting();
        addEmergencyApnSetting();
        mProvisionActionName = "com.android.internal.telephony.PROVISION" + p.getPhoneId();
        mPhone.getContext().registerReceiver(subInfoBroadcastReceiver,
                new IntentFilter(TelephonyIntents.ACTION_SUBINFO_RECORD_UPDATED));
        mPhone.getContext().registerReceiver(defaultDdsBroadcastReceiver,
                new IntentFilter(TelephonyIntents.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED));

#endif
}

ECode DcTracker::ProcessPendingNetworkRequests(
    /* [in] */ INetworkRequest* n)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ((TelephonyNetworkFactory)mNetworkFactory).processPendingNetworkRequests(n);

#endif
}

ECode DcTracker::UpdateSubIdAndCapability()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ((TelephonyNetworkFactory)mNetworkFactory).updateNetworkCapability(mSubId);

#endif
}

ECode DcTracker::ReleaseAllNetworkRequests()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ((TelephonyNetworkFactory)mNetworkFactory).releaseAllNetworkRequests();

#endif
}

ECode DcTracker::RegisterForAllEvents()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mPhone.mCi.registerForAvailable(this, DctConstants.EVENT_RADIO_AVAILABLE, null);
        mPhone.mCi.registerForOffOrNotAvailable(this,
               DctConstants.EVENT_RADIO_OFF_OR_NOT_AVAILABLE, null);
        mPhone.mCi.registerForWwanIwlanCoexistence(this,
                DctConstants.EVENT_GET_WWAN_IWLAN_COEXISTENCE_DONE, null);
        mPhone.mCi.registerForDataNetworkStateChanged(this,
               DctConstants.EVENT_DATA_STATE_CHANGED, null);
        mPhone.getCallTracker().registerForVoiceCallEnded (this,
               DctConstants.EVENT_VOICE_CALL_ENDED, null);
        mPhone.getCallTracker().registerForVoiceCallStarted (this,
               DctConstants.EVENT_VOICE_CALL_STARTED, null);
        mPhone.getServiceStateTracker().registerForDataConnectionAttached(this,
               DctConstants.EVENT_DATA_CONNECTION_ATTACHED, null);
        mPhone.getServiceStateTracker().registerForDataConnectionDetached(this,
               DctConstants.EVENT_DATA_CONNECTION_DETACHED, null);
        mPhone.getServiceStateTracker().registerForRoamingOn(this,
               DctConstants.EVENT_ROAMING_ON, null);
        mPhone.getServiceStateTracker().registerForRoamingOff(this,
               DctConstants.EVENT_ROAMING_OFF, null);
        mPhone.getServiceStateTracker().registerForPsRestrictedEnabled(this,
                DctConstants.EVENT_PS_RESTRICT_ENABLED, null);
        mPhone.getServiceStateTracker().registerForPsRestrictedDisabled(this,
                DctConstants.EVENT_PS_RESTRICT_DISABLED, null);
     //   SubscriptionManager.registerForDdsSwitch(this,
     //          DctConstants.EVENT_CLEAN_UP_ALL_CONNECTIONS, null);
        mPhone.getServiceStateTracker().registerForDataRegStateOrRatChanged(this,
                DctConstants.EVENT_DATA_RAT_CHANGED, null);
        if (mPhone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
            mCdmaSsm = CdmaSubscriptionSourceManager.getInstance(
                    mPhone.getContext(), mPhone.mCi, this,
                    DctConstants.EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, null);
            // CdmaSsm doesn't send this event whenever you register - fake it ourselves
            sendMessage(obtainMessage(DctConstants.EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED));
        }

#endif
}

ECode DcTracker::Dispose()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("GsmDCT.dispose");
        releaseAllNetworkRequests();
        if (mProvisionBroadcastReceiver != null) {
            mPhone.getContext().unregisterReceiver(mProvisionBroadcastReceiver);
            mProvisionBroadcastReceiver = null;
        }
        if (mProvisioningSpinner != null) {
            mProvisioningSpinner.dismiss();
            mProvisioningSpinner = null;
        }
        ConnectivityManager cm = (ConnectivityManager)mPhone.getContext().getSystemService(
                Context.CONNECTIVITY_SERVICE);
        cm.unregisterNetworkFactory(mNetworkFactoryMessenger);
        mPhone.getContext().unregisterReceiver(defaultDdsBroadcastReceiver);
        mPhone.getContext().unregisterReceiver(subInfoBroadcastReceiver);
        mNetworkFactoryMessenger = null;
        cleanUpAllConnections(true, null);
        super.dispose();
        mPhone.getContext().getContentResolver().unregisterContentObserver(mApnObserver);
        mApnContexts.clear();
        mPrioritySortedApnContexts.clear();
        if (mCdmaSsm != null) {
            mCdmaSsm.dispose(this);
        }
        if (mOmhApt != null) {
            mOmhApt.unregisterForModemProfileReady(this);
        }
        destroyDataConnections();

#endif
}

ECode DcTracker::UnregisterForAllEvents()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
         //Unregister for all events
        mPhone.mCi.unregisterForAvailable(this);
        mPhone.mCi.unregisterForOffOrNotAvailable(this);
        IccRecords r = mIccRecords.get();
        if (r != null) {
            r.unregisterForRecordsLoaded(this);
            mIccRecords.set(null);
        }
        mPhone.mCi.unregisterForDataNetworkStateChanged(this);
        mPhone.getCallTracker().unregisterForVoiceCallEnded(this);
        mPhone.getCallTracker().unregisterForVoiceCallStarted(this);
        mPhone.getServiceStateTracker().unregisterForDataConnectionAttached(this);
        mPhone.getServiceStateTracker().unregisterForDataConnectionDetached(this);
        mPhone.getServiceStateTracker().unregisterForRoamingOn(this);
        mPhone.getServiceStateTracker().unregisterForRoamingOff(this);
        mPhone.getServiceStateTracker().unregisterForPsRestrictedEnabled(this);
        mPhone.getServiceStateTracker().unregisterForPsRestrictedDisabled(this);
        //SubscriptionManager.unregisterForDdsSwitch(this);

#endif
}

ECode DcTracker::ApnContextForNetworkRequest(
    /* [in] */ INetworkRequest* nr,
    /* [out] */ IApnContext** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        NetworkCapabilities nc = nr.networkCapabilities;
        // for now, ignore the bandwidth stuff
        if (nc.getTransportTypes().length > 0 &&
                nc.hasTransport(NetworkCapabilities.TRANSPORT_CELLULAR) == false) {
            return null;
        }
        // in the near term just do 1-1 matches.
        // TODO - actually try to match the set of capabilities
        int type = -1;
        String name = null;
        boolean error = false;
        if (nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)) {
            if (name != null) error = true;
            name = PhoneConstants.APN_TYPE_DEFAULT;
            type = ConnectivityManager.TYPE_MOBILE;
        }
        if (nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_MMS)) {
            if (name != null) error = true;
            name = PhoneConstants.APN_TYPE_MMS;
            type = ConnectivityManager.TYPE_MOBILE_MMS;
        }
        if (nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_SUPL)) {
            if (name != null) error = true;
            name = PhoneConstants.APN_TYPE_SUPL;
            type = ConnectivityManager.TYPE_MOBILE_SUPL;
        }
        if (nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_DUN)) {
            if (name != null) error = true;
            name = PhoneConstants.APN_TYPE_DUN;
            type = ConnectivityManager.TYPE_MOBILE_DUN;
        }
        if (nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_FOTA)) {
            if (name != null) error = true;
            name = PhoneConstants.APN_TYPE_FOTA;
            type = ConnectivityManager.TYPE_MOBILE_FOTA;
        }
        if (nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_IMS)) {
            if (name != null) error = true;
            name = PhoneConstants.APN_TYPE_IMS;
            type = ConnectivityManager.TYPE_MOBILE_IMS;
        }
        if (nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_CBS)) {
            if (name != null) error = true;
            name = PhoneConstants.APN_TYPE_CBS;
            type = ConnectivityManager.TYPE_MOBILE_CBS;
        }
        if (nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_IA)) {
            if (name != null) error = true;
            name = PhoneConstants.APN_TYPE_IA;
            type = ConnectivityManager.TYPE_MOBILE_IA;
        }
        if (nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_RCS)) {
            if (name != null) error = true;
            name = null;
            loge("RCS APN type not yet supported");
        }
        if (nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_XCAP)) {
            if (name != null) error = true;
            name = null;
            loge("XCAP APN type not yet supported");
        }
        if (nc.hasCapability(NetworkCapabilities.NET_CAPABILITY_EIMS)) {
            if (name != null) error = true;
            name = null;
            loge("EIMS APN type not yet supported");
        }
        if (error) {
            loge("Multiple apn types specified in request - result is unspecified!");
        }
        if (type == -1 || name == null) {
            loge("Unsupported NetworkRequest in Telephony: " + nr);
            return null;
        }
        ApnContext apnContext = mApnContexts.get(name);
        if (apnContext == null) {
            loge("Request for unsupported mobile type: " + type);
        }
        return apnContext;

#endif
}

ECode DcTracker::SetRadio(
    /* [in] */ Boolean on)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        final ITelephony phone = ITelephony.Stub.asInterface(ServiceManager.checkService("phone"));
        try {
            phone.setRadio(on);
        } catch (Exception e) {
            // Ignore.
        }

#endif
}

ECode DcTracker::IsApnTypeActive(
    /* [in] */ const String& type,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ApnContext apnContext = mApnContexts.get(type);
        if (apnContext == null) return false;
        return (apnContext.getDcAc() != null);

#endif
}

ECode DcTracker::IsOnDemandDataPossible(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        /*
         * Check if APN enabled
         * Check if MobileData is ON
         * Check if MobileData UI override present
         */
        boolean flag = false;
        ApnContext apnContext = mApnContexts.get(apnType);
        if (apnContext == null) {
            return false;
        }
        boolean apnContextIsEnabled = apnContext.isEnabled();
        DctConstants.State apnContextState = apnContext.getState();
        boolean apnTypePossible = !(apnContextIsEnabled &&
                (apnContextState == DctConstants.State.FAILED));
        boolean userDataEnabled = mUserDataEnabled;
        if (PhoneConstants.APN_TYPE_MMS.equals(apnType)) {
            boolean mobileDataOffOveride = mPhone.getContext().getResources().
                getBoolean(com.android.internal.R.bool.config_enable_mms_with_mobile_data_off);
            log("isOnDemandDataPossible MobileDataEnabled override = " + mobileDataOffOveride);
            userDataEnabled = (mUserDataEnabled || mobileDataOffOveride);
        }
        flag = apnTypePossible && userDataEnabled;
        log("isOnDemandDataPossible, possible =" + flag + ", apnContext = " + apnContext);
        return flag;

#endif
}

ECode DcTracker::IsDataPossible(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ApnContext apnContext = mApnContexts.get(apnType);
        if (apnContext == null) {
            return false;
        }
        boolean apnContextIsEnabled = apnContext.isEnabled();
        DctConstants.State apnContextState = apnContext.getState();
        boolean apnTypePossible = !(apnContextIsEnabled &&
                (apnContextState == DctConstants.State.FAILED));
        boolean isEmergencyApn = apnContext.getApnType().equals(PhoneConstants.APN_TYPE_EMERGENCY);
        // Set the emergency APN availability status as TRUE irrespective of conditions checked in
        // isDataAllowed() like IN_SERVICE, MOBILE DATA status etc.
        boolean dataAllowed = isEmergencyApn || isDataAllowed();
        boolean possible = dataAllowed && apnTypePossible;
        if ((apnContext.getApnType().equals(PhoneConstants.APN_TYPE_DEFAULT)
                    || apnContext.getApnType().equals(PhoneConstants.APN_TYPE_IA))
                && (mPhone.getServiceState().getRilDataRadioTechnology()
                == ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN)
                && (!mWwanIwlanCoexistFlag)) {
            log("Default data call activation not possible in iwlan.");
            possible = false;
        }
        if (VDBG) {
            log(String.format("isDataPossible(%s): possible=%b isDataAllowed=%b " +
                    "apnTypePossible=%b apnContextisEnabled=%b apnContextState()=%s",
                    apnType, possible, dataAllowed, apnTypePossible,
                    apnContextIsEnabled, apnContextState));
        }
        return possible;

#endif
}

ECode DcTracker::Finalize()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if(DBG) log("finalize");

#endif
}

ECode DcTracker::SupplyMessenger()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
       // Supply the data connection tracker messenger only if
       // this is corresponding to the current DDS.
       if (!isActiveDataSubscription()) {
           return;
       }
        ConnectivityManager cm = (ConnectivityManager)mPhone.getContext().getSystemService(
                Context.CONNECTIVITY_SERVICE);
        cm.supplyMessenger(ConnectivityManager.TYPE_MOBILE, new Messenger(this));
        cm.supplyMessenger(ConnectivityManager.TYPE_MOBILE_MMS, new Messenger(this));
        cm.supplyMessenger(ConnectivityManager.TYPE_MOBILE_SUPL, new Messenger(this));
        cm.supplyMessenger(ConnectivityManager.TYPE_MOBILE_DUN, new Messenger(this));
        cm.supplyMessenger(ConnectivityManager.TYPE_MOBILE_HIPRI, new Messenger(this));
        cm.supplyMessenger(ConnectivityManager.TYPE_MOBILE_FOTA, new Messenger(this));
        cm.supplyMessenger(ConnectivityManager.TYPE_MOBILE_IMS, new Messenger(this));
        cm.supplyMessenger(ConnectivityManager.TYPE_MOBILE_CBS, new Messenger(this));
        cm.supplyMessenger(ConnectivityManager.TYPE_MOBILE_EMERGENCY, new Messenger(this));

#endif
}

ECode DcTracker::AddApnContext(
    /* [in] */ const String& type,
    /* [in] */ INetworkConfig* networkConfig,
    /* [out] */ IApnContext** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ApnContext apnContext = new ApnContext(mPhone.getContext(), type, LOG__TAG, networkConfig,
                this);
        mApnContexts.put(type, apnContext);
        mPrioritySortedApnContexts.add(apnContext);
        return apnContext;

#endif
}

ECode DcTracker::InitApnContexts()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        log("initApnContexts: E");
        // Load device network attributes from resources
        String[] networkConfigStrings = mPhone.getContext().getResources().getStringArray(
                com.android.internal.R.array.networkAttributes);
        for (String networkConfigString : networkConfigStrings) {
            NetworkConfig networkConfig = new NetworkConfig(networkConfigString);
            ApnContext apnContext = null;
            switch (networkConfig.type) {
            case ConnectivityManager.TYPE_MOBILE:
                apnContext = addApnContext(PhoneConstants.APN_TYPE_DEFAULT, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_MMS:
                apnContext = addApnContext(PhoneConstants.APN_TYPE_MMS, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_SUPL:
                apnContext = addApnContext(PhoneConstants.APN_TYPE_SUPL, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_DUN:
                apnContext = addApnContext(PhoneConstants.APN_TYPE_DUN, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_HIPRI:
                apnContext = addApnContext(PhoneConstants.APN_TYPE_HIPRI, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_FOTA:
                apnContext = addApnContext(PhoneConstants.APN_TYPE_FOTA, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_IMS:
                apnContext = addApnContext(PhoneConstants.APN_TYPE_IMS, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_CBS:
                apnContext = addApnContext(PhoneConstants.APN_TYPE_CBS, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_IA:
                apnContext = addApnContext(PhoneConstants.APN_TYPE_IA, networkConfig);
                break;
            case ConnectivityManager.TYPE_MOBILE_EMERGENCY:
                apnContext = addApnContext(PhoneConstants.APN_TYPE_EMERGENCY, networkConfig);
                break;
            default:
                log("initApnContexts: skipping unknown type=" + networkConfig.type);
                continue;
            }
            log("initApnContexts: apnContext=" + apnContext);
        }
        log("initApnContexts: X mApnContexts=" + mApnContexts);

#endif
}

ECode DcTracker::GetLinkProperties(
    /* [in] */ const String& apnType,
    /* [out] */ ILinkProperties** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ApnContext apnContext = mApnContexts.get(apnType);
        if (apnContext != null) {
            DcAsyncChannel dcac = apnContext.getDcAc();
            if (dcac != null) {
                if (DBG) log("return link properites for " + apnType);
                return dcac.getLinkPropertiesSync();
            }
        }
        if (DBG) log("return new LinkProperties");
        return new LinkProperties();

#endif
}

ECode DcTracker::GetNetworkCapabilities(
    /* [in] */ const String& apnType,
    /* [out] */ INetworkCapabilities** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ApnContext apnContext = mApnContexts.get(apnType);
        if (apnContext!=null) {
            DcAsyncChannel dataConnectionAc = apnContext.getDcAc();
            if (dataConnectionAc != null) {
                if (DBG) {
                    log("get active pdp is not null, return NetworkCapabilities for " + apnType);
                }
                return dataConnectionAc.getNetworkCapabilitiesSync();
            }
        }
        if (DBG) log("return new NetworkCapabilities");
        return new NetworkCapabilities();

#endif
}

ECode DcTracker::GetActiveApnTypes(
    /* [out, callee] */ ArrayOf<String>** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("get all active apn types");
        ArrayList<String> result = new ArrayList<String>();
        for (ApnContext apnContext : mApnContexts.values()) {
            if (mAttached.get() && apnContext.isReady()) {
                result.add(apnContext.getApnType());
            }
        }
        return result.toArray(new String[0]);

#endif
}

ECode DcTracker::GetActiveApnString(
    /* [in] */ const String& apnType,
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (VDBG) log( "get active apn string for type:" + apnType);
        ApnContext apnContext = mApnContexts.get(apnType);
        if (apnContext != null) {
            ApnSetting apnSetting = apnContext.getApnSetting();
            if (apnSetting != null) {
                return apnSetting.apn;
            }
        }
        return null;

#endif
}

ECode DcTracker::IsApnTypeEnabled(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ApnContext apnContext = mApnContexts.get(apnType);
        if (apnContext == null) {
            return false;
        }
        return apnContext.isEnabled();

#endif
}

ECode DcTracker::SetState(
    /* [in] */ DctConstantsState s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("setState should not be used in GSM" + s);

#endif
}

ECode DcTracker::GetState(
    /* [in] */ const String& apnType,
    /* [out] */ DctConstantsState* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ApnContext apnContext = mApnContexts.get(apnType);
        if (apnContext != null) {
            return apnContext.getState();
        }
        return DctConstants.State.FAILED;

#endif
}

ECode DcTracker::IsProvisioningApn(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ApnContext apnContext = mApnContexts.get(apnType);
        if (apnContext != null) {
            return apnContext.isProvisioningApn();
        }
        return false;

#endif
}

ECode DcTracker::GetOverallState(
    /* [out] */ DctConstantsState* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        boolean isConnecting = false;
        boolean isFailed = true; // All enabled Apns should be FAILED.
        boolean isAnyEnabled = false;
        for (ApnContext apnContext : mApnContexts.values()) {
            if (apnContext.isEnabled()) {
                isAnyEnabled = true;
                switch (apnContext.getState()) {
                case CONNECTED:
                case DISCONNECTING:
                    if (DBG) log("overall state is CONNECTED");
                    return DctConstants.State.CONNECTED;
                case RETRYING:
                case CONNECTING:
                    isConnecting = true;
                    isFailed = false;
                    break;
                case IDLE:
                case SCANNING:
                    isFailed = false;
                    break;
                default:
                    isAnyEnabled = true;
                    break;
                }
            }
        }
        if (!isAnyEnabled) { // Nothing enabled. return IDLE.
            if (DBG) log( "overall state is IDLE");
            return DctConstants.State.IDLE;
        }
        if (isConnecting) {
            if (DBG) log( "overall state is CONNECTING");
            return DctConstants.State.CONNECTING;
        } else if (!isFailed) {
            if (DBG) log( "overall state is IDLE");
            return DctConstants.State.IDLE;
        } else {
            if (DBG) log( "overall state is FAILED");
            return DctConstants.State.FAILED;
        }

#endif
}

ECode DcTracker::IsApnTypeAvailable(
    /* [in] */ const String& type,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (type.equals(PhoneConstants.APN_TYPE_DUN) && fetchDunApn() != null) {
            return true;
        }
        if (mAllApnSettings != null) {
            for (ApnSetting apn : mAllApnSettings) {
                if (apn.canHandleType(type)) {
                    return true;
                }
            }
        }
        return false;

#endif
}

ECode DcTracker::GetAnyDataEnabled(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mDataEnabledLock) {
            if (!(mInternalDataEnabled && mUserDataEnabled && sPolicyDataEnabled)) return false;
            for (ApnContext apnContext : mApnContexts.values()) {
                // Make sure we don't have a context that is going down
                // and is explicitly disabled.
                if (isDataAllowed(apnContext)) {
                    return true;
                }
            }
            return false;
        }

#endif
}

ECode DcTracker::GetAnyDataEnabled(
    /* [in] */ Boolean checkUserDataEnabled,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mDataEnabledLock) {
            if (!(mInternalDataEnabled && (!checkUserDataEnabled || mUserDataEnabled)
                        && (!checkUserDataEnabled || sPolicyDataEnabled)))
                return false;
            for (ApnContext apnContext : mApnContexts.values()) {
                // Make sure we dont have a context that going down
                // and is explicitly disabled.
                if (isDataAllowed(apnContext)) {
                    return true;
                }
            }
            return false;
        }

#endif
}

ECode DcTracker::IsDataAllowed(
    /* [in] */ IApnContext* apnContext,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        //If RAT is iwlan then dont allow default/IA PDP at all.
        //Rest of APN types can be evaluated for remaining conditions.
        if ((apnContext.getApnType().equals(PhoneConstants.APN_TYPE_DEFAULT)
                    || apnContext.getApnType().equals(PhoneConstants.APN_TYPE_IA))
                && (mPhone.getServiceState().getRilDataRadioTechnology()
                    == ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN)
                && (!mWwanIwlanCoexistFlag)) {
            log("Default data call activation not allowed in iwlan.");
            return false;
        } else {
            return apnContext.isReady() && isDataAllowed();
        }

#endif
}

ECode DcTracker::OnDataConnectionDetached()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        /*
         * We presently believe it is unnecessary to tear down the PDP context
         * when GPRS detaches, but we should stop the network polling.
         */
        if (DBG) log ("onDataConnectionDetached: stop polling and notify detached");
        stopNetStatPoll();
        stopDataStallAlarm();
        notifyDataConnection(Phone.REASON_DATA_DETACHED);
        mAttached.set(false);

#endif
}

ECode DcTracker::OnDataConnectionAttached()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("onDataConnectionAttached");
        mAttached.set(true);
        if (getOverallState() == DctConstants.State.CONNECTED) {
            if (DBG) log("onDataConnectionAttached: start polling notify attached");
            startNetStatPoll();
            startDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
            notifyDataConnection(Phone.REASON_DATA_ATTACHED);
        } else {
            // update APN availability so that APN can be enabled.
            notifyOffApnsOfAvailability(Phone.REASON_DATA_ATTACHED);
        }
        if (mAutoAttachOnCreationConfig) {
            mAutoAttachOnCreation = true;
        }
        setupDataOnConnectableApns(Phone.REASON_DATA_ATTACHED);

#endif
}

ECode DcTracker::IsDataAllowed(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        final boolean internalDataEnabled;
        synchronized (mDataEnabledLock) {
            internalDataEnabled = mInternalDataEnabled;
        }
        boolean attachedState = mAttached.get();
        boolean desiredPowerState = mPhone.getServiceStateTracker().getDesiredPowerState();
        int radioTech = mPhone.getServiceState().getRilDataRadioTechnology();
        if (radioTech == ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN
                && desiredPowerState == false) {
            desiredPowerState = true;
            attachedState = true;
        }
        IccRecords r = mIccRecords.get();
        boolean recordsLoaded = (r != null) ? r.getRecordsLoaded() : false;
        boolean subscriptionFromNv = isNvSubscription();
        boolean allowed =
                    (attachedState || mAutoAttachOnCreation) &&
                    (subscriptionFromNv || recordsLoaded) &&
                    (mPhone.getState() == PhoneConstants.State.IDLE ||
                     mPhone.getServiceStateTracker().isConcurrentVoiceAndDataAllowed()) &&
                    internalDataEnabled &&
                    (!mPhone.getServiceState().getRoaming() || getDataOnRoamingEnabled()) &&
                    !mIsPsRestricted &&
                    desiredPowerState;
        if (!allowed && DBG) {
            String reason = "";
            if (!(attachedState || mAutoAttachOnCreation)) {
                reason += " - Attached= " + attachedState;
            }
            if (!(subscriptionFromNv || recordsLoaded)) {
                reason += " - SIM not loaded and not NV subscription";
            }
            if (mPhone.getState() != PhoneConstants.State.IDLE &&
                    !mPhone.getServiceStateTracker().isConcurrentVoiceAndDataAllowed()) {
                reason += " - PhoneState= " + mPhone.getState();
                reason += " - Concurrent voice and data not allowed";
            }
            if (!internalDataEnabled) reason += " - mInternalDataEnabled= false";
            if (mPhone.getServiceState().getRoaming() && !getDataOnRoamingEnabled()) {
                reason += " - Roaming and data roaming not enabled";
            }
            if (mIsPsRestricted) reason += " - mIsPsRestricted= true";
            if (!desiredPowerState) reason += " - desiredPowerState= false";
            if (DBG) log("isDataAllowed: not allowed due to" + reason);
        }
        return allowed;

#endif
}

ECode DcTracker::SetupDataOnConnectableApns(
    /* [in] */ const String& reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("setupDataOnConnectableApns: " + reason);
        for (ApnContext apnContext : mPrioritySortedApnContexts) {
            if (DBG) log("setupDataOnConnectableApns: apnContext " + apnContext);
            if (apnContext.getState() == DctConstants.State.FAILED) {
                apnContext.setState(DctConstants.State.IDLE);
            }
            if (apnContext.isConnectable()) {
                log("setupDataOnConnectableApns: isConnectable() call trySetupData");
                apnContext.setReason(reason);
                trySetupData(apnContext);
            }
        }

#endif
}

ECode DcTracker::TrySetupData(
    /* [in] */ IApnContext* apnContext,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) {
            log("trySetupData for type:" + apnContext.getApnType() +
                    " due to " + apnContext.getReason() + " apnContext=" + apnContext);
            log("trySetupData with mIsPsRestricted=" + mIsPsRestricted);
        }
        if (mPhone.getSimulatedRadioControl() != null) {
            // Assume data is connected on the simulator
            // FIXME  this can be improved
            apnContext.setState(DctConstants.State.CONNECTED);
            mPhone.notifyDataConnection(apnContext.getReason(), apnContext.getApnType());
            log("trySetupData: X We're on the simulator; assuming connected retValue=true");
            return true;
        }
        // Allow SETUP_DATA request for E-APN to be completed during emergency call
        // and MOBILE DATA On/Off cases as well.
        boolean isEmergencyApn = apnContext.getApnType().equals(PhoneConstants.APN_TYPE_EMERGENCY);
        boolean desiredPowerState = mPhone.getServiceStateTracker().getDesiredPowerState();
        boolean checkUserDataEnabled =
                    !(apnContext.getApnType().equals(PhoneConstants.APN_TYPE_IMS));
        // If set the special property, enable mms data even if mobile data is turned off.
        if (apnContext.getApnType().equals(PhoneConstants.APN_TYPE_MMS)) {
            checkUserDataEnabled = checkUserDataEnabled && !(mPhone.getContext().getResources().
                    getBoolean(com.android.internal.R.bool.config_enable_mms_with_mobile_data_off));
        }
        if (apnContext.isConnectable() && (isEmergencyApn ||
                (isDataAllowed(apnContext) &&
                getAnyDataEnabled(checkUserDataEnabled) && !isEmergency()))) {
            if (apnContext.getState() == DctConstants.State.FAILED) {
                if (DBG) log("trySetupData: make a FAILED ApnContext IDLE so its reusable");
                apnContext.setState(DctConstants.State.IDLE);
            }
            int radioTech = mPhone.getServiceState().getRilDataRadioTechnology();
            if (apnContext.getState() == DctConstants.State.IDLE) {
                ArrayList<ApnSetting> waitingApns = buildWaitingApns(apnContext.getApnType(),
                        radioTech);
                if (waitingApns.isEmpty()) {
                    notifyNoData(DcFailCause.MISSING_UNKNOWN_APN, apnContext);
                    notifyOffApnsOfAvailability(apnContext.getReason());
                    if (DBG) log("trySetupData: X No APN found retValue=false");
                    return false;
                } else {
                    apnContext.setWaitingApns(waitingApns);
                    if (DBG) {
                        log ("trySetupData: Create from mAllApnSettings : "
                                    + apnListToString(mAllApnSettings));
                    }
                }
            }
            if (DBG) {
                if (apnContext.getWaitingApns() == null) {
                    log("trySetupData: call setupData, waitingApns : null");
                }
                else {
                    log("trySetupData: call setupData, waitingApns : "
                            + apnListToString(apnContext.getWaitingApns()));
                }
            }
            boolean retValue = setupData(apnContext, radioTech);
            notifyOffApnsOfAvailability(apnContext.getReason());
            if (DBG) log("trySetupData: X retValue=" + retValue);
            return retValue;
        } else {
            if (!apnContext.getApnType().equals(PhoneConstants.APN_TYPE_DEFAULT)
                    && apnContext.isConnectable()) {
                mPhone.notifyDataConnectionFailed(apnContext.getReason(), apnContext.getApnType());
            }
            notifyOffApnsOfAvailability(apnContext.getReason());
            if (DBG) log ("trySetupData: X apnContext not 'ready' retValue=false");
            return false;
        }

#endif
}

ECode DcTracker::NotifyOffApnsOfAvailability(
    /* [in] */ const String& reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        for (ApnContext apnContext : mApnContexts.values()) {
            if ((!mAttached.get() && mOosIsDisconnect) || !apnContext.isReady()) {
                if (VDBG) log("notifyOffApnOfAvailability type:" + apnContext.getApnType());
                mPhone.notifyDataConnection(reason != null ? reason : apnContext.getReason(),
                                            apnContext.getApnType(),
                                            PhoneConstants.DataState.DISCONNECTED);
            } else {
                if (VDBG) {
                    log("notifyOffApnsOfAvailability skipped apn due to attached && isReady " +
                            apnContext.toString());
                }
            }
        }

#endif
}

ECode DcTracker::CleanUpAllConnections(
    /* [in] */ Boolean tearDown,
    /* [in] */ const String& reason,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("cleanUpAllConnections: tearDown=" + tearDown + " reason=" + reason);
        boolean didDisconnect = false;
        boolean specificdisable = false;
        if (!TextUtils.isEmpty(reason)) {
            specificdisable = reason.equals(Phone.REASON_DATA_SPECIFIC_DISABLED);
        }
        for (ApnContext apnContext : mApnContexts.values()) {
            if (apnContext.isDisconnected() == false) didDisconnect = true;
            if (specificdisable) {
                if (!apnContext.getApnType().equals(PhoneConstants.APN_TYPE_IMS)) {
                    if (DBG) log("ApnConextType: " + apnContext.getApnType());
                    apnContext.setReason(reason);
                    cleanUpConnection(tearDown, apnContext);
                }
            } else {
                // TODO - only do cleanup if not disconnected
                apnContext.setReason(reason);
                cleanUpConnection(tearDown, apnContext);
            }
        }
        stopNetStatPoll();
        stopDataStallAlarm();
        // TODO: Do we need mRequestedApnType?
        mRequestedApnType = PhoneConstants.APN_TYPE_DEFAULT;
        log("cleanUpConnection: mDisconnectPendingCount = " + mDisconnectPendingCount);
        if (tearDown && mDisconnectPendingCount == 0) {
            notifyDataDisconnectComplete();
            notifyAllDataDisconnected();
        }
        return didDisconnect;

#endif
}

ECode DcTracker::OnCleanUpAllConnections(
    /* [in] */ const String& cause)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        cleanUpAllConnections(true, cause);

#endif
}

ECode DcTracker::CleanUpConnection(
    /* [in] */ Boolean tearDown,
    /* [in] */ IApnContext* apnContext)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (apnContext == null) {
            if (DBG) log("cleanUpConnection: apn context is null");
            return;
        }
        DcAsyncChannel dcac = apnContext.getDcAc();
        if (DBG) {
            log("cleanUpConnection: E tearDown=" + tearDown + " reason=" + apnContext.getReason() +
                    " apnContext=" + apnContext);
        }
        if (tearDown) {
            if (apnContext.isDisconnected()) {
                // The request is tearDown and but ApnContext is not connected.
                // If apnContext is not enabled anymore, break the linkage to the DCAC/DC.
                apnContext.setState(DctConstants.State.IDLE);
                if (!apnContext.isReady()) {
                    if (dcac != null) {
                        dcac.tearDown(apnContext, "", null);
                    }
                    apnContext.setDataConnectionAc(null);
                }
            } else {
                // Connection is still there. Try to clean up.
                if (dcac != null) {
                    if (apnContext.getState() != DctConstants.State.DISCONNECTING) {
                        boolean disconnectAll = false;
                        if (PhoneConstants.APN_TYPE_DUN.equals(apnContext.getApnType())) {
                            // CAF_MSIM is this below condition required.
                            // if (PhoneConstants.APN_TYPE_DUN.equals(PhoneConstants.APN_TYPE_DEFAULT)) {
                            if (teardownForDun()) {
                                if (DBG) log("tearing down dedicated DUN connection");
                                // we need to tear it down - we brought it up just for dun and
                                // other people are camped on it and now dun is done.  We need
                                // to stop using it and let the normal apn list get used to find
                                // connections for the remaining desired connections
                                disconnectAll = true;
                            }
                        }
                        if (DBG) {
                            log("cleanUpConnection: tearing down" + (disconnectAll ? " all" :""));
                        }
                        Message msg = obtainMessage(DctConstants.EVENT_DISCONNECT_DONE, apnContext);
                        if (disconnectAll) {
                            apnContext.getDcAc().tearDownAll(apnContext.getReason(), msg);
                        } else {
                            apnContext.getDcAc()
                                .tearDown(apnContext, apnContext.getReason(), msg);
                        }
                        apnContext.setState(DctConstants.State.DISCONNECTING);
                        mDisconnectPendingCount++;
                    }
                } else {
                    // apn is connected but no reference to dcac.
                    // Should not be happen, but reset the state in case.
                    apnContext.setState(DctConstants.State.IDLE);
                    mPhone.notifyDataConnection(apnContext.getReason(),
                                                apnContext.getApnType());
                }
            }
        } else {
            // force clean up the data connection.
            if (dcac != null) dcac.reqReset();
            apnContext.setState(DctConstants.State.IDLE);
            mPhone.notifyDataConnection(apnContext.getReason(), apnContext.getApnType());
            apnContext.setDataConnectionAc(null);
        }
        if (mOmhApt != null) {
            mOmhApt.clearActiveApnProfile();
        }
        // Make sure reconnection alarm is cleaned up if there is no ApnContext
        // associated to the connection.
        if (dcac != null) {
            cancelReconnectAlarm(apnContext);
        }
        setupDataForSinglePdnArbitration(apnContext.getReason());
        if (DBG) {
            log("cleanUpConnection: X tearDown=" + tearDown + " reason=" + apnContext.getReason() +
                    " apnContext=" + apnContext + " dcac=" + apnContext.getDcAc());
        }

#endif
}

ECode DcTracker::SetupDataForSinglePdnArbitration(
    /* [in] */ const String& reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // In single pdn case, if a higher priority call which was scheduled for retry gets
        // cleaned up due to say apn disabled, we need to try setup data on connectable apns
        // as there won't be any EVENT_DISCONNECT_DONE call back.
        if(DBG) {
            log("setupDataForSinglePdn: reason = " + reason
                    + " isDisconnected = " + isDisconnected());
        }
        if (isOnlySingleDcAllowed(mPhone.getServiceState().getRilDataRadioTechnology())
                && isDisconnected()
                && !Phone.REASON_SINGLE_PDN_ARBITRATION.equals(reason)) {
            setupDataOnConnectableApns(Phone.REASON_SINGLE_PDN_ARBITRATION);
        }

#endif
}

ECode DcTracker::TeardownForDun(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // CDMA always needs to do this the profile id is correct
        final int rilRat = mPhone.getServiceState().getRilDataRadioTechnology();
        if (ServiceState.isCdma(rilRat)) return true;
        return (fetchDunApn() != null);

#endif
}

ECode DcTracker::CancelReconnectAlarm(
    /* [in] */ IApnContext* apnContext)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (apnContext == null) return;
        PendingIntent intent = apnContext.getReconnectIntent();
        if (intent != null) {
                AlarmManager am =
                    (AlarmManager) mPhone.getContext().getSystemService(Context.ALARM_SERVICE);
                am.cancel(intent);
                apnContext.setReconnectIntent(null);
        }

#endif
}

ECode DcTracker::ParseTypes(
    /* [in] */ const String& types,
    /* [out, callee] */ ArrayOf<String>** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String[] result;
        // If unset, set to DEFAULT.
        if (types == null || types.equals("")) {
            result = new String[1];
            result[0] = PhoneConstants.APN_TYPE_ALL;
        } else {
            result = types.split(",");
        }
        return result;

#endif
}

ECode DcTracker::ImsiMatches(
    /* [in] */ const String& imsiDB,
    /* [in] */ const String& imsiSIM,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Note: imsiDB value has digit number or 'x' character for seperating USIM information
        // for MVNO operator. And then digit number is matched at same order and 'x' character
        // could replace by any digit number.
        // ex) if imsiDB inserted '310260x10xxxxxx' for GG Operator,
        //     that means first 6 digits, 8th and 9th digit
        //     should be set in USIM for GG Operator.
        int len = imsiDB.length();
        int idxCompare = 0;
        if (len <= 0) return false;
        if (len > imsiSIM.length()) return false;
        for (int idx=0; idx<len; idx++) {
            char c = imsiDB.charAt(idx);
            if ((c == 'x') || (c == 'X') || (c == imsiSIM.charAt(idx))) {
                continue;
            } else {
                return false;
            }
        }
        return true;

#endif
}

ECode DcTracker::MvnoMatches(
    /* [in] */ IIccRecords* r,
    /* [in] */ const String& mvnoType,
    /* [in] */ const String& mvnoMatchData,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mvnoType.equalsIgnoreCase("spn")) {
            if ((r.getServiceProviderName() != null) &&
                    r.getServiceProviderName().equalsIgnoreCase(mvnoMatchData)) {
                return true;
            }
        } else if (mvnoType.equalsIgnoreCase("imsi")) {
            String imsiSIM = r.getIMSI();
            if ((imsiSIM != null) && imsiMatches(mvnoMatchData, imsiSIM)) {
                return true;
            }
        } else if (mvnoType.equalsIgnoreCase("gid")) {
            String gid1 = r.getGid1();
            int mvno_match_data_length = mvnoMatchData.length();
            if ((gid1 != null) && (gid1.length() >= mvno_match_data_length) &&
                    gid1.substring(0, mvno_match_data_length).equalsIgnoreCase(mvnoMatchData)) {
                return true;
            }
        }
        return false;

#endif
}

ECode DcTracker::IsPermanentFail(
    /* [in] */ IDcFailCause* dcFailCause,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (dcFailCause.isPermanentFail() &&
                (mAttached.get() == false || dcFailCause != DcFailCause.SIGNAL_LOST));

#endif
}

ECode DcTracker::MakeApnSetting(
    /* [in] */ ICursor* cursor,
    /* [out] */ IApnSetting** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String[] types = parseTypes(
                cursor.getString(cursor.getColumnIndexOrThrow(Telephony.Carriers.TYPE)));
        ApnSetting apn = new ApnSetting(
                cursor.getInt(cursor.getColumnIndexOrThrow(Telephony.Carriers._ID)),
                cursor.getString(cursor.getColumnIndexOrThrow(Telephony.Carriers.NUMERIC)),
                cursor.getString(cursor.getColumnIndexOrThrow(Telephony.Carriers.NAME)),
                cursor.getString(cursor.getColumnIndexOrThrow(Telephony.Carriers.APN)),
                NetworkUtils.trimV4AddrZeros(
                        cursor.getString(
                        cursor.getColumnIndexOrThrow(Telephony.Carriers.PROXY))),
                cursor.getString(cursor.getColumnIndexOrThrow(Telephony.Carriers.PORT)),
                NetworkUtils.trimV4AddrZeros(
                        cursor.getString(
                        cursor.getColumnIndexOrThrow(Telephony.Carriers.MMSC))),
                NetworkUtils.trimV4AddrZeros(
                        cursor.getString(
                        cursor.getColumnIndexOrThrow(Telephony.Carriers.MMSPROXY))),
                cursor.getString(cursor.getColumnIndexOrThrow(Telephony.Carriers.MMSPORT)),
                cursor.getString(cursor.getColumnIndexOrThrow(Telephony.Carriers.USER)),
                cursor.getString(cursor.getColumnIndexOrThrow(Telephony.Carriers.PASSWORD)),
                cursor.getInt(cursor.getColumnIndexOrThrow(Telephony.Carriers.AUTH_TYPE)),
                types,
                cursor.getString(cursor.getColumnIndexOrThrow(Telephony.Carriers.PROTOCOL)),
                cursor.getString(cursor.getColumnIndexOrThrow(
                        Telephony.Carriers.ROAMING_PROTOCOL)),
                cursor.getInt(cursor.getColumnIndexOrThrow(
                        Telephony.Carriers.CARRIER_ENABLED)) == 1,
                cursor.getInt(cursor.getColumnIndexOrThrow(Telephony.Carriers.BEARER)),
                cursor.getInt(cursor.getColumnIndexOrThrow(Telephony.Carriers.PROFILE_ID)),
                cursor.getInt(cursor.getColumnIndexOrThrow(
                        Telephony.Carriers.MODEM_COGNITIVE)) == 1,
                cursor.getInt(cursor.getColumnIndexOrThrow(Telephony.Carriers.MAX_CONNS)),
                cursor.getInt(cursor.getColumnIndexOrThrow(
                        Telephony.Carriers.WAIT_TIME)),
                cursor.getInt(cursor.getColumnIndexOrThrow(Telephony.Carriers.MAX_CONNS_TIME)),
                cursor.getInt(cursor.getColumnIndexOrThrow(Telephony.Carriers.MTU)),
                cursor.getString(cursor.getColumnIndexOrThrow(Telephony.Carriers.MVNO_TYPE)),
                cursor.getString(cursor.getColumnIndexOrThrow(Telephony.Carriers.MVNO_MATCH_DATA)));
        return apn;

#endif
}

ECode DcTracker::CreateApnList(
    /* [in] */ ICursor* cursor,
    /* [out] */ IArrayList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ArrayList<ApnSetting> mnoApns = new ArrayList<ApnSetting>();
        ArrayList<ApnSetting> mvnoApns = new ArrayList<ApnSetting>();
        IccRecords r = mIccRecords.get();
        if (cursor.moveToFirst()) {
            do {
                ApnSetting apn = makeApnSetting(cursor);
                if (apn == null) {
                    continue;
                }
                if (apn.hasMvnoParams()) {
                    if (r != null && mvnoMatches(r, apn.mvnoType, apn.mvnoMatchData)) {
                        mvnoApns.add(apn);
                    }
                } else {
                    mnoApns.add(apn);
                }
            } while (cursor.moveToNext());
        }
        ArrayList<ApnSetting> result = mvnoApns.isEmpty() ? mnoApns : mvnoApns;
        if (DBG) log("createApnList: X result=" + result);
        return result;

#endif
}

ECode DcTracker::DataConnectionNotInUse(
    /* [in] */ IDcAsyncChannel* dcac,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("dataConnectionNotInUse: check if dcac is inuse dcac=" + dcac);
        for (ApnContext apnContext : mApnContexts.values()) {
            if (apnContext.getDcAc() == dcac) {
                if (DBG) log("dataConnectionNotInUse: in use by apnContext=" + apnContext);
                return false;
            }
        }
        // TODO: Fix retry handling so free DataConnections have empty apnlists.
        // Probably move retry handling into DataConnections and reduce complexity
        // of DCT.
        if (DBG) log("dataConnectionNotInUse: tearDownAll");
        dcac.tearDownAll("No connection", null);
        if (DBG) log("dataConnectionNotInUse: not in use return true");
        return true;

#endif
}

ECode DcTracker::FindFreeDataConnection(
    /* [out] */ IDcAsyncChannel** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        for (DcAsyncChannel dcac : mDataConnectionAcHashMap.values()) {
            if (dcac.isInactiveSync() && dataConnectionNotInUse(dcac)) {
                if (DBG) {
                    log("findFreeDataConnection: found free DataConnection=" +
                        " dcac=" + dcac);
                }
                return dcac;
            }
        }
        log("findFreeDataConnection: NO free DataConnection");
        return null;

#endif
}

ECode DcTracker::SetupData(
    /* [in] */ IApnContext* apnContext,
    /* [in] */ Int32 radioTech,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("setupData: apnContext=" + apnContext);
        ApnSetting apnSetting;
        DcAsyncChannel dcac = null;
        apnSetting = apnContext.getNextWaitingApn();
        if (apnSetting == null) {
            if (DBG) log("setupData: return for no apn found!");
            return false;
        }
        int profileId = apnSetting.profileId;
        if (profileId == 0) {
            profileId = getApnProfileID(apnContext.getApnType());
        }
        // On CDMA, if we're explicitly asking for DUN, we need have
        // a dun-profiled connection so we can't share an existing one
        // On GSM/LTE we can share existing apn connections provided they support
        // this type.
        if (apnContext.getApnType() != PhoneConstants.APN_TYPE_DUN ||
                teardownForDun() == false) {
            dcac = checkForCompatibleConnectedApnContext(apnContext);
            if (dcac != null) {
                // Get the dcacApnSetting for the connection we want to share.
                ApnSetting dcacApnSetting = dcac.getApnSettingSync();
                if (dcacApnSetting != null) {
                    // Setting is good, so use it.
                    apnSetting = dcacApnSetting;
                }
            }
        }
        if (dcac == null) {
            if (isOnlySingleDcAllowed(radioTech)) {
                if (isHigherPriorityApnContextActive(apnContext)) {
                    if (DBG) {
                        log("setupData: Higher priority ApnContext active.  Ignoring call");
                    }
                    return false;
                }
                // Only lower priority calls left.  Disconnect them all in this single PDP case
                // so that we can bring up the requested higher priority call (once we receive
                // repsonse for deactivate request for the calls we are about to disconnect
                if (cleanUpAllConnections(true, Phone.REASON_SINGLE_PDN_ARBITRATION)) {
                    // If any call actually requested to be disconnected, means we can't
                    // bring up this connection yet as we need to wait for those data calls
                    // to be disconnected.
                    if (DBG) log("setupData: Some calls are disconnecting first.  Wait and retry");
                    return false;
                }
                // No other calls are active, so proceed
                if (DBG) log("setupData: Single pdp. Continue setting up data call.");
            }
            dcac = findFreeDataConnection();
            if (dcac == null) {
                dcac = createDataConnection();
            }
            if (dcac == null) {
                if (DBG) log("setupData: No free DataConnection and couldn't create one, WEIRD");
                return false;
            }
        }
        if (DBG) log("setupData: dcac=" + dcac + " apnSetting=" + apnSetting);
        apnContext.setDataConnectionAc(dcac);
        apnContext.setApnSetting(apnSetting);
        apnContext.setState(DctConstants.State.CONNECTING);
        mPhone.notifyDataConnection(apnContext.getReason(), apnContext.getApnType());
        Message msg = obtainMessage();
        msg.what = DctConstants.EVENT_DATA_SETUP_COMPLETE;
        msg.obj = apnContext;
        dcac.bringUp(apnContext, getInitialMaxRetry(), profileId, radioTech, mAutoAttachOnCreation,
                msg);
        if (DBG) log("setupData: initing!");
        return true;

#endif
}

ECode DcTracker::OnApnChanged()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("onApnChanged: tryRestartDataConnections");
        tryRestartDataConnections(true, Phone.REASON_APN_CHANGED);

#endif
}

ECode DcTracker::TryRestartDataConnections(
    /* [in] */ Boolean isCleanupNeeded,
    /* [in] */ const String& reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        DctConstants.State overallState = getOverallState();
        boolean isDisconnected = (overallState == DctConstants.State.IDLE ||
                overallState == DctConstants.State.FAILED);
        if (mPhone instanceof GSMPhone) {
            // The "current" may no longer be valid.  MMS depends on this to send properly. TBD
            ((GSMPhone)mPhone).updateCurrentCarrierInProvider();
        }
        // TODO: It'd be nice to only do this if the changed entrie(s)
        // match the current operator.
        if (DBG) log("tryRestartDataConnections: createAllApnList and cleanUpAllConnections");
        createAllApnList();
        setInitialAttachApn();
        if (isCleanupNeeded) {
            cleanUpAllConnections(!isDisconnected, reason);
        }
        // If the state is already connected don't setup data now.
        if (isDisconnected) {
            setupDataOnConnectableApns(reason);
        }

#endif
}

ECode DcTracker::OnWwanIwlanCoexistenceDone(
    /* [in] */ IAsyncResult* ar)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (ar.exception != null) {
            log("onWwanIwlanCoexistenceDone: error = " + ar.exception);
        } else {
            byte[] array = (byte[])ar.result;
            log("onWwanIwlanCoexistenceDone, payload hexdump = "
                    + IccUtils.bytesToHexString (array));
            ByteBuffer oemHookResponse = ByteBuffer.wrap(array);
            oemHookResponse.order(ByteOrder.nativeOrder());
            int resp = oemHookResponse.get();
            log("onWwanIwlanCoexistenceDone: resp = " + resp);
            boolean tempStatus = (resp > 0)? true : false;
            if (mWwanIwlanCoexistFlag == tempStatus) {
                log("onWwanIwlanCoexistenceDone: no change in status, ignore.");
                return;
            }
            mWwanIwlanCoexistFlag = tempStatus;
            if (mPhone.getServiceState().getRilDataRadioTechnology()
                    == ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN) {
                log("notifyDataConnection IWLAN_AVAILABLE");
                notifyDataConnection(Phone.REASON_IWLAN_AVAILABLE);
            }
        }

#endif
}

ECode DcTracker::OnModemApnProfileReady()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState == DctConstants.State.FAILED) {
            cleanUpAllConnections(false, Phone.REASON_PS_RESTRICT_ENABLED);
        }
        if (DBG) log("OMH: onModemApnProfileReady(): Setting up data call");
        tryRestartDataConnections(false, Phone.REASON_SIM_LOADED);

#endif
}

ECode DcTracker::FindDataConnectionAcByCid(
    /* [in] */ Int32 cid,
    /* [out] */ IDcAsyncChannel** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        for (DcAsyncChannel dcac : mDataConnectionAcHashMap.values()) {
            if (dcac.getCidSync() == cid) {
                return dcac;
            }
        }
        return null;

#endif
}

ECode DcTracker::GotoIdleAndNotifyDataConnection(
    /* [in] */ const String& reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("gotoIdleAndNotifyDataConnection: reason=" + reason);
        notifyDataConnection(reason);
        mActiveApn = null;

#endif
}

ECode DcTracker::IsHigherPriorityApnContextActive(
    /* [in] */ IApnContext* apnContext,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        for (ApnContext otherContext : mPrioritySortedApnContexts) {
            if (apnContext.getApnType().equalsIgnoreCase(otherContext.getApnType())) return false;
            if (otherContext.isEnabled() && otherContext.getState() != DctConstants.State.FAILED) {
                return true;
            }
        }
        return false;

#endif
}

ECode DcTracker::IsOnlySingleDcAllowed(
    /* [in] */ Int32 rilRadioTech,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int[] singleDcRats = mPhone.getContext().getResources().getIntArray(
                com.android.internal.R.array.config_onlySingleDcAllowed);
        boolean onlySingleDcAllowed = false;
        if (Build.IS_DEBUGGABLE &&
                SystemProperties.getBoolean("persist.telephony.test.singleDc", false)) {
            onlySingleDcAllowed = true;
        }
        if (singleDcRats != null) {
            for (int i=0; i < singleDcRats.length && onlySingleDcAllowed == false; i++) {
                if (rilRadioTech == singleDcRats[i]) onlySingleDcAllowed = true;
            }
        }
        if (DBG) log("isOnlySingleDcAllowed(" + rilRadioTech + "): " + onlySingleDcAllowed);
        return onlySingleDcAllowed;

#endif
}

ECode DcTracker::RestartRadio()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("restartRadio: ************TURN OFF RADIO**************");
        cleanUpAllConnections(true, Phone.REASON_RADIO_TURNED_OFF);
        mPhone.getServiceStateTracker().powerOffRadioSafely(this);
        /* Note: no need to call setRadioPower(true).  Assuming the desired
         * radio power state is still ON (as tracked by ServiceStateTracker),
         * ServiceStateTracker will call setRadioPower when it receives the
         * RADIO_STATE_CHANGED notification for the power off.  And if the
         * desired power state has changed in the interim, we don't want to
         * override it with an unconditional power on.
         */
        int reset = Integer.parseInt(SystemProperties.get("net.ppp.reset-by-timeout", "0"));
        SystemProperties.set("net.ppp.reset-by-timeout", String.valueOf(reset+1));

#endif
}

ECode DcTracker::RetryAfterDisconnected(
    /* [in] */ IApnContext* apnContext,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        boolean retry = true;
        String reason = apnContext.getReason();
        if ( Phone.REASON_RADIO_TURNED_OFF.equals(reason) ||
                (isOnlySingleDcAllowed(mPhone.getServiceState().getRilDataRadioTechnology())
                 && isHigherPriorityApnContextActive(apnContext))) {
            retry = false;
        }
        return retry;

#endif
}

ECode DcTracker::StartAlarmForReconnect(
    /* [in] */ Int32 delay,
    /* [in] */ IApnContext* apnContext)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String apnType = apnContext.getApnType();
        Intent intent = new Intent(INTENT_RECONNECT_ALARM + "." + apnType);
        intent.putExtra(INTENT_RECONNECT_ALARM_EXTRA_REASON, apnContext.getReason());
        intent.putExtra(INTENT_RECONNECT_ALARM_EXTRA_TYPE, apnType);
        // Get current sub id.
        long subId = SubscriptionManager.getDefaultDataSubId();
        intent.putExtra(PhoneConstants.SUBSCRIPTION_KEY, subId);
        if (DBG) {
            log("startAlarmForReconnect: delay=" + delay + " action=" + intent.getAction()
                    + " apn=" + apnContext);
        }
        PendingIntent alarmIntent = PendingIntent.getBroadcast (mPhone.getContext(), 0,
                                        intent, PendingIntent.FLAG_UPDATE_CURRENT);
        apnContext.setReconnectIntent(alarmIntent);
        mAlarmManager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                SystemClock.elapsedRealtime() + delay, alarmIntent);

#endif
}

ECode DcTracker::StartAlarmForRestartTrySetup(
    /* [in] */ Int32 delay,
    /* [in] */ IApnContext* apnContext)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String apnType = apnContext.getApnType();
        Intent intent = new Intent(INTENT_RESTART_TRYSETUP_ALARM + "." + apnType);
        intent.putExtra(INTENT_RESTART_TRYSETUP_ALARM_EXTRA_TYPE, apnType);
        if (DBG) {
            log("startAlarmForRestartTrySetup: delay=" + delay + " action=" + intent.getAction()
                    + " apn=" + apnContext);
        }
        PendingIntent alarmIntent = PendingIntent.getBroadcast (mPhone.getContext(), 0,
                                        intent, PendingIntent.FLAG_UPDATE_CURRENT);
        apnContext.setReconnectIntent(alarmIntent);
        mAlarmManager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                SystemClock.elapsedRealtime() + delay, alarmIntent);

#endif
}

ECode DcTracker::NotifyNoData(
    /* [in] */ IDcFailCause* lastFailCauseCode,
    /* [in] */ IApnContext* apnContext)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log( "notifyNoData: type=" + apnContext.getApnType());
        if (isPermanentFail(lastFailCauseCode)
            && (!apnContext.getApnType().equals(PhoneConstants.APN_TYPE_DEFAULT))) {
            mPhone.notifyDataConnectionFailed(apnContext.getReason(), apnContext.getApnType());
        }

#endif
}

ECode DcTracker::OnRecordsLoaded()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mOmhApt != null) {
            log("OMH: onRecordsLoaded(): calling loadProfiles()");
            /* query for data profiles stored in the modem */
            mOmhApt.loadProfiles();
            if (mPhone.mCi.getRadioState().isOn()) {
                if (DBG) log("onRecordsLoaded: notifying data availability");
                notifyOffApnsOfAvailability(Phone.REASON_SIM_LOADED);
            }
        } else {
            if (DBG) log("onRecordsLoaded: createAllApnList");
            if (mPhone.mCi.getRadioState().isOn()) {
                if (DBG) log("onRecordsLoaded: notifying data availability");
                notifyOffApnsOfAvailability(Phone.REASON_SIM_LOADED);
            }
            tryRestartDataConnections(false, Phone.REASON_SIM_LOADED);
       }

#endif
}

ECode DcTracker::OnNvReady()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("onNvReady");
        createAllApnList();
        setupDataOnConnectableApns(Phone.REASON_NV_READY);

#endif
}

ECode DcTracker::OnSetDependencyMet(
    /* [in] */ const String& apnType,
    /* [in] */ Boolean met)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // don't allow users to tweak hipri to work around default dependency not met
        if (PhoneConstants.APN_TYPE_HIPRI.equals(apnType)) return;
        ApnContext apnContext = mApnContexts.get(apnType);
        if (apnContext == null) {
            loge("onSetDependencyMet: ApnContext not found in onSetDependencyMet(" +
                    apnType + ", " + met + ")");
            return;
        }
        applyNewState(apnContext, apnContext.isEnabled(), met);
        if (PhoneConstants.APN_TYPE_DEFAULT.equals(apnType)) {
            // tie actions on default to similar actions on HIPRI regarding dependencyMet
            apnContext = mApnContexts.get(PhoneConstants.APN_TYPE_HIPRI);
            if (apnContext != null) applyNewState(apnContext, apnContext.isEnabled(), met);
        }

#endif
}

ECode DcTracker::ApplyNewState(
    /* [in] */ IApnContext* apnContext,
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean met)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        boolean cleanup = false;
        boolean trySetup = false;
        if (DBG) {
            log("applyNewState(" + apnContext.getApnType() + ", " + enabled +
                    "(" + apnContext.isEnabled() + "), " + met + "(" +
                    apnContext.getDependencyMet() +"))");
        }
        if (apnContext.isReady()) {
            cleanup = true;
            if (enabled && met) {
                DctConstants.State state = apnContext.getState();
                switch(state) {
                    case CONNECTING:
                    case SCANNING:
                    case CONNECTED:
                    case DISCONNECTING:
                        // We're "READY" and active so just return
                        if (DBG) log("applyNewState: 'ready' so return");
                        return;
                    case IDLE:
                        // fall through: this is unexpected but if it happens cleanup and try setup
                    case FAILED:
                    case RETRYING: {
                        // We're "READY" but not active so disconnect (cleanup = true) and
                        // connect (trySetup = true) to be sure we retry the connection.
                        trySetup = true;
                        apnContext.setReason(Phone.REASON_DATA_ENABLED);
                        break;
                    }
                }
            } else if (met) {
                apnContext.setReason(Phone.REASON_DATA_DISABLED);
                // If ConnectivityService has disabled this network, stop trying to bring
                // it up, but do not tear it down - ConnectivityService will do that
                // directly by talking with the DataConnection.
                //
                // This doesn't apply to DUN, however.  Those connections have special
                // requirements from carriers and we need stop using them when the dun
                // request goes away.  This applies to both CDMA and GSM because they both
                // can declare the DUN APN sharable by default traffic, thus still satisfying
                // those requests and not torn down organically.
                if ((apnContext.getApnType() == PhoneConstants.APN_TYPE_DUN && teardownForDun()) ||
                        (apnContext.getApnType() == PhoneConstants.APN_TYPE_MMS)) {
                    cleanup = true;
                } else {
                    cleanup = false;
                }
            } else {
                apnContext.setReason(Phone.REASON_DATA_DEPENDENCY_UNMET);
            }
        } else {
            if (enabled && met) {
                if (apnContext.isEnabled()) {
                    apnContext.setReason(Phone.REASON_DATA_DEPENDENCY_MET);
                } else {
                    apnContext.setReason(Phone.REASON_DATA_ENABLED);
                }
                if (apnContext.getState() == DctConstants.State.FAILED) {
                    apnContext.setState(DctConstants.State.IDLE);
                }
                trySetup = true;
            }
        }
        apnContext.setEnabled(enabled);
        apnContext.setDependencyMet(met);
        if (cleanup) cleanUpConnection(true, apnContext);
        if (trySetup) trySetupData(apnContext);

#endif
}

ECode DcTracker::CheckForCompatibleConnectedApnContext(
    /* [in] */ IApnContext* apnContext,
    /* [out] */ IDcAsyncChannel** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String apnType = apnContext.getApnType();
        ApnSetting dunSetting = null;
        if (PhoneConstants.APN_TYPE_DUN.equals(apnType)) {
            dunSetting = fetchDunApn();
        }
        if (DBG) {
            log("checkForCompatibleConnectedApnContext: apnContext=" + apnContext );
        }
        DcAsyncChannel potentialDcac = null;
        ApnContext potentialApnCtx = null;
        for (ApnContext curApnCtx : mApnContexts.values()) {
            DcAsyncChannel curDcac = curApnCtx.getDcAc();
            log("curDcac: " + curDcac);
            if (curDcac != null) {
                ApnSetting apnSetting = curApnCtx.getApnSetting();
                log("apnSetting: " + apnSetting);
                if (dunSetting != null) {
                    if (dunSetting.equals(apnSetting)) {
                        switch (curApnCtx.getState()) {
                            case CONNECTED:
                                if (DBG) {
                                    log("checkForCompatibleConnectedApnContext:"
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
                                if (potentialDcac == null) {
                                    potentialDcac = curDcac;
                                    potentialApnCtx = curApnCtx;
                                }
                            default:
                                // Not connected, potential unchanged
                                break;
                        }
                    }
                } else if (apnSetting != null && apnSetting.canHandleType(apnType)) {
                    switch (curApnCtx.getState()) {
                        case CONNECTED:
                            if (DBG) {
                                log("checkForCompatibleConnectedApnContext:"
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
                            if (potentialDcac == null) {
                                potentialDcac = curDcac;
                                potentialApnCtx = curApnCtx;
                            }
                        default:
                            // Not connected, potential unchanged
                            break;
                    }
                }
            } else {
                if (VDBG) {
                    log("checkForCompatibleConnectedApnContext: not conn curApnCtx=" + curApnCtx);
                }
            }
        }
        if (potentialDcac != null) {
            if (DBG) {
                log("checkForCompatibleConnectedApnContext: found potential conn=" + potentialDcac
                        + " curApnCtx=" + potentialApnCtx);
            }
            return potentialDcac;
        }
        if (DBG) log("checkForCompatibleConnectedApnContext: NO conn apnContext=" + apnContext);
        return null;

#endif
}

ECode DcTracker::OnEnableApn(
    /* [in] */ Int32 apnId,
    /* [in] */ Int32 enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ApnContext apnContext = mApnContexts.get(apnIdToType(apnId));
        if (apnContext == null) {
            loge("onEnableApn(" + apnId + ", " + enabled + "): NO ApnContext");
            return;
        }
        // TODO change our retry manager to use the appropriate numbers for the new APN
        if (DBG) log("onEnableApn: apnContext=" + apnContext + " call applyNewState");
        applyNewState(apnContext, enabled == DctConstants.ENABLED, apnContext.getDependencyMet());

#endif
}

ECode DcTracker::OnTrySetupData(
    /* [in] */ const String& reason,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("onTrySetupData: reason=" + reason);
        setupDataOnConnectableApns(reason);
        return true;

#endif
}

ECode DcTracker::OnTrySetupData(
    /* [in] */ IApnContext* apnContext,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("onTrySetupData: apnContext=" + apnContext);
        return trySetupData(apnContext);

#endif
}

ECode DcTracker::OnRoamingOff()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("onRoamingOff");
        if (mUserDataEnabled == false) return;
        if (getDataOnRoamingEnabled() == false) {
            notifyOffApnsOfAvailability(Phone.REASON_ROAMING_OFF);
            setupDataOnConnectableApns(Phone.REASON_ROAMING_OFF);
        } else {
            notifyDataConnection(Phone.REASON_ROAMING_OFF);
        }

#endif
}

ECode DcTracker::OnRoamingOn()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mUserDataEnabled == false) return;
        if (getDataOnRoamingEnabled()) {
            if (DBG) log("onRoamingOn: setup data on roaming");
            setupDataOnConnectableApns(Phone.REASON_ROAMING_ON);
            notifyDataConnection(Phone.REASON_ROAMING_ON);
        } else {
            if (DBG) log("onRoamingOn: Tear down data connection on roaming.");
            cleanUpAllConnections(true, Phone.REASON_ROAMING_ON);
            notifyOffApnsOfAvailability(Phone.REASON_ROAMING_ON);
        }

#endif
}

ECode DcTracker::OnRadioAvailable()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("onRadioAvailable");
        if (mPhone.getSimulatedRadioControl() != null) {
            // Assume data is connected on the simulator
            // FIXME  this can be improved
            // setState(DctConstants.State.CONNECTED);
            notifyDataConnection(null);
            log("onRadioAvailable: We're on the simulator; assuming data is connected");
        }
        IccRecords r = mIccRecords.get();
        if (r != null && r.getRecordsLoaded()) {
            notifyOffApnsOfAvailability(null);
        }
        if (getOverallState() != DctConstants.State.IDLE) {
            cleanUpConnection(true, null);
        }

#endif
}

ECode DcTracker::OnRadioOffOrNotAvailable()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Make sure our reconnect delay starts at the initial value
        // next time the radio comes on
        mReregisterOnReconnectFailure = false;
        if (mPhone.getSimulatedRadioControl() != null) {
            // Assume data is connected on the simulator
            // FIXME  this can be improved
            log("We're on the simulator; assuming radio off is meaningless");
        } else {
            if (DBG) log("onRadioOffOrNotAvailable: is off and clean up all connections");
            cleanUpAllConnections(false, Phone.REASON_RADIO_TURNED_OFF);
        }
        notifyOffApnsOfAvailability(null);

#endif
}

ECode DcTracker::CompleteConnection(
    /* [in] */ IApnContext* apnContext)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        boolean isProvApn = apnContext.isProvisioningApn();
        if (DBG) log("completeConnection: successful, notify the world apnContext=" + apnContext);
        if (mIsProvisioning && !TextUtils.isEmpty(mProvisioningUrl)) {
            if (DBG) {
                log("completeConnection: MOBILE_PROVISIONING_ACTION url="
                        + mProvisioningUrl);
            }
            Intent newIntent = Intent.makeMainSelectorActivity(Intent.ACTION_MAIN,
                    Intent.CATEGORY_APP_BROWSER);
            newIntent.setData(Uri.parse(mProvisioningUrl));
            newIntent.setFlags(Intent.FLAG_ACTIVITY_BROUGHT_TO_FRONT |
                    Intent.FLAG_ACTIVITY_NEW_TASK);
            try {
                mPhone.getContext().startActivity(newIntent);
            } catch (ActivityNotFoundException e) {
                loge("completeConnection: startActivityAsUser failed" + e);
            }
        }
        mIsProvisioning = false;
        mProvisioningUrl = null;
        if (mProvisioningSpinner != null) {
            sendMessage(obtainMessage(DctConstants.CMD_CLEAR_PROVISIONING_SPINNER,
                    mProvisioningSpinner));
        }
        mPhone.notifyDataConnection(apnContext.getReason(), apnContext.getApnType());
        startNetStatPoll();
        startDataStallAlarm(DATA_STALL_NOT_SUSPECTED);

#endif
}

ECode DcTracker::OnDataSetupComplete(
    /* [in] */ IAsyncResult* ar)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        DcFailCause cause = DcFailCause.UNKNOWN;
        boolean handleError = false;
        ApnContext apnContext = null;
        if(ar.userObj instanceof ApnContext){
            apnContext = (ApnContext)ar.userObj;
        } else {
            throw new RuntimeException("onDataSetupComplete: No apnContext");
        }
        if (ar.exception == null) {
            DcAsyncChannel dcac = apnContext.getDcAc();
            if (RADIO_TESTS) {
                // Note: To change radio.test.onDSC.null.dcac from command line you need to
                // adb root and adb remount and from the command line you can only change the
                // value to 1 once. To change it a second time you can reboot or execute
                // adb shell stop and then adb shell start. The command line to set the value is:
                // adb shell sqlite3 /data/data/com.android.providers.settings/databases/settings.db "insert into system (name,value) values ('radio.test.onDSC.null.dcac', '1');"
                ContentResolver cr = mPhone.getContext().getContentResolver();
                String radioTestProperty = "radio.test.onDSC.null.dcac";
                if (Settings.System.getInt(cr, radioTestProperty, 0) == 1) {
                    log("onDataSetupComplete: " + radioTestProperty +
                            " is true, set dcac to null and reset property to false");
                    dcac = null;
                    Settings.System.putInt(cr, radioTestProperty, 0);
                    log("onDataSetupComplete: " + radioTestProperty + "=" +
                            Settings.System.getInt(mPhone.getContext().getContentResolver(),
                                    radioTestProperty, -1));
                }
            }
            if (dcac == null) {
                log("onDataSetupComplete: no connection to DC, handle as error");
                cause = DcFailCause.CONNECTION_TO_DATACONNECTIONAC_BROKEN;
                handleError = true;
            } else {
                ApnSetting apn = apnContext.getApnSetting();
                if (DBG) {
                    log("onDataSetupComplete: success apn=" + (apn == null ? "unknown" : apn.apn));
                }
                if (apn != null && apn.proxy != null && apn.proxy.length() != 0) {
                    try {
                        String port = apn.port;
                        if (TextUtils.isEmpty(port)) port = "8080";
                        ProxyInfo proxy = new ProxyInfo(apn.proxy,
                                Integer.parseInt(port), null);
                        dcac.setLinkPropertiesHttpProxySync(proxy);
                    } catch (NumberFormatException e) {
                        loge("onDataSetupComplete: NumberFormatException making ProxyProperties (" +
                                apn.port + "): " + e);
                    }
                }
                // everything is setup
                if(TextUtils.equals(apnContext.getApnType(),PhoneConstants.APN_TYPE_DEFAULT)) {
                    SystemProperties.set(PUPPET_MASTER_RADIO_STRESS_TEST, "true");
                    if (mCanSetPreferApn && mPreferredApn == null) {
                        if (DBG) log("onDataSetupComplete: PREFERED APN is null");
                        mPreferredApn = apn;
                        if (mPreferredApn != null) {
                            setPreferredApn(mPreferredApn.id);
                        }
                    }
                } else {
                    SystemProperties.set(PUPPET_MASTER_RADIO_STRESS_TEST, "false");
                }
                // A connection is setup
                apnContext.setState(DctConstants.State.CONNECTED);
                boolean isProvApn = apnContext.isProvisioningApn();
                final ConnectivityManager cm = ConnectivityManager.from(mPhone.getContext());
                if (mProvisionBroadcastReceiver != null) {
                    mPhone.getContext().unregisterReceiver(mProvisionBroadcastReceiver);
                    mProvisionBroadcastReceiver = null;
                }
                if ((!isProvApn) || mIsProvisioning) {
                    // Hide any provisioning notification.
                    cm.setProvisioningNotificationVisible(false, ConnectivityManager.TYPE_MOBILE,
                            mProvisionActionName);
                    // Complete the connection normally notifying the world we're connected.
                    // We do this if this isn't a special provisioning apn or if we've been
                    // told its time to provision.
                    completeConnection(apnContext);
                } else {
                    // This is a provisioning APN that we're reporting as connected. Later
                    // when the user desires to upgrade this to a "default" connection,
                    // mIsProvisioning == true, we'll go through the code path above.
                    // mIsProvisioning becomes true when CMD_ENABLE_MOBILE_PROVISIONING
                    // is sent to the DCT.
                    if (DBG) {
                        log("onDataSetupComplete: successful, BUT send connected to prov apn as"
                                + " mIsProvisioning:" + mIsProvisioning + " == false"
                                + " && (isProvisioningApn:" + isProvApn + " == true");
                    }
                    // While radio is up, grab provisioning URL.  The URL contains ICCID which
                    // disappears when radio is off.
                    mProvisionBroadcastReceiver = new ProvisionNotificationBroadcastReceiver(
                            cm.getMobileProvisioningUrl(),
                            TelephonyManager.getDefault().getNetworkOperatorName());
                    mPhone.getContext().registerReceiver(mProvisionBroadcastReceiver,
                            new IntentFilter(mProvisionActionName));
                    // Put up user notification that sign-in is required.
                    cm.setProvisioningNotificationVisible(true, ConnectivityManager.TYPE_MOBILE,
                            mProvisionActionName);
                    // Turn off radio to save battery and avoid wasting carrier resources.
                    // The network isn't usable and network validation will just fail anyhow.
                    setRadio(false);
                    Intent intent = new Intent(
                            TelephonyIntents.ACTION_DATA_CONNECTION_CONNECTED_TO_PROVISIONING_APN);
                    intent.putExtra(PhoneConstants.DATA_APN_KEY, apnContext.getApnSetting().apn);
                    intent.putExtra(PhoneConstants.DATA_APN_TYPE_KEY, apnContext.getApnType());
                    String apnType = apnContext.getApnType();
                    LinkProperties linkProperties = getLinkProperties(apnType);
                    if (linkProperties != null) {
                        intent.putExtra(PhoneConstants.DATA_LINK_PROPERTIES_KEY, linkProperties);
                        String iface = linkProperties.getInterfaceName();
                        if (iface != null) {
                            intent.putExtra(PhoneConstants.DATA_IFACE_NAME_KEY, iface);
                        }
                    }
                    NetworkCapabilities networkCapabilities = getNetworkCapabilities(apnType);
                    if (networkCapabilities != null) {
                        intent.putExtra(PhoneConstants.DATA_NETWORK_CAPABILITIES_KEY,
                                networkCapabilities);
                    }
                    mPhone.getContext().sendBroadcastAsUser(intent, UserHandle.ALL);
                }
                if (DBG) {
                    log("onDataSetupComplete: SETUP complete type=" + apnContext.getApnType()
                        + ", reason:" + apnContext.getReason());
                }
            }
        } else {
            cause = (DcFailCause) (ar.result);
            if (DBG) {
                ApnSetting apn = apnContext.getApnSetting();
                log(String.format("onDataSetupComplete: error apn=%s cause=%s",
                        (apn == null ? "unknown" : apn.apn), cause));
            }
            if (cause == null) {
                cause = DcFailCause.UNKNOWN;
            }
            if (cause.isEventLoggable()) {
                // Log this failure to the Event Logs.
                int cid = getCellLocationId();
                EventLog.writeEvent(EventLogTags.PDP_SETUP_FAIL,
                        cause.ordinal(), cid, TelephonyManager.getDefault().getNetworkType());
            }
            ApnSetting apn = apnContext.getApnSetting();
            mPhone.notifyPreciseDataConnectionFailed(apnContext.getReason(),
                    apnContext.getApnType(), apn != null ? apn.apn : "unknown", cause.toString());
            // Count permanent failures and remove the APN we just tried
            if (isPermanentFail(cause)) apnContext.decWaitingApnsPermFailCount();
            apnContext.removeWaitingApn(apnContext.getApnSetting());
            if (DBG) {
                if (apnContext.getWaitingApns() == null){
                    log(String.format("onDataSetupComplete: WaitingApns.size = null" +
                            " WaitingApnsPermFailureCountDown=%d",
                            apnContext.getWaitingApnsPermFailCount()));
                }
                else {
                    log(String.format("onDataSetupComplete: WaitingApns.size=%d" +
                            " WaitingApnsPermFailureCountDown=%d",
                            apnContext.getWaitingApns().size(),
                            apnContext.getWaitingApnsPermFailCount()));
                }
            }
            handleError = true;
        }
        if (handleError) {
            onDataSetupCompleteError(ar);
        }
        /* If flag is set to false after SETUP_DATA_CALL is invoked, we need
         * to clean data connections.
         */
        if (!mInternalDataEnabled) {
            cleanUpAllConnections(null);
        }

#endif
}

ECode DcTracker::GetApnDelay(
    /* [in] */ const String& reason,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mFailFast || Phone.REASON_NW_TYPE_CHANGED.equals(reason) ||
                Phone.REASON_APN_CHANGED.equals(reason)) {
            return SystemProperties.getInt("persist.radio.apn_ff_delay",
                    APN_FAIL_FAST_DELAY_DEFAULT_MILLIS);
        } else {
            return SystemProperties.getInt("persist.radio.apn_delay", APN_DELAY_DEFAULT_MILLIS);
        }

#endif
}

ECode DcTracker::OnDataSetupCompleteError(
    /* [in] */ IAsyncResult* ar)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String reason = "";
        ApnContext apnContext = null;
        if(ar.userObj instanceof ApnContext){
            apnContext = (ApnContext)ar.userObj;
        } else {
            throw new RuntimeException("onDataSetupCompleteError: No apnContext");
        }
        // See if there are more APN's to try
        if (apnContext.getWaitingApns() == null || apnContext.getWaitingApns().isEmpty()) {
            apnContext.setState(DctConstants.State.FAILED);
            mPhone.notifyDataConnection(Phone.REASON_APN_FAILED, apnContext.getApnType());
            apnContext.setDataConnectionAc(null);
            if (apnContext.getWaitingApnsPermFailCount() == 0) {
                if (DBG) {
                    log("onDataSetupCompleteError: All APN's had permanent failures, stop retrying");
                }
            } else {
                int delay = getApnDelay(Phone.REASON_APN_FAILED);
                if (DBG) {
                    log("onDataSetupCompleteError: Not all APN's had permanent failures delay="
                            + delay);
                }
                startAlarmForRestartTrySetup(delay, apnContext);
            }
        } else {
            if (DBG) log("onDataSetupCompleteError: Try next APN");
            apnContext.setState(DctConstants.State.SCANNING);
            // Wait a bit before trying the next APN, so that
            // we're not tying up the RIL command channel
            startAlarmForReconnect(getApnDelay(Phone.REASON_APN_FAILED), apnContext);
        }

#endif
}

ECode DcTracker::OnDisconnectDone(
    /* [in] */ Int32 connId,
    /* [in] */ IAsyncResult* ar)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ApnContext apnContext = null;
        if (ar.userObj instanceof ApnContext) {
            apnContext = (ApnContext) ar.userObj;
        } else {
            loge("onDisconnectDone: Invalid ar in onDisconnectDone, ignore");
            return;
        }
        if(DBG) log("onDisconnectDone: EVENT_DISCONNECT_DONE apnContext=" + apnContext);
        apnContext.setState(DctConstants.State.IDLE);
        mPhone.notifyDataConnection(apnContext.getReason(), apnContext.getApnType());
        // if all data connection are gone, check whether Airplane mode request was
        // pending.
        if (isDisconnected()) {
            if (mPhone.getServiceStateTracker().processPendingRadioPowerOffAfterDataOff()) {
                if(DBG) log("onDisconnectDone: radio will be turned off, no retries");
                // Radio will be turned off. No need to retry data setup
                apnContext.setApnSetting(null);
                apnContext.setDataConnectionAc(null);
                // Need to notify disconnect as well, in the case of switching Airplane mode.
                // Otherwise, it would cause 30s delayed to turn on Airplane mode.
                if (mDisconnectPendingCount > 0)
                    mDisconnectPendingCount--;
                if (mDisconnectPendingCount == 0) {
                    notifyDataDisconnectComplete();
                    notifyAllDataDisconnected();
                }
                return;
            }
        }
        // If APN is still enabled, try to bring it back up automatically
        if (mAttached.get() && apnContext.isReady() && retryAfterDisconnected(apnContext)) {
            SystemProperties.set(PUPPET_MASTER_RADIO_STRESS_TEST, "false");
            // Wait a bit before trying the next APN, so that
            // we're not tying up the RIL command channel.
            // This also helps in any external dependency to turn off the context.
            if(DBG) log("onDisconnectDone: attached, ready and retry after disconnect");
            startAlarmForReconnect(getApnDelay(apnContext.getReason()), apnContext);
        } else {
            boolean restartRadioAfterProvisioning = mPhone.getContext().getResources().getBoolean(
                    com.android.internal.R.bool.config_restartRadioAfterProvisioning);
            if (apnContext.isProvisioningApn() && restartRadioAfterProvisioning) {
                log("onDisconnectDone: restartRadio after provisioning");
                restartRadio();
            }
            apnContext.setApnSetting(null);
            apnContext.setDataConnectionAc(null);
            if (isOnlySingleDcAllowed(mPhone.getServiceState().getRilDataRadioTechnology())) {
                if(DBG) log("onDisconnectDone: isOnlySigneDcAllowed true so setup single apn");
                setupDataOnConnectableApns(Phone.REASON_SINGLE_PDN_ARBITRATION);
            } else {
                if(DBG) log("onDisconnectDone: not retrying");
            }
        }
        if (mDisconnectPendingCount > 0)
            mDisconnectPendingCount--;
        if (mDisconnectPendingCount == 0) {
            notifyDataDisconnectComplete();
            notifyAllDataDisconnected();
        }

#endif
}

ECode DcTracker::OnDisconnectDcRetrying(
    /* [in] */ Int32 connId,
    /* [in] */ IAsyncResult* ar)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // We could just do this in DC!!!
        ApnContext apnContext = null;
        if (ar.userObj instanceof ApnContext) {
            apnContext = (ApnContext) ar.userObj;
        } else {
            loge("onDisconnectDcRetrying: Invalid ar in onDisconnectDone, ignore");
            return;
        }
        apnContext.setState(DctConstants.State.RETRYING);
        if(DBG) log("onDisconnectDcRetrying: apnContext=" + apnContext);
        mPhone.notifyDataConnection(apnContext.getReason(), apnContext.getApnType());

#endif
}

ECode DcTracker::OnVoiceCallStarted()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("onVoiceCallStarted");
        mInVoiceCall = true;
        if (isConnected() && ! mPhone.getServiceStateTracker().isConcurrentVoiceAndDataAllowed()) {
            if (DBG) log("onVoiceCallStarted stop polling");
            stopNetStatPoll();
            stopDataStallAlarm();
            notifyDataConnection(Phone.REASON_VOICE_CALL_STARTED);
        }

#endif
}

ECode DcTracker::OnVoiceCallEnded()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("onVoiceCallEnded");
        mInVoiceCall = false;
        if (isConnected()) {
            if (!mPhone.getServiceStateTracker().isConcurrentVoiceAndDataAllowed()) {
                startNetStatPoll();
                startDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
                notifyDataConnection(Phone.REASON_VOICE_CALL_ENDED);
            } else {
                // clean slate after call end.
                resetPollStats();
            }
        }
        // reset reconnect timer
        setupDataOnConnectableApns(Phone.REASON_VOICE_CALL_ENDED);

#endif
}

ECode DcTracker::OnCleanUpConnection(
    /* [in] */ Boolean tearDown,
    /* [in] */ Int32 apnId,
    /* [in] */ const String& reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("onCleanUpConnection");
        ApnContext apnContext = mApnContexts.get(apnIdToType(apnId));
        if (apnContext != null) {
            apnContext.setReason(reason);
            cleanUpConnection(tearDown, apnContext);
        }

#endif
}

ECode DcTracker::IsConnected(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        for (ApnContext apnContext : mApnContexts.values()) {
            if (apnContext.getState() == DctConstants.State.CONNECTED) {
                // At least one context is connected, return true
                return true;
            }
        }
        // There are not any contexts connected, return false
        return false;

#endif
}

ECode DcTracker::IsDisconnected(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        for (ApnContext apnContext : mApnContexts.values()) {
            if (!apnContext.isDisconnected()) {
                // At least one context was not disconnected return false
                return false;
            }
        }
        // All contexts were disconnected so return true
        return true;

#endif
}

ECode DcTracker::NotifyDataConnection(
    /* [in] */ const String& reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("notifyDataConnection: reason=" + reason);
        for (ApnContext apnContext : mApnContexts.values()) {
            if ((mAttached.get() || !mOosIsDisconnect) && apnContext.isReady()) {
                if (DBG) log("notifyDataConnection: type:" + apnContext.getApnType());
                mPhone.notifyDataConnection(reason != null ? reason : apnContext.getReason(),
                        apnContext.getApnType());
            }
        }
        notifyOffApnsOfAvailability(reason);

#endif
}

ECode DcTracker::IsNvSubscription(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int radioTech = mPhone.getServiceState().getRilVoiceRadioTechnology();
        if (mCdmaSsm == null) {
            return false;
        }
        if (UiccController.getFamilyFromRadioTechnology(radioTech) == UiccController.APP_FAM_3GPP2
                && mCdmaSsm.getCdmaSubscriptionSource() ==
                        CdmaSubscriptionSourceManager.SUBSCRIPTION_FROM_NV) {
            return true;
        }
        return false;

#endif
}

ECode DcTracker::GetOperatorNumeric(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String result;
        if (isNvSubscription()) {
            result = SystemProperties.get(CDMAPhone.PROPERTY_CDMA_HOME_OPERATOR_NUMERIC);
            log("getOperatorNumberic - returning from NV: " + result);
        } else {
            IccRecords r = mIccRecords.get();
            result = (r != null) ? r.getOperatorNumeric() : "";
            log("getOperatorNumberic - returning from card: " + result);
        }
        if (result == null) result = "";
        return result;

#endif
}

ECode DcTracker::CreateAllApnList()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mAllApnSettings.clear();
        String _operator = getOperatorNumeric();
        int radioTech = mPhone.getServiceState().getRilDataRadioTechnology();
        if (mOmhApt != null && (ServiceState.isCdma(radioTech) &&
                ServiceState.RIL_RADIO_TECHNOLOGY_EHRPD != radioTech)) {
            ArrayList<ApnSetting> mOmhApnsList = new ArrayList<ApnSetting>();
            mOmhApnsList = mOmhApt.getOmhApnProfilesList();
            if (!mOmhApnsList.isEmpty()) {
                if (DBG) log("createAllApnList: Copy Omh profiles");
                mAllApnSettings.addAll(mOmhApnsList);
            }
        }
        if (mAllApnSettings.isEmpty()) {
            if (_operator != null && !_operator.isEmpty()) {
                String selection = "numeric = '" + _operator + "'";
                // query only enabled apn.
                // carrier_enabled : 1 means enabled apn, 0 disabled apn.
                selection += " and carrier_enabled = 1";
                if (DBG) log("createAllApnList: selection=" + selection);
                Cursor cursor = mPhone.getContext().getContentResolver().query(
                        Telephony.Carriers.CONTENT_URI, null, selection, null, null);
                if (cursor != null) {
                    if (cursor.getCount() > 0) {
                        mAllApnSettings = createApnList(cursor);
                    }
                    cursor.close();
                }
            }
        }
        dedupeApnSettings();
        if (mAllApnSettings.isEmpty() && isDummyProfileNeeded()) {
            addDummyApnSettings(_operator);
        }
        addEmergencyApnSetting();
        if (mAllApnSettings.isEmpty()) {
            if (DBG) log("createAllApnList: No APN found for carrier: " + _operator);
            mPreferredApn = null;
            // TODO: What is the right behavior?
            //notifyNoData(DataConnection.FailCause.MISSING_UNKNOWN_APN);
        } else {
            mPreferredApn = getPreferredApn();
            if (mPreferredApn != null && !mPreferredApn.numeric.equals(_operator)) {
                mPreferredApn = null;
                setPreferredApn(-1);
            }
            if (DBG) log("createAllApnList: mPreferredApn=" + mPreferredApn);
        }
        if (DBG) log("createAllApnList: X mAllApnSettings=" + mAllApnSettings);
        setDataProfilesAsNeeded();

#endif
}

ECode DcTracker::DedupeApnSettings()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ArrayList<ApnSetting> resultApns = new ArrayList<ApnSetting>();
        // coalesce APNs if they are similar enough to prevent
        // us from bringing up two data calls with the same interface
        int i = 0;
        while (i < mAllApnSettings.size() - 1) {
            ApnSetting first = mAllApnSettings.get(i);
            ApnSetting second = null;
            int j = i + 1;
            while (j < mAllApnSettings.size()) {
                second = mAllApnSettings.get(j);
                if (apnsSimilar(first, second)) {
                    ApnSetting newApn = mergeApns(first, second);
                    mAllApnSettings.set(i, newApn);
                    first = newApn;
                    mAllApnSettings.remove(j);
                } else {
                    j++;
                }
            }
            i++;
        }

#endif
}

ECode DcTracker::ApnTypeSameAny(
    /* [in] */ IApnSetting* first,
    /* [in] */ IApnSetting* second,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if(VDBG) {
            StringBuilder apnType1 = new StringBuilder(first.apn + ": ");
            for(int index1 = 0; index1 < first.types.length; index1++) {
                apnType1.append(first.types[index1]);
                apnType1.append(",");
            }
            StringBuilder apnType2 = new StringBuilder(second.apn + ": ");
            for(int index1 = 0; index1 < second.types.length; index1++) {
                apnType2.append(second.types[index1]);
                apnType2.append(",");
            }
            log("APN1: is " + apnType1);
            log("APN2: is " + apnType2);
        }
        for(int index1 = 0; index1 < first.types.length; index1++) {
            for(int index2 = 0; index2 < second.types.length; index2++) {
                if(first.types[index1].equals(PhoneConstants.APN_TYPE_ALL) ||
                        second.types[index2].equals(PhoneConstants.APN_TYPE_ALL) ||
                        first.types[index1].equals(second.types[index2])) {
                    if(VDBG)log("apnTypeSameAny: return true");
                    return true;
                }
            }
        }
        if(VDBG)log("apnTypeSameAny: return false");
        return false;

#endif
}

ECode DcTracker::ApnsSimilar(
    /* [in] */ IApnSetting* first,
    /* [in] */ IApnSetting* second,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (first.canHandleType(PhoneConstants.APN_TYPE_DUN) == false &&
                second.canHandleType(PhoneConstants.APN_TYPE_DUN) == false &&
                Objects.equals(first.apn, second.apn) &&
                !apnTypeSameAny(first, second) &&
                xorEquals(first.proxy, second.proxy) &&
                xorEquals(first.port, second.port) &&
                first.carrierEnabled == second.carrierEnabled &&
                first.bearer == second.bearer &&
                first.profileId == second.profileId &&
                Objects.equals(first.mvnoType, second.mvnoType) &&
                Objects.equals(first.mvnoMatchData, second.mvnoMatchData) &&
                xorEquals(first.mmsc, second.mmsc) &&
                xorEquals(first.mmsProxy, second.mmsProxy) &&
                xorEquals(first.mmsPort, second.mmsPort));

#endif
}

ECode DcTracker::XorEquals(
    /* [in] */ const String& first,
    /* [in] */ const String& second,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (Objects.equals(first, second) ||
                TextUtils.isEmpty(first) ||
                TextUtils.isEmpty(second));

#endif
}

ECode DcTracker::MergeApns(
    /* [in] */ IApnSetting* dest,
    /* [in] */ IApnSetting* src,
    /* [out] */ IApnSetting** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ArrayList<String> resultTypes = new ArrayList<String>();
        resultTypes.addAll(Arrays.asList(dest.types));
        for (String srcType : src.types) {
            if (resultTypes.contains(srcType) == false) resultTypes.add(srcType);
        }
        String mmsc = (TextUtils.isEmpty(dest.mmsc) ? src.mmsc : dest.mmsc);
        String mmsProxy = (TextUtils.isEmpty(dest.mmsProxy) ? src.mmsProxy : dest.mmsProxy);
        String mmsPort = (TextUtils.isEmpty(dest.mmsPort) ? src.mmsPort : dest.mmsPort);
        String proxy = (TextUtils.isEmpty(dest.proxy) ? src.proxy : dest.proxy);
        String port = (TextUtils.isEmpty(dest.port) ? src.port : dest.port);
        String protocol = src.protocol.equals("IPV4V6") ? src.protocol : dest.protocol;
        String roamingProtocol = src.roamingProtocol.equals("IPV4V6") ? src.roamingProtocol :
                dest.roamingProtocol;
        return new ApnSetting(dest.id, dest.numeric, dest.carrier, dest.apn,
                proxy, port, mmsc, mmsProxy, mmsPort, dest.user, dest.password,
                dest.authType, resultTypes.toArray(new String[0]), protocol,
                roamingProtocol, dest.carrierEnabled, dest.bearer, dest.profileId,
                (dest.modemCognitive || src.modemCognitive), dest.maxConns, dest.waitTime,
                dest.maxConnsTime, dest.mtu, dest.mvnoType, dest.mvnoMatchData);

#endif
}

ECode DcTracker::IsDummyProfileNeeded(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int radioTech = mPhone.getServiceState().getRilDataRadioTechnology();
        int radioTechFam = UiccController.getFamilyFromRadioTechnology(radioTech);
        IccRecords r = mIccRecords.get();
        if (DBG) log("isDummyProfileNeeded: radioTechFam = " + radioTechFam);
        // If uicc app family based on data rat is unknown,
        // check if records selected is RuimRecords.
        return (radioTechFam == UiccController.APP_FAM_3GPP2 ||
                ((radioTechFam == UiccController.APP_FAM_UNKNOWN) &&
                (r != null) && (r instanceof RuimRecords)));

#endif
}

ECode DcTracker::AddDummyApnSettings(
    /* [in] */ const String& _operator)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Create dummy data profiles.
        if (DBG) log("createAllApnList: Creating dummy apn for cdma operator:" + _operator);
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
        ApnSetting apn = new ApnSetting(DctConstants.APN_DEFAULT_ID, _operator, null, null,
                null, null, null, null, null, null, null,
                RILConstants.SETUP_DATA_AUTH_PAP_CHAP, defaultApnTypes,
                PROPERTY_CDMA_IPPROTOCOL, PROPERTY_CDMA_ROAMING_IPPROTOCOL, true, 0,
                0, false, 0, 0, 0, PhoneConstants.UNSET_MTU, "", "");
        mAllApnSettings.add(apn);
        apn = new ApnSetting(DctConstants.APN_DUN_ID, _operator, null, null,
                null, null, null, null, null, null, null,
                RILConstants.SETUP_DATA_AUTH_PAP_CHAP, dunApnTypes,
                PROPERTY_CDMA_IPPROTOCOL, PROPERTY_CDMA_ROAMING_IPPROTOCOL, true, 0,
                0, false, 0, 0, 0, PhoneConstants.UNSET_MTU, "", "");
        mAllApnSettings.add(apn);

#endif
}

ECode DcTracker::CreateDataConnection(
    /* [out] */ IDcAsyncChannel** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("createDataConnection E");
        int id = mUniqueIdGenerator.getAndIncrement();
        DataConnection conn = DataConnection.makeDataConnection(mPhone, id,
                                                this, mDcTesterFailBringUpAll, mDcc);
        mDataConnections.put(id, conn);
        DcAsyncChannel dcac = new DcAsyncChannel(conn, LOG__TAG);
        int status = dcac.fullyConnectSync(mPhone.getContext(), this, conn.getHandler());
        if (status == AsyncChannel.STATUS_SUCCESSFUL) {
            mDataConnectionAcHashMap.put(dcac.getDataConnectionIdSync(), dcac);
        } else {
            loge("createDataConnection: Could not connect to dcac=" + dcac + " status=" + status);
        }
        if (DBG) log("createDataConnection() X id=" + id + " dc=" + conn);
        return dcac;

#endif
}

ECode DcTracker::DestroyDataConnections()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if(mDataConnections != null) {
            if (DBG) log("destroyDataConnections: clear mDataConnectionList");
            mDataConnections.clear();
        } else {
            if (DBG) log("destroyDataConnections: mDataConnecitonList is empty, ignore");
        }

#endif
}

ECode DcTracker::BuildWaitingApns(
    /* [in] */ const String& requestedApnType,
    /* [in] */ Int32 radioTech,
    /* [out] */ IArrayList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("buildWaitingApns: E requestedApnType=" + requestedApnType);
        ArrayList<ApnSetting> apnList = new ArrayList<ApnSetting>();
        if (requestedApnType.equals(PhoneConstants.APN_TYPE_DUN)) {
            ApnSetting dun = fetchDunApn();
            if (dun != null) {
                apnList.add(dun);
                if (DBG) log("buildWaitingApns: X added APN_TYPE_DUN apnList=" + apnList);
                return apnList;
            }
        }
        String _operator = getOperatorNumeric();
        // This is a workaround for a bug (7305641) where we don't failover to other
        // suitable APNs if our preferred APN fails.  On prepaid ATT sims we need to
        // failover to a provisioning APN, but once we've used their default data
        // connection we are locked to it for life.  This change allows ATT devices
        // to say they don't want to use preferred at all.
        boolean usePreferred = true;
        try {
            usePreferred = ! mPhone.getContext().getResources().getBoolean(com.android.
                    internal.R.bool.config_dontPreferApn);
        } catch (Resources.NotFoundException e) {
            if (DBG) log("buildWaitingApns: usePreferred NotFoundException set to true");
            usePreferred = true;
        }
        if (usePreferred) {
            mPreferredApn = getPreferredApn();
        }
        if (DBG) {
            log("buildWaitingApns: usePreferred=" + usePreferred
                    + " canSetPreferApn=" + mCanSetPreferApn
                    + " mPreferredApn=" + mPreferredApn
                    + " operator=" + _operator + " radioTech=" + radioTech);
        }
        if (usePreferred && mCanSetPreferApn && mPreferredApn != null &&
                mPreferredApn.canHandleType(requestedApnType)) {
            if (DBG) {
                log("buildWaitingApns: Preferred APN:" + _operator + ":"
                        + mPreferredApn.numeric + ":" + mPreferredApn);
            }
            if (mPreferredApn.numeric != null && mPreferredApn.numeric.equals(_operator)) {
                if (mPreferredApn.bearer == 0 || mPreferredApn.bearer == radioTech) {
                    apnList.add(mPreferredApn);
                    if (DBG) log("buildWaitingApns: X added preferred apnList=" + apnList);
                    return apnList;
                } else {
                    if (DBG) log("buildWaitingApns: no preferred APN");
                    setPreferredApn(-1);
                    mPreferredApn = null;
                }
            } else {
                if (DBG) log("buildWaitingApns: no preferred APN");
                setPreferredApn(-1);
                mPreferredApn = null;
            }
        }
        if (mAllApnSettings != null && !mAllApnSettings.isEmpty()) {
            if (DBG) log("buildWaitingApns: mAllApnSettings=" + mAllApnSettings);
            for (ApnSetting apn : mAllApnSettings) {
                if (DBG) log("buildWaitingApns: apn=" + apn);
                if (apn.canHandleType(requestedApnType)) {
                    if (apn.bearer == 0 || apn.bearer == radioTech) {
                        if (DBG) log("buildWaitingApns: adding apn=" + apn.toString());
                        apnList.add(apn);
                    } else {
                        if (DBG) {
                            log("buildWaitingApns: bearer:" + apn.bearer + " != "
                                    + "radioTech:" + radioTech);
                        }
                    }
                } else {
                    if (DBG) {
                        log("buildWaitingApns: couldn't handle requesedApnType="
                                + requestedApnType);
                    }
                }
            }
        } else {
            loge("mAllApnSettings is null!");
        }
        if (DBG) log("buildWaitingApns: X apnList=" + apnList);
        return apnList;

#endif
}

ECode DcTracker::ApnListToString(
    /* [in] */ IArrayList* apns,
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (apns == null) {
            log("apnListToString: apns = null.");
            return "";
        }
        else {
            StringBuilder result = new StringBuilder();
            for (int i = 0, size = apns.size(); i < size; i++) {
                result.append('[')
                      .append(apns.get(i).toString())
                      .append(']');
            }
            return result.toString();
        }

#endif
}

ECode DcTracker::SetPreferredApn(
    /* [in] */ Int32 pos)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (!mCanSetPreferApn) {
            log("setPreferredApn: X !canSEtPreferApn");
            return;
        }
        log("setPreferredApn: delete");
        ContentResolver resolver = mPhone.getContext().getContentResolver();
        resolver.delete(getUri(PREFERAPN_NO_UPDATE_URI), null, null);
        if (pos >= 0) {
            log("setPreferredApn: insert");
            ContentValues values = new ContentValues();
            values.put(APN_ID, pos);
            resolver.insert(getUri(PREFERAPN_NO_UPDATE_URI), values);
        }

#endif
}

ECode DcTracker::GetPreferredApn(
    /* [out] */ IApnSetting** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mAllApnSettings == null || mAllApnSettings.isEmpty()) {
            log("getPreferredApn: mAllApnSettings is " + ((mAllApnSettings == null)?"null":"empty"));
            return null;
        }
        Cursor cursor = mPhone.getContext().getContentResolver().query(
                getUri(PREFERAPN_NO_UPDATE_URI), new String[] { "_id", "name", "apn" },
                null, null, Telephony.Carriers.DEFAULT_SORT_ORDER);
        if (cursor != null) {
            mCanSetPreferApn = true;
        } else {
            mCanSetPreferApn = false;
        }
        log("getPreferredApn: mRequestedApnType=" + mRequestedApnType + " cursor=" + cursor
                + " cursor.count=" + ((cursor != null) ? cursor.getCount() : 0));
        if (mCanSetPreferApn && cursor.getCount() > 0) {
            int pos;
            cursor.moveToFirst();
            pos = cursor.getInt(cursor.getColumnIndexOrThrow(Telephony.Carriers._ID));
            for(ApnSetting p : mAllApnSettings) {
                log("getPreferredApn: apnSetting=" + p);
                if (p.id == pos && p.canHandleType(mRequestedApnType)) {
                    log("getPreferredApn: X found apnSetting" + p);
                    cursor.close();
                    return p;
                }
            }
        }
        if (cursor != null) {
            cursor.close();
        }
        log("getPreferredApn: X not found");
        return null;

#endif
}

ECode DcTracker::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("handleMessage msg=" + msg);
        if (!mPhone.mIsTheCurrentActivePhone || mIsDisposed) {
            loge("handleMessage: Ignore GSM msgs since GSM phone is inactive");
            return;
        }
        switch (msg.what) {
            case DctConstants.EVENT_RECORDS_LOADED:
                onRecordsLoaded();
                break;
            case DctConstants.EVENT_DATA_CONNECTION_DETACHED:
                onDataConnectionDetached();
                break;
            case DctConstants.EVENT_DATA_CONNECTION_ATTACHED:
                onDataConnectionAttached();
                break;
            case DctConstants.EVENT_DO_RECOVERY:
                doRecovery();
                break;
            case DctConstants.EVENT_APN_CHANGED:
                onApnChanged();
                break;
            case DctConstants.EVENT_PS_RESTRICT_ENABLED:
                /**
                 * We don't need to explicitly to tear down the PDP context
                 * when PS restricted is enabled. The base band will deactive
                 * PDP context and notify us with PDP_CONTEXT_CHANGED.
                 * But we should stop the network polling and prevent reset PDP.
                 */
                if (DBG) log("EVENT_PS_RESTRICT_ENABLED " + mIsPsRestricted);
                stopNetStatPoll();
                stopDataStallAlarm();
                mIsPsRestricted = true;
                break;
            case DctConstants.EVENT_PS_RESTRICT_DISABLED:
                /**
                 * When PS restrict is removed, we need setup PDP connection if
                 * PDP connection is down.
                 */
                if (DBG) log("EVENT_PS_RESTRICT_DISABLED " + mIsPsRestricted);
                mIsPsRestricted  = false;
                if (isConnected()) {
                    startNetStatPoll();
                    startDataStallAlarm(DATA_STALL_NOT_SUSPECTED);
                } else {
                    // TODO: Should all PDN states be checked to fail?
                    if (mState == DctConstants.State.FAILED) {
                        cleanUpAllConnections(false, Phone.REASON_PS_RESTRICT_ENABLED);
                        mReregisterOnReconnectFailure = false;
                    }
                    ApnContext apnContext = mApnContexts.get(PhoneConstants.APN_TYPE_DEFAULT);
                    if (apnContext != null) {
                        apnContext.setReason(Phone.REASON_PS_RESTRICT_ENABLED);
                        trySetupData(apnContext);
                    } else {
                        loge("**** Default ApnContext not found ****");
                        if (Build.IS_DEBUGGABLE) {
                            throw new RuntimeException("Default ApnContext not found");
                        }
                    }
                }
                break;
            case DctConstants.EVENT_TRY_SETUP_DATA:
                if (msg.obj instanceof ApnContext) {
                    onTrySetupData((ApnContext)msg.obj);
                } else if (msg.obj instanceof String) {
                    onTrySetupData((String)msg.obj);
                } else {
                    loge("EVENT_TRY_SETUP request w/o apnContext or String");
                }
                break;
            case DctConstants.EVENT_CLEAN_UP_CONNECTION:
                boolean tearDown = (msg.arg1 == 0) ? false : true;
                if (DBG) log("EVENT_CLEAN_UP_CONNECTION tearDown=" + tearDown);
                if (msg.obj instanceof ApnContext) {
                    cleanUpConnection(tearDown, (ApnContext)msg.obj);
                } else {
                    loge("EVENT_CLEAN_UP_CONNECTION request w/o apn context, call super");
                    super.handleMessage(msg);
                }
                break;
            case DctConstants.EVENT_SET_INTERNAL_DATA_ENABLE:
                boolean enabled = (msg.arg1 == DctConstants.ENABLED) ? true : false;
                onSetInternalDataEnabled(enabled, (Message) msg.obj);
                break;
            case DctConstants.EVENT_CLEAN_UP_ALL_CONNECTIONS:
                Message mCause = obtainMessage(DctConstants.EVENT_CLEAN_UP_ALL_CONNECTIONS, null);
                if ((msg.obj != null) && (msg.obj instanceof String)) {
                    mCause.obj = msg.obj;
                }
                super.handleMessage(mCause);
                break;
            case DctConstants.EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED: // fall thru
            case DctConstants.EVENT_DATA_RAT_CHANGED:
                // When data rat changes we might need to load different
                // set of apns (example, LTE->1x)
                if (onUpdateIcc()) {
                    log("onUpdateIcc: tryRestartDataConnections " + Phone.REASON_NW_TYPE_CHANGED);
                    tryRestartDataConnections(true, Phone.REASON_NW_TYPE_CHANGED);
                } else if (isNvSubscription()){
                    // If cdma subscription source changed to NV or data rat changed to cdma
                    // (while subscription source was NV) - we need to trigger NV ready
                    onNvReady();
                }
                break;
            case DctConstants.CMD_CLEAR_PROVISIONING_SPINNER:
                // Check message sender intended to clear the current spinner.
                if (mProvisioningSpinner == msg.obj) {
                    mProvisioningSpinner.dismiss();
                    mProvisioningSpinner = null;
                }
                break;
            case DctConstants.EVENT_GET_WWAN_IWLAN_COEXISTENCE_DONE:
                onWwanIwlanCoexistenceDone((AsyncResult)msg.obj);
                break;
            case DctConstants.EVENT_MODEM_DATA_PROFILE_READY:
                onModemApnProfileReady();
                break;
            default:
                // handle the message in the super class DataConnectionTracker
                super.handleMessage(msg);
                break;
        }

#endif
}

ECode DcTracker::GetApnProfileID(
    /* [in] */ const String& apnType,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (TextUtils.equals(apnType, PhoneConstants.APN_TYPE_IMS)) {
            return RILConstants.DATA_PROFILE_IMS;
        } else if (TextUtils.equals(apnType, PhoneConstants.APN_TYPE_FOTA)) {
            return RILConstants.DATA_PROFILE_FOTA;
        } else if (TextUtils.equals(apnType, PhoneConstants.APN_TYPE_CBS)) {
            return RILConstants.DATA_PROFILE_CBS;
        } else if (TextUtils.equals(apnType, PhoneConstants.APN_TYPE_IA)) {
            return RILConstants.DATA_PROFILE_DEFAULT; // DEFAULT for now
        } else if (TextUtils.equals(apnType, PhoneConstants.APN_TYPE_DUN)) {
            return RILConstants.DATA_PROFILE_TETHERED;
        } else {
            return RILConstants.DATA_PROFILE_DEFAULT;
        }

#endif
}

ECode DcTracker::GetCellLocationId(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int cid = -1;
        CellLocation loc = mPhone.getCellLocation();
        if (loc != null) {
            if (loc instanceof GsmCellLocation) {
                cid = ((GsmCellLocation)loc).getCid();
            } else if (loc instanceof CdmaCellLocation) {
                cid = ((CdmaCellLocation)loc).getBaseStationId();
            }
        }
        return cid;

#endif
}

ECode DcTracker::GetUiccRecords(
    /* [in] */ Int32 appFamily,
    /* [out] */ IIccRecords** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mUiccController.getIccRecords(mPhone.getPhoneId(), appFamily);

#endif
}

ECode DcTracker::OnUpdateIcc(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        boolean result = false;
        if (mUiccController == null ) {
            loge("onUpdateIcc: mUiccController is null. Error!");
            return false;
        }
        int dataRat = mPhone.getServiceState().getRilDataRadioTechnology();
        int appFamily = UiccController.getFamilyFromRadioTechnology(dataRat);
        IccRecords newIccRecords = getUiccRecords(appFamily);
        log("onUpdateIcc: newIccRecords " + ((newIccRecords != null) ?
                newIccRecords.getClass().getName() : null));
        if (dataRat == ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN) {
            // Ignore this. This could be due to data not registered
            // We want to ignore RADIO_TECHNOLOGY_UNKNOWN so that we do not tear down data
            // call in case we are out of service.
            return false;
        }
        IccRecords r = mIccRecords.get();
        if (r != newIccRecords) {
            if (r != null) {
                log("Removing stale icc objects. " + ((r != null) ?
                        r.getClass().getName() : null));
                r.unregisterForRecordsLoaded(this);
                mIccRecords.set(null);
            }
            if (newIccRecords != null) {
                log("New records found " + ((newIccRecords != null) ?
                        newIccRecords.getClass().getName() : null));
                mIccRecords.set(newIccRecords);
                newIccRecords.registerForRecordsLoaded(
                        this, DctConstants.EVENT_RECORDS_LOADED, null);
            }
            // Records changed -> return true
            result = true;
        }
        return result;

#endif
}

ECode DcTracker::Update()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        log("update sub = " + mPhone.getSubId());
        log("update(): Active DDS, register for all events now!");
        registerForAllEvents();
        onUpdateIcc();
        mUserDataEnabled = Settings.Global.getInt(mPhone.getContext().getContentResolver(),
                Settings.Global.MOBILE_DATA + mPhone.getPhoneId(), 1) == 1;
        if (mPhone instanceof CDMALTEPhone) {
            ((CDMALTEPhone)mPhone).updateCurrentCarrierInProvider();
            supplyMessenger();
        } else if (mPhone instanceof GSMPhone) {
            ((GSMPhone)mPhone).updateCurrentCarrierInProvider();
            supplyMessenger();
        } else {
            log("Phone object is not MultiSim. This should not hit!!!!");
        }

#endif
}

ECode DcTracker::CleanUpAllConnections(
    /* [in] */ const String& cause)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        cleanUpAllConnections(cause, null);

#endif
}

ECode DcTracker::UpdateRecords()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        onUpdateIcc();

#endif
}

ECode DcTracker::CleanUpAllConnections(
    /* [in] */ const String& cause,
    /* [in] */ IMessage* disconnectAllCompleteMsg)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        log("cleanUpAllConnections");
        if (disconnectAllCompleteMsg != null) {
            mDisconnectAllCompleteMsgList.add(disconnectAllCompleteMsg);
        }
        Message msg = obtainMessage(DctConstants.EVENT_CLEAN_UP_ALL_CONNECTIONS);
        msg.obj = cause;
        sendMessage(msg);

#endif
}

ECode DcTracker::NotifyDataDisconnectComplete()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        log("notifyDataDisconnectComplete");
        for (Message m: mDisconnectAllCompleteMsgList) {
            m.sendToTarget();
        }
        mDisconnectAllCompleteMsgList.clear();

#endif
}

ECode DcTracker::NotifyAllDataDisconnected()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sEnableFailFastRefCounter = 0;
        mFailFast = false;
        mAllDataDisconnectedRegistrants.notifyRegistrants();

#endif
}

ECode DcTracker::RegisterForAllDataDisconnected(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mAllDataDisconnectedRegistrants.addUnique(h, what, obj);
        if (isDisconnected()) {
            log("notify All Data Disconnected");
            notifyAllDataDisconnected();
        }

#endif
}

ECode DcTracker::UnregisterForAllDataDisconnected(
    /* [in] */ IHandler* h)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mAllDataDisconnectedRegistrants.remove(h);

#endif
}

ECode DcTracker::OnSetInternalDataEnabled(
    /* [in] */ Boolean enable)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        onSetInternalDataEnabled(enable, null);

#endif
}

ECode DcTracker::OnSetInternalDataEnabled(
    /* [in] */ Boolean enabled,
    /* [in] */ IMessage* onCompleteMsg)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        boolean sendOnComplete = true;
        synchronized (mDataEnabledLock) {
            mInternalDataEnabled = enabled;
            if (enabled) {
                log("onSetInternalDataEnabled: changed to enabled, try to setup data call");
                onTrySetupData(Phone.REASON_DATA_ENABLED);
            } else {
                sendOnComplete = false;
                log("onSetInternalDataEnabled: changed to disabled, cleanUpAllConnections");
                cleanUpAllConnections(null, onCompleteMsg);
            }
        }
        if (sendOnComplete) {
            if (onCompleteMsg != null) {
                onCompleteMsg.sendToTarget();
            }
        }

#endif
}

ECode DcTracker::SetInternalDataEnabledFlag(
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG)
            log("setInternalDataEnabledFlag(" + enable + ")");
        if (mInternalDataEnabled != enable) {
            mInternalDataEnabled = enable;
        }
        return true;

#endif
}

ECode DcTracker::SetInternalDataEnabled(
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return setInternalDataEnabled(enable, null);

#endif
}

ECode DcTracker::SetInternalDataEnabled(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onCompleteMsg,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG)
            log("setInternalDataEnabled(" + enable + ")");
        Message msg = obtainMessage(DctConstants.EVENT_SET_INTERNAL_DATA_ENABLE, onCompleteMsg);
        msg.arg1 = (enable ? DctConstants.ENABLED : DctConstants.DISABLED);
        sendMessage(msg);
        return true;

#endif
}

ECode DcTracker::IsActiveDataSubscription(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // FIXME This should have code like
        // return (mPhone.getSubId() == SubscriptionManager.getDefaultDataSubId());
        return true;

#endif
}

ECode DcTracker::SetDataAllowed(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* response)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
         mIsCleanupRequired = !enable;
         mPhone.mCi.setDataAllowed(enable, response);
         mInternalDataEnabled = enable;

#endif
}

ECode DcTracker::Log(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.d(LOG__TAG, "[" + mPhone.getPhoneId() + "]" + s);

#endif
}

ECode DcTracker::Loge(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.e(LOG__TAG, "[" + mPhone.getPhoneId() + "]" + s);

#endif
}

ECode DcTracker::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        pw.println("DataConnectionTracker extends:");
        super.dump(fd, pw, args);
        pw.println(" mReregisterOnReconnectFailure=" + mReregisterOnReconnectFailure);
        pw.println(" canSetPreferApn=" + mCanSetPreferApn);
        pw.println(" mApnObserver=" + mApnObserver);
        pw.println(" getOverallState=" + getOverallState());
        pw.println(" mDataConnectionAsyncChannels=%s\n" + mDataConnectionAcHashMap);
        pw.println(" mAttached=" + mAttached.get());

#endif
}

ECode DcTracker::GetPcscfAddress(
    /* [in] */ const String& apnType,
    /* [out, callee] */ ArrayOf<String>** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        log("getPcscfAddress()");
        ApnContext apnContext = null;
        if(apnType == null){
            log("apnType is null, return null");
            return null;
        }
        if (TextUtils.equals(apnType, PhoneConstants.APN_TYPE_EMERGENCY)) {
            apnContext = mApnContexts.get(PhoneConstants.APN_TYPE_EMERGENCY);
        } else if (TextUtils.equals(apnType, PhoneConstants.APN_TYPE_IMS)) {
            apnContext = mApnContexts.get(PhoneConstants.APN_TYPE_IMS);
        } else {
            log("apnType is invalid, return null");
            return null;
        }
        if (apnContext == null) {
            log("apnContext is null, return null");
            return null;
        }
        DcAsyncChannel dcac = apnContext.getDcAc();
        String[] result = null;
        if (dcac != null) {
            result = dcac.getPcscfAddr();
            for (int i = 0; i < result.length; i++) {
                log("Pcscf[" + i + "]: " + result[i]);
            }
            return result;
        }
        return null;

#endif
}

ECode DcTracker::SetImsRegistrationState(
    /* [in] */ Boolean registered)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        log("setImsRegistrationState - mImsRegistrationState(before): "+ mImsRegistrationState
                + ", registered(current) : " + registered);
        if (mPhone == null) return;
        ServiceStateTracker sst = mPhone.getServiceStateTracker();
        if (sst == null) return;
        sst.setImsRegistrationState(registered);

#endif
}

ECode DcTracker::InitEmergencyApnSetting()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Operator Numeric is not available when sim records are not loaded.
        // Query Telephony.db with APN type as EPDN request does not
        // require APN name, plmn and all operators support same APN config.
        // DB will contain only one entry for Emergency APN
        String selection = "type=\"emergency\"";
        Cursor cursor = mPhone.getContext().getContentResolver().query(
                Telephony.Carriers.CONTENT_URI, null, selection, null, null);
        if (cursor != null) {
            if (cursor.getCount() > 0) {
                if (cursor.moveToFirst()) {
                    mEmergencyApn = makeApnSetting(cursor);
                }
            }
            cursor.close();
        }

#endif
}

ECode DcTracker::AddEmergencyApnSetting()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if(mEmergencyApn != null) {
            if(mAllApnSettings == null) {
                mAllApnSettings = new ArrayList<ApnSetting>();
            } else {
                boolean hasEmergencyApn = false;
                for (ApnSetting apn : mAllApnSettings) {
                    if (ArrayUtils.contains(apn.types, PhoneConstants.APN_TYPE_EMERGENCY)) {
                        hasEmergencyApn = true;
                        break;
                    }
                }
                if(hasEmergencyApn == false) {
                    mAllApnSettings.add(mEmergencyApn);
                } else {
                    log("addEmergencyApnSetting - E-APN setting is already present");
                }
            }
        }

#endif
}

ECode DcTracker::GetUri(
    /* [in] */ IUri* uri,
    /* [out] */ IUri** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return Uri.withAppendedPath(uri, "/subId/" + mSubId);

#endif
}

ECode DcTracker::GetImsRegistrationState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mImsRegistrationState;
    return NOERROR;
}

AutoPtr<IUri> DcTracker::InitPREFERAPN_NO_UPDATE_URI()
{
    AutoPtr<IUri> rev;
#if 0 // TODO: Translate codes below
    Uri::Parse(String("content://telephony/carriers/preferapn_no_update"), (IUri**)&rev);
#endif
    return rev;
}

String DcTracker::InitPROPERTY_CDMA_IPPROTOCOL()
{
    String rev;
#if 0 // TODO: Translate codes below
    SystemProperties.get(
            "persist.telephony.cdma.protocol", "IP");
#endif
    return rev;
}

String DcTracker::InitPROPERTY_CDMA_ROAMING_IPPROTOCOL()
{
    String rev;
#if 0 // TODO: Translate codes below
    SystemProperties.get(
            "persist.telephony.cdma.rproto", "IP");
#endif
    return rev;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
