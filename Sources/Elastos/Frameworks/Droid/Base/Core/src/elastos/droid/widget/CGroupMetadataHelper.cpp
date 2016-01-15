#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/CGroupMetadataHelper.h"


namespace Elastos {
namespace Droid {
namespace Widget {

ECode CGroupMetadataHelper::Obtain(
    /* [in] */ Int32 flPos,
    /* [in] */ Int32 lastChildFlPos,
    /* [in] */ Int32 gPos,
    /* [in] */ Int64 gId,
    /* [out] */ IGroupMetadata** groupMetadata)
{
    VALIDATE_NOT_NULL(groupMetadata);
    *groupMetadata = GroupMetadata::Obtain(flPos, lastChildFlPos,
        gPos, gId).Get();
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos