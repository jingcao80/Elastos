

#include "elastos/apps/dialer/calllog/CIntentProviderHelper.h"
#include "elastos/apps/dialer/calllog/IntentProvider.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CIntentProviderHelper, Singleton, IIntentProviderHelper);

CAR_SINGLETON_IMPL(CIntentProviderHelper);

ECode CIntentProviderHelper::GetReturnCallIntentProvider(
    /* [in] */ const String& number,
    /* [out] */ IIntentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    AutoPtr<IIntentProvider> result = IntentProvider::GetReturnCallIntentProvider(number);
    *provider = result;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CIntentProviderHelper::GetReturnCallIntentProvider(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [out] */ IIntentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    AutoPtr<IIntentProvider> result = IntentProvider::GetReturnCallIntentProvider(
            number, accountHandle);
    *provider = result;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CIntentProviderHelper::GetReturnVideoCallIntentProvider(
    /* [in] */ const String& number,
    /* [out] */ IIntentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    AutoPtr<IIntentProvider> result = IntentProvider::GetReturnVideoCallIntentProvider(number);
    *provider = result;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CIntentProviderHelper::GetReturnVideoCallIntentProvider(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [out] */ IIntentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    AutoPtr<IIntentProvider> result = IntentProvider::GetReturnVideoCallIntentProvider(
            number, accountHandle);
    *provider = result;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CIntentProviderHelper::GetPlayVoicemailIntentProvider(
    /* [in] */ Int64 rowId,
    /* [in] */ const String& voicemailUri,
    /* [out] */ IIntentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    AutoPtr<IIntentProvider> result = IntentProvider::GetPlayVoicemailIntentProvider(
            rowId, voicemailUri);
    *provider = result;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CIntentProviderHelper::GetCallDetailIntentProvider(
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int64>* extraIds,
    /* [in] */ const String& voicemailUri,
    /* [out] */ IIntentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    AutoPtr<IIntentProvider> result = IntentProvider::GetCallDetailIntentProvider(
            id, extraIds, voicemailUri);
    *provider = result;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
