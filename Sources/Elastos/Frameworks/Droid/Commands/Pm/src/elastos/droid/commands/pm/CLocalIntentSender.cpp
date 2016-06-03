
#include "CLocalIntentSender.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Content::EIID_IIIntentSender;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Concurrent::IBlockingQueue;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Pm {

CAR_OBJECT_IMPL(CLocalIntentSender)

CAR_INTERFACE_IMPL_2(CLocalIntentSender, Object, IIIntentSender, IBinder);

ECode CLocalIntentSender::constructor(
    /* [in] */ ISynchronousQueue* result)
{
    mResult = result;
    return NOERROR;
}

ECode CLocalIntentSender::Send(
    /* [in] */ Int32 code,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IIntentReceiver* finishedReceiver,
    /* [in] */ const String& requiredPermission,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ITimeUnitHelper> tuHelper;
    CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&tuHelper);
    AutoPtr<ITimeUnit> tu;
    tuHelper->GetSECONDS((ITimeUnit**)&tu);
    Boolean res;
    if (FAILED(IBlockingQueue::Probe(mResult)->Offer(intent, 5, tu, &res))) {
        return E_RUNTIME_EXCEPTION;
    }
    *result = 0;
    return NOERROR;
}

ECode CLocalIntentSender::ToString(
    /* [in] */ String* str)
{
    return Object::ToString(str);
}

}
}
}
}

