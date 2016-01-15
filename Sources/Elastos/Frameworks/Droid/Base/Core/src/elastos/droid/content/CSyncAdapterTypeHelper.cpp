
#include "elastos/droid/content/CSyncAdapterTypeHelper.h"
#include "elastos/droid/content/CSyncAdapterType.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CSyncAdapterTypeHelper, Singleton, ISyncAdapterTypeHelper)

CAR_SINGLETON_IMPL(CSyncAdapterTypeHelper)

ECode CSyncAdapterTypeHelper::NewKey(
    /* [in] */ const String& authority,
    /* [in] */ const String& accountType,
    /* [out] */ ISyncAdapterType** syncAdapterType)
{
    AutoPtr<ISyncAdapterType> sat = CSyncAdapterType::NewKey(authority, accountType);
    *syncAdapterType = sat;
    REFCOUNT_ADD(*syncAdapterType)
    return NOERROR;
}

}
}
}

