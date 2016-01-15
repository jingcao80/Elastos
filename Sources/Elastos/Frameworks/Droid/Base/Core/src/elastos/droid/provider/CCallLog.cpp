#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/provider/CCallLog.h"
#include <elastos/coredef.h>

using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CCallLog)

CAR_INTERFACE_IMPL(CCallLog, Singleton, ICallLog)

static AutoPtr<IUri> initCONTENTURI()
{
    AutoPtr<IUri> uri;
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    helper->Parse(String("content://") + ICallLog::AUTHORITY, (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> CCallLog::CONTENT_URI = initCONTENTURI();

ECode CCallLog::GetCONTENT_URI(
        /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    *uri = CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos