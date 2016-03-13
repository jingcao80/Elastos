
#include "elastos/droid/telephony/CSubscriptionManager.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Provider::EIID_IBaseColumns;

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL_2(CSubscriptionManager, Singleton, ISubscriptionManager, IBaseColumns)

CAR_SINGLETON_IMPL(CSubscriptionManager)

ECode CSubscriptionManager::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    return SubscriptionManager::GetCONTENT_URI(uri);
}

ECode CSubscriptionManager::GetSubInfoForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ ISubInfoRecord** result)
{
    return SubscriptionManager::GetSubInfoForSubscriber(subId, result);
}

ECode CSubscriptionManager::GetSubInfoUsingIccId(
    /* [in] */ const String& iccId,
    /* [out] */ IList** result)
{
    return SubscriptionManager::GetSubInfoUsingIccId(iccId, result);
}

ECode CSubscriptionManager::GetSubInfoUsingSlotId(
    /* [in] */ Int32 slotId,
    /* [out] */ IList** result)
{
    return SubscriptionManager::GetSubInfoUsingSlotId(slotId, result);
}

ECode CSubscriptionManager::GetAllSubInfoList(
    /* [out] */ IList** result)
{
    return SubscriptionManager::GetAllSubInfoList(result);
}

ECode CSubscriptionManager::GetActiveSubInfoList(
    /* [out] */ IList** result)
{
    return SubscriptionManager::GetActiveSubInfoList(result);
}

ECode CSubscriptionManager::GetAllSubInfoCount(
    /* [out] */ Int32* result)
{
    return SubscriptionManager::GetAllSubInfoCount(result);
}

ECode CSubscriptionManager::GetActiveSubInfoCount(
    /* [out] */ Int32* result)
{
    return SubscriptionManager::GetActiveSubInfoCount(result);
}

ECode CSubscriptionManager::AddSubInfoRecord(
    /* [in] */ const String& iccId,
    /* [in] */ Int32 slotId,
    /* [out] */ IUri** result)
{
    return SubscriptionManager::AddSubInfoRecord(iccId, slotId, result);
}

ECode CSubscriptionManager::SetColor(
    /* [in] */ Int32 color,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    return SubscriptionManager::SetColor(color, subId, result);
}

ECode CSubscriptionManager::SetDisplayName(
    /* [in] */ const String& displayName,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    return SubscriptionManager::SetDisplayName(displayName, subId, result);
}

ECode CSubscriptionManager::SetDisplayName(
    /* [in] */ const String& displayName,
    /* [in] */ Int64 subId,
    /* [in] */ Int64 nameSource,
    /* [out] */ Int32* result)
{
    return SubscriptionManager::SetDisplayName(displayName, subId, nameSource, result);
}

ECode CSubscriptionManager::SetDisplayNumber(
    /* [in] */ const String& number,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    return SubscriptionManager::SetDisplayNumber(number, subId, result);
}

ECode CSubscriptionManager::SetDisplayNumberFormat(
    /* [in] */ Int32 format,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    return SubscriptionManager::SetDisplayNumberFormat(format, subId, result);
}

ECode CSubscriptionManager::SetDataRoaming(
    /* [in] */ Int32 roaming,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    return SubscriptionManager::SetDataRoaming(roaming, subId, result);
}

ECode CSubscriptionManager::GetSlotId(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    return SubscriptionManager::GetSlotId(subId, result);
}

ECode CSubscriptionManager::GetSubId(
    /* [in] */ Int32 slotId,
    /* [out, callee] */ ArrayOf<Int64>** result)
{
    return SubscriptionManager::GetSubId(slotId, result);
}

ECode CSubscriptionManager::GetPhoneId(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    return SubscriptionManager::GetPhoneId(subId, result);
}

ECode CSubscriptionManager::GetDefaultSubId(
    /* [out] */ Int64* result)
{
    return SubscriptionManager::GetDefaultSubId(result);
}

ECode CSubscriptionManager::GetDefaultVoiceSubId(
    /* [out] */ Int64* result)
{
    return SubscriptionManager::GetDefaultVoiceSubId(result);
}

ECode CSubscriptionManager::SetDefaultVoiceSubId(
    /* [in] */ Int64 id)
{
    return SubscriptionManager::SetDefaultVoiceSubId(id);
}

ECode CSubscriptionManager::GetDefaultVoiceSubInfo(
    /* [out] */ ISubInfoRecord** result)
{
    return SubscriptionManager::GetDefaultVoiceSubInfo(result);
}

ECode CSubscriptionManager::GetDefaultVoicePhoneId(
    /* [out] */ Int32* result)
{
    return SubscriptionManager::GetDefaultVoicePhoneId(result);
}

ECode CSubscriptionManager::GetDefaultSmsSubId(
    /* [out] */ Int64* result)
{
    return SubscriptionManager::GetDefaultSmsSubId(result);
}

ECode CSubscriptionManager::SetDefaultSmsSubId(
    /* [in] */ Int64 id)
{
    return SubscriptionManager::SetDefaultSmsSubId(id);
}

ECode CSubscriptionManager::GetDefaultSmsSubInfo(
    /* [out] */ ISubInfoRecord** result)
{
    return SubscriptionManager::GetDefaultSmsSubInfo(result);
}

ECode CSubscriptionManager::GetDefaultSmsPhoneId(
    /* [out] */ Int32* result)
{
    return SubscriptionManager::GetDefaultSmsPhoneId(result);
}

ECode CSubscriptionManager::GetDefaultDataSubId(
    /* [out] */ Int64* result)
{
    return SubscriptionManager::GetDefaultDataSubId(result);
}

ECode CSubscriptionManager::SetDefaultDataSubId(
    /* [in] */ Int64 id)
{
    return SubscriptionManager::SetDefaultDataSubId(id);
}

ECode CSubscriptionManager::GetDefaultDataSubInfo(
    /* [out] */ ISubInfoRecord** result)
{
    return SubscriptionManager::GetDefaultDataSubInfo(result);
}

ECode CSubscriptionManager::GetDefaultDataPhoneId(
    /* [out] */ Int32* result)
{
    return SubscriptionManager::GetDefaultDataPhoneId(result);
}

ECode CSubscriptionManager::ClearSubInfo()
{
    return SubscriptionManager::ClearSubInfo();
}

ECode CSubscriptionManager::AllDefaultsSelected(
    /* [out] */ Boolean* result)
{
    return SubscriptionManager::AllDefaultsSelected(result);
}

ECode CSubscriptionManager::ClearDefaultsForInactiveSubIds()
{
    return SubscriptionManager::ClearDefaultsForInactiveSubIds();
}

ECode CSubscriptionManager::IsValidSubId(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    return SubscriptionManager::IsValidSubId(subId, result);
}

ECode CSubscriptionManager::IsValidSlotId(
    /* [in] */ Int32 slotId,
    /* [out] */ Boolean* result)
{
    return SubscriptionManager::IsValidSlotId(slotId, result);
}

ECode CSubscriptionManager::IsValidPhoneId(
    /* [in] */ Int32 phoneId,
    /* [out] */ Boolean* result)
{
    return SubscriptionManager::IsValidPhoneId(phoneId, result);
}

ECode CSubscriptionManager::PutPhoneIdAndSubIdExtra(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 phoneId)
{
    return SubscriptionManager::PutPhoneIdAndSubIdExtra(intent, phoneId);
}

ECode CSubscriptionManager::PutPhoneIdAndSubIdExtra(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 phoneId,
    /* [in] */ Int64 subId)
{
    return SubscriptionManager::PutPhoneIdAndSubIdExtra(intent, phoneId, subId);
}

ECode CSubscriptionManager::GetActiveSubIdList(
    /* [out, callee] */ ArrayOf<Int64>** result)
{
    return SubscriptionManager::GetActiveSubIdList(result);
}

} //namespace Elastos
} //namespace Droid
} //namespace Telephony