
#include "elastos/droid/internal/telephony/IccPhoneBookInterfaceManagerProxy.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(IccPhoneBookInterfaceManagerProxy, Object, IIccPhoneBookInterfaceManagerProxy)

IccPhoneBookInterfaceManagerProxy::IccPhoneBookInterfaceManagerProxy(
    /* [in] */ IIccPhoneBookInterfaceManager* iccPhoneBookInterfaceManager)
{
    mIccPhoneBookInterfaceManager = iccPhoneBookInterfaceManager;
}

ECode IccPhoneBookInterfaceManagerProxy::SetmIccPhoneBookInterfaceManager(
    /* [in] */ IIccPhoneBookInterfaceManager* iccPhoneBookInterfaceManager)
{
    mIccPhoneBookInterfaceManager = iccPhoneBookInterfaceManager;
    return NOERROR;
}

ECode IccPhoneBookInterfaceManagerProxy::UpdateAdnRecordsInEfBySearch(
    /* [in] */ Int32 efid,
    /* [in] */ const String& oldTag,
    /* [in] */ const String& oldPhoneNumber,
    /* [in] */ const String& newTag,
    /* [in] */ const String& newPhoneNumber,
    /* [in] */ const String& pin2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mIccPhoneBookInterfaceManager->UpdateAdnRecordsInEfBySearch(
            efid, oldTag, oldPhoneNumber, newTag, newPhoneNumber, pin2, result);
}

ECode IccPhoneBookInterfaceManagerProxy::UpdateAdnRecordsWithContentValuesInEfBySearch(
    /* [in] */ Int32 efid,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& pin2,
    /* [out] */ Boolean* result) /*throws android.os.RemoteException*/
{
    VALIDATE_NOT_NULL(result);
    return mIccPhoneBookInterfaceManager->UpdateAdnRecordsWithContentValuesInEfBySearch(efid,
            values, pin2, result);
}

ECode IccPhoneBookInterfaceManagerProxy::UpdateAdnRecordsInEfByIndex(
    /* [in] */ Int32 efid,
    /* [in] */ const String& newTag,
    /* [in] */ const String& newPhoneNumber,
    /* [in] */ Int32 index,
    /* [in] */ const String& pin2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mIccPhoneBookInterfaceManager->UpdateAdnRecordsInEfByIndex(efid,
            newTag, newPhoneNumber, index, pin2, result);
}

ECode IccPhoneBookInterfaceManagerProxy::GetAdnRecordsSize(
    /* [in] */ Int32 efid,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    return mIccPhoneBookInterfaceManager->GetAdnRecordsSize(efid, result);
}

ECode IccPhoneBookInterfaceManagerProxy::GetAdnRecordsInEf(
    /* [in] */ Int32 efid,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    return mIccPhoneBookInterfaceManager->GetAdnRecordsInEf(efid, result);
}

ECode IccPhoneBookInterfaceManagerProxy::GetAdnCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    return mIccPhoneBookInterfaceManager->GetAdnCount(count);
}

ECode IccPhoneBookInterfaceManagerProxy::GetAnrCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    return mIccPhoneBookInterfaceManager->GetAnrCount(count);
}

ECode IccPhoneBookInterfaceManagerProxy::GetEmailCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    return mIccPhoneBookInterfaceManager->GetEmailCount(count);
}

ECode IccPhoneBookInterfaceManagerProxy::GetSpareAnrCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    return mIccPhoneBookInterfaceManager->GetSpareAnrCount(count);
}

ECode IccPhoneBookInterfaceManagerProxy::GetSpareEmailCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    return mIccPhoneBookInterfaceManager->GetSpareEmailCount(count);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
