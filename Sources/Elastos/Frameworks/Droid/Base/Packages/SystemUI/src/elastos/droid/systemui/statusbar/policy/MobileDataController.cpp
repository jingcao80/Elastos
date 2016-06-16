
#include "elastos/droid/systemui/statusbar/policy/MobileDataController.h"
#include "elastos/droid/systemui/statusbar/policy/NetworkController.h"
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Net::CConnectivityManagerHelper;
using Elastos::Droid::Net::CNetworkTemplateHelper;
using Elastos::Droid::Net::CNetworkPolicyManagerHelper;
using Elastos::Droid::Net::IConnectivityManagerHelper;
using Elastos::Droid::Net::INetworkPolicyManagerHelper;
using Elastos::Droid::Net::INetworkStatsHistory;
using Elastos::Droid::Net::INetworkTemplateHelper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Text::Format::CDateUtils;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Core::CSystem;
using Elastos::Core::IAppendable;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CDate;
using Elastos::Utility::CFormatter;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::IDate;
using Elastos::Utility::IFormatter;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String MobileDataController::TAG("MobileDataController");
const Boolean MobileDataController::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
const Int64 MobileDataController::DEFAULT_WARNING_LEVEL = 2LL * 1024ul * 1024ul * 1024ul;
const Int32 MobileDataController::FIELDS = INetworkStatsHistory::FIELD_RX_BYTES | INetworkStatsHistory::FIELD_TX_BYTES;
Boolean MobileDataController::sInit = InitStatic();
AutoPtr<IStringBuilder> MobileDataController::PERIOD_BUILDER;
AutoPtr<IFormatter> MobileDataController::PERIOD_FORMATTER;

CAR_INTERFACE_IMPL(MobileDataController, Object, IMobileDataController)
MobileDataController::MobileDataController(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<ITelephonyManagerHelper> tHelper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tHelper);
    tHelper->From(context, (ITelephonyManager**)&mTelephonyManager);
    AutoPtr<IConnectivityManagerHelper> helper;
    CConnectivityManagerHelper::AcquireSingleton((IConnectivityManagerHelper**)&helper);
    helper->From(context, (IConnectivityManager**)&mConnectivityManager);

    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::NETWORK_STATS_SERVICE);
    mStatsService = IINetworkStatsService::Probe(obj);

    AutoPtr<INetworkPolicyManagerHelper> nHelper;
    CNetworkPolicyManagerHelper::AcquireSingleton((INetworkPolicyManagerHelper**)&nHelper);
    Logger::D(TAG, "TODO: Need NetworkPolicyManager.");
    // nHelper->From(mContext, (INetworkPolicyManager**)&mPolicyManager);
}

Boolean MobileDataController::InitStatic()
{
    PERIOD_BUILDER = new StringBuilder(50);

    AutoPtr<ILocaleHelper> localehelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localehelper);
    AutoPtr<ILocale> lc;
    localehelper->GetDefault((ILocale**)&lc);
    CFormatter::New(IAppendable::Probe(PERIOD_BUILDER), lc, (IFormatter**)&PERIOD_FORMATTER);
    return TRUE;
}

AutoPtr<IINetworkStatsSession> MobileDataController::GetSession()
{
    if (mSession == NULL) {
        if (FAILED(mStatsService->OpenSession((IINetworkStatsSession**)&mSession))) {
            Logger::W(TAG, "Failed to open stats session");
        }
    }
    return mSession;
}

ECode MobileDataController::SetCallback(
    /* [in] */ IMobileDataControllerCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

AutoPtr<INetworkControllerDataUsageInfo> MobileDataController::Warn(
    /* [in] */ const String& msg)
{
    Logger::W(TAG, "Failed to get data usage, %s", msg.string());
    return NULL;
}

AutoPtr<ITime> MobileDataController::AddMonth(
    /* [in] */ ITime* t,
    /* [in] */ Int32 months)
{
    AutoPtr<ITime> rt;
    CTime::New(t, (ITime**)&rt);
    Int32 monthDay = 0, month = 0, year = 0;
    t->GetMonthDay(&monthDay);
    t->GetMonth(&month);
    t->GetYear(&year);
    rt->Set(monthDay, month + months, year);
    Int64 v = 0;
    rt->Normalize(FALSE, &v);
    return rt;
}

ECode MobileDataController::GetDataUsageInfo(
    /* [out] */ INetworkControllerDataUsageInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;
    String subscriberId = GetActiveSubscriberId(mContext);
    if (subscriberId == NULL) {
        AutoPtr<INetworkControllerDataUsageInfo> i = Warn(String("no subscriber id"));
        *info = i;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }
    AutoPtr<IINetworkStatsSession> session = GetSession();
    if (session == NULL) {
        AutoPtr<INetworkControllerDataUsageInfo> i = Warn(String("no stats session"));
        *info = i;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }

    AutoPtr<INetworkTemplateHelper> ntHelper;
    CNetworkTemplateHelper::AcquireSingleton((INetworkTemplateHelper**)&ntHelper);
    AutoPtr<INetworkTemplate> tem;
    ntHelper->BuildTemplateMobileAll(subscriberId, (INetworkTemplate**)&tem);
    AutoPtr<INetworkPolicy> policy = FindNetworkPolicy(tem);
    ECode ec = NOERROR;
    do {
        AutoPtr<INetworkStatsHistory> history;
        ec = mSession->GetHistoryForNetwork(tem, FIELDS, (INetworkStatsHistory**)&history);
        if (FAILED(ec)) {
            break;
        }
        Int64 now = 0;
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&now);
        Int64 start = 0, end = 0;
        Int32 cycleDay = 0;
        if (policy != NULL && (policy->GetCycleDay(&cycleDay), cycleDay) > 0) {
            // period = determined from cycleDay
            String cycleTimezone;
            policy->GetCycleTimezone(&cycleTimezone);
            if (DEBUG) {
                Logger::D(TAG, "Cycle day=%d tz=%s", cycleDay, cycleTimezone.string());
            }
            AutoPtr<ITime> nowTime;
            CTime::New(cycleTimezone, (ITime**)&nowTime);
            nowTime->SetToNow();
            AutoPtr<ITime> policyTime;
            CTime::New(nowTime, (ITime**)&policyTime);

            Int32 month = 0, year = 0;
            policyTime->GetMonth(&month);
            policyTime->GetYear(&year);
            policyTime->Set(cycleDay, month, year);
            Int64 v = 0;
            policyTime->Normalize(FALSE, &v);
            Boolean tmp = FALSE;
            if (nowTime->After(policyTime, &tmp), tmp) {
                policyTime->ToMillis(FALSE, &start);
                AddMonth(policyTime, 1)->ToMillis(FALSE, &end);
            }
            else {
                AddMonth(policyTime, -1)->ToMillis(FALSE, &start);
                policyTime->ToMillis(FALSE, &end);
            }
        }
        else {
            // period = last 4 wks
            end = now;
            start = now - IDateUtils::WEEK_IN_MILLIS * 4;
        }
        Int64 callStart = 0;
        system->GetCurrentTimeMillis(&callStart);
        AutoPtr<INetworkStatsHistoryEntry> entry;
        history->GetValues(start, end, now, NULL, (INetworkStatsHistoryEntry**)&entry);
        Int64 callEnd = 0;
        system->GetCurrentTimeMillis(&callEnd);
        if (DEBUG) {
            AutoPtr<IDate> startDate;
            CDate::New(start, (IDate**)&startDate);
            AutoPtr<IDate> endDate;
            CDate::New(end, (IDate**)&endDate);
            AutoPtr<IDate> nowDate;
            CDate::New(now, (IDate**)&nowDate);
            String ss, es, ns;
            startDate->ToString(&ss);
            endDate->ToString(&es);
            nowDate->ToString(&ns);
            Logger::D(TAG, "history call from %s to %s now=%s took %lldms: %s",
                ss.string(), es.string(), ns.string(), callEnd - callStart,
                HistoryEntryToString(entry).string());
        }
        if (entry == NULL) {
            AutoPtr<INetworkControllerDataUsageInfo> i = Warn(String("no entry data"));
            *info = i;
            REFCOUNT_ADD(*info);
            return NOERROR;
        }
        Int64 rxBytes = 0, txBytes = 0;
        entry->GetRxBytes(&rxBytes);
        entry->GetTxBytes(&txBytes);
        Int64 totalBytes = rxBytes + txBytes;
        AutoPtr<INetworkControllerDataUsageInfo> usage = new DataUsageInfo();
        usage->SetUsageLevel(totalBytes);
        usage->SetPeriod(FormatDateRange(start, end));
        if (policy != NULL) {
            Int64 limitBytes = 0;
            policy->GetLimitBytes(&limitBytes);
            usage->SetLimitLevel(limitBytes > 0 ? limitBytes : 0);

            Int64 warningBytes = 0;
            policy->GetWarningBytes(&warningBytes);
            usage->SetWarningLevel(warningBytes > 0 ? warningBytes : 0);
        }
        else {
            usage->SetWarningLevel(DEFAULT_WARNING_LEVEL);
        }

        *info = usage;
        REFCOUNT_ADD(*info);
        return NOERROR;
    } while (0);

    if (FAILED(ec)) {
        AutoPtr<INetworkControllerDataUsageInfo> i = Warn(String("remote call failed"));
        *info = i;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }
    return NOERROR;
}

AutoPtr<INetworkPolicy> MobileDataController::FindNetworkPolicy(
    /* [in] */ INetworkTemplate* tem)
{
    if (mPolicyManager == NULL || tem == NULL) return NULL;
    AutoPtr<ArrayOf<INetworkPolicy*> > policies;
    mPolicyManager->GetNetworkPolicies((ArrayOf<INetworkPolicy*>**)&policies);
    if (policies == NULL) return NULL;
    const Int32 N = policies->GetLength();
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<INetworkPolicy> policy = (*policies)[i];
        if (policy != NULL) {
            AutoPtr<INetworkTemplate> t;
            policy->GetTemplate((INetworkTemplate**)&t);
            Boolean e = FALSE;
            if (IObject::Probe(tem)->Equals(t, &e), e)
            return policy;
        }
    }
    return NULL;
}

String MobileDataController::HistoryEntryToString(
    /* [in] */ INetworkStatsHistoryEntry* entry)
{
    if (entry == NULL) {
        return String(NULL);
    }
    StringBuilder sb("Entry[");
    sb.Append("bucketDuration=");
    Int64 value = 0;
    entry->GetBucketDuration(&value);
    sb.Append(value);
    sb.Append(",bucketStart=");
    entry->GetBucketStart(&value);
    sb.Append(value);
    sb.Append(",activeTime=");

    entry->GetActiveTime(&value);
    sb.Append(value);
    sb.Append(",rxBytes=");
    entry->GetRxBytes(&value);
    sb.Append(value);
    sb.Append(",rxPackets=");
    entry->GetRxPackets(&value);
    sb.Append(value);
    sb.Append(",txBytes=");
    entry->GetTxBytes(&value);
    sb.Append(value);
    sb.Append(",txPackets=");
    entry->GetTxPackets(&value);
    sb.Append(value);
    sb.Append(",operations=");
    entry->GetOperations(&value);
    sb.Append(value);
    sb.AppendChar(']');
    return sb.ToString();
}

ECode MobileDataController::SetMobileDataEnabled(
    /* [in] */ Boolean enabled)
{
    mTelephonyManager->SetDataEnabled(enabled);
    if (mCallback != NULL) {
        mCallback->OnMobileDataEnabled(enabled);
    }
    return NOERROR;
}

ECode MobileDataController::IsMobileDataSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // require both supported network and ready SIM
    Boolean tmp = FALSE;
    Int32 flag = 0;
    *result = (mConnectivityManager->IsNetworkSupported(IConnectivityManager::TYPE_MOBILE, &tmp), tmp)
            && (mTelephonyManager->GetSimState(&flag), flag) == ITelephonyManager::SIM_STATE_READY;

    return NOERROR;
}

ECode MobileDataController::IsMobileDataEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mTelephonyManager->GetDataEnabled(result);
}

String MobileDataController::GetActiveSubscriberId(
    /* [in] */ IContext* context)
{
    AutoPtr<ITelephonyManager> tele;
    AutoPtr<ITelephonyManagerHelper> helper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper);
    helper->From(context, (ITelephonyManager**)&tele);

    String actualSubscriberId;
    tele->GetSubscriberId(&actualSubscriberId);
    return actualSubscriberId;
}

String MobileDataController::FormatDateRange(
    /* [in] */ Int64 start,
    /* [in] */ Int64 end)
{
    const Int32 flags = IDateUtils::FORMAT_SHOW_DATE | IDateUtils::FORMAT_ABBREV_MONTH;
    {    AutoLock syncLock(PERIOD_BUILDER);
        PERIOD_BUILDER->SetLength(0);
        AutoPtr<IDateUtils> du;
        CDateUtils::AcquireSingleton((IDateUtils**)&du);
        AutoPtr<IFormatter> f;
        du->FormatDateRange(mContext, PERIOD_FORMATTER, start, end, flags, String(NULL) ,(IFormatter**)&f);
        String str;
        f->ToString(&str);
        return str;
    }
    return String(NULL);
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
