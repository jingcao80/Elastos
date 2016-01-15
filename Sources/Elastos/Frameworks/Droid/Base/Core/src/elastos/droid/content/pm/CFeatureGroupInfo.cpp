#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/CFeatureGroupInfo.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(CFeatureGroupInfo, Object, IFeatureGroupInfo, IParcelable)

CAR_OBJECT_IMPL(CFeatureGroupInfo)

CFeatureGroupInfo::CFeatureGroupInfo()
{
}

CFeatureGroupInfo::~CFeatureGroupInfo()
{
}

ECode CFeatureGroupInfo::constructor()
{
    return NOERROR;
}

ECode CFeatureGroupInfo::constructor(
    /* [in] */ IFeatureGroupInfo* other)
{
    VALIDATE_NOT_NULL(other)

    CFeatureGroupInfo* cfgi = (CFeatureGroupInfo*)other;
    mFeatures = cfgi->mFeatures;
    return NOERROR;
}

ECode CFeatureGroupInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadArrayOf((Handle32*)&mFeatures);
}

ECode CFeatureGroupInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteArrayOf((Handle32)mFeatures.Get());
}

ECode CFeatureGroupInfo::SetFeatures(
    /* [in] */ ArrayOf<IFeatureInfo*>* features)
{
    mFeatures = features;
    return NOERROR;
}

ECode CFeatureGroupInfo::GetFeatures(
    /* [out, callee] */ ArrayOf<IFeatureInfo*>** features)
{
    VALIDATE_NOT_NULL(features)
    *features = mFeatures;
    REFCOUNT_ADD(*features)
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
