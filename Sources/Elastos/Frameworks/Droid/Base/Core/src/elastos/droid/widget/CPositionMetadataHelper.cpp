#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/CPositionMetadataHelper.h"

using Elastos::Droid::Widget::IPositionMetadata;
using Elastos::Droid::Widget::IGroupMetadata;

namespace Elastos {
namespace Droid {
namespace Widget {

ECode CPositionMetadataHelper::Obtain(
    /* [in] */ Int32 flatListPos,
    /* [in] */ Int32 type,
    /* [in] */ Int32 groupPos,
    /* [in] */ Int32 childPos,
    /* [in] */ IGroupMetadata* groupMetadata,
    /* [in] */ Int32 groupInsertIndex,
    /* [out] */ IPositionMetadata** posMetadata)
{
    VALIDATE_NOT_NULL(posMetadata);
    *posMetadata = PositionMetadata::Obtain(flatListPos, type,
        groupPos, childPos, groupMetadata, groupInsertIndex).Get();
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos