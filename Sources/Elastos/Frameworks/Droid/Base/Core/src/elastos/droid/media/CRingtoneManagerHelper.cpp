
#include "elastos/droid/media/CRingtoneManagerHelper.h"
#include "elastos/droid/media/CRingtoneManager.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CRingtoneManagerHelper)

CAR_INTERFACE_IMPL(CRingtoneManagerHelper, Singleton, IRingtoneManagerHelper)

ECode CRingtoneManagerHelper::GetValidRingtoneUri(
    /* [in] */ IContext* context,
    /* [out] */ IUri** result)
{
    return CRingtoneManager::GetValidRingtoneUri(context, result);
}

ECode CRingtoneManagerHelper::GetRingtone(
    /* [in] */ IContext* context,
    /* [in] */ IUri* ringtoneUri,
    /* [out] */ IRingtone** result)
{
    return CRingtoneManager::GetRingtone(context, ringtoneUri, result);
}

ECode CRingtoneManagerHelper::GetActualDefaultRingtoneUri(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type,
    /* [out] */ IUri** result)
{
    return CRingtoneManager::GetActualDefaultRingtoneUri(context, type, result);
}

ECode CRingtoneManagerHelper::SetActualDefaultRingtoneUri(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type,
    /* [in] */ IUri* ringtoneUri)
{
    return CRingtoneManager::SetActualDefaultRingtoneUri(context, type, ringtoneUri);
}

ECode CRingtoneManagerHelper::IsDefault(
    /* [in] */ IUri* ringtoneUri,
    /* [out] */ Boolean* result)
{
    return CRingtoneManager::IsDefault(ringtoneUri, result);
}

ECode CRingtoneManagerHelper::GetDefaultType(
    /* [in] */ IUri* defaultRingtoneUri,
    /* [out] */ Int32* result)
{
    return CRingtoneManager::GetDefaultType(defaultRingtoneUri, result);
}

ECode CRingtoneManagerHelper::GetDefaultUri(
    /* [in] */ Int32 type,
    /* [out] */ IUri** result)
{
    return CRingtoneManager::GetDefaultUri(type, result);
}

ECode CRingtoneManagerHelper::GetDefaultRingtoneUriBySubId(
    /* [in] */ Int32 subId,
    /* [out] */ IUri** result)
{
    return CRingtoneManager::GetDefaultRingtoneUriBySubId(subId, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
