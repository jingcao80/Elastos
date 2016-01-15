
#include "location/GpsLocationProvider.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Droid::Utility::INtpTrustedTimeHelper;
using Elastos::Droid::Utility::CNtpTrustedTimeHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::EIID_IPowerManager;
using Elastos::Droid::Location::ILocationProvider;
using Elastos::Droid::Location::ICriteria;
using Elastos::Droid::Location::IGpsNiNotification;
using Elastos::Droid::Location::CGpsNiNotification;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::CUri;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Location::CGpsNetInitiatedHandler;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

static AutoPtr<IProviderProperties> InitGpsLocationProvider()
{
    GpsLocationProvider::Class_init_native();
    AutoPtr<IProviderProperties> temp;
    CProviderProperties::New(TRUE, TRUE, FALSE, FALSE, TRUE, TRUE, TRUE,
            ICriteria::POWER_HIGH, ICriteria::ACCURACY_FINE, (IProviderProperties**)&temp);
    return temp;
}
const String GpsLocationProvider::TAG("GpsLocationProvider");

const Boolean GpsLocationProvider::DEBUG = FALSE;//Log.isLoggable(TAG, Log.DEBUG);
const Boolean GpsLocationProvider::VERBOSE = FALSE;//Log.isLoggable(TAG, Log.VERBOSE);

const AutoPtr<IProviderProperties> GpsLocationProvider::PROPERTIES = InitGpsLocationProvider();

// these need to match GpsPositionMode enum in gps.h
const Int32 GpsLocationProvider::GPS_POSITION_MODE_STANDALONE = 0;
const Int32 GpsLocationProvider::GPS_POSITION_MODE_MS_BASED = 1;
const Int32 GpsLocationProvider::GPS_POSITION_MODE_MS_ASSISTED = 2;

// these need to match GpsPositionRecurrence enum in gps.h
const Int32 GpsLocationProvider::GPS_POSITION_RECURRENCE_PERIODIC = 0;
const Int32 GpsLocationProvider::GPS_POSITION_RECURRENCE_SINGLE = 1;

// these need to match GpsStatusValue defines in gps.h
const Int32 GpsLocationProvider::GPS_STATUS_NONE = 0;
const Int32 GpsLocationProvider::GPS_STATUS_SESSION_BEGIN = 1;
const Int32 GpsLocationProvider::GPS_STATUS_SESSION_END = 2;
const Int32 GpsLocationProvider::GPS_STATUS_ENGINE_ON = 3;
const Int32 GpsLocationProvider::GPS_STATUS_ENGINE_OFF = 4;

// these need to match GpsApgsStatusValue defines in gps.h
/** AGPS status event values. */
const Int32 GpsLocationProvider::GPS_REQUEST_AGPS_DATA_CONN = 1;
const Int32 GpsLocationProvider::GPS_RELEASE_AGPS_DATA_CONN = 2;
const Int32 GpsLocationProvider::GPS_AGPS_DATA_CONNECTED = 3;
const Int32 GpsLocationProvider::GPS_AGPS_DATA_CONN_DONE = 4;
const Int32 GpsLocationProvider::GPS_AGPS_DATA_CONN_FAILED = 5;

// these need to match GpsLocationFlags enum in gps.h
const Int32 GpsLocationProvider::LOCATION_INVALID = 0;
const Int32 GpsLocationProvider::LOCATION_HAS_LAT_LONG = 1;
const Int32 GpsLocationProvider::LOCATION_HAS_ALTITUDE = 2;
const Int32 GpsLocationProvider::LOCATION_HAS_SPEED = 4;
const Int32 GpsLocationProvider::LOCATION_HAS_BEARING = 8;
const Int32 GpsLocationProvider::LOCATION_HAS_ACCURACY = 16;

// IMPORTANT - the GPS_DELETE_* symbols here must match constants in gps.h
const Int32 GpsLocationProvider::GPS_DELETE_EPHEMERIS = 0x0001;
const Int32 GpsLocationProvider::GPS_DELETE_ALMANAC = 0x0002;
const Int32 GpsLocationProvider::GPS_DELETE_POSITION = 0x0004;
const Int32 GpsLocationProvider::GPS_DELETE_TIME = 0x0008;
const Int32 GpsLocationProvider::GPS_DELETE_IONO = 0x0010;
const Int32 GpsLocationProvider::GPS_DELETE_UTC = 0x0020;
const Int32 GpsLocationProvider::GPS_DELETE_HEALTH = 0x0040;
const Int32 GpsLocationProvider::GPS_DELETE_SVDIR = 0x0080;
const Int32 GpsLocationProvider::GPS_DELETE_SVSTEER = 0x0100;
const Int32 GpsLocationProvider::GPS_DELETE_SADATA = 0x0200;
const Int32 GpsLocationProvider::GPS_DELETE_RTI = 0x0400;
const Int32 GpsLocationProvider::GPS_DELETE_CELLDB_INFO = 0x8000;
const Int32 GpsLocationProvider::GPS_DELETE_ALL = 0xFFFF;

// The GPS_CAPABILITY_* flags must match the values in gps.h
const Int32 GpsLocationProvider::GPS_CAPABILITY_SCHEDULING = 0x0000001;
const Int32 GpsLocationProvider::GPS_CAPABILITY_MSB = 0x0000002;
const Int32 GpsLocationProvider::GPS_CAPABILITY_MSA = 0x0000004;
const Int32 GpsLocationProvider::GPS_CAPABILITY_SINGLE_SHOT = 0x0000008;
const Int32 GpsLocationProvider::GPS_CAPABILITY_ON_DEMAND_TIME = 0x0000010;

// these need to match AGpsType enum in gps.h
const Int32 GpsLocationProvider::AGPS_TYPE_SUPL = 1;
const Int32 GpsLocationProvider::AGPS_TYPE_C2K = 2;

// for mAGpsDataConnectionState
const Int32 GpsLocationProvider::AGPS_DATA_CONNECTION_CLOSED = 0;
const Int32 GpsLocationProvider::AGPS_DATA_CONNECTION_OPENING = 1;
const Int32 GpsLocationProvider::AGPS_DATA_CONNECTION_OPEN = 2;

// Handler messages
const Int32 GpsLocationProvider::CHECK_LOCATION = 1;
const Int32 GpsLocationProvider::ENABLE = 2;
const Int32 GpsLocationProvider::SET_REQUEST = 3;
const Int32 GpsLocationProvider::UPDATE_NETWORK_STATE = 4;
const Int32 GpsLocationProvider::INJECT_NTP_TIME = 5;
const Int32 GpsLocationProvider::DOWNLOAD_XTRA_DATA = 6;
const Int32 GpsLocationProvider::UPDATE_LOCATION = 7;
const Int32 GpsLocationProvider::ADD_LISTENER = 8;
const Int32 GpsLocationProvider::REMOVE_LISTENER = 9;
const Int32 GpsLocationProvider::INJECT_NTP_TIME_FINISHED = 10;
const Int32 GpsLocationProvider::DOWNLOAD_XTRA_DATA_FINISHED = 11;

// Request setid
const Int32 GpsLocationProvider::AGPS_RIL_REQUEST_SETID_IMSI = 1;
const Int32 GpsLocationProvider::AGPS_RIL_REQUEST_SETID_MSISDN = 2;

// Request ref location
const Int32 GpsLocationProvider::AGPS_RIL_REQUEST_REFLOC_CELLID = 1;
const Int32 GpsLocationProvider::AGPS_RIL_REQUEST_REFLOC_MAC = 2;

// ref. location info
const Int32 GpsLocationProvider::AGPS_REF_LOCATION_TYPE_GSM_CELLID = 1;
const Int32 GpsLocationProvider::AGPS_REF_LOCATION_TYPE_UMTS_CELLID = 2;
const Int32 GpsLocationProvider::AGPS_REG_LOCATION_TYPE_MAC        = 3;

// set id info
const Int32 GpsLocationProvider::AGPS_SETID_TYPE_NONE = 0;
const Int32 GpsLocationProvider::AGPS_SETID_TYPE_IMSI = 1;
const Int32 GpsLocationProvider::AGPS_SETID_TYPE_MSISDN = 2;

const String GpsLocationProvider::PROPERTIES_FILE("/etc/gps.conf");

// turn off GPS fix icon if we haven't received a fix in 10 seconds
const Int64 GpsLocationProvider::RECENT_FIX_TIMEOUT = 10 * 1000;

// stop trying if we do not receive a fix within 60 seconds
const Int32 GpsLocationProvider::NO_FIX_TIMEOUT = 60 * 1000;

// if the fix interval is below this we leave GPS on,
// if above then we cycle the GPS driver.
// Typical hot TTTF is ~5 seconds, so 10 seconds seems sane.
const Int32 GpsLocationProvider::GPS_POLLING_THRESHOLD_INTERVAL = 10 * 1000;

// how often to request NTP time, in milliseconds
// current setting 24 hours
const Int64 GpsLocationProvider::NTP_INTERVAL = 24L * 60L * 60L * 1000L;
// how long to wait if we have a network error in NTP or XTRA downloading
// current setting - 5 minutes
const Int64 GpsLocationProvider::RETRY_INTERVAL = 5L * 60L * 1000;

// states for injecting ntp and downloading xtra data
const Int32 GpsLocationProvider::STATE_PENDING_NETWORK = 0;
const Int32 GpsLocationProvider::STATE_DOWNLOADING = 1;
const Int32 GpsLocationProvider::STATE_IDLE = 2;

// Wakelocks
const String GpsLocationProvider::WAKELOCK_KEY("GpsLocationProvider");

// Alarms
const String GpsLocationProvider::ALARM_WAKEUP("com.android.internal.location.ALARM_WAKEUP");
const String GpsLocationProvider::ALARM_TIMEOUT("com.android.internal.location.ALARM_TIMEOUT");

// for GPS SV statistics
const Int32 GpsLocationProvider::MAX_SVS = 32;
const Int32 GpsLocationProvider::EPHEMERIS_MASK = 0;
const Int32 GpsLocationProvider::ALMANAC_MASK = 1;
const Int32 GpsLocationProvider::USED_FOR_FIX_MASK = 2;

CAR_INTERFACE_IMPL(GpsLocationProvider, ILocationProviderInterface)
CAR_INTERFACE_IMPL(GpsLocationProvider::ConstructorRunnable, IRunnable)
CAR_INTERFACE_IMPL(GpsLocationProvider::NetworkLocationListener, ILocationListener)

AutoPtr<IIGpsStatusProvider> GpsLocationProvider::GetGpsStatusProvider()
{
    return mGpsStatusProvider;
}
/*****************************************************************************/
/******       GpsLocationProvider::NetworkLocationListener Start       *******/
/*****************************************************************************/

GpsLocationProvider::NetworkLocationListener::NetworkLocationListener(
    /* [in] */ GpsLocationProvider* host) : mHost(host)
{}

ECode GpsLocationProvider::NetworkLocationListener::OnLocationChanged(
    /* [in] */ ILocation* location)
{
    String provider;
    location->GetProvider(&provider);
    if(provider.Equals(ILocationManager::NETWORK_PROVIDER))
    {
        mHost->HandleUpdateLocation(location);
    }
    return NOERROR;
}

ECode GpsLocationProvider::NetworkLocationListener::OnStatusChanged(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras)
{
    return NOERROR;
}

ECode GpsLocationProvider::NetworkLocationListener::OnProviderEnabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

ECode GpsLocationProvider::NetworkLocationListener::OnProviderDisabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

/*****************************************************************************/
/********       GpsLocationProvider::ConstructorRunnable Start       *********/
/*****************************************************************************/

GpsLocationProvider::ConstructorRunnable::ConstructorRunnable(
    /* [in] */ GpsLocationProvider* host) : mHost(host)
{}

ECode GpsLocationProvider::ConstructorRunnable::Run()
{
    AutoPtr<IInterface> svTemp;
    mHost->mContext->GetSystemService(IContext::LOCATION_SERVICE, (IInterface**)&svTemp);
    AutoPtr<ILocationManager> locManager = (ILocationManager*)svTemp->Probe(EIID_ILocationManager);
    AutoPtr<ILooper> lp;
    mHandler->GetLooper((ILooper**)&lp);
    AutoPtr<ILocationListener> ls = new NetworkLocationListener(mHost);
    locManager->RequestLocationUpdates(ILocationManager::PASSIVE_PROVIDER,
            0, 0, ls, lp);
    return NOERROR;
}

Boolean GpsLocationProvider::IsSupported()
{
    return Native_is_supported();
}

GpsLocationProvider::GpsLocationProvider(
    /* [in] */ IContext* context,
    /* [in] */ IILocationManager* ilocationManager)
    : mLocationFlags(LOCATION_INVALID)
    , mStatus(ILocationProvider::TEMPORARILY_UNAVAILABLE)
    , mStatusUpdateTime(SystemClock::GetElapsedRealtime())
    , mEnabled(FALSE)
    , mNetworkAvailable(FALSE)
    , mInjectNtpTimePending(STATE_PENDING_NETWORK)
    , mDownloadXtraDataPending(STATE_PENDING_NETWORK)
    , mPeriodicTimeInjection(FALSE)
    , mNavigating(FALSE)
    , mEngineOn(FALSE)
    , mFixInterval(1000)
    , mStarted(FALSE)
    , mEngineCapabilities(0)
    , mSupportsXtra(FALSE)
    , mFixRequestTime(0)
    , mTimeToFirstFix(0)
    , mLastFixTime(0)
    , mPositionMode(0)
    , mSuplServerPort(0)
    , mC2KServerPort(0)
    , mContext(context)
    , mILocationManager(ilocationManager)
    , mAGpsDataConnectionState(0)
    , mAGpsDataConnectionIpAddr(0)
    , mSvCount(0)
{
    CLocation::New(ILocationManager::GPS_PROVIDER, (ILocation**)&mLocation);
    CBundle::New((IBundle**)&mLocationExtras);

    mBroadcastReciever = new MyBroadcastReceiver(this);
    PFL_EX("Not Init mGpsStatusProvider && mNetInitiatedListener")
    // preallocated arrays, to avoid memory allocation in reportStatus()
    mSvs = ArrayOf<Int32>::Alloc(MAX_SVS);
    mSnrs = ArrayOf<Float>::Alloc(MAX_SVS);
    mSvElevations = ArrayOf<Float>::Alloc(MAX_SVS);
    mSvAzimuths = ArrayOf<Float>::Alloc(MAX_SVS);
    mSvMasks = ArrayOf<Int32>::Alloc(3);
    // preallocated to avoid memory allocation in reportNmea()
    mNmeaBuffer = ArrayOf<Byte>::Alloc(120);

    CGpsNetInitialtedHandler::New(context, (IGpsNetInitiatedHandler**)&mNIHandler);

    AutoPtr<INtpTrustedTimeHelper> helper;
    CNtpTrustedTimeHelper::AcquireSingleton((INtpTrustedTimeHelper**)&helper);
    helper->GetInstance(context, (INtpTrustedTimeHelper**)&mNtpTime);

    mLocation->SetExtras(mLocationExtras);
    AutoPtr<IInterface> svTemp;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&svTemp);
    AutoPtr<IPowerManager> powerManager = (IPowerManager*)svTemp->Probe(EIID_IPowerManager);
    powerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, WAKELOCK_KEY, (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->SetReferenceCounted(TRUE);

    svTemp = NULL;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&svTemp);
    mAlarmManager = (IAlarmManager*)svTemp->Probe(EIID_IAlarmManager);
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IIntent> wakeupIntent;
    AutoPtr<IIntent> timeoutIntent;
    CIntent::New(ALARM_WAKEUP, (IIntent**)&wakeupIntent);
    CIntent::New(ALARM_TIMEOUT, (IIntent**)&timeoutIntent);
    helper->GetBroadcast(mContext, 0, wakeupIntent, 0, (IPendingIntent**)&mWakeupIntent);
    helper->GetBroadcast(mContext, 0, timeoutIntent, 0, (IPendingIntent**)&mTimeoutIntent);

    svTemp = NULL;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&svTemp);
    mConnMgr = (IConnectivityManager*)svTemp->Probe(EIID_IConnectivityManager);

    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    svTemp = NULL;
    sm->GetService(String("batteryinfo"), (IInterface**)&svTemp);
    mBatteryStats = (IIBatteryStats*)svTemp->Probe(EIID_iIBatteryStats);
    CProperties::New((IProperties)&mProperties);

    AutoPtr<IFile> file;
    CFile::New(PROPERTIES_FILE, (IFile**)&file);
    AutoPtr<IFileInputStream> stream;
    CFileInputStream::New(file, (IFileInputStream**)&stream);
    mProperties->Load(stream);
    stream->Close();

    mProperties->GetProperty(String("SUPL_HOST"), &mSuplServerHost);
    String portString;
    mProperties->GetProperty(String("SUPL_PORT"), &portString);
    if (mSuplServerHost != NULL && portString != NULL) {
        mSuplServerPort = StringUtils::ParseInt32(portString);
    }

    mProperties->GetProperty(String("C2K_HOST"), &mC2KServerHost);
    mProperties->GetProperty(String("C2K_PORT"), &portString);
    if (mC2KServerHost != NULL && portString != NULL) {
        mC2KServerPort = StringUtils::ParseInt32(portString);
    }

    mHandler = new ProviderHandler(this);
    ListenForBroadcasts();

    // also listen for PASSIVE_PROVIDER updates
    AutoPtr<IRunnable> runnable = new ConstructorRunnable(this);
    Boolean result;
    mHandler->Post(runnable, &result)
}

/**
 * Returns the name of this provider.
 */
//@Override
ECode GpsLocationProvider::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = ILocationManager::GPS_PROVIDER;
    return NOERROR;
}

//@Override
ECode GpsLocationProvider::GetProperties(
    /* [out] */ IProviderProperties** properties)
{
    VALIDATE_NOT_NULL(properties);
    *properties = PROPERTIES;
    REFCOUNT_ADD(*properties);
    return NOERROR;
}

void GpsLocationProvider::UpdateNetworkState(
    /* [in] */ Int32 state,
    /* [in] */ INetworkInfo* info)
{
     SendMessage(UPDATE_NETWORK_STATE, state, info);
}

/**
 * Enables this provider.  When enabled, calls to getStatus()
 * must be handled.  Hardware may be started up
 * when the provider is enabled.
 */
//@Override
ECode GpsLocationProvider::Enable()
{
    SendMessage(ENABLE, 1, NULL);
    return NOERROR;
}

/**
 * Disables this provider.  When disabled, calls to getStatus()
 * need not be handled.  Hardware may be shut
 * down while the provider is disabled.
 */
//@Override
ECode GpsLocationProvider::Disable()
{
    SendMessage(ENABLE, 0, NULL);
    return NOERROR;
}

//@Override
ECode GpsLocationProvider::IsEnabled(
    /* [out] */ Boolean* enable)
{
    VALIDATE_NOT_NULL(enable);
    {
        AutoLock lock(mLock);
        * enable = mEnabled;
        return NOERROR;
    }
}

//@Override
ECode GpsLocationProvider::GetStatus(
    /* [in] */ IBundle* extras,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);
    if (extras != NULL) {
        extras->PutInt32(String("satellites"), mSvCount);
    }
    *status = mStatus;

    return NOERROR;
}

//@Override
ECode GpsLocationProvider::GetStatusUpdateTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mStatusUpdateTime;
    return NOERROR;
}

//@Override
ECode GpsLocationProvider::SetRequest(
    /* [in] */ IProviderRequest* request,
    /* [in] */ IWorkSource* source)
{
    SendMessage(SET_REQUEST, 0, new GpsRequest(request, source));
}

//@Override
ECode GpsLocationProvider::SwitchUser(
    /* [in] */ Int32 userId)
{
    // nothing to do here

    return NOERROR;
}

//@Override
ECode GpsLocationProvider::SendExtraCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(extras);
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
    Int64 identity;
    binderHelper->ClearCallingIdentity(&identity);
    *result = FALSE;

    if (command.Equals("delete_aiding_data")) {
        *result = DeleteAidingData(extras);
    } else if (command.Equals("force_time_injection")) {
        SendMessage(INJECT_NTP_TIME, 0, null);
        *result = TRUE;
    } else if (command.Equals("force_xtra_injection")) {
        if (mSupportsXtra) {
            XtraDownloadRequest();
            *result = TRUE;
        }
    } else {
//        Log.w(TAG, "sendExtraCommand: unknown command " + command);
    }

    binderHelper->RestoreCallingIdentity(identity);

    return NOERROR;
}

AutoPtr<IINetInitiatedListener> GpsLocationProvider::GetNetInitiatedListener()
{
    return mNetInitiatedListener;
}

// Called by JNI function to report an NI request.
void GpsLocationProvider::ReportNiNotification(
    /* [in] */ Int32 notificationId,
    /* [in] */ Int32 niType,
    /* [in] */ Int32 notifyFlags,
    /* [in] */ Int32 timeout,
    /* [in] */ Int32 defaultResponse,
    /* [in] */ const String& requestorId,
    /* [in] */ const String& text,
    /* [in] */ Int32 requestorIdEncoding,
    /* [in] */ Int32 textEncoding,
    /* [in] */ const String& extras  // Encoded extra data
    )
{
//    Log.i(TAG, "reportNiNotification: entered");
//    Log.i(TAG, "notificationId: " + notificationId +
//            ", niType: " + niType +
//            ", notifyFlags: " + notifyFlags +
//            ", timeout: " + timeout +
//            ", defaultResponse: " + defaultResponse);

//    Log.i(TAG, "requestorId: " + requestorId +
//            ", text: " + text +
//            ", requestorIdEncoding: " + requestorIdEncoding +
//            ", textEncoding: " + textEncoding);
    AutoPtr<IGpsNiNotification> notification;
    CGpsNiNotification::New((IGpsNiNotification**)&notification);
    notification->SetNotificationId(notificationId);
    notification->SetNiType(niType);
    notification->SetNeedNotify((notifyFlags & GpsNetInitiatedHandler::GPS_NI_NEED_NOTIFY) != 0);
    notification->SetNeedVerify((notifyFlags & GpsNetInitiatedHandler::GPS_NI_NEED_VERIFY) != 0);
    notification->SetPrivacyOverride((notifyFlags & GpsNetInitiatedHandler::GPS_NI_PRIVACY_OVERRIDE) != 0);
    notification->SetTimeOut(timeout);
    notification->SetDefaultResponse(defaultResponse);
    notification->SetRequestorId(requestorId);
    notification->SetText(text);
    notification->SetRequestorIdEncoding(requestorIdEncoding);
    notification->SetTextEncoding(textEncoding);

    // Process extras, assuming the format is
    // one of more lines of "key = value"
    AutoPtr<IBundle> bundle;// = new Bundle();
    CBundle::New((IBundle**)&bundle);
    String extrasTemp(extras);
    if (extrasTemp.IsNull())
    {
        extrasTemp = "";
    }
    AutoPtr<IProperties> extraProp;// = new Properties();
    CProperties::New((IProperties**)&extraProp);

    //try {
        extraProp->Load(new StringReader(extrasTemp));
    //}
    //catch (IOException e)
    //{
    //    Log.e(TAG, "reportNiNotification cannot parse extras data: " + extras);
    //}

    for (Entry<Object, Object> ent : extraProp.entrySet())
    {
        bundle->PutString((String) ent.getKey(), (String) ent.getValue());
    }

    notification->extras = bundle;

    mNIHandler->HandleNiNotification(notification);
}

//@Override
ECode GpsLocationProvider::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<StringBuilder> s = new StringBuilder();
    s->AppendCStr("  mFixInterval=");
    s->AppendInt32(mFixInterval);
    s->AppendCStr("\n");
    s->AppendCStr("  mEngineCapabilities=0x");
//    s->Append(Integer.toHexString(mEngineCapabilities))
    s->AppendCStr(" (");
    if (HasCapability(GPS_CAPABILITY_SCHEDULING)) s->AppendCStr("SCHED ");
    if (HasCapability(GPS_CAPABILITY_MSB)) s->AppendCStr("MSB ");
    if (HasCapability(GPS_CAPABILITY_MSA)) s->AppendCStr("MSA ");
    if (HasCapability(GPS_CAPABILITY_SINGLE_SHOT)) s->AppendCStr("SINGLE_SHOT ");
    if (HasCapability(GPS_CAPABILITY_ON_DEMAND_TIME)) s->AppendCStr("ON_DEMAND_TIME ");
    s->AppendCStr(")\n");

    s->AppendCStr(Native_get_internal_state());
    AutoPtr<ICharSequence> cs;
    s->SubSequence(0, s->GetLength(), (ICharSequence**)&cs);
    pw->AppendCharSequence(cs);

    return NOERROR;
}

void GpsLocationProvider::CheckSmsSuplInit(
    /* [in] */ IIntent* intent)
{
#if 0
    assert(intent);
    AutoPtr<ArrayOf<ISmsMessage*> > messages = CIntents::GetMessagesFromIntent(intent);
    for (Int32 i=0; i <messages.length; i++) {
        AutoPtr<ArrayOf<Byte> > supl_init;
        messages[i]->GetUserData(supl_init);
        Native_agps_ni_message(supl_init,supl_init.length);
    }
#endif
}

void GpsLocationProvider::CheckWapSuplInit(
    /* [in] */ IIntent* intent)
{
    assert(intent != NULL);
    AutoPtr<ArrayOf<Byte> > supl_init;
    intent->GetExtra(String("data"), (IInterface**)&supl_init);
    Native_agps_ni_message(supl_init, supl_init.Get()->GetLength());
}

void GpsLocationProvider::ListenForBroadcasts()
{
    AutoPtr<IIntentFilter> intentFilter;// = new IntentFilter();
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntents::DATA_SMS_RECEIVED_ACTION);
    intentFilter->AddDataScheme(String("sms"));
    intentFilter->AddDataAuthority(String("localhost"),String("7275"));
    mContext->RegisterReceiver(mBroadcastReciever, intentFilter, NULL, mHandler);
    intentFilter = NULL;

    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntents::WAP_PUSH_RECEIVED_ACTION);
    //try {
        intentFilter->AddDataType(String("application/vnd.omaloc-supl-init"));
    //} catch (IntentFilter.MalformedMimeTypeException e) {
    //    Log.w(TAG, "Malformed SUPL init mime type");
    //}
//    mContext->RegisterReceiver(mBroadcastReciever, intentFilter, NULL, mHandler);
    intentFilter = NULL;

    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(ALARM_WAKEUP);
    intentFilter->AddAction(ALARM_TIMEOUT);
    intentFilter->AddAction(IConnectivityManager::CONNECTIVITY_ACTION);
    mContext->RegisterReceiver(mBroadcastReciever, intentFilter, NULL, mHandler);
}

void GpsLocationProvider::HandleUpdateNetworkState(
    /* [in] */ Int32 state,
    /* [in] */ INetworkInfo* info)
{
    mNetworkAvailable = (state == ILocationProvider::AVAILABLE);

//    if (DEBUG) {
//        Log.d(TAG, "updateNetworkState " + (mNetworkAvailable ? "available" : "unavailable")
//            + " info: " + info);
//    }
#if 0
    if (info != NULL) {
        Boolean dataEnabled = FALSE;

        AutoPtr<ISettingsGlobal> settingGlobal;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingGlobal);

        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);

        Int32 getInt;
        settingGlobal->GetInt32(resolver, ISettingsGlobal::MOBILE_DATA, 1, &getInt);

        dataEnabled = getInt == 1;

        Boolean tmp = FALSE;
        info->IsAvailable(&tmp);
        Boolean networkAvailable = tmp && dataEnabled;
        String defaultApn = GetSelectedApn();
        if (defaultApn == NULL) {
            defaultApn = "dummy-apn";
        }

        Boolean connected;
        Int32 type;
        Boolean roaming;
        String extraInfo;

        info->IsConnected(&connected);
        info->GetType(&type);
        info->IsRoaming(&roaming);
        info->GetExtraInfo(&extraInfo);

        Native_update_network_state(connected, type,
                                    roaming, networkAvailable,
                                    extraInfo, defaultApn);
    }

    Int32 type;
    if (info != NULL && (info->GetType(&type), type == ConnectivityManager::TYPE_MOBILE_SUPL)
            && mAGpsDataConnectionState == AGPS_DATA_CONNECTION_OPENING) {
        String apnName;
        info->GetExtraInfo(&apnName);
        if (mNetworkAvailable) {
            if (apnName.GetLength() == 0) {
                /* Assign a dummy value in the case of C2K as otherwise we will have a runtime
                exception in the following call to native_agps_data_conn_open*/
                apnName = "dummy-apn";
            }
            mAGpsApn = apnName;
//            if (DEBUG) Log.d(TAG, "mAGpsDataConnectionIpAddr " + mAGpsDataConnectionIpAddr);
            if (mAGpsDataConnectionIpAddr != 0xffffffff) {
                Boolean route_result;
//                if (DEBUG) Log.d(TAG, "call requestRouteToHost");
                mConnMgr->RequestRouteToHost(ConnectivityManager::TYPE_MOBILE_SUPL,
                    mAGpsDataConnectionIpAddr, &route_result);
//                if (route_result == false) Log.d(TAG, "call requestRouteToHost failed");
            }
//            if (DEBUG) Log.d(TAG, "call native_agps_data_conn_open");
            Native_agps_data_conn_open(apnName);
            mAGpsDataConnectionState = AGPS_DATA_CONNECTION_OPEN;
        } else {
//            if (DEBUG) Log.d(TAG, "call native_agps_data_conn_failed");
            mAGpsApn = "";
            mAGpsDataConnectionState = AGPS_DATA_CONNECTION_CLOSED;
            Native_agps_data_conn_failed();
        }
    }

    if (mNetworkAvailable) {
        if (mInjectNtpTimePending == STATE_PENDING_NETWORK) {
//            SendMessage(INJECT_NTP_TIME, 0, null);
        }
        if (mDownloadXtraDataPending == STATE_PENDING_NETWORK) {
//            SendMessage(DOWNLOAD_XTRA_DATA, 0, null);
        }
    }
#endif
}

void GpsLocationProvider::HandleInjectNtpTime()
{
/*
    if (mInjectNtpTimePending == STATE_DOWNLOADING) {
        // already downloading data
        return;
    }
    if (!mNetworkAvailable) {
        // try again when network is up
        mInjectNtpTimePending = STATE_PENDING_NETWORK;
        return;
    }
    mInjectNtpTimePending = STATE_DOWNLOADING;

    // hold wake lock while task runs
    mWakeLock.acquire();
    AsyncTask.THREAD_POOL_EXECUTOR.execute(new Runnable() {
        @Override
        public void run() {
            long delay;

            // force refresh NTP cache when outdated
            if (mNtpTime.getCacheAge() >= NTP_INTERVAL) {
                mNtpTime.forceRefresh();
            }

            // only update when NTP time is fresh
            if (mNtpTime.getCacheAge() < NTP_INTERVAL) {
                long time = mNtpTime.getCachedNtpTime();
                long timeReference = mNtpTime.getCachedNtpTimeReference();
                long certainty = mNtpTime.getCacheCertainty();
                long now = System.currentTimeMillis();

                Log.d(TAG, "NTP server returned: "
                        + time + " (" + new Date(time)
                        + ") reference: " + timeReference
                        + " certainty: " + certainty
                        + " system time offset: " + (time - now));

                native_inject_time(time, timeReference, (int) certainty);
                delay = NTP_INTERVAL;
            } else {
                if (DEBUG) Log.d(TAG, "requestTime failed");
                delay = RETRY_INTERVAL;
            }

            SendMessage(INJECT_NTP_TIME_FINISHED, 0, null);

            if (mPeriodicTimeInjection) {
                // send delayed message for next NTP injection
                // since this is delayed and not urgent we do not hold a wake lock here
                mHandler.sendEmptyMessageDelayed(INJECT_NTP_TIME, delay);
            }

            // release wake lock held by task
            mWakeLock.release();
        }
    });*/
}

void GpsLocationProvider::HandleDownloadXtraData()
{
/*
    if (mDownloadXtraDataPending == STATE_DOWNLOADING) {
        // already downloading data
        return;
    }
    if (!mNetworkAvailable) {
        // try again when network is up
        mDownloadXtraDataPending = STATE_PENDING_NETWORK;
        return;
    }
    mDownloadXtraDataPending = STATE_DOWNLOADING;

    // hold wake lock while task runs
    mWakeLock.acquire();
    AsyncTask.THREAD_POOL_EXECUTOR.execute(new Runnable() {
        @Override
        public void run() {
            GpsXtraDownloader xtraDownloader = new GpsXtraDownloader(mContext, mProperties);
            byte[] data = xtraDownloader.downloadXtraData();
            if (data != null) {
                if (DEBUG) {
                    Log.d(TAG, "calling native_inject_xtra_data");
                }
                native_inject_xtra_data(data, data.length);
            }

            SendMessage(DOWNLOAD_XTRA_DATA_FINISHED, 0, null);

            if (data == null) {
                // try again later
                // since this is delayed and not urgent we do not hold a wake lock here
                mHandler.sendEmptyMessageDelayed(DOWNLOAD_XTRA_DATA, RETRY_INTERVAL);
            }

            // release wake lock held by task
            mWakeLock.release();
        }
    });*/
}

void GpsLocationProvider::HandleUpdateLocation(
    /* [in] */ ILocation* location)
{
    assert(location);

    Boolean bAccuracy = FALSE;
    location->HasAccuracy(&bAccuracy);
    if (bAccuracy) {

        Double latitude;
        Double longitude;
        Float accuracy;

        location->GetLatitude(&latitude);
        location->GetLongitude(&longitude);
        location->GetAccuracy(&accuracy);

        Native_inject_location(latitude, longitude, accuracy);
    }
}

void GpsLocationProvider::HandleEnable()
{
//    if (DEBUG) Log.d(TAG, "handleEnable");

    //synchronized(mLock)
    {
        AutoLock lock(mLock);
        if (mEnabled) return;
        mEnabled = TRUE;
    }

    Boolean enabled = Native_init();

    if (enabled) {
        mSupportsXtra = Native_supports_xtra();
        if (mSuplServerHost != NULL) {
            Native_set_agps_server(AGPS_TYPE_SUPL, mSuplServerHost, mSuplServerPort);
        }
        if (mC2KServerHost != NULL) {
            Native_set_agps_server(AGPS_TYPE_C2K, mC2KServerHost, mC2KServerPort);
        }
    } else {
        //synchronized(mLock)
        {
            AutoLock lock(mLock);
            mEnabled = FALSE;
        }
//        Log.w(TAG, "Failed to enable location provider");
    }
}

void GpsLocationProvider::HandleDisable()
{
//    if (DEBUG) Log.d(TAG, "handleDisable");

    //synchronized(mLock)
    {
        AutoLock lock(mLock);
        if (!mEnabled) return;
        mEnabled = FALSE;
    }

    StopNavigating();
//    mAlarmManager->Cancel(mWakeupIntent);
//    mAlarmManager->Cancel(mTimeoutIntent);

    // do this before releasing wakelock
    Native_cleanup();
}

void GpsLocationProvider::UpdateStatus(
    /* [in] */ Int32 status,
    /* [in] */ Int32 svCount)
{
    if (status != mStatus || svCount != mSvCount) {
        mStatus = status;
        mSvCount = svCount;
        mLocationExtras->PutInt32(String("satellites"), svCount);
        mStatusUpdateTime = SystemClock::GetElapsedRealtime();
    }
}
#if 0
void GpsLocationProvider::HandleSetRequest(
    /* [in] */ IProviderRequest* request,
    /* [in] */ IWorkSource* source)
{
    assert(request != NULL);
    assert(source != NULL);

//    if (DEBUG) Log.d(TAG, "setRequest " + request);

    if (((CProviderRequest*))(request.Get()))->reportLocation) {
        // update client uids
        Int32 size = 0;
        source->Size(&size);
        AutoPtr<ArrayOf<Int32> > uids = ArrayOf<Int32>::Alloc(size);
        for (Int32 i=0; i < size; i++) {
            source->Get(i, &uids[i]);
        }

        UpdateClientUids(uids);

        Int32 interval = (Int32) ((CProviderRequest*))(request.Get()))->interval;

        mFixInterval = interval;

        // check for overflow
        if (mFixInterval != interval) {
//            Log.w(TAG, "interval overflow: " + interval);
            mFixInterval = Elastos::Core::Math::Integer_MAX_VALUE;
        }

        // apply request to GPS engine
        if (mStarted && HasCapability(GPS_CAPABILITY_SCHEDULING)) {
            // change period
            if (!Native_set_position_mode(mPositionMode, GPS_POSITION_RECURRENCE_PERIODIC,
                    mFixInterval, 0, 0)) {
//                Log.e(TAG, "set_position_mode failed in setMinTime()");
            }
        } else if (!mStarted) {
            // start GPS
            StartNavigating();
        }
    } else {
        UpdateClientUids(new int[0]);

        StopNavigating();
        mAlarmManager->Cancel(mWakeupIntent);
        mAlarmManager->Cancel(mTimeoutIntent);
    }
}
#endif
void GpsLocationProvider::UpdateClientUids(
    /* [in] */ ArrayOf<Int32>* uids)
{
    assert(uids != NULL);

    Int32 uidsSize = uids->GetLength();
    Int32 clientUidsSize = mClientUids.Get()->GetLength();

    // Find uid's that were not previously tracked
    for (Int32 i = 0; i < uidsSize; i++) {
        Boolean newUid = TRUE;
        for (Int32 j = 0; j < clientUidsSize; j++) {
            if ((*uids)[i] == (*(mClientUids.Get()))[j]) {
                newUid = FALSE;
                break;
            }
        }
        if (newUid) {
            //try {
//                mBatteryStats->NoteStartGps((*uids)[i]);
            //} catch (RemoteException e) {
            //    Log.w(TAG, "RemoteException", e);
            //}
        }
    }

    // Find uid'd that were tracked but have now disappeared
    for (Int32 i = 0; i < clientUidsSize; i++) {
        Boolean oldUid = TRUE;
        for (Int32 j = 0; j < uidsSize; j++) {
            if ((*uids)[j] == (*(mClientUids.Get()))[i]) {
                oldUid = FALSE;
                break;
            }
        }
        if (oldUid) {
            //try {
//                mBatteryStats->NoteStopGps(uid1);
            //} catch (RemoteException e) {
            //    Log.w(TAG, "RemoteException", e);
            //}
        }
    }

    // save current uids
    mClientUids = uids;
}

Boolean GpsLocationProvider::DeleteAidingData(
    /* [in] */ IBundle* extras)
{
    Int32 flags;
    Boolean bFlag = FALSE;

    if (extras == NULL) {
        flags = GPS_DELETE_ALL;
    } else {
        flags = 0;
        if (extras->GetBoolean(String("ephemeris"), &bFlag), bFlag) {
            flags |= GPS_DELETE_EPHEMERIS;
        }

        if (extras->GetBoolean(String("almanac"), &bFlag), bFlag) {
            flags |= GPS_DELETE_ALMANAC;
        }

        if (extras->GetBoolean(String("position"), &bFlag), bFlag) {
            flags |= GPS_DELETE_POSITION;
        }

        if (extras->GetBoolean(String("time"), &bFlag), bFlag) {
            flags |= GPS_DELETE_TIME;
        }

        if (extras->GetBoolean(String("iono"), &bFlag), bFlag) {
            flags |= GPS_DELETE_IONO;
        }

        if (extras->GetBoolean(String("utc"), &bFlag), bFlag) {
            flags |= GPS_DELETE_UTC;
        }

        if (extras->GetBoolean(String("health"), &bFlag), bFlag) {
            flags |= GPS_DELETE_HEALTH;
        }

        if (extras->GetBoolean(String("svdir"), &bFlag), bFlag) {
            flags |= GPS_DELETE_SVDIR;
        }

        if (extras->GetBoolean(String("svsteer"), &bFlag), bFlag) {
            flags |= GPS_DELETE_SVSTEER;
        }

        if (extras->GetBoolean(String("sadata"), &bFlag), bFlag) {
            flags |= GPS_DELETE_SADATA;
        }

        if (extras->GetBoolean(String("rti"), &bFlag), bFlag) {
            flags |= GPS_DELETE_RTI;
        }

        if (extras->GetBoolean(String("celldb-info"), &bFlag), bFlag) {
            flags |= GPS_DELETE_CELLDB_INFO;
        }

        if (extras->GetBoolean(String("all"), &bFlag), bFlag) {
            flags |= GPS_DELETE_ALL;
        }
    }

    if (flags != 0) {
        Native_delete_aiding_data(flags);
        return TRUE;
    }

    return FALSE;
}

void GpsLocationProvider::StartNavigating()
{
    if (!mStarted) {
//        if (DEBUG) Log.d(TAG, "startNavigating");
        mTimeToFirstFix = 0;
        mLastFixTime = 0;
        mStarted = TRUE;
        mPositionMode = GPS_POSITION_MODE_STANDALONE;

//         if (Settings.Global.getInt(mContext.getContentResolver(),
//                Settings.Global.ASSISTED_GPS_ENABLED, 1) != 0) {
            if (HasCapability(GPS_CAPABILITY_MSB)) {
                mPositionMode = GPS_POSITION_MODE_MS_BASED;
            }
//        }

        Int32 interval = (HasCapability(GPS_CAPABILITY_SCHEDULING) ? mFixInterval : 1000);
        if (!Native_set_position_mode(mPositionMode, GPS_POSITION_RECURRENCE_PERIODIC,
                interval, 0, 0)) {
            mStarted = FALSE;
//            Log.e(TAG, "set_position_mode failed in startNavigating()");
            return;
        }
        if (!Native_start()) {
            mStarted = FALSE;
//            Log.e(TAG, "native_start failed in startNavigating()");
            return;
        }

        // reset SV count to zero
        UpdateStatus(ILocationProvider::TEMPORARILY_UNAVAILABLE, 0);
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&mFixRequestTime);
        if (!HasCapability(GPS_CAPABILITY_SCHEDULING)) {
            // set timer to give up if we do not receive a fix within NO_FIX_TIMEOUT
            // and our fix interval is not short
            if (mFixInterval >= NO_FIX_TIMEOUT) {
//                mAlarmManager->Set(IAlarmManager::ELAPSED_REALTIME_WAKEUP,
//                        SystemClock::GetElapsedRealtime() + NO_FIX_TIMEOUT, mTimeoutIntent);
            }
        }
    }
}

void GpsLocationProvider::StopNavigating()
{
//    if (DEBUG) Log.d(TAG, "stopNavigating");
    if (mStarted) {
        mStarted = FALSE;
        Native_stop();
        mTimeToFirstFix = 0;
        mLastFixTime = 0;
        mLocationFlags = LOCATION_INVALID;

        // reset SV count to zero
        UpdateStatus(ILocationProvider::TEMPORARILY_UNAVAILABLE, 0);
    }
}

void GpsLocationProvider::Hibernate()
{
    // stop GPS until our next fix interval arrives
    StopNavigating();
//    mAlarmManager->Cancel(mTimeoutIntent);
//    mAlarmManager->Cancel(mWakeupIntent);
    Int64 now = SystemClock::GetElapsedRealtime();
//    mAlarmManager->Set(IAlarmManager::ELAPSED_REALTIME_WAKEUP, now + mFixInterval, mWakeupIntent);
}

Boolean GpsLocationProvider::HasCapability(
    /* [in] */ Int32 capability)
{
    return ((mEngineCapabilities & capability) != 0);
}

/**
 * called from native code to update our position.
 */
void GpsLocationProvider::ReportLocation(
    /* [in] */ Int32 flags,
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Double altitude,
    /* [in] */ Float speed,
    /* [in] */ Float bearing,
    /* [in] */ Float accuracy,
    /* [in] */ Int64 timestamp)
{
//    if (VERBOSE) Log.v(TAG, "reportLocation lat: " + latitude + " long: " + longitude +
//                " timestamp: " + timestamp);

    //synchronized(mLocation)
    {
        AutoLock lock(mLocationMutex);
        mLocationFlags = flags;
        if ((flags & LOCATION_HAS_LAT_LONG) == LOCATION_HAS_LAT_LONG) {
            mLocation->SetLatitude(latitude);
            mLocation->SetLongitude(longitude);
            mLocation->SetTime(timestamp);
            // It would be nice to push the elapsed real-time timestamp
            // further down the stack, but this is still useful
            mLocation->SetElapsedRealtimeNanos(SystemClock::GetElapsedRealtimeNanos());
        }
        if ((flags & LOCATION_HAS_ALTITUDE) == LOCATION_HAS_ALTITUDE) {
            mLocation->SetAltitude(altitude);
        } else {
            mLocation->RemoveAltitude();
        }
        if ((flags & LOCATION_HAS_SPEED) == LOCATION_HAS_SPEED) {
            mLocation->SetSpeed(speed);
        } else {
            mLocation->RemoveSpeed();
        }
        if ((flags & LOCATION_HAS_BEARING) == LOCATION_HAS_BEARING) {
            mLocation->SetBearing(bearing);
        } else {
            mLocation->RemoveBearing();
        }
        if ((flags & LOCATION_HAS_ACCURACY) == LOCATION_HAS_ACCURACY) {
            mLocation->SetAccuracy(accuracy);
        } else {
            mLocation->RemoveAccuracy();
        }
        mLocation->SetExtras(mLocationExtras);

        //try {
            mILocationManager->ReportLocation(mLocation, FALSE);
        //} catch (RemoteException e) {
        //    Log.e(TAG, "RemoteException calling reportLocation");
        //}
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mLastFixTime);
    // report time to first fix
    if (mTimeToFirstFix == 0 && (flags & LOCATION_HAS_LAT_LONG) == LOCATION_HAS_LAT_LONG) {
        mTimeToFirstFix = (Int32)(mLastFixTime - mFixRequestTime);
//        if (DEBUG) Log.d(TAG, "TTFF: " + mTimeToFirstFix);

        // notify status listeners
        //synchronized(mListeners)
        {
            AutoLock lock(mListenersMutex);
            Int32 size = mListeners.GetSize();
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<Listener> listener = mListeners[i];
                //try {
                    listener->mListener->OnFirstFix(mTimeToFirstFix);
                //} catch (RemoteException e) {
//                    Log.w(TAG, "RemoteException in stopNavigating");
                //    mListeners->Remove(listener);
                    // adjust for size of list changing
                //    size--;
                //}
            }
        }
    }

    if (mStarted && mStatus != ILocationProvider::AVAILABLE) {
        // we want to time out if we do not receive a fix
        // within the time out and we are requesting infrequent fixes
        if (!HasCapability(GPS_CAPABILITY_SCHEDULING) && mFixInterval < NO_FIX_TIMEOUT) {
//            mAlarmManager->Cancel(mTimeoutIntent);
        }

        // send an intent to notify that the GPS is receiving fixes.
        //Intent intent = new Intent(LocationManager.GPS_FIX_CHANGE_ACTION);
        AutoPtr<IIntent> intent;
        CIntent::New(ILocationManager::GPS_FIX_CHANGE_ACTION, (IIntent**)&intent);
        intent->PutBooleanExtra(ILocationManager::EXTRA_GPS_ENABLED, TRUE);

        AutoPtr<IUserHandleHelper> userHelper;
        AutoPtr<IUserHandle> ALL;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHelper);
        userHelper->GetALL((IUserHandle**)&ALL);
        mContext->SendBroadcastAsUser(intent, ALL);
        UpdateStatus(ILocationProvider::AVAILABLE, mSvCount);
    }

   if (!HasCapability(GPS_CAPABILITY_SCHEDULING) && mStarted &&
           mFixInterval > GPS_POLLING_THRESHOLD_INTERVAL) {
//        if (DEBUG) Log.d(TAG, "got fix, hibernating");
        Hibernate();
    }
}

/**
 * called from native code to update our status
 */
void GpsLocationProvider::ReportStatus(
    /* [in] */ Int32 status)
{
//    if (DEBUG) Log.v(TAG, "reportStatus status: " + status);

    //synchronized(mListeners)
    {
        AutoLock lock(mListenersMutex);

        Boolean wasNavigating = mNavigating;

        switch (status) {
            case GPS_STATUS_SESSION_BEGIN:
                mNavigating = TRUE;
                mEngineOn = TRUE;
                break;
            case GPS_STATUS_SESSION_END:
                mNavigating = FALSE;
                break;
            case GPS_STATUS_ENGINE_ON:
                mEngineOn = TRUE;
                break;
            case GPS_STATUS_ENGINE_OFF:
                mEngineOn = FALSE;
                mNavigating = FALSE;
                break;
        }

        if (wasNavigating != mNavigating) {
            Int32 size = mListeners.GetSize();
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<Listener> listener = mListeners[i];
                //try {
                    if (mNavigating) {
                        listener->mListener->OnGpsStarted();
                    } else {
                        listener->mListener->OnGpsStopped();
                    }
                //} catch (RemoteException e) {
                //    Log.w(TAG, "RemoteException in reportStatus");
                //    mListeners.remove(listener);
                    // adjust for size of list changing
                //    size--;
                //}
            }

            // send an intent to notify that the GPS has been enabled or disabled.
            //Intent intent = new Intent(LocationManager.GPS_ENABLED_CHANGE_ACTION);
            AutoPtr<IIntent> intent;
            CIntent::New(ILocationManager::GPS_ENABLED_CHANGE_ACTION, (IIntent**)&intent);
            intent->PutBooleanExtra(ILocationManager::EXTRA_GPS_ENABLED, mNavigating);

            AutoPtr<IUserHandleHelper> userHelper;
            AutoPtr<IUserHandle> ALL;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHelper);
            userHelper->GetALL((IUserHandle**)&ALL);
            mContext->SendBroadcastAsUser(intent, ALL);
        }
    }
}

/**
 * called from native code to update SV info
 */
void GpsLocationProvider::ReportSvStatus()
{
    Int32 svCount = Native_read_sv_status(mSvs, mSnrs, mSvElevations, mSvAzimuths, mSvMasks);

    //synchronized(mListeners)
    {
        AutoLock lock(mListenersMutex);

        Int32 size = mListeners.GetSize();
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<Listener> listener = mListeners[i];
            //try {
                listener->mListener->OnSvStatusChanged(svCount, mSvs, mSnrs,
                        mSvElevations, mSvAzimuths, (*(mSvMasks.Get()))[EPHEMERIS_MASK],
                        (*(mSvMasks.Get()))[ALMANAC_MASK], (*(mSvMasks.Get()))[USED_FOR_FIX_MASK]);
            //} catch (RemoteException e) {
            //    Log.w(TAG, "RemoteException in reportSvInfo");
            //    mListeners.remove(listener);
                // adjust for size of list changing
            //    size--;
            //}
        }
    }
/*
    if (VERBOSE) {
        Log.v(TAG, "SV count: " + svCount +
                " ephemerisMask: " + Integer.toHexString(mSvMasks[EPHEMERIS_MASK]) +
                " almanacMask: " + Integer.toHexString(mSvMasks[ALMANAC_MASK]));
        for (int i = 0; i < svCount; i++) {
            Log.v(TAG, "sv: " + mSvs[i] +
                    " snr: " + mSnrs[i]/10 +
                    " elev: " + mSvElevations[i] +
                    " azimuth: " + mSvAzimuths[i] +
                    ((mSvMasks[EPHEMERIS_MASK] & (1 << (mSvs[i] - 1))) == 0 ? "  " : " E") +
                    ((mSvMasks[ALMANAC_MASK] & (1 << (mSvs[i] - 1))) == 0 ? "  " : " A") +
                    ((mSvMasks[USED_FOR_FIX_MASK] & (1 << (mSvs[i] - 1))) == 0 ? "" : "U"));
        }
    }
*/
    // return number of sets used in fix instead of total
//    UpdateStatus(mStatus, Integer.bitCount(mSvMasks[USED_FOR_FIX_MASK]));

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    if (mNavigating && mStatus == ILocationProvider::AVAILABLE && mLastFixTime > 0
        && now - mLastFixTime > RECENT_FIX_TIMEOUT) {
        // send an intent to notify that the GPS is no longer receiving fixes.
        //Intent intent = new Intent(LocationManager.GPS_FIX_CHANGE_ACTION);
        AutoPtr<IIntent> intent;
        CIntent::New(ILocationManager::GPS_FIX_CHANGE_ACTION, (IIntent**)&intent);
        intent->PutBooleanExtra(ILocationManager::EXTRA_GPS_ENABLED, FALSE);

        AutoPtr<IUserHandleHelper> userHelper;
        AutoPtr<IUserHandle> ALL;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHelper);
        userHelper->GetALL((IUserHandle**)&ALL);
        mContext->SendBroadcastAsUser(intent, ALL);
        UpdateStatus(ILocationProvider::TEMPORARILY_UNAVAILABLE, mSvCount);
    }
}

/**
 * called from native code to update AGPS status
 */
void GpsLocationProvider::ReportAGpsStatus(
    /* [in] */ Int32 type,
    /* [in] */ Int32 status,
    /* [in] */ Int32 ipaddr)
{
    switch (status) {
        case GPS_REQUEST_AGPS_DATA_CONN:
//            if (DEBUG) Log.d(TAG, "GPS_REQUEST_AGPS_DATA_CONN");
            // Set mAGpsDataConnectionState before calling startUsingNetworkFeature
            //  to avoid a race condition with handleUpdateNetworkState()
            mAGpsDataConnectionState = AGPS_DATA_CONNECTION_OPENING;
            Int32 result;
//            mConnMgr->StartUsingNetworkFeature(
//                    IConnectivityManager::TYPE_MOBILE, IPhone::FEATURE_ENABLE_SUPL, &result);
            mAGpsDataConnectionIpAddr = ipaddr;
            if (/*result == IPhoneConstants::APN_ALREADY_ACTIVE*/0) {
//                if (DEBUG) Log.d(TAG, "PhoneConstants.APN_ALREADY_ACTIVE");
                if (mAGpsApn != NULL) {
//                    Log.d(TAG, "mAGpsDataConnectionIpAddr " + mAGpsDataConnectionIpAddr);
                    if (mAGpsDataConnectionIpAddr != 0xffffffff) {
                        Boolean route_result;
//                        if (DEBUG) Log.d(TAG, "call requestRouteToHost");
//                        mConnMgr->RequestRouteToHost(
//                            IConnectivityManager::TYPE_MOBILE_SUPL,
//                            mAGpsDataConnectionIpAddr, &route_result);
//                        if (route_result == false) Log.d(TAG, "call requestRouteToHost failed");
                    }
                    Native_agps_data_conn_open(mAGpsApn);
                    mAGpsDataConnectionState = AGPS_DATA_CONNECTION_OPEN;
                } else {
//                    Log.e(TAG, "mAGpsApn not set when receiving PhoneConstants.APN_ALREADY_ACTIVE");
                    mAGpsDataConnectionState = AGPS_DATA_CONNECTION_CLOSED;
                    Native_agps_data_conn_failed();
                }
            }/* else if (result == IPhoneConstants::APN_REQUEST_STARTED) {
//                if (DEBUG) Log.d(TAG, "PhoneConstants.APN_REQUEST_STARTED");
                // Nothing to do here
            }*/ else {
//                if (DEBUG) Log.d(TAG, "startUsingNetworkFeature failed");
                mAGpsDataConnectionState = AGPS_DATA_CONNECTION_CLOSED;
                Native_agps_data_conn_failed();
            }
            break;
        case GPS_RELEASE_AGPS_DATA_CONN:
//            if (DEBUG) Log.d(TAG, "GPS_RELEASE_AGPS_DATA_CONN");
            if (mAGpsDataConnectionState != AGPS_DATA_CONNECTION_CLOSED) {
//                mConnMgr->StopUsingNetworkFeature(
//                        IConnectivityManager::TYPE_MOBILE, Phone.FEATURE_ENABLE_SUPL);
                Native_agps_data_conn_closed();
                mAGpsDataConnectionState = AGPS_DATA_CONNECTION_CLOSED;
            }
            break;
        case GPS_AGPS_DATA_CONNECTED:
//            if (DEBUG) Log.d(TAG, "GPS_AGPS_DATA_CONNECTED");
            break;
        case GPS_AGPS_DATA_CONN_DONE:
//            if (DEBUG) Log.d(TAG, "GPS_AGPS_DATA_CONN_DONE");
            break;
        case GPS_AGPS_DATA_CONN_FAILED:
//            if (DEBUG) Log.d(TAG, "GPS_AGPS_DATA_CONN_FAILED");
            break;
    }
}

/**
 * called from native code to report NMEA data received
 */
void GpsLocationProvider::ReportNmea(
    /* [in] */ Int64 timestamp)
{
    //synchronized(mListeners)
    {
        AutoLock lock(mListenersMutex);
        Int32 size = mListeners.GetSize();
        if (size > 0) {
            // don't bother creating the String if we have no listeners
            Int32 length = Native_read_nmea(mNmeaBuffer, mNmeaBuffer->GetLength());
            String nmea((char const*)(mNmeaBuffer->GetPayload()), length);// = new String(mNmeaBuffer, 0, length);

            for (Int32 i = 0; i < size; i++) {
                AutoPtr<Listener> listener = mListeners[i];
                //try {
                    listener->mListener->OnNmeaReceived(timestamp, nmea);
                //} catch (RemoteException e) {
                //    Log.w(TAG, "RemoteException in reportNmea");
                //    mListeners.remove(listener);
                    // adjust for size of list changing
                //    size--;
                //}
            }
        }
    }
}

/**
 * called from native code to inform us what the GPS engine capabilities are
 */
void GpsLocationProvider::SetEngineCapabilities(
    /* [in] */ Int32 capabilities)
{
    mEngineCapabilities = capabilities;

    if (!HasCapability(GPS_CAPABILITY_ON_DEMAND_TIME) && !mPeriodicTimeInjection) {
        mPeriodicTimeInjection = TRUE;
        RequestUtcTime();
    }
}

/**
 * called from native code to request XTRA data
 */
void GpsLocationProvider::XtraDownloadRequest()
{
//    if (DEBUG) Log.d(TAG, "xtraDownloadRequest");
//    SendMessage(DOWNLOAD_XTRA_DATA, 0, null);
}


/**
 * Called from native code to request set id info.
 * We should be careful about receiving null string from the TelephonyManager,
 * because sending null String to JNI function would cause a crash.
 */

void GpsLocationProvider::RequestSetID(
    /* [in] */ Int32 flags)
{
//    AutoPtr<ITelephonyManager> phone;
//    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&phone);
    Int32    type = AGPS_SETID_TYPE_NONE;
    String data("");

    if ((flags & AGPS_RIL_REQUEST_SETID_IMSI) == AGPS_RIL_REQUEST_SETID_IMSI) {
        String data_temp;
//        phone->GetSubscriberId(&data_temp);
        if (data_temp.GetLength() == 0) {
            // This means the framework does not have the SIM card ready.
        } else {
            // This means the framework has the SIM card.
            data = data_temp;
            type = AGPS_SETID_TYPE_IMSI;
        }
    }
    else if ((flags & AGPS_RIL_REQUEST_SETID_MSISDN) == AGPS_RIL_REQUEST_SETID_MSISDN) {
        String data_temp;
//        phone->GetLine1Number(&data_temp);
        if (data_temp.GetLength() == 0) {
            // This means the framework does not have the SIM card ready.
        } else {
            // This means the framework has the SIM card.
            data = data_temp;
            type = AGPS_SETID_TYPE_MSISDN;
        }
    }
    Native_agps_set_id(type, data);
}

/**
 * Called from native code to request utc time info
 */

void GpsLocationProvider::RequestUtcTime()
{
//    SendMessage(INJECT_NTP_TIME, 0, null);
}

/**
 * Called from native code to request reference location info
 */

void GpsLocationProvider::RequestRefLocation(
    /* [in] */ Int32 flags)
{
/*
    AutoPtr<ITelephonyManager> phone;
    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (ITelephonyManager**)&phone);
    Int32 phoneType;
    phone->GetPhoneType(&phoneType);
    if (phoneType == TelephonyManager.PHONE_TYPE_GSM) {
        GsmCellLocation gsm_cell = (GsmCellLocation) phone.getCellLocation();
        if ((gsm_cell != null) && (phone.getPhoneType() == TelephonyManager.PHONE_TYPE_GSM) &&
                (phone.getNetworkOperator() != null) &&
                    (phone.getNetworkOperator().length() > 3)) {
            int type;
            int mcc = Integer.parseInt(phone.getNetworkOperator().substring(0,3));
            int mnc = Integer.parseInt(phone.getNetworkOperator().substring(3));
            int networkType = phone.getNetworkType();
            if (networkType == TelephonyManager.NETWORK_TYPE_UMTS
                || networkType == TelephonyManager.NETWORK_TYPE_HSDPA
                || networkType == TelephonyManager.NETWORK_TYPE_HSUPA
                || networkType == TelephonyManager.NETWORK_TYPE_HSPA) {
                type = AGPS_REF_LOCATION_TYPE_UMTS_CELLID;
            } else {
                type = AGPS_REF_LOCATION_TYPE_GSM_CELLID;
            }
            native_agps_set_ref_location_cellid(type, mcc, mnc,
                    gsm_cell.getLac(), gsm_cell.getCid());
        } else {
            Log.e(TAG,"Error getting cell location info.");
        }
    }
    else {
        Log.e(TAG,"CDMA not supported.");
    }
*/
}

void GpsLocationProvider::SendMessage(
    /* [in] */ Int32 message,
    /* [in] */ Int32 arg,
    /* [in] */ IInterface* obj)
{}

String GpsLocationProvider::GetSelectedApn()
{
    AutoPtr<IUri> uri;// = CUri::Parse("content://telephony/carriers/preferapn");
    String apn("");

    AutoPtr<ICursor> cursor;
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
//    contentResolver->Query(uri, String("apn"),
//            NULL, NULL, ICarriers::DEFAULT_SORT_ORDER, (ICursor**)&cursor);

    if (NULL != cursor) {
        //try {
            Boolean bFlag = FALSE;
            if (cursor->MoveToFirst(&bFlag), bFlag) {
                cursor->GetString(0, &apn);
            }
        //} finally {
        //    cursor.close();
        //}
    }

    return apn;
}

//static { class_init_native(); }
void GpsLocationProvider::Class_init_native()
{
}

Boolean GpsLocationProvider::Native_is_supported()
{
    return FALSE;//temp
}

Boolean GpsLocationProvider::Native_init()
{
    return FALSE;//temp
}

void GpsLocationProvider::Native_cleanup()
{
}

Boolean GpsLocationProvider::Native_set_position_mode(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 recurrence,
    /* [in] */ Int32 min_interval,
    /* [in] */ Int32 preferred_accuracy,
    /* [in] */ Int32 preferred_time)
{
    return FALSE;//temp
}

Boolean GpsLocationProvider::Native_start()
{
    return FALSE;//temp
}

Boolean GpsLocationProvider::Native_stop()
{
    return FALSE;//temp
}

void GpsLocationProvider::Native_delete_aiding_data(
    /* [in] */ Int32 flags)
{
}

// returns number of SVs
// mask[0] is ephemeris mask and mask[1] is almanac mask
Int32 GpsLocationProvider::Native_read_sv_status(
    /* [in] */ ArrayOf<Int32>* svs,
    /* [in] */ ArrayOf<Float>* snrs,
    /* [in] */ ArrayOf<Float>* elevations,
    /* [in] */ ArrayOf<Float>* azimuths,
    /* [in] */ ArrayOf<Int32>* masks)
{
    return -1;//temp
}

Int32 GpsLocationProvider::Native_read_nmea(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 bufferSize)
{
    return -1;//temp
}

void GpsLocationProvider::Native_inject_location(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Float accuracy)
{
}

// XTRA Support
void GpsLocationProvider::Native_inject_time(
    /* [in] */ Int64 time,
    /* [in] */ Int64 timeReference,
    /* [in] */ Int32 uncertaInt32y)
{
}

Boolean GpsLocationProvider::Native_supports_xtra()
{
    return FALSE;//temp
}

void GpsLocationProvider::Native_inject_xtra_data(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length)
{
}

// DEBUG Support
String GpsLocationProvider::Native_get_internal_state()
{
    return String("");//temp
}

// AGPS Support
void GpsLocationProvider::Native_agps_data_conn_open(
    /* [in] */ const String& apn)
{
}

void GpsLocationProvider::Native_agps_data_conn_closed()
{
}

void GpsLocationProvider::Native_agps_data_conn_failed()
{
}

void GpsLocationProvider::Native_agps_ni_message(
    /* [in] */ ArrayOf<Byte>* msg,
    /* [in] */ Int32 length)
{
}

void GpsLocationProvider::Native_set_agps_server(
    /* [in] */ Int32 type,
    /* [in] */ const String& hostname,
    /* [in] */ Int32 port)
{
}

// Network-initiated (NI) Support
void GpsLocationProvider::Native_send_ni_response(
    /* [in] */ Int32 notificationId,
    /* [in] */ Int32 userResponse)
{
}

// AGPS ril suport
void GpsLocationProvider::Native_agps_set_ref_location_cellid(
    /* [in] */ Int32 type,
    /* [in] */ Int32 mcc,
    /* [in] */ Int32 mnc,
    /* [in] */ Int32 lac,
    /* [in] */ Int32 cid)
{
}

void GpsLocationProvider::Native_agps_set_id(
    /* [in] */ Int32 type,
    /* [in] */ const String& setid)
{
}

void GpsLocationProvider::Native_update_network_state(
    /* [in] */ Boolean connected,
    /* [in] */ Int32 type,
    /* [in] */ Boolean roaming,
    /* [in] */ Boolean available,
    /* [in] */ const String& extraInfo,
    /* [in] */ const String& defaultAPN)
{
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
