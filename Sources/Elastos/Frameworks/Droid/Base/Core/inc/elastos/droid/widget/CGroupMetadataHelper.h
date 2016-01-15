#ifndef __ELASTOS_DROID_WIDGET_CGROUPMETADATAHELPER_H__
#define __ELASTOS_DROID_WIDGET_CGROUPMETADATAHELPER_H__

#include "_Elastos_Droid_Widget_CGroupMetadataHelper.h"
#include "elastos/droid/widget/GroupMetadata.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CGroupMetadataHelper)
{

    CARAPI Obtain(
        /* [in] */ Int32 flPos,
        /* [in] */ Int32 lastChildFlPos,
        /* [in] */ Int32 gPos,
        /* [in] */ Int64 gId,
        /* [out] */ IGroupMetadata** groupMetadata);

};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CGROUPMETADATAHELPER_H__
