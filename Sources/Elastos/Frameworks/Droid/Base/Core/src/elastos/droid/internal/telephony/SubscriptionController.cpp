
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/internal/telephony/SubscriptionController.h"
#include "elastos/droid/internal/telephony/SubscriptionHelper.h"
#include "elastos/droid/internal/telephony/CSubscriptionController.h"
#include "elastos/droid/internal/telephony/CallManager.h"
#include "elastos/droid/internal/telephony/MccTable.h"
#include "elastos/droid/internal/telephony/ModemStackController.h"
#include "elastos/droid/internal/telephony/dataconnection/CDdsSchedulerHelper.h"
#include "elastos/droid/internal/telephony/dataconnection/CDdsSchedulerAc.h"
#include "elastos/droid/internal/telephony/dataconnection/CDctControllerHelper.h"
#include "elastos/droid/internal/telephony/uicc/CSpnOverride.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/provider/CSettingsGlobal.h"
#include "elastos/droid/preference/CPreferenceManagerHelper.h"
#include "elastos/droid/text/format/CTime.h"
#include "elastos/droid/telephony/CSubscriptionManager.h"
#include "elastos/droid/telephony/CSubInfoRecord.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Droid::Internal::Telephony::Uicc::CSpnOverride;
using Elastos::Droid::Internal::Telephony::Uicc::ISpnOverride;
using Elastos::Droid::Internal::Telephony::DataConnection::IDdsSchedulerHelper;
using Elastos::Droid::Internal::Telephony::DataConnection::CDdsSchedulerHelper;
using Elastos::Droid::Internal::Telephony::DataConnection::CDdsSchedulerAc;
using Elastos::Droid::Internal::Telephony::DataConnection::IDctControllerHelper;
using Elastos::Droid::Internal::Telephony::DataConnection::CDctControllerHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Telephony::CSubInfoRecord;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Manifest;
using Elastos::Droid::R;

using Elastos::Core::AutoLock;
using Elastos::Core::CInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IStackTraceElement;
using Elastos::Core::StringUtils;
using Elastos::IO::IFlushable;
using Elastos::IO::ICloseable;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::IListIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  SubscriptionController::ScLocalLog::
//==============================================================

SubscriptionController::ScLocalLog::ScLocalLog(
    /* [in] */ Int32 maxLines)
{
    CLinkedList::New((ILinkedList**)&mLog);
    mMaxLines = maxLines;
    CTime::New((ITime**)&mNow);
}

void SubscriptionController::ScLocalLog::Log(
    /* [in] */ const String& msg)
{
    if (mMaxLines > 0) {
        Int32 pid = Process::MyPid();
        Int32 tid = Process::MyTid();
        mNow->SetToNow();
        String str;
        mNow->Format(String("%m-%d %H:%M:%S"), &str);
        StringBuilder sb(str);
        sb += " pid=";
        sb += pid;
        sb += " tid=";
        sb += tid;
        sb += " ";
        sb += msg;
        AutoPtr<ICharSequence> cs;
        CString::New(sb.ToString(), (ICharSequence**)&cs);
        mLog->Add(cs);
        str += msg;
        mLog->Add(CoreUtils::Convert(str));
        Int32 size = 0;
        while ((mLog->GetSize(&size), size) > mMaxLines) {
            AutoPtr<IInterface> p;
            mLog->Remove((IInterface**)&p);
        }
    }
}

void SubscriptionController::ScLocalLog::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Int32 LOOPS_PER_FLUSH = 10; // Flush every N loops.
    AutoPtr<IListIterator> itr;
    mLog->GetListIterator(0, (IListIterator**)&itr);
    Int32 i = 0;
    Boolean bHasNext = FALSE;
    while ((IIterator::Probe(itr)->HasNext(&bHasNext), bHasNext)) {
        String str(StringUtils::ToString(i++));
        str += ": ";
        AutoPtr<IInterface> p;
        IIterator::Probe(itr)->GetNext((IInterface**)&p);
        str += TO_CSTR(p);
        pw->Println(str);
        // Flush periodically so we don't drop lines
        if ((i % LOOPS_PER_FLUSH) == 0) {
            IFlushable::Probe(pw)->Flush();
        }
    }
}

//==============================================================
//  SubscriptionController::DataConnectionHandler::
//==============================================================
SubscriptionController::DataConnectionHandler::DataConnectionHandler(
    /* [in] */ SubscriptionController* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode SubscriptionController::DataConnectionHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case 1: {       // SubscriptionController::EVENT_SET_DEFAULT_DATA_DONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> ar = (AsyncResult*)IAsyncResult::Probe(obj);
            String str("EVENT_SET_DEFAULT_DATA_DONE subId:");
            str += TO_CSTR(ar->mResult);
            mHost->Logd(str);
            mHost->UpdateDataSubId(ar);
            break;
        }
    }
    return NOERROR;
}

//==============================================================
//  SubscriptionController::MyHandler::
//==============================================================
SubscriptionController::MyHandler::MyHandler(
    /* [in] */ SubscriptionController* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode SubscriptionController::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;

    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case 1: {           // SubscriptionController::EVENT_WRITE_MSISDN_DONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            {
                AutoLock syncLock(mHost->mLock);
                mHost->mSuccess = (ar->mException == NULL);
                // String str("EVENT_WRITE_MSISDN_DONE, mSuccess = ");
                // str += mHost->mSuccess;
                // mHost->Logd(str);
                mHost->mLock.NotifyAll();
            }
            break;
        }
    }
    return NOERROR;
}

//==============================================================
//  SubscriptionController::
//==============================================================
const String SubscriptionController::LOGTAG("SubscriptionController");
const Boolean SubscriptionController::DBG = TRUE;
const Boolean SubscriptionController::VDBG = TRUE;
const Int32 SubscriptionController::MAX_LOCAL_LOG_LINES = 500; // TODO: Reduce to 100 when 17678050 is fixed

AutoPtr<ISubscriptionController> SubscriptionController::sInstance;
AutoPtr<ArrayOf<IPhoneProxy*> > SubscriptionController::sProxyPhones;

const Int32 SubscriptionController::RES_TYPE_BACKGROUND_DARK = 0;
const Int32 SubscriptionController::RES_TYPE_BACKGROUND_LIGHT = 1;

const AutoPtr<ArrayOf<Int32> > SubscriptionController::sSimBackgroundDarkRes = SetSimResource(RES_TYPE_BACKGROUND_DARK);
const AutoPtr<ArrayOf<Int32> > SubscriptionController::sSimBackgroundLightRes = SetSimResource(RES_TYPE_BACKGROUND_LIGHT);

static AutoPtr<IHashMap> InitInfo()
{
    AutoPtr<IHashMap> p;
    CHashMap::New((IHashMap**)&p);
    return p;
}
AutoPtr<IHashMap> SubscriptionController::mSimInfo = InitInfo();
Int64 SubscriptionController::mDefaultVoiceSubId = ISubscriptionManager::DEFAULT_SUB_ID;
Int32 SubscriptionController::mDefaultPhoneId = 0;

const Int32 SubscriptionController::EVENT_WRITE_MSISDN_DONE = 1;

const Int32 SubscriptionController::EVENT_SET_DEFAULT_DATA_DONE = 1;

const Int32 SubscriptionController::DUMMY_SUB_ID = -1;
Object SubscriptionController::sLock;

CAR_INTERFACE_IMPL_3(SubscriptionController, Object, ISubscriptionController, IISub, IBinder)

SubscriptionController::SubscriptionController()
{
    mLocalLog = new ScLocalLog(MAX_LOCAL_LOG_LINES);

    mSuccess = FALSE;

    mHandler = new MyHandler(this);

    CHashMap::New((IHashMap**)&mOnDemandDdsLockNotificationRegistrants);
}

AutoPtr<ISubscriptionController> SubscriptionController::Init(
    /* [in] */ IPhone* phone)
{
    AutoLock syncLock(sLock);
    if (sInstance == NULL) {
        CSubscriptionController::New(phone, (ISubscriptionController**)&sInstance);
    }
    else {
        Logger::D(LOGTAG, "Init() called multiple times!  sInstance = %p", sInstance.Get());
    }
    return sInstance;
}

AutoPtr<ISubscriptionController> SubscriptionController::Init(
    /* [in] */ IContext* c,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci)
{
    AutoLock syncLock(sLock);
    if (sInstance == NULL) {
        CSubscriptionController::New(c, (ISubscriptionController**)&sInstance);
    }
    else {
        Logger::D(LOGTAG, "Init() called multiple times!  sInstance = %p", sInstance.Get());
    }
    return sInstance;
}

AutoPtr<ISubscriptionController> SubscriptionController::GetInstance()
{
    if (sInstance == NULL) {
       Logger::D(LOGTAG, "getInstance NULL");
    }

    return sInstance;
}

ECode SubscriptionController::constructor(
    /* [in] */ IContext* c)
{
    Logd(String("SubscriptionController init by Context"));
    mContext = c;
    mCM = CallManager::GetInstance();

    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> pServ;
    sm->GetService(String("isub"), (IInterface**)&pServ);
    if (pServ == NULL) {
        sm->AddService(String("isub"), IISub::Probe(this));
    }

    Logdl(String("[SubscriptionController] dl init by Context"));
    mDataConnectionHandler = new DataConnectionHandler(this);

    AutoPtr<IDdsSchedulerHelper> dshlp;
    CDdsSchedulerHelper::AcquireSingleton((IDdsSchedulerHelper**)&dshlp);
    dshlp->GetInstance((IDdsScheduler**)&mScheduler);

    CDdsSchedulerAc::New((IDdsSchedulerAc**)&mSchedulerAc);
    AutoPtr<IHandler> h;
    IStateMachine::Probe(mScheduler)->GetHandler((IHandler**)&h);
    IAsyncChannel::Probe(mSchedulerAc)->Connect(mContext, mDataConnectionHandler, h);
    return NOERROR;
}

ECode SubscriptionController::constructor(
    /* [in] */ IPhone* phone)
{
    phone->GetContext((IContext**)&mContext);
    mCM = CallManager::GetInstance();

    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> pServ;
    sm->GetService(String("isub"), (IInterface**)&pServ);
    if (pServ == NULL) {
        sm->AddService(String("isub"), IISub::Probe(this));
    }

    Logdl(String("[SubscriptionController] init by Phone"));
    return NOERROR;
}

ECode SubscriptionController::GetSubIdFromNetworkRequest(
    /* [in] */ INetworkRequest* n,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 subId = 0;
    if (n == NULL) {
        return GetDefaultDataSubId(result);
    }

    AutoPtr<INetworkCapabilities> netcap;
    n->GetNetworkCapabilities((INetworkCapabilities**)&netcap);
    String str;
    netcap->GetNetworkSpecifier(&str);
    // try {
        subId = StringUtils::ParseInt64(str);
    // } Catch(NumberFormatException e) {
    //     Loge("Exception e = " + e);
    //     subId = GetDefaultDataSubId();
    // }

    *result = subId;
    return NOERROR;
}

ECode SubscriptionController::StartOnDemandDataSubscriptionRequest(
    /* [in] */ INetworkRequest* n)
{
    String str("startOnDemandDataSubscriptionRequest = ");
    str += TO_CSTR(n);
    Logd(str);
    mSchedulerAc->AllocateDds(n);
    return NOERROR;
}

ECode SubscriptionController::StopOnDemandDataSubscriptionRequest(
    /* [in] */ INetworkRequest* n)
{
    String str("stopOnDemandDataSubscriptionRequest = ");
    str += TO_CSTR(n);
    Logd(str);
    mSchedulerAc->FreeDds(n);
    return NOERROR;
}

Boolean SubscriptionController::IsSubInfoReady()
{
    Int32 size = 0;
    mSimInfo->GetSize(&size);
    return size > 0;
}

void SubscriptionController::EnforceSubscriptionPermission()
{
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::READ_PHONE_STATE,
            String("Requires READ_PHONE_STATE"));
}

void SubscriptionController::BroadcastSimInfoContentChanged(
    /* [in] */ Int64 subId,
    /* [in] */ const String& columnName,
    /* [in] */ Int32 intContent,
    /* [in] */ const String& stringContent)
{
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_SUBINFO_CONTENT_CHANGE, (IIntent**)&intent);
    intent->PutExtra(IBaseColumns::ID, subId);
    intent->PutExtra(ITelephonyIntents::EXTRA_COLUMN_NAME, columnName);
    intent->PutExtra(ITelephonyIntents::EXTRA_INT_CONTENT, intContent);
    intent->PutExtra(ITelephonyIntents::EXTRA_STRING_CONTENT, stringContent);
    if (intContent != ISubscriptionManager::DEFAULT_INT_VALUE) {
        // String str("[broadcastSimInfoContentChanged] subId");
        // str += subId;
        // str += " changed, ";
        // str += columnName;
        // str += " -> ";
        // str += intContent;
        // Logd(str);
    }
    else {
        // String str("[broadcastSimInfoContentChanged] subId");
        // str += subId;
        // str += " changed, ";
        // str += columnName;
        // str += " -> ";
        // str += stringContent;
        // Logd(str);
    }
    mContext->SendBroadcast(intent);
}

AutoPtr<ISubInfoRecord> SubscriptionController::GetSubInfoRecord(
    /* [in] */ ICursor* cursor)
{
    AutoPtr<ISubInfoRecord> info;
    CSubInfoRecord::New((ISubInfoRecord**)&info);
    AutoPtr<CSubInfoRecord> _info = (CSubInfoRecord*)info.Get();
    Int32 subId = 0;
    cursor->GetColumnIndexOrThrow(IBaseColumns::ID, &subId);
    cursor->GetInt64(subId, &(_info->mSubId));
    Int32 iccId = 0;
    cursor->GetColumnIndexOrThrow(
            ISubscriptionManager::ICC_ID, &iccId);
    cursor->GetString(iccId, &(_info->mIccId));
    Int32 slotId = 0;
    cursor->GetColumnIndexOrThrow(
            ISubscriptionManager::SIM_ID, &slotId);
    cursor->GetInt32(slotId, &(_info->mSlotId));
    Int32 displayName = 0;
    cursor->GetColumnIndexOrThrow(
            ISubscriptionManager::DISPLAY_NAME, &displayName);
    cursor->GetString(displayName, &(_info->mDisplayName));
    Int32 nameSource = 0;
    cursor->GetColumnIndexOrThrow(
            ISubscriptionManager::NAME_SOURCE, &nameSource);
    cursor->GetInt32(nameSource, &(_info->mNameSource));
    Int32 color = 0;
    cursor->GetColumnIndexOrThrow(
            ISubscriptionManager::COLOR, &color);
    cursor->GetInt32(color, &(_info->mColor));
    Int32 number = 0;
    cursor->GetColumnIndexOrThrow(
            ISubscriptionManager::NUMBER, &number);
    cursor->GetString(number, &(_info->mNumber));
    Int32 displayNumberFormat = 0;
    cursor->GetColumnIndexOrThrow(
            ISubscriptionManager::DISPLAY_NUMBER_FORMAT, &displayNumberFormat);
    cursor->GetInt32(displayNumberFormat, &(_info->mDisplayNumberFormat));
    Int32 dataRoaming = 0;
    cursor->GetColumnIndexOrThrow(
            ISubscriptionManager::DATA_ROAMING, &dataRoaming);
    cursor->GetInt32(dataRoaming, &(_info->mDataRoaming));

    Int32 size = sSimBackgroundDarkRes->GetLength();
    if (_info->mColor >= 0 && _info->mColor < size) {
        AutoPtr<ArrayOf<Int32> > iconRes = _info->mSimIconRes;
        (*iconRes)[RES_TYPE_BACKGROUND_DARK] = (*sSimBackgroundDarkRes)[_info->mColor];
        (*iconRes)[RES_TYPE_BACKGROUND_LIGHT] = (*sSimBackgroundLightRes)[_info->mColor];
    }
    Int32 mcc = 0;
    cursor->GetColumnIndexOrThrow(
            ISubscriptionManager::MCC, &mcc);
    cursor->GetInt32(mcc, &(_info->mMcc));
    Int32 mnc = 0;
    cursor->GetColumnIndexOrThrow(
            ISubscriptionManager::MNC, &mnc);
    cursor->GetInt32(mnc, &(_info->mMnc));
    Int32 mStatus = 0;
    cursor->GetColumnIndexOrThrow(
            ISubscriptionManager::SUB_STATE, &mStatus);
    cursor->GetInt32(mStatus, &(_info->mStatus));
    Int32 mNwMode = 0;
    cursor->GetColumnIndexOrThrow(
            ISubscriptionManager::NETWORK_MODE, &mNwMode);
    cursor->GetInt32(mNwMode, &(_info->mNwMode));

    StringBuilder str("[getSubInfoRecord] SubId:");
    str += _info->mSubId;
    str += " iccid:";
    str += _info->mIccId;
    str += " slotId:";
    str += _info->mSlotId;
    str += " displayName:";
    str += _info->mDisplayName;
    str += " color:";
    str += _info->mColor;
    str += " mcc/mnc:";
    str += _info->mMcc;
    str += "/";
    str += _info->mMnc;
    str += " subStatus: ";
    str += _info->mStatus;
    str += " Nwmode: ";
    str += _info->mNwMode;
    Logd(str.ToString());

    return info;
}

AutoPtr<IList> SubscriptionController::GetSubInfo(
    /* [in] */ const String& selection,
    /* [in] */ IInterface* queryKey)
{
    String str("selection:");
    str += selection;
    str += " ";
    if (queryKey != NULL) {
        str += TO_CSTR(queryKey);
    }
    Logd(str);
    AutoPtr<ArrayOf<String> > selectionArgs;
    if (queryKey != NULL) {
        selectionArgs = ArrayOf<String>::Alloc(1);
        (*selectionArgs)[0] = TO_CSTR(queryKey);
    }
    AutoPtr<IArrayList> subList;
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    AutoPtr<IUri> content_uri;
    sm->GetCONTENT_URI((IUri**)&content_uri);
    AutoPtr<ICursor> cursor;
    cr->Query(content_uri,
            NULL, selection, selectionArgs, ISubscriptionManager::SIM_ID, (ICursor**)&cursor);

    if (cursor != NULL) {
        Boolean bMN = FALSE;
        while ((cursor->MoveToNext(&bMN), bMN)) {
            AutoPtr<ISubInfoRecord> subInfo = GetSubInfoRecord(cursor);
            AutoPtr<ISubscriptionManager> sm;
            CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
            AutoPtr<CSubInfoRecord> _subInfo = (CSubInfoRecord*)subInfo.Get();
            Boolean bValid = FALSE;
            if (subInfo != NULL && (sm->IsValidSlotId(_subInfo->mSlotId, &bValid), bValid)) {
                if (subList == NULL) {
                    CArrayList::New((IArrayList**)&subList);
                }
                subList->Add(subInfo);
            }
        }
    }
    else {
        Logd(String("Query fail"));
    }

    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }

    return IList::Probe(subList);
}

ECode SubscriptionController::GetSubInfoForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ ISubInfoRecord** result)
{
    VALIDATE_NOT_NULL(result)
    Logd(String("[getSubInfoForSubscriberx]+ subId:") + StringUtils::ToString(subId));
    EnforceSubscriptionPermission();

    if (subId == ISubscriptionManager::DEFAULT_SUB_ID) {
        GetDefaultSubId(&subId);
    }
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Boolean bValid = FALSE;
    sm->IsValidSubId(subId, &bValid);
    if (!bValid || !IsSubInfoReady()) {
        Logd(String("[getSubInfoForSubscriberx]- invalid subId or not ready, subId = ")
                + StringUtils::ToString(subId));
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = StringUtils::ToString(subId);
    StringBuilder sb(IBaseColumns::ID); sb += "=?";
    AutoPtr<IUri> content_uri;
    sm->GetCONTENT_URI((IUri**)&content_uri);
    AutoPtr<ICursor> cursor;
    cr->Query(content_uri,
            NULL, sb.ToString(), arr, String(NULL), (ICursor**)&cursor);

    if (cursor != NULL) {
        Boolean bMF = FALSE;
        cursor->MoveToFirst(&bMF);
        if (bMF) {
            Logd(String("[getSubInfoForSubscriberx]- Info detail:"));
            AutoPtr<ISubInfoRecord> res = GetSubInfoRecord(cursor);
            *result = res;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }

    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    Logd(String("[getSubInfoForSubscriber]- NULL info return"));
    *result = NULL;
    return NOERROR;
}

ECode SubscriptionController::GetSubInfoUsingIccId(
    /* [in] */ const String& iccId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    String str("[getSubInfoUsingIccId]+ iccId:");
    str += iccId;
    Logd(str);
    EnforceSubscriptionPermission();

    if (iccId == NULL || !IsSubInfoReady()) {
        Logd(String("[getSubInfoUsingIccId]- NULL iccid or not ready"));
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = iccId;
    StringBuilder sb(ISubscriptionManager::ICC_ID);
    sb += "=?";
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    AutoPtr<IUri> content_uri;
    sm->GetCONTENT_URI((IUri**)&content_uri);
    AutoPtr<ICursor> cursor;
    cr->Query(content_uri,
            NULL, sb.ToString(), arr, String(NULL), (ICursor**)&cursor);
    AutoPtr<IArrayList> subList;

    if (cursor != NULL) {
        Boolean bMN = FALSE;
        while ((cursor->MoveToNext(&bMN), bMN)) {
            AutoPtr<ISubInfoRecord> subInfo = GetSubInfoRecord(cursor);
            if (subInfo != NULL) {
                if (subList == NULL) {
                    CArrayList::New((IArrayList**)&subList);
                }
                subList->Add(subInfo);
            }
        }
    }
    else {
        Logd(String("Query fail"));
    }

    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }

    *result = IList::Probe(subList);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SubscriptionController::GetSubInfoUsingSlotId(
    /* [in] */ Int32 slotId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    return GetSubInfoUsingSlotIdWithCheck(slotId, TRUE, result);
}

ECode SubscriptionController::GetAllSubInfoList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    Logd(String("[getAllSubInfoList]+"));
    EnforceSubscriptionPermission();

    AutoPtr<IList> subList;
    subList = GetSubInfo(String(NULL), NULL);
    if (subList != NULL) {
        Int32 size = 0;
        subList->GetSize(&size);
        // String str("[getAllSubInfoList]- ");
        // str += size;
        // str += " infos return";
        // Logd(str);
    }
    else {
        Logd(String("[getAllSubInfoList]- no info return"));
    }

    *result = subList;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SubscriptionController::GetActiveSubInfoList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    EnforceSubscriptionPermission();
    Logdl(String("[getActiveSubInfoList]+"));

    AutoPtr<IList> subList;

    if (!IsSubInfoReady()) {
        Logdl(String("[getActiveSubInfoList] Sub Controller not ready"));
        *result = subList;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    StringBuilder sb(ISubscriptionManager::SIM_ID);
    sb += "!="; sb += ISubscriptionManager::INVALID_SLOT_ID;
    subList = GetSubInfo(sb.ToString(), NULL);
    if (subList != NULL) {
        Int32 size = 0;
        subList->GetSize(&size);
        // str = "[getActiveSubInfoList]- ";
        // str += size;
        // str += " infos return";
        // Logdl(str);
    }
    else {
        Logdl(String("[getActiveSubInfoList]- no info return"));
    }

    *result = subList;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SubscriptionController::GetActiveSubInfoCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Logd(String("[getActiveSubInfoCount]+"));
    AutoPtr<IList> records;
    GetActiveSubInfoList((IList**)&records);
    if (records == NULL) {
        Logd(String("[getActiveSubInfoCount] records NULL"));
        *result = 0;
        return NOERROR;
    }
    String str("[getActiveSubInfoCount]- count: ");
    Int32 size = 0;
    records->GetSize(&size);
    // str += size;
    // Logd(str);
    *result = size;
    return NOERROR;
}

ECode SubscriptionController::GetAllSubInfoCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Logd(String("[getAllSubInfoCount]+"));
    EnforceSubscriptionPermission();

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    AutoPtr<IUri> content_uri;
    sm->GetCONTENT_URI((IUri**)&content_uri);
    AutoPtr<ICursor> cursor;
    cr->Query(content_uri,
            NULL, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    if (cursor != NULL) {
        Int32 count = 0;
        cursor->GetCount(&count);
        // String str("[getAllSubInfoCount]- ");
        // str += count;
        // str += " SUB(s) in DB";
        // Logd(str);
        *result = count;
        return NOERROR;
    }
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    Logd(String("[getAllSubInfoCount]- no SUB in DB"));

    *result = 0;
    return NOERROR;
}

ECode SubscriptionController::AddSubInfoRecord(
    /* [in] */ const String& iccId,
    /* [in] */ Int32 slotId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    String str;
    StringBuilder sb1("[addSubInfoRecord]+ iccId:");
    sb1 += iccId;
    sb1 += " slotId:";
    sb1 += StringUtils::ToString(slotId);
    Logdl(sb1.ToString());
    EnforceSubscriptionPermission();

    if (iccId == NULL) {
        Logdl(String("[addSubInfoRecord]- NULL iccId"));
    }

    String nameToSet;
    if (IsSubInfoReady()) { // we can only get a subId if SubInfo is ready
        AutoPtr<ArrayOf<Int64> > subIds;
        GetSubId(slotId, (ArrayOf<Int64>**)&subIds);
        if (subIds == NULL || subIds->GetLength() == 0) {
            Logdl(String("[addSubInfoRecord]- getSubId fail"));
            *result = 0;
            return NOERROR;
        }

        Int32 subIdsIndex = 0;
        if (subIds->GetLength() > 1) { // one SIM can have more than one subId
            for (Int32 i = 0; i < subIds->GetLength(); i++) {
                StringBuilder sb("[addSubInfoRecord] inspecting subIds[");
                sb += i;
                sb += "]: ";
                sb += (*subIds)[i];
                Logdl(sb.ToString());
                AutoPtr<ITelephonyManagerHelper> tmhlp;
                CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmhlp);
                AutoPtr<ITelephonyManager> tm;
                tmhlp->GetDefault((ITelephonyManager**)&tm);
                String name;
                tm->GetSimOperatorName((*subIds)[i], &name);
                if (!name.Equals("")) {
                    // We have a Carrier here, with a CarrierName and everything!
                    subIdsIndex = i;
                    sb.Reset();
                    sb += ("[addSubInfoRecord] using subIds[");
                    sb += i;
                    sb += "]: it has a Carrier";
                    Logdl(sb.ToString());
                }
            }
        }

        if ((*subIds)[subIdsIndex] == -1) { // DUMMY value from getSubId
            AutoPtr<IInterface> p;
            mSimInfo->Get(CoreUtils::Convert(slotId), (IInterface**)&p);
            if (p == NULL) {
                Logdl(String("[addSubInfoRecord] currentSubId is NULL, proceed with dummy value"));
            }
            else {
                Int64 currentSubId = 0;
                IInteger64::Probe(p)->GetValue(&currentSubId);
                StringBuilder sb("[addSubInfoRecord] currentSubId ");
                sb += currentSubId;
                sb += ", proceed with it";
                Logdl(sb.ToString());
                (*subIds)[subIdsIndex] = currentSubId;
            }
        }

        AutoPtr<ITelephonyManagerHelper> tmhlp;
        CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmhlp);
        AutoPtr<ITelephonyManager> tm;
        tmhlp->GetDefault((ITelephonyManager**)&tm);
        String Carrier;
        tm->GetSimOperator((*subIds)[subIdsIndex], &Carrier);
        str = String("[addSubInfoRecord] Carrier = ");
        str += Carrier;
        Logdl(str);
        String CarrierName;
        tm->GetSimOperatorName((*subIds)[subIdsIndex], &CarrierName);
        str = "[addSubInfoRecord] CarrierName = ";
        str += CarrierName;
        Logdl(str);
        AutoPtr<ISpnOverride> mSpnOverride;
        CSpnOverride::New((ISpnOverride**)&mSpnOverride);

        Boolean bContain = FALSE;
        mSpnOverride->ContainsCarrier(Carrier, &bContain);
        if (bContain) {
            mSpnOverride->GetSpn(Carrier, &nameToSet);
            str = "[addSubInfoRecord] Found, SpnOverride, name = ";
            str += nameToSet;
            Logdl(str);
        }
        else if (!CarrierName.Equals("")) {
            nameToSet = CarrierName;
            str = "[addSubInfoRecord] Found, name = ";
            str += nameToSet;
            Logdl(str);
        }
        else {
            nameToSet = "SIM ";
            nameToSet += StringUtils::ToString(slotId + 1);
            str = "[addSubInfoRecord] Not found, name = ";
            str += nameToSet;
            Logdl(str);
        }
    }
    else {
        nameToSet = "SIM ";
        nameToSet += StringUtils::ToString(slotId + 1);
        str = "[addSubInfoRecord] SubInfo not ready, name = ";
        str += nameToSet;
        Logdl(str);
    }

    AutoPtr<ArrayOf<String> > arr1 = ArrayOf<String>::Alloc(3);
    (*arr1)[0] = IBaseColumns::ID;
    (*arr1)[1] = ISubscriptionManager::SIM_ID;
    (*arr1)[2] = ISubscriptionManager::NAME_SOURCE;
    AutoPtr<ArrayOf<String> > arr2 = ArrayOf<String>::Alloc(1);
    (*arr2)[0] = iccId;
    str = ISubscriptionManager::ICC_ID;
    str += "=?";
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    AutoPtr<IUri> content_uri;
    sm->GetCONTENT_URI((IUri**)&content_uri);
    AutoPtr<ICursor> cursor;
    resolver->Query(content_uri,
            arr1, str,
            arr2, String(NULL),
            (ICursor**)&cursor);

    Boolean bMF = FALSE;
    if (cursor == NULL || !(cursor->MoveToFirst(&bMF), bMF)) {
        AutoPtr<IContentValues> value;
        CContentValues::New((IContentValues**)&value);
        value->Put(ISubscriptionManager::ICC_ID, iccId);
        // default SIM color differs between slots
        value->Put(ISubscriptionManager::COLOR, slotId);
        value->Put(ISubscriptionManager::SIM_ID, slotId);
        value->Put(ISubscriptionManager::DISPLAY_NAME, nameToSet);
        AutoPtr<IUri> uri;
        resolver->Insert(content_uri, value, (IUri**)&uri);
        str = "[addSubInfoRecord]- New record created: ";
        String uriStr;
        if (uri != NULL) {
            IObject::Probe(uri)->ToString(&uriStr);
        }
        str += uriStr;
        Logdl(str);
    }
    else {
        Int64 subId = 0;
        cursor->GetInt64(0, &subId);
        Int32 oldSimInfoId = 0;
        cursor->GetInt32(1, &oldSimInfoId);
        Int32 nameSource = 0;
        cursor->GetInt32(2, &nameSource);
        AutoPtr<IContentValues> value;
        CContentValues::New((IContentValues**)&value);

        if (slotId != oldSimInfoId) {
            value->Put(ISubscriptionManager::SIM_ID, slotId);
        }

        if (nameSource == ISubscriptionManager::NAME_SOURCE_UNDEFINDED) {
            value->Put(ISubscriptionManager::DISPLAY_NAME, nameToSet);
            str = "[addSubInfoRecord]- going to update SIM name to ";
            str += nameToSet;
            Logdl(str);
        }

        Int32 size = 0;
        value->GetSize(&size);
        if (size > 0) {
            StringBuilder sb(IBaseColumns::ID); sb += "="; sb += subId;
            Int32 res = 0;
            resolver->Update(content_uri, value,
                    sb.ToString(), NULL, &res);
        }

        Logdl(String("[addSubInfoRecord]- Record already exist"));
    }
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }

    StringBuilder sb(ISubscriptionManager::SIM_ID);
    sb += "=?";
    AutoPtr<ArrayOf<String> > arrVal = ArrayOf<String>::Alloc(1);
    (*arrVal)[0] = StringUtils::ToString(slotId);
    resolver->Query(content_uri, NULL,
            sb.ToString(), arrVal, String(NULL), (ICursor**)&cursor);

    Boolean bMN = FALSE;
    bMF = FALSE;
    if (cursor != NULL && (cursor->MoveToFirst(&bMF), bMF)) {
        do {
            Int32 id = 0;
            cursor->GetColumnIndexOrThrow(IBaseColumns::ID, &id);
            Int64 subId = 0;
            cursor->GetInt64(id, &subId);
            // if mSimInfo already has a valid subId for a slotId/phoneId,
            // do not add another subId for same slotId/phoneId.
            AutoPtr<IInterface> p;
            mSimInfo->Get(CoreUtils::Convert(slotId), (IInterface**)&p);
            Int64 currentSubId = 0;
            if (p != NULL) {
                IInteger64::Probe(p)->GetValue(&currentSubId);
            }
            AutoPtr<ISubscriptionManager> sm;
            CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
            Boolean bValid = FALSE;
            if (p == NULL || !(sm->IsValidSubId(currentSubId, &bValid), bValid)) {
                // TODO while two subs active, if user deactivats first
                // one, need to update the default subId with second one.

                // FIXME: Currently we assume phoneId and slotId may not be TRUE
                // when we cross map modem or when multiple subs per slot.
                // But is TRUE at the moment.
                mSimInfo->Put(CoreUtils::Convert(slotId), CoreUtils::Convert(subId));
                AutoPtr<ITelephonyManagerHelper> tmhlp;
                CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmhlp);
                AutoPtr<ITelephonyManager> tm;
                tmhlp->GetDefault((ITelephonyManager**)&tm);
                Int32 simCount = 0;
                tm->GetSimCount(&simCount);
                Int64 defaultSubId = 0;
                GetDefaultSubId(&defaultSubId);
                StringBuilder sb("[addSubInfoRecord] mSimInfo.size=");
                Int32 size = 0;
                mSimInfo->GetSize(&size);
                sb += size;
                sb += " slotId=";
                sb += slotId;
                sb += " subId=";
                sb += subId;
                sb += " defaultSubId=";
                sb += defaultSubId;
                sb += " simCount=";
                sb += simCount;
                Logdl(sb.ToString());

                // Set the default sub if not set or if single sim device
                if (!(sm->IsValidSubId(defaultSubId, &bValid), bValid) || simCount == 1) {
                    SetDefaultSubId(subId);
                }
                // if single sim device, set this subscription as the default for everything
                if (simCount == 1) {
                    sb.Reset();
                    sb += "[addSubInfoRecord] one sim set defaults to subId=";
                    sb += subId;
                    Logdl(sb.ToString());
                    SetDefaultDataSubId(subId);
                    SetDataSubId(subId);
                    SetDefaultSmsSubId(subId);
                    SetDefaultVoiceSubId(subId);
                }
            }
            else {
                Logdl(String("[addSubInfoRecord] currentSubId != NULL && currentSubId is valid, IGNORE"));
            }
            StringBuilder sb("[addSubInfoRecord]- Hashmap(");
            sb += slotId;
            sb += ",";
            sb += subId;
            sb += ")";
            Logdl(sb.ToString());
        } while ((cursor->MoveToNext(&bMN), bMN));
    }
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }

    Int32 size = 0;
    mSimInfo->GetSize(&size);
    Logdl(String("[addSubInfoRecord]- info size=") + StringUtils::ToString(size));

    // Once the records are loaded, notify DcTracker
    UpdateAllDataConnectionTrackers();

    // FIXME this does not match the javadoc
    *result = 1;
    return NOERROR;
}

ECode SubscriptionController::SetColor(
    /* [in] */ Int32 color,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // String str("[setColor]+ color:");
    // str += color;
    // str += " subId:";
    // str += subId;
    // Logd(str);
    EnforceSubscriptionPermission();

    ValidateSubId(subId);
    Int32 size = sSimBackgroundDarkRes->GetLength();
    if (color < 0 || color >= size) {
        Logd(String("[setColor]- fail"));
        *result = -1;
        return NOERROR;
    }
    AutoPtr<IContentValues> value;
    CContentValues::New(1, (IContentValues**)&value);
    value->Put(ISubscriptionManager::COLOR, color);
    // str = "[setColor]- color:";
    // str += color;
    // str += " set";
    // Logd(str);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    StringBuilder sb(IBaseColumns::ID); sb += "="; sb += subId;
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    AutoPtr<IUri> content_uri;
    sm->GetCONTENT_URI((IUri**)&content_uri);
    Int32 res = 0;
    cr->Update(content_uri, value,
            sb.ToString(), NULL, &res);
    BroadcastSimInfoContentChanged(subId, ISubscriptionManager::COLOR,
            color, ISubscriptionManager::DEFAULT_STRING_VALUE);

    *result = res;
    return NOERROR;
}

ECode SubscriptionController::SetDisplayName(
    /* [in] */ const String& displayName,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return SetDisplayNameUsingSrc(displayName, subId, -1, result);
}

ECode SubscriptionController::SetDisplayNameUsingSrc(
    /* [in] */ const String& displayName,
    /* [in] */ Int64 subId,
    /* [in] */ Int64 nameSource,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // String str("[setDisplayName]+  displayName:");
    // str += displayName;
    // str += " subId:";
    // str += subId;
    // str += " nameSource:";
    // str += nameSource;
    // Logd(str);
    EnforceSubscriptionPermission();

    ValidateSubId(subId);
    String nameToSet;
    if (displayName == NULL) {
        mContext->GetString(ISubscriptionManager::DEFAULT_NAME_RES, &nameToSet);
    }
    else {
        nameToSet = displayName;
    }
    AutoPtr<IContentValues> value;
    CContentValues::New(1, (IContentValues**)&value);
    value->Put(ISubscriptionManager::DISPLAY_NAME, nameToSet);
    if (nameSource >= ISubscriptionManager::NAME_SOURCE_DEFAULT_SOURCE) {
        // str = "Set nameSource=";
        // str += nameSource;
        // Logd(str);
        value->Put(ISubscriptionManager::NAME_SOURCE, nameSource);
    }
    // str = "[setDisplayName]- mDisplayName:";
    // str += nameToSet;
    // str += " set";
    // Logd(str);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    StringBuilder sb(IBaseColumns::ID); sb += "="; sb += subId;
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    AutoPtr<IUri> content_uri;
    sm->GetCONTENT_URI((IUri**)&content_uri);
    Int32 res = 0;
    cr->Update(content_uri, value,
            sb.ToString(), NULL, &res);
    BroadcastSimInfoContentChanged(subId, ISubscriptionManager::DISPLAY_NAME,
            ISubscriptionManager::DEFAULT_INT_VALUE, nameToSet);

    *result = res;
    return NOERROR;
}

ECode SubscriptionController::SetDisplayNumber(
    /* [in] */ const String& number,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // String str("[setDisplayNumber]+ number:");
    // str += number;
    // str += " subId:";
    // str += subId;
    // Logd(str);
    EnforceSubscriptionPermission();

    ValidateSubId(subId);
    Int32 res = 0;
    Int32 phoneId = 0;
    GetPhoneId(subId, &phoneId);

    AutoPtr<ITelephonyManagerHelper> tmhlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmhlp);
    AutoPtr<ITelephonyManager> tm;
    tmhlp->GetDefault((ITelephonyManager**)&tm);
    Int32 phonecount = 0;
    tm->GetPhoneCount(&phonecount);
    if (number == NULL || phoneId < 0 ||
            phoneId >= phonecount) {
        Logd(String("[setDispalyNumber]- fail"));
        *result = -1;
        return NOERROR;
    }
    AutoPtr<IContentValues> value;
    CContentValues::New(1, (IContentValues**)&value);
    value->Put(ISubscriptionManager::NUMBER, number);
    // str = "[setDisplayNumber]- number:";
    // str += number;
    // str += " set";
    // Logd(str);

    AutoPtr<IPhone> phone = IPhone::Probe((*sProxyPhones)[phoneId]);
    String alphaTag;
    tm->GetLine1AlphaTagForSubscriber(subId, &alphaTag);

    {
        AutoLock syncLock(mLock);
        mSuccess = FALSE;
        AutoPtr<IMessage> response;
        mHandler->ObtainMessage(EVENT_WRITE_MSISDN_DONE, (IMessage**)&response);

        phone->SetLine1Number(alphaTag, number, response);

        // try {
            mLock.Wait();
        // } Catch (InterruptedException e) {
        //     Loge("interrupted while trying to write MSISDN");
        // }
    }

    if (mSuccess) {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        StringBuilder sb(IBaseColumns::ID); sb += "="; sb += subId;
        AutoPtr<ISubscriptionManager> sm;
        CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
        AutoPtr<IUri> content_uri;
        sm->GetCONTENT_URI((IUri**)&content_uri);
        cr->Update(content_uri, value,
                sb.ToString(), NULL, &res);
        // str = "[setDisplayNumber]- update result :";
        // str += res;
        // Logd(str);
        BroadcastSimInfoContentChanged(subId, ISubscriptionManager::NUMBER,
                ISubscriptionManager::DEFAULT_INT_VALUE, number);
    }

    *result = res;
    return NOERROR;
}

ECode SubscriptionController::SetDisplayNumberFormat(
    /* [in] */ Int32 format,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // String str("[setDisplayNumberFormat]+ format:");
    // str += format;
    // str += " subId:";
    // str += subId;
    // Logd(str);
    EnforceSubscriptionPermission();

    ValidateSubId(subId);
    if (format < 0) {
        Logd(String("[setDisplayNumberFormat]- fail, return -1"));
        *result = -1;
        return NOERROR;
    }
    AutoPtr<IContentValues> value;
    CContentValues::New(1, (IContentValues**)&value);
    value->Put(ISubscriptionManager::DISPLAY_NUMBER_FORMAT, format);
    // str = "[setDisplayNumberFormat]- format:";
    // str += format;
    // str += " set";
    // Logd(str);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    StringBuilder sb(IBaseColumns::ID); sb += "="; sb += subId;
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    AutoPtr<IUri> content_uri;
    sm->GetCONTENT_URI((IUri**)&content_uri);
    Int32 res = 0;
    cr->Update(content_uri, value,
            sb.ToString(), NULL, &res);
    BroadcastSimInfoContentChanged(subId, ISubscriptionManager::DISPLAY_NUMBER_FORMAT,
            format, ISubscriptionManager::DEFAULT_STRING_VALUE);

    *result = res;
    return NOERROR;
}

ECode SubscriptionController::SetDataRoaming(
    /* [in] */ Int32 roaming,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    String str("[setDataRoaming]+ roaming:");
    // str += roaming;
    // str += " subId:";
    // str += subId;
    // Logd(str);
    EnforceSubscriptionPermission();

    ValidateSubId(subId);
    if (roaming < 0) {
        Logd(String("[setDataRoaming]- fail"));
        *result = -1;
        return NOERROR;
    }
    AutoPtr<IContentValues> value;
    CContentValues::New(1, (IContentValues**)&value);
    value->Put(ISubscriptionManager::DATA_ROAMING, roaming);
    // str = "[setDataRoaming]- roaming:";
    // str += roaming;
    // str += " set";
    // Logd(str);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    StringBuilder sb(IBaseColumns::ID); sb += "="; sb += subId;
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    AutoPtr<IUri> content_uri;
    sm->GetCONTENT_URI((IUri**)&content_uri);
    Int32 res = 0;
    cr->Update(content_uri, value,
            sb.ToString(), NULL, &res);
    BroadcastSimInfoContentChanged(subId, ISubscriptionManager::DATA_ROAMING,
            roaming, ISubscriptionManager::DEFAULT_STRING_VALUE);

    *result = res;
    return NOERROR;
}

ECode SubscriptionController::SetMccMnc(
    /* [in] */ const String& mccMnc,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 mcc = 0;
    Int32 mnc = 0;
    // try {
        mcc = StringUtils::ParseInt32(mccMnc.Substring(0, 3));
        mnc = StringUtils::ParseInt32(mccMnc.Substring(3));
    // } Catch (NumberFormatException e) {
    //     Logd("[setMccMnc] - couldn't parse mcc/mnc: " + mccMnc);
    // }
    // String str("[setMccMnc]+ mcc/mnc:");
    // str += mcc;
    // str += "/";
    // str += mnc;
    // str += " subId:";
    // str += subId;
    // Logd(str);
    AutoPtr<IContentValues> value;
    CContentValues::New(2, (IContentValues**)&value);
    value->Put(ISubscriptionManager::MCC, mcc);
    value->Put(ISubscriptionManager::MNC, mnc);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    StringBuilder sb(IBaseColumns::ID); sb += "="; sb += subId;
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    AutoPtr<IUri> content_uri;
    sm->GetCONTENT_URI((IUri**)&content_uri);
    Int32 res = 0;
    cr->Update(content_uri, value,
            sb.ToString(), NULL, &res);
    BroadcastSimInfoContentChanged(subId, ISubscriptionManager::MCC, mcc, String(NULL));

    *result = res;
    return NOERROR;
}

ECode SubscriptionController::GetSlotId(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (VDBG) {
        String str("[GetSlotId] subId=");
        str += StringUtils::ToString(subId);
        // PrintStackTrace(str);
        Logger::E("SubscriptionController", "%s", str.string());
    }

    if (subId == ISubscriptionManager::DEFAULT_SUB_ID) {
        GetDefaultSubId(&subId);
    }
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Boolean bValid = FALSE;
    sm->IsValidSubId(subId, &bValid);
    if (!bValid) {
        Logd(String("[getSlotId]- subId invalid"));
        *result = ISubscriptionManager::INVALID_SLOT_ID;
        return NOERROR;
    }

    Int32 size = 0;
    mSimInfo->GetSize(&size);

    if (size == 0) {
        Logd(String("[getSlotId]- size == 0, return SIM_NOT_INSERTED instead"));
        *result = ISubscriptionManager::SIM_NOT_INSERTED;
        return NOERROR;
    }

    AutoPtr<ISet> st;
    mSimInfo->GetEntrySet((ISet**)&st);
    AutoPtr<IIterator> it;
    st->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(p);
        AutoPtr<IInterface> key;
        entry->GetKey((IInterface**)&key);
        Int32 sim = 0;
        IInteger32::Probe(key)->GetValue(&sim);
        AutoPtr<IInterface> value;
        entry->GetValue((IInterface**)&value);
        Int64 sub = 0;
        IInteger64::Probe(value)->GetValue(&sub);

        if (subId == sub) {
            if (VDBG) {
                // String str("[getSlotId]- return = ");
                // str += sim;
                // Logv(str);
            }
            *result = sim;
            return NOERROR;
        }
    }

    Logd(String("[getSlotId]- return fail"));
    *result = ISubscriptionManager::INVALID_SLOT_ID;
    return NOERROR;
}

ECode SubscriptionController::GetSubId(
    /* [in] */ Int32 slotId,
    /* [out] */ ArrayOf<Int64>** result)
{
    VALIDATE_NOT_NULL(result)
    if (VDBG) {
        //String str("[getSubId] slotId=");
        //str += StringUtils::ToString(slotId);
        // PrintStackTrace(str);
        Logger::E("SubscriptionController", "[getSubId] slotId=%d", slotId);
    }

    if (slotId == ISubscriptionManager::DEFAULT_SLOT_ID) {
        Logd(String("[getSubId]- default slotId"));
        Int64 subId = 0;
        GetDefaultSubId(&subId);
        GetSlotId(subId, &slotId);
    }

    //FIXME remove this
    AutoPtr<ArrayOf<Int64> > DUMMY_VALUES = ArrayOf<Int64>::Alloc(2);
    (*DUMMY_VALUES)[0] = -1 - slotId;
    (*DUMMY_VALUES)[1] = -1 - slotId;

    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Boolean bValid = FALSE;
    sm->IsValidSlotId(slotId, &bValid);
    if (!bValid) {
        Logd(String("[getSubId]- invalid slotId"));
        *result = NULL;
        return NOERROR;
    }

    //FIXME remove this
    if (slotId < 0) {
        Logd(String("[getSubId]- slotId < 0, return dummy instead"));
        *result = DUMMY_VALUES;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    Int32 size = 0;
    mSimInfo->GetSize(&size);

    if (size == 0) {
        Logd(String("[getSubId]- size == 0, return dummy instead"));
        //FIXME return NULL
        *result = DUMMY_VALUES;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<IArrayList> subIds;
    CArrayList::New((IArrayList**)&subIds);
    AutoPtr<ISet> st;
    mSimInfo->GetEntrySet((ISet**)&st);
    AutoPtr<IIterator> it;
    st->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(p);
        AutoPtr<IInterface> key;
        entry->GetKey((IInterface**)&key);
        Int32 slot = 0;
        IInteger32::Probe(key)->GetValue(&slot);
        AutoPtr<IInterface> value;
        entry->GetValue((IInterface**)&value);
        Int64 sub = 0;
        IInteger64::Probe(value)->GetValue(&sub);
        if (slotId == slot) {
            subIds->Add(CoreUtils::Convert(sub));
        }
    }

    if (VDBG) {
        String str("[getSubId]-, subIds = ");
        str += TO_CSTR(subIds);
        Logd(str);
    }
    Int32 numSubIds = 0;
    subIds->GetSize(&numSubIds);

    if (numSubIds == 0) {
        Logd(String("[getSubId]- numSubIds == 0, return dummy instead"));
        *result = DUMMY_VALUES;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int64> > subIdArr = ArrayOf<Int64>::Alloc(numSubIds);
    for (Int32 i = 0; i < numSubIds; i++) {
        AutoPtr<IInterface> p;
        subIds->Get(i, (IInterface**)&p);
        IInteger64::Probe(p)->GetValue(&((*subIdArr)[i]));
    }

    *result = subIdArr;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode SubscriptionController::GetPhoneId(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (VDBG) {
        // String str("[getPhoneId] subId=");
        // str += subId;
        // PrintStackTrace(str);
    }
    Int32 phoneId = 0;

    if (subId == ISubscriptionManager::DEFAULT_SUB_ID) {
        GetDefaultSubId(&subId);
        // String str("[getPhoneId] asked for default subId=");
        // str += subId;
        // Logdl(str);
    }

    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Boolean bValid = FALSE;
    sm->IsValidSubId(subId, &bValid);
    if (!bValid) {
        // String str("[getPhoneId]- invalid subId return=");
        // str += ISubscriptionManager::INVALID_PHONE_ID;
        // Logdl(str);
        *result = ISubscriptionManager::INVALID_PHONE_ID;
        return NOERROR;
    }

    //FIXME remove this
    if (subId < 0) {
        phoneId = (Int32) (-1 - subId);
        if (VDBG) {
            // String str("[getPhoneId]- map subId=");
            // str += subId;
            // str += " phoneId=";
            // str += phoneId;
            // Logdl(str);
        }
        *result = phoneId;
        return NOERROR;
    }

    Int32 size = 0;
    mSimInfo->GetSize(&size);

    if (size == 0) {
        phoneId = mDefaultPhoneId;
        // String str("[getPhoneId]- no sims, returning default phoneId=");
        // str += phoneId;
        // Logdl(str);
        *result = phoneId;
        return NOERROR;
    }

    // FIXME: Assumes phoneId == slotId
    AutoPtr<IArrayList> subIds;
    CArrayList::New((IArrayList**)&subIds);
    AutoPtr<ISet> st;
    mSimInfo->GetEntrySet((ISet**)&st);
    AutoPtr<IIterator> it;
    st->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(p);
        AutoPtr<IInterface> key;
        entry->GetKey((IInterface**)&key);
        Int32 sim = 0;
        IInteger32::Probe(key)->GetValue(&sim);
        AutoPtr<IInterface> value;
        entry->GetValue((IInterface**)&value);
        Int64 sub = 0;
        IInteger64::Probe(value)->GetValue(&sub);

        if (subId == sub) {
            if (VDBG) {
                // String str("[getPhoneId]- found subId=");
                // str += subId;
                // str += " phoneId=";
                // str += sim;
                // Logdl(str);
            }
            *result = sim;
            return NOERROR;
        }
    }

    phoneId = mDefaultPhoneId;
    // String str("[getPhoneId]- subId=");
    // str += subId;
    // str += " not found return default phoneId=";
    // str += phoneId;
    // Logdl(str);
    *result = phoneId;
    return NOERROR;
}

ECode SubscriptionController::ClearSubInfo(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    EnforceSubscriptionPermission();
    Logd(String("[clearSubInfo]+"));

    Int32 size = 0;
    mSimInfo->GetSize(&size);

    if (size == 0) {
        // String str("[clearSubInfo]- no simInfo size=");
        // str += size;
        // Logdl(str);
        *result = 0;
        return NOERROR;
    }

    mSimInfo->Clear();
    // String str("[clearSubInfo]- clear size=");
    // str += size;
    // Logdl(str);
    *result = size;
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > SubscriptionController::SetSimResource(
    /* [in] */ Int32 type)
{
    AutoPtr<ArrayOf<Int32> > simResource;

    switch (type) {
        case RES_TYPE_BACKGROUND_DARK: {
            simResource = ArrayOf<Int32>::Alloc(4);
            (*simResource)[0] = R::drawable::sim_dark_blue;
            (*simResource)[1] = R::drawable::sim_dark_orange;
            (*simResource)[2] = R::drawable::sim_dark_green;
            (*simResource)[3] = R::drawable::sim_dark_purple;
            break;
        }
        case RES_TYPE_BACKGROUND_LIGHT: {
            simResource = ArrayOf<Int32>::Alloc(4);
            (*simResource)[0] = R::drawable::sim_light_blue;
            (*simResource)[1] = R::drawable::sim_light_orange;
            (*simResource)[2] = R::drawable::sim_light_green;
            (*simResource)[3] = R::drawable::sim_light_purple;
            break;
        }
    }

    return simResource;
}

void SubscriptionController::Logvl(
    /* [in] */ const String& msg)
{
    Logv(msg);
    mLocalLog->Log(msg);
}

void SubscriptionController::Logv(
    /* [in] */ const String& msg)
{
    //TODO Rlog::V(LOGTAG, msg);
    Logger::V(LOGTAG, "%s", msg.string());
}

void SubscriptionController::Logdl(
    /* [in] */ const String& msg)
{
    Logd(msg);
    mLocalLog->Log(msg);
}

void SubscriptionController::Slogd(
    /* [in] */ const String& msg)
{
    //TODO Rlog::D(LOGTAG, msg);
    Logger::D(LOGTAG, "%s", msg.string());
}

void SubscriptionController::Logd(
    /* [in] */ const String& msg)
{
    //TODO Rlog::D(LOGTAG, msg);
    Logger::D(LOGTAG, "%s", msg.string());
}

void SubscriptionController::Logel(
    /* [in] */ const String& msg)
{
    Loge(msg);
    mLocalLog->Log(msg);
}

void SubscriptionController::Loge(
    /* [in] */ const String& msg)
{
    //TODO Rlog::E(LOGTAG, msg);
    Logger::D(LOGTAG, "%s", msg.string());
}

ECode SubscriptionController::GetDefaultSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    //FIXME: Make this smarter, need to handle data only and voice devices
    Int64 subId = mDefaultVoiceSubId;
    if (VDBG) {
        // String str("[getDefaultSubId] value = ");
        // str += subId;
        // Logv(str);
    }
    *result = subId;
    return NOERROR;
}

ECode SubscriptionController::SetDefaultSmsSubId(
    /* [in] */ Int64 subId)
{
    if (subId == ISubscriptionManager::DEFAULT_SUB_ID) {
        // throw new RuntimeException("setDefaultSmsSubId called with DEFAULT_SUB_ID");
        return E_RUNTIME_EXCEPTION;
    }
    // String str("[setDefaultSmsSubId] subId=");
    // str += subId;
    // Logdl(str);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Boolean res = FALSE;
    sg->PutInt64(cr,
            ISettingsGlobal::MULTI_SIM_SMS_SUBSCRIPTION, subId, &res);
    BroadcastDefaultSmsSubIdChanged(subId);
    return NOERROR;
}

void SubscriptionController::BroadcastDefaultSmsSubIdChanged(
    /* [in] */ Int64 subId)
{
    // Broadcast an Intent for default sms sub change
    // String str("[broadcastDefaultSmsSubIdChanged] subId=");
    // str += subId;
    // Logdl(str);
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_DEFAULT_SMS_SUBSCRIPTION_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    intent->PutExtra(IPhoneConstants::SUBSCRIPTION_KEY, subId);
    AutoPtr<IUserHandleHelper> uhhlp;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhhlp);
    AutoPtr<IUserHandle> uh;
    uhhlp->GetALL((IUserHandle**)&uh);
    mContext->SendStickyBroadcastAsUser(intent, uh);
}

ECode SubscriptionController::GetDefaultSmsSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Int64 subId = 0;
    sg->GetInt64(cr,
            ISettingsGlobal::MULTI_SIM_SMS_SUBSCRIPTION,
            ISubscriptionManager::INVALID_SUB_ID,
            &subId);
    if (VDBG) {
        // String str("[getDefaultSmsSubId] subId=");
        // str += subId;
        // Logd(str);
    }
    *result = subId;
    return NOERROR;
}

ECode SubscriptionController::SetDefaultVoiceSubId(
    /* [in] */ Int64 subId)
{
    if (subId == ISubscriptionManager::DEFAULT_SUB_ID) {
        // throw new RuntimeException("setDefaultVoiceSubId called with DEFAULT_SUB_ID");
        return E_RUNTIME_EXCEPTION;
    }
    // String str("[setDefaultVoiceSubId] subId=");
    // str += subId;
    // Logdl(str);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Boolean res = FALSE;
    sg->PutInt64(cr,
            ISettingsGlobal::MULTI_SIM_VOICE_CALL_SUBSCRIPTION, subId, &res);
    BroadcastDefaultVoiceSubIdChanged(subId);
    return NOERROR;
}

void SubscriptionController::BroadcastDefaultVoiceSubIdChanged(
    /* [in] */ Int64 subId)
{
    // Broadcast an Intent for default voice sub change
    // String str("[broadcastDefaultVoiceSubIdChanged] subId=");
    // str += subId;
    // Logdl(str);
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_DEFAULT_VOICE_SUBSCRIPTION_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    intent->PutExtra(IPhoneConstants::SUBSCRIPTION_KEY, subId);
    AutoPtr<IUserHandleHelper> uhhlp;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhhlp);
    AutoPtr<IUserHandle> uh;
    uhhlp->GetALL((IUserHandle**)&uh);
    mContext->SendStickyBroadcastAsUser(intent, uh);
}

ECode SubscriptionController::GetDefaultVoiceSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Int64 subId = 0;
    sg->GetInt64(cr,
            ISettingsGlobal::MULTI_SIM_VOICE_CALL_SUBSCRIPTION,
            ISubscriptionManager::INVALID_SUB_ID,
            &subId);
    if (VDBG) {
        // String str("[getDefaultVoiceSubId] subId=");
        // str += subId;
        // Logd(str);
    }
    *result = subId;
    return NOERROR;
}

ECode SubscriptionController::IsSMSPromptEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean prompt = FALSE;
    Int32 value = 0;
    // try {
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    sg->GetInt32(cr,
            ISettingsGlobal::MULTI_SIM_SMS_PROMPT, &value);
    // } Catch (SettingNotFoundException snfe) {
    //     Loge("Settings Exception Reading Dual Sim SMS Prompt Values");
    // }
    prompt = (value == 0) ? FALSE : TRUE ;
    if (VDBG) {
        // String str("SMS Prompt option:");
        // str += prompt;
        // Logd(str);
    }

   *result = prompt;
   return NOERROR;
}

ECode SubscriptionController::SetSMSPromptEnabled(
    /* [in] */ Boolean enabled)
{
    Int32 value = (enabled == FALSE) ? 0 : 1;
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Boolean res = FALSE;
    sg->PutInt32(cr,
            ISettingsGlobal::MULTI_SIM_SMS_PROMPT, value, &res);
    // String str("setSMSPromptOption to ");
    // str += enabled;
    // Logd(str);
    return NOERROR;
}

ECode SubscriptionController::GetDefaultDataSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Int64 subId = 0;
    sg->GetInt64(cr,
            ISettingsGlobal::MULTI_SIM_DATA_CALL_SUBSCRIPTION,
            ISubscriptionManager::INVALID_SUB_ID,
            &subId);
    if (VDBG) {
        // String str("[getDefaultDataSubId] subId= ");
        // str += subId;
        // Logd(str);
    }
    *result = subId;
    return NOERROR;
}

ECode SubscriptionController::SetDefaultDataSubId(
    /* [in] */ Int64 subId)
{
    // String str("[setDefaultDataSubId] subId=");
    // str += subId;
    // Logdl(str);

    if (mDctController == NULL) {
        AutoPtr<IDctControllerHelper> hlp;
        CDctControllerHelper::AcquireSingleton((IDctControllerHelper**)&hlp);
        hlp->GetInstance((IDctController**)&mDctController);
        mDctController->RegisterForDefaultDataSwitchInfo(mDataConnectionHandler,
                EVENT_SET_DEFAULT_DATA_DONE, NULL);
    }
    mDctController->SetDefaultDataSubId(subId);
    return NOERROR;
}

ECode SubscriptionController::GetCurrentDds(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mScheduler->GetCurrentDds(result);
}

void SubscriptionController::UpdateDataSubId(
    /* [in] */ AsyncResult* ar)
{
    Int64 subId = 0;
    IInteger64::Probe(ar->mResult)->GetValue(&subId);
    Int32 reqStatus = IPhoneConstants::FAILURE;

    String str(" updateDataSubId,  subId=");
    str += StringUtils::ToString(subId);
    str += " exception ";
    if (ar->mException != NULL) {
        str += TO_CSTR(ar->mException);
    }
    else {
        str += "null";
    }
    Logd(str);
    // Update newDds in database if the DDS request succeeded.
    if (ar->mException == NULL) {
        SetDataSubId(subId);
        reqStatus = IPhoneConstants::SUCCESS;
    }
    mScheduler->UpdateCurrentDds(NULL);
    BroadcastDefaultDataSubIdChanged(reqStatus);

    // FIXME is this still needed?
    UpdateAllDataConnectionTrackers();
}

ECode SubscriptionController::SetDataSubId(
    /* [in] */ Int64 subId)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Boolean res = FALSE;
    sg->PutInt64(cr,
            ISettingsGlobal::MULTI_SIM_DATA_CALL_SUBSCRIPTION, subId, &res);
    return NOERROR;
}

void SubscriptionController::UpdateAllDataConnectionTrackers()
{
    // Tell Phone Proxies to update data connection tracker
    Int32 len = sProxyPhones->GetLength();
    // String str("[updateAllDataConnectionTrackers] sProxyPhones.length=");
    // str += len;
    // Logdl(str);
    for (Int32 phoneId = 0; phoneId < len; phoneId++) {
        (*sProxyPhones)[phoneId]->UpdateDataConnectionTracker();
    }
}

void SubscriptionController::BroadcastDefaultDataSubIdChanged(
    /* [in] */ Int32 status)
{
    // Broadcast an Intent for default data sub change
    Int64 subId = 0;
    GetDefaultDataSubId(&subId);
    // String str("[broadcastDefaultDataSubIdChanged] subId = ");
    // str += subId;
    // str += " status ";
    // str += status;
    // Logdl(str);
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    intent->PutExtra(IPhoneConstants::SUBSCRIPTION_KEY, subId);
    intent->PutExtra(ITelephonyIntents::EXTRA_RESULT, status);
    AutoPtr<IUserHandleHelper> uhhlp;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhhlp);
    AutoPtr<IUserHandle> uh;
    uhhlp->GetALL((IUserHandle**)&uh);
    mContext->SendStickyBroadcastAsUser(intent, uh);
}

ECode SubscriptionController::SetDefaultSubId(
    /* [in] */ Int64 subId)
{
    Logger::E("SubscriptionController", "TODO this should be called to update mDefaultVoiceSubId");
    if (subId == ISubscriptionManager::DEFAULT_SUB_ID) {
        Logger::E("SubscriptionController", "TODO setDefaultSubId called with DEFAULT_SUB_ID");
        // throw new RuntimeException("setDefaultSubId called with DEFAULT_SUB_ID");
        return E_RUNTIME_EXCEPTION;
    }
    // String str("[setDefaultSubId] subId=");
    // str += subId;
    // Logdl(str);
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Boolean bValid = FALSE;
    sm->IsValidSubId(subId, &bValid);
    if (bValid) {
        Int32 phoneId = 0;
        GetPhoneId(subId, &phoneId);
        AutoPtr<ITelephonyManagerHelper> tmhlp;
        CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmhlp);
        AutoPtr<ITelephonyManager> tm;
        tmhlp->GetDefault((ITelephonyManager**)&tm);
        Int32 phonecount = 0, simCount = 0;
        tm->GetPhoneCount(&phonecount);
        tm->GetSimCount(&simCount);
        if (phoneId >= 0 && (phoneId < phonecount
                || simCount == 1)) {
            // str = "[setDefaultSubId] set mDefaultVoiceSubId=";
            // str += subId;
            // Logdl(str);
            mDefaultVoiceSubId = subId;
            // Update MCC MNC device configuration information
            String defaultMccMnc;
            tm->GetSimOperator(phoneId, &defaultMccMnc);
            MccTable::UpdateMccMncConfiguration(mContext, defaultMccMnc, FALSE);

            // Broadcast an Intent for default sub change
            AutoPtr<IIntent> intent;
            CIntent::New(ITelephonyIntents::ACTION_DEFAULT_SUBSCRIPTION_CHANGED, (IIntent**)&intent);
            intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
            sm->PutPhoneIdAndSubIdExtra(intent, phoneId, subId);
            if (VDBG) {
                // str = "[setDefaultSubId] broadcast default subId changed phoneId=";
                // str += phoneId;
                // str += " subId=";
                // str += subId;
                // Logdl(str);
            }
            AutoPtr<IUserHandleHelper> uhhlp;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhhlp);
            AutoPtr<IUserHandle> uh;
            uhhlp->GetALL((IUserHandle**)&uh);
            mContext->SendStickyBroadcastAsUser(intent, uh);
        }
        else {
            if (VDBG) {
                // String str("[setDefaultSubId] not set invalid phoneId=");
                // str += phoneId;
                // str += " subId=";
                // str += subId;
                // Logdl(str);
            }
        }
    }
    return NOERROR;
}

ECode SubscriptionController::ClearDefaultsForInactiveSubIds()
{
    AutoPtr<IList> records;
    GetActiveSubInfoList((IList**)&records);
    String str("[clearDefaultsForInactiveSubIds] records: ");
    str += TO_CSTR(records);
    Logdl(str);
    Int64 subId = 0;
    GetDefaultDataSubId(&subId);
    if (ShouldDefaultBeCleared(records, subId)) {
        Logd(String("[clearDefaultsForInactiveSubIds] clearing default data sub id"));
        SetDefaultDataSubId(ISubscriptionManager::INVALID_SUB_ID);
    }
    Int64 smsSubId = 0;
    GetDefaultSmsSubId(&smsSubId);
    if (ShouldDefaultBeCleared(records, smsSubId)) {
        Logdl(String("[clearDefaultsForInactiveSubIds] clearing default sms sub id"));
        SetDefaultSmsSubId(ISubscriptionManager::INVALID_SUB_ID);
    }
    Int64 voiceSubId = 0;
    GetDefaultVoiceSubId(&voiceSubId);
    if (ShouldDefaultBeCleared(records, voiceSubId)) {
        Logdl(String("[clearDefaultsForInactiveSubIds] clearing default voice sub id"));
        SetDefaultVoiceSubId(ISubscriptionManager::INVALID_SUB_ID);
    }
    return NOERROR;
}

Boolean SubscriptionController::ShouldDefaultBeCleared(
    /* [in] */ IList* records,
    /* [in] */ Int64 subId)
{
    // String str("[shouldDefaultBeCleared: subId] ");
    // str += subId;
    // Logdl(str);
    if (records == NULL) {
        // str = "[shouldDefaultBeCleared] return TRUE no records subId=";
        // str += subId;
        // Logdl(str);
        return TRUE;
    }
    Int32 size = 0;
    records->GetSize(&size);
    if (subId == ISubscriptionManager::ASK_USER_SUB_ID && size > 1) {
        // Only allow ASK_USER_SUB_ID if there is more than 1 subscription.
        // str = "[shouldDefaultBeCleared] return FALSE only one subId, subId=";
        // str += subId;
        // Logdl(str);
        return FALSE;
    }
    AutoPtr<IIterator> it;
    records->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<CSubInfoRecord> record = (CSubInfoRecord*)ISubInfoRecord::Probe(p);
        // str = "[shouldDefaultBeCleared] Record.subId: ";
        // str += record->mSubId;
        // Logdl(str);
        if (record->mSubId == subId) {
            // str = "[shouldDefaultBeCleared] return FALSE subId is active, subId=";
            // str += subId;
            // Logdl(str);
            return FALSE;
        }
    }
    // str = "[shouldDefaultBeCleared] return TRUE not active subId=";
    // str += subId;
    // Logdl(str);
    return TRUE;
}

ECode SubscriptionController::GetSubIdUsingPhoneId(
    /* [in] */ Int32 phoneId,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Int64> > subIds;
    GetSubId(phoneId, (ArrayOf<Int64>**)&subIds);
    if (subIds == NULL || subIds->GetLength() == 0) {
        *result = ISubscriptionManager::INVALID_SUB_ID;
        return NOERROR;
    }
    *result = (*subIds)[0];
    return NOERROR;
}

ECode SubscriptionController::GetSubIdUsingSlotId(
    /* [in] */ Int32 slotId,
    /* [out] */ ArrayOf<Int64>** result)
{
    VALIDATE_NOT_NULL(result)
    return GetSubId(slotId, result);
}

ECode SubscriptionController::GetSubInfoUsingSlotIdWithCheck(
    /* [in] */ Int32 slotId,
    /* [in] */ Boolean needCheck,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    // String str("[getSubInfoUsingSlotIdWithCheck]+ slotId:");
    // str += slotId;
    // Logd(str);
    EnforceSubscriptionPermission();

    if (slotId == ISubscriptionManager::DEFAULT_SLOT_ID) {
        Int64 subId = 0;
        GetDefaultSubId(&subId);
        GetSlotId(subId, &slotId);
    }
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Boolean bValid = FALSE;
    sm->IsValidSlotId(slotId, &bValid);
    if (!bValid) {
        Logd(String("[getSubInfoUsingSlotIdWithCheck]- invalid slotId"));
        *result = NULL;
        return NOERROR;
    }

    if (needCheck && !IsSubInfoReady()) {
        Logd(String("[getSubInfoUsingSlotIdWithCheck]- not ready"));
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = StringUtils::ToString(slotId);
    StringBuilder sb(ISubscriptionManager::SIM_ID); sb += "=?";
    AutoPtr<IUri> content_uri;
    sm->GetCONTENT_URI((IUri**)&content_uri);
    AutoPtr<ICursor> cursor;
    cr->Query(content_uri,
            NULL, sb.ToString(), arr, String(NULL),
            (ICursor**)&cursor);
    AutoPtr<IArrayList> subList;
    Boolean bMF = FALSE, bMN = FALSE;
    if (cursor != NULL && (cursor->MoveToFirst(&bMF), bMF)) {
        do {
            AutoPtr<ISubInfoRecord> subInfo = GetSubInfoRecord(cursor);
            if (subInfo != NULL) {
                if (subList == NULL) {
                    CArrayList::New((IArrayList**)&subList);
                }
                subList->Add(subInfo);
            }
        } while ((cursor->MoveToNext(&bMN), bMN));
    }
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    Logd(String("[getSubInfoUsingSlotId]- NULL info return"));

    *result = IList::Probe(subList);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void SubscriptionController::ValidateSubId(
    /* [in] */ Int64 subId)
{
    // String str("validateSubId subId: ");
    // str += subId;
    // Logd(str);
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Boolean bValid = FALSE;
    sm->IsValidSubId(subId, &bValid);
    if (!bValid) {
        // throw new RuntimeException("Invalid sub id passed as parameter");
        return;
    }
    else if (subId == ISubscriptionManager::DEFAULT_SUB_ID) {
        // throw new RuntimeException("Default sub id passed as parameter");
        return;
    }
}

ECode SubscriptionController::UpdatePhonesAvailability(
    /* [in] */ ArrayOf<IPhoneProxy*>* phones)
{
    sProxyPhones = phones;
    return NOERROR;
}

ECode SubscriptionController::GetActiveSubIdList(
    /* [out] */ ArrayOf<Int64>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISet> simInfoSet;
    mSimInfo->GetEntrySet((ISet**)&simInfoSet);
    String str("[getActiveSubIdList] simInfoSet=");
    str += TO_CSTR(simInfoSet);
    Logdl(str);

    Int32 size = 0;
    simInfoSet->GetSize(&size);
    AutoPtr<ArrayOf<Int64> > subIdArr = ArrayOf<Int64>::Alloc(size);
    Int32 i = 0;
    AutoPtr<IIterator> it;
    simInfoSet->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(p);
        AutoPtr<IInterface> val;
        entry->GetValue((IInterface**)&val);
        Int64 sub = 0;
        IInteger64::Probe(val)->GetValue(&sub);
        (*subIdArr)[i] = sub;
        i++;
    }

    // str = "[getActiveSubIdList] X subIdArr.length=";
    // str += subIdArr->GetLength();
    // Logdl(str);
    *result = subIdArr;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void SubscriptionController::PrintStackTrace(
    /* [in] */ const String& msg)
{
    assert(0 && "TODO");
    // AutoPtr<IRuntimeException> re;
    // CRuntimeException::New((IRuntimeException**)&re);
    String str("StackTrace - ");
    str += msg;
    Slogd(str);
    AutoPtr<ArrayOf<IStackTraceElement*> > st;
    assert(0 && "TODO");
    // re->GetStackTrace((ArrayOf<IStackTraceElement*>**)&st);
    Boolean first = TRUE;
    for (Int32 i = 0; i < st->GetLength(); i++) {
        AutoPtr<IStackTraceElement> ste = (*st)[i];
        if (first) {
            first = FALSE;
        }
        else {
            assert(0 && "TODO");
            // Slogd(ste->ToString());
        }
    }
}

ECode SubscriptionController::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    assert(0 && "TODO");
    // mContext->EnforceCallingOrSelfPermission(Manifest::permission::DUMP,
    //         "Requires DUMP");
    pw->Println(String("SubscriptionController:"));
    // pw->Println(" defaultSubId=" + GetDefaultSubId());
    // pw->Println(" defaultDataSubId=" + GetDefaultDataSubId());
    // pw->Println(" defaultVoiceSubId=" + GetDefaultVoiceSubId());
    // pw->Println(" defaultSmsSubId=" + GetDefaultSmsSubId());

    // pw->Println(" defaultDataPhoneId=" + SubscriptionManager->GetDefaultDataPhoneId());
    // pw->Println(" defaultVoicePhoneId=" + SubscriptionManager->GetDefaultVoicePhoneId());
    // pw->Println(" defaultSmsPhoneId=" + SubscriptionManager->GetDefaultSmsPhoneId());
    IFlushable::Probe(pw)->Flush();

    // for (Entry<Integer, Long> entry : mSimInfo->EntrySet()) {
    //     pw->Println(" mSimInfo[" + entry->GetKey() + "]: subId=" + entry->GetValue());
    // }
    IFlushable::Probe(pw)->Flush();
    pw->Println(String("++++++++++++++++++++++++++++++++"));

    // AutoPtr<IList> sirl = GetActiveSubInfoList();
    // if (sirl != NULL) {
    //     pw->Println(" ActiveSubInfoList:");
    //     for (SubInfoRecord entry : sirl) {
    //         pw->Println("  " + entry->ToString());
    //     }
    // }
    // else {
    //     pw->Println(" ActiveSubInfoList: is NULL");
    // }
    IFlushable::Probe(pw)->Flush();
    pw->Println(String("++++++++++++++++++++++++++++++++"));

    // sirl = GetAllSubInfoList();
    // if (sirl != NULL) {
    //     pw->Println(" AllSubInfoList:");
    //     for (SubInfoRecord entry : sirl) {
    //         pw->Println("  " + entry->ToString());
    //     }
    // }
    // else {
    //     pw->Println(String(" AllSubInfoList: is NULL"));
    // }
    IFlushable::Probe(pw)->Flush();
    pw->Println(String("++++++++++++++++++++++++++++++++"));

    mLocalLog->Dump(fd, pw, args);
    IFlushable::Probe(pw)->Flush();
    pw->Println(String("++++++++++++++++++++++++++++++++"));
    IFlushable::Probe(pw)->Flush();
    return NOERROR;
}

ECode SubscriptionController::ActivateSubId(
    /* [in] */ Int64 subId)
{
    Int32 state = 0;
    GetSubState(subId, &state);
    if (state == ISubscriptionManager::ACTIVE) {
        Logd(String("activateSubId: subscription already active, subId = ") + StringUtils::ToString(subId));
        return NOERROR;
    }

    Int32 slotId = 0;
    GetSlotId(subId, &slotId);
    SubscriptionHelper::GetInstance()->SetUiccSubscription(slotId, ISubscriptionManager::ACTIVE);
    return NOERROR;
}

ECode SubscriptionController::DeactivateSubId(
    /* [in] */ Int64 subId)
{
    Int32 state = 0;
    GetSubState(subId, &state);
    if (state == ISubscriptionManager::INACTIVE) {
        Logd(String("activateSubId: subscription already deactivated, subId = ") + StringUtils::ToString(subId));
        return NOERROR;
    }

    Int32 slotId = 0;
    GetSlotId(subId, &slotId);
    SubscriptionHelper::GetInstance()->SetUiccSubscription(slotId, ISubscriptionManager::INACTIVE);
    return NOERROR;
}

ECode SubscriptionController::SetNwMode(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 nwMode)
{
    StringBuilder str("setNwMode, nwMode: ");
    str += nwMode;
    str += " subId: ";
    str += subId;
    Logd(str.ToString());
    AutoPtr<IContentValues> value;
    CContentValues::New(1, (IContentValues**)&value);
    value->Put(ISubscriptionManager::NETWORK_MODE, nwMode);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    StringBuilder sb(IBaseColumns::ID); sb += "="; sb += subId;
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    AutoPtr<IUri> content_uri;
    sm->GetCONTENT_URI((IUri**)&content_uri);
    Int32 res = 0;
    cr->Update(content_uri,
            value, sb.ToString(), NULL, &res);
    return NOERROR;
}

ECode SubscriptionController::GetNwMode(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubInfoRecord> subInfo;
    GetSubInfoForSubscriber(subId, (ISubInfoRecord**)&subInfo);
    if (subInfo != NULL)  {
        AutoPtr<CSubInfoRecord> _subInfo = (CSubInfoRecord*)subInfo.Get();
        *result = _subInfo->mNwMode;
        return NOERROR;
    }
    else {
        Loge(String("getSubState: invalid subId = ") + StringUtils::ToString(subId));
        *result = ISubscriptionManager::DEFAULT_NW_MODE;
        return NOERROR;
    }
}

ECode SubscriptionController::SetSubState(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 subStatus,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 res = 0;
    StringBuilder str("setSubState, subStatus: ");
    str += subStatus;
    str += " subId: ";
    str += subId;
    Logd(str.ToString());
    AutoPtr<ModemStackController> mc = ModemStackController::GetInstance();
    Boolean ready = mc->IsStackReady();
    if (ready) {
        AutoPtr<IContentValues> value;
        CContentValues::New(1, (IContentValues**)&value);
        value->Put(ISubscriptionManager::SUB_STATE, subStatus);
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        StringBuilder sb(IBaseColumns::ID); sb += "="; sb += subId;
        AutoPtr<ISubscriptionManager> sm;
        CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
        AutoPtr<IUri> content_uri;
        sm->GetCONTENT_URI((IUri**)&content_uri);
        cr->Update(content_uri,
                value, sb.ToString(), NULL, &res);

    }
    BroadcastSimInfoContentChanged(subId,
            ISubscriptionManager::SUB_STATE, subStatus, ISubscriptionManager::DEFAULT_STRING_VALUE);
    *result = res;
    return NOERROR;
}

ECode SubscriptionController::GetSubState(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubInfoRecord> subInfo;
    GetSubInfoForSubscriber(subId, (ISubInfoRecord**)&subInfo);
    Int32 subStatus = ISubscriptionManager::INACTIVE;

    // Consider the subStatus from subInfo record only if the
    //  record is associated with a valid slot Id.
    if ((subInfo != NULL) && (((CSubInfoRecord*)subInfo.Get())->mSlotId >= 0)) {
        subStatus = ((CSubInfoRecord*)subInfo.Get())->mStatus;
    }
    *result = subStatus;
    return NOERROR;
}

ECode SubscriptionController::UpdateUserPrefs(
    /* [in] */ Boolean setDds)
{
    AutoPtr<IList> subInfoList;
    GetActiveSubInfoList((IList**)&subInfoList);
    Int32 mActCount = 0;
    AutoPtr<ISubInfoRecord> mNextActivatedSub;

    if (subInfoList == NULL) {
        String str("updateUserPrefs: subscription are not avaiable dds = ");
        Int64 dataSubId = 0;
        GetDefaultDataSubId(&dataSubId);
        str += StringUtils::ToString(dataSubId);
        str += " voice = ";
        Int64 voiceSubId = 0;
        GetDefaultVoiceSubId(&voiceSubId);
        str += StringUtils::ToString(voiceSubId);
        str += " sms = ";
        Int64 smsSubId = 0;
        GetDefaultSmsSubId(&smsSubId);
        str += StringUtils::ToString(smsSubId);
        str += " setDDs = ";
        str += StringUtils::ToString(setDds);
        Logd(str);
        // if no SIM cards present on device, set dummy subId
        // as data/sms/voice preferred subId.
        SetDefaultSubId(DUMMY_SUB_ID);
        SetDefaultVoiceSubId(DUMMY_SUB_ID);
        SetDefaultSmsSubId(DUMMY_SUB_ID);
        SetDataSubId(DUMMY_SUB_ID);
        return NOERROR;
    }

    //Get num of activated Subs and next available activated sub info.
    AutoPtr<IIterator> it;
    subInfoList->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<CSubInfoRecord> subInfo = (CSubInfoRecord*)ISubInfoRecord::Probe(p);
        Int32 state = 0;
        GetSubState(subInfo->mSubId, &state);
        if (state == ISubscriptionManager::ACTIVE) {
            mActCount++;
            if (mNextActivatedSub == NULL) {
                mNextActivatedSub = subInfo;
            }
        }
    }

    Int64 smsSubId = 0;
    Int64 dataSubId = 0;
    Int64 voiceSubId = 0;
    String str("updateUserPrefs: active sub count = ");
    str += StringUtils::ToString(mActCount);
    str += " dds = ";
    GetDefaultDataSubId(&dataSubId);
    str += StringUtils::ToString(dataSubId);
    str += " voice = ";
    GetDefaultVoiceSubId(&voiceSubId);
    str += StringUtils::ToString(voiceSubId);
    str += " sms = ";
    GetDefaultSmsSubId(&smsSubId);
    str += StringUtils::ToString(smsSubId);
    str += " setDDs = ";
    str += StringUtils::ToString(setDds);
    Logd(str);
    //if activated sub count is less than 2, disable prompt.
    if (mActCount < 2) {
        SetSMSPromptEnabled(FALSE);
        SetVoicePromptEnabled(FALSE);
    }

    //if there are no activated subs available, no need to update. EXIT.
    if (mNextActivatedSub == NULL) return NOERROR;

    Int64 subId = 0;
    GetDefaultSubId(&subId);
    Int32 state = 0;
    GetSubState(subId, &state);
    AutoPtr<CSubInfoRecord> _mNextActivatedSub = (CSubInfoRecord*)mNextActivatedSub.Get();
    if (state == ISubscriptionManager::INACTIVE) {
        SetDefaultSubId(_mNextActivatedSub->mSubId);
    }

    Int64 ddsSubId = 0;
    GetDefaultDataSubId(&ddsSubId);
    Int32 ddsSubState = 0;
    GetSubState(ddsSubId, &ddsSubState);
    //if current data sub is not active, fallback to next active sub.
    if (setDds || (ddsSubState == ISubscriptionManager::INACTIVE)) {
        if (ddsSubState == ISubscriptionManager::INACTIVE) ddsSubId = _mNextActivatedSub->mSubId;
        SetDefaultDataSubId(ddsSubId);
    }
    //if current voice sub is not active and prompt not enabled, fallback to next active sub.
    GetDefaultVoiceSubId(&voiceSubId);
    GetSubState(voiceSubId, &ddsSubState);
    Boolean bVoiceEnabled = FALSE;
    IsVoicePromptEnabled(&bVoiceEnabled);
    if (ddsSubState == ISubscriptionManager::INACTIVE &&
        !bVoiceEnabled) {
        SetDefaultVoiceSubId(_mNextActivatedSub->mSubId);
    }
    //if current sms sub is not active and prompt not enabled, fallback to next active sub.
    GetDefaultSmsSubId(&smsSubId);
    GetSubState(smsSubId, &ddsSubState);
    Boolean bSmsEnabled = FALSE;
    IsSMSPromptEnabled(&bSmsEnabled);
    if (ddsSubState == ISubscriptionManager::INACTIVE &&
        !bSmsEnabled) {
        SetDefaultSmsSubId(_mNextActivatedSub->mSubId);
    }
    StringBuilder sb("updateUserPrefs: after currentDds = ");
    GetDefaultDataSubId(&ddsSubId);
    sb += ddsSubId;
    sb += " voice = ";
    GetDefaultVoiceSubId(&voiceSubId);
    sb += voiceSubId;
    sb += " sms = ";
    GetDefaultSmsSubId(&smsSubId);
    sb += smsSubId;
    sb += " newDds = ";
    sb += ddsSubId;
    Logd(sb.ToString());
    return NOERROR;
}

ECode SubscriptionController::IsVoicePromptEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean prompt = FALSE;
    Int32 value = 0;
    // try {
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    sg->GetInt32(cr,
            ISettingsGlobal::MULTI_SIM_VOICE_PROMPT,
            &value);
    // } Catch (SettingNotFoundException snfe) {
    //     Loge("Settings Exception Reading Dual Sim Voice Prompt Values");
    // }
    prompt = (value == 0) ? FALSE : TRUE ;
    if (VDBG) {
        String str("Voice Prompt option:");
        str += StringUtils::ToString(prompt);
        Logd(str);
    }

    *result = prompt;
    return NOERROR;
}

ECode SubscriptionController::SetVoicePromptEnabled(
    /* [in] */ Boolean enabled)
{
    Int32 value = (enabled == FALSE) ? 0 : 1;
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Boolean res = FALSE;
    sg->PutInt32(cr,
            ISettingsGlobal::MULTI_SIM_VOICE_PROMPT, value, &res);
    Logd(String("setVoicePromptOption to ") + StringUtils::ToString(enabled));
    return NOERROR;
}

ECode SubscriptionController::GetOnDemandDataSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return GetCurrentDds(result);
}

ECode SubscriptionController::RegisterForOnDemandDdsLockNotification(
    /* [in] */ Int64 clientSubId,
    /* [in] */ ISubscriptionControllerOnDemandDdsLockNotifier* callback)
{
    StringBuilder str("registerForOnDemandDdsLockNotification for client=");
    str += clientSubId;
    Logd(str.ToString());
    mOnDemandDdsLockNotificationRegistrants->Put(CoreUtils::Convert(clientSubId), callback);
    return NOERROR;
}

ECode SubscriptionController::NotifyOnDemandDataSubIdChanged(
    /* [in] */ INetworkRequest* n)
{
    Int64 subId = 0;
    GetSubIdFromNetworkRequest(n, &subId);
    AutoPtr<IInteger32> pSubId;
    CInteger32::New(subId, (IInteger32**)&pSubId);
    AutoPtr<IInterface> p;
    mOnDemandDdsLockNotificationRegistrants->Get(
            pSubId, (IInterface**)&p);
    AutoPtr<ISubscriptionControllerOnDemandDdsLockNotifier> notifier =
        ISubscriptionControllerOnDemandDdsLockNotifier::Probe(p);
    if (notifier != NULL) {
        notifier->NotifyOnDemandDdsLockGranted(n);
    }
    else {
        Logd(String("No registrants for OnDemandDdsLockGranted event"));
    }
    return NOERROR;
}

ECode SubscriptionController::RemoveStaleSubPreferences(
    /* [in] */ const String& prefKey)
{
    AutoPtr<IList> subInfoList;
    GetAllSubInfoList((IList**)&subInfoList);
    AutoPtr<IPreferenceManagerHelper> hlp;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&hlp);
    AutoPtr<ISharedPreferences> sp;
    hlp->GetDefaultSharedPreferences(mContext, (ISharedPreferences**)&sp);
    AutoPtr<IIterator> it;
    subInfoList->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<CSubInfoRecord> subInfo = (CSubInfoRecord*)ISubInfoRecord::Probe(p);
        if (subInfo->mSlotId == -1) {
            AutoPtr<ISharedPreferencesEditor> edit;
            sp->Edit((ISharedPreferencesEditor**)&edit);
            edit->Remove(prefKey + StringUtils::ToString(subInfo->mSubId));
            Boolean tmp = FALSE;
            edit->Commit(&tmp);
        }
    }
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
