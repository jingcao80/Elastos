#ifndef __ELASTOS_DROID_WIDGET_CEXPANDABLELISTVIEWHELPER_H__
#define __ELASTOS_DROID_WIDGET_CEXPANDABLELISTVIEWHELPER_H__

#include "_Elastos_Droid_Widget_CExpandableListViewHelper.h"
#include "elastos/droid/widget/ExpandableListView.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CExpandableListViewHelper)
    , public Singleton
    , public IExpandableListViewHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI GetPackedPositionType(
        /* [in] */ Int64 packedPosition,
        /* [out] */ Int32* type);

    CARAPI GetPackedPositionGroup(
        /* [in] */ Int64 packedPosition,
        /* [out] */ Int32* position);

    CARAPI GetPackedPositionChild(
        /* [in] */ Int64 packedPosition,
        /* [out] */ Int32* position);

    CARAPI GetPackedPositionForChild(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition,
        /* [out] */ Int64* position);

    CARAPI GetPackedPositionForGroup(
        /* [in] */ Int32 groupPosition,
        /* [out] */ Int64* position);
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CEXPANDABLELISTVIEWHELPER_H__
