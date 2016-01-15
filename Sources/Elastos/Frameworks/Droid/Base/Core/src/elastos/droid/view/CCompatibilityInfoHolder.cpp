
#include "elastos/droid/view/CCompatibilityInfoHolder.h"
#include "elastos/droid/content/res/CCompatibilityInfo.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::Res::CCompatibilityInfo;

namespace Elastos {
namespace Droid {
namespace View {

CCompatibilityInfoHolder::CCompatibilityInfoHolder()
{
    mCompatInfo = CCompatibilityInfo::DEFAULT_COMPATIBILITY_INFO;
}

ECode CCompatibilityInfoHolder::Set(
    /* [in] */ ICompatibilityInfo* compatInfo)
{
    Boolean isRequired, isSupports;
    if (compatInfo != NULL && ((compatInfo->IsScalingRequired(&isRequired), isRequired)
        || !(compatInfo->SupportsScreen(&isSupports), isSupports))) {
        mCompatInfo = compatInfo;
    }
    else {
        mCompatInfo = CCompatibilityInfo::DEFAULT_COMPATIBILITY_INFO;
    }

    return NOERROR;
}

ECode CCompatibilityInfoHolder::Get(
    /* [out] */ ICompatibilityInfo** compatInfo)
{
    VALIDATE_NOT_NULL(compatInfo);
    *compatInfo = mCompatInfo;
    REFCOUNT_ADD(*compatInfo);

    return NOERROR;
}

ECode CCompatibilityInfoHolder::GetIfNeeded(
    /* [out] */ ICompatibilityInfo** compatInfo)
{
    VALIDATE_NOT_NULL(compatInfo);
    ICompatibilityInfo* ci = mCompatInfo;
    if (mCompatInfo == NULL ||
        mCompatInfo == CCompatibilityInfo::DEFAULT_COMPATIBILITY_INFO) {
        *compatInfo = NULL;
    }
    else {
        *compatInfo = mCompatInfo;
        REFCOUNT_ADD(*compatInfo);
    }

    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
