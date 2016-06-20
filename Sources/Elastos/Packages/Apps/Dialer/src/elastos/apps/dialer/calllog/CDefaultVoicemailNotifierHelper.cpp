
#include "elastos/apps/dialer/calllog/CDefaultVoicemailNotifierHelper.h"
#include "elastos/apps/dialer/calllog/DefaultVoicemailNotifier.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CDefaultVoicemailNotifierHelper, Singleton, IDefaultVoicemailNotifierHelper);

CAR_SINGLETON_IMPL(CDefaultVoicemailNotifierHelper);

ECode CDefaultVoicemailNotifierHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ IDefaultVoicemailNotifier* notifier)
{
    VALIDATE_NOT_NULL(notifier);
    AutoPtr<IDefaultVoicemailNotifier> result =
            DefaultVoicemailNotifier::GetInstance(context);
    *notifier = result;
    REFCOUNT_ADD(*notifier);

    return NOERROR;
}

ECode CDefaultVoicemailNotifierHelper::CreateNewCallsQuery(
    /* [in] */ IContentResolver* contentResolver,
    /* [out] */ IDefaultVoicemailNotifierNewCallsQuery* query)
{
    VALIDATE_NOT_NULL(query);
    AutoPtr<IDefaultVoicemailNotifierNewCallsQuery> result =
            DefaultVoicemailNotifier::CreateNewCallsQuery(contentResolver);
    *query = result;
    REFCOUNT_ADD(*query);

    return NOERROR;
}

ECode CDefaultVoicemailNotifierHelper::CreatePhoneNumberHelper(
    /* [in] */ IContext* context,
    /* [out] */ IPhoneNumberDisplayHelper* helper)
{
    VALIDATE_NOT_NULL(helper);
    AutoPtr<IPhoneNumberDisplayHelper> result =
            DefaultVoicemailNotifier::CreatePhoneNumberHelper(context);
    *helper = result;
    REFCOUNT_ADD(*helper);

    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
