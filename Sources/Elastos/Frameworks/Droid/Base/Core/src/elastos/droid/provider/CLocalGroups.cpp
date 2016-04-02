
#include "elastos/droid/provider/CLocalGroups.h"
#include "elastos/droid/provider/LocalGroups.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CLocalGroups)

CAR_INTERFACE_IMPL(CLocalGroups, Singleton, ILocalGroups)

ECode CLocalGroups::GetAUTHORITY_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = LocalGroups::AUTHORITY_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CLocalGroups::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = LocalGroups::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos
