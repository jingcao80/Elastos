#ifndef __ELASTOS_DROID_WIDGET_CPOSITIONMETADATAHELPER_H__
#define __ELASTOS_DROID_WIDGET_CPOSITIONMETADATAHELPER_H__

#include "_Elastos_Droid_Widget_CPositionMetadataHelper.h"
#include "elastos/droid/widget/PositionMetadata.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CPositionMetadataHelper)
{
public:

    CARAPI Obtain(
        /* [in] */ Int32 flatListPos,
        /* [in] */ Int32 type,
        /* [in] */ Int32 groupPos,
        /* [in] */ Int32 childPos,
        /* [in] */ IGroupMetadata* groupMetadata,
        /* [in] */ Int32 groupInsertIndex,
        /* [out] */ IPositionMetadata** posMetadata);

};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif
