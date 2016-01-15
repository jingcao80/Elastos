
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/content/CContentResolverOpenResourceIdResult.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CContentResolverOpenResourceIdResult, Object, IContentResolverOpenResourceIdResult)

CAR_OBJECT_IMPL(CContentResolverOpenResourceIdResult)

CContentResolverOpenResourceIdResult::CContentResolverOpenResourceIdResult()
    : mRes(NULL)
    , mId(0)
{}

ECode CContentResolverOpenResourceIdResult::constructor(
    /* [in] */ IResources* res,
    /* [in] */ Int32 id)
{
    mRes = res;
    mId = id;
    return NOERROR;
}

ECode CContentResolverOpenResourceIdResult::GetResources(
    /* [out] */ IResources** resources)
{
    VALIDATE_NOT_NULL(resources)
    *resources = mRes;
    REFCOUNT_ADD(*resources)
    return NOERROR;
}

ECode CContentResolverOpenResourceIdResult::SetResources(
    /* [in] */ IResources* resources)
{
    mRes = resources;
    return NOERROR;
}

ECode CContentResolverOpenResourceIdResult::GetResourceId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mId;
    return NOERROR;
}

ECode CContentResolverOpenResourceIdResult::SetResourceId(
    /* [in] */ Int32 id)
{
    mId = id;
    return NOERROR;
}

}
}
}

