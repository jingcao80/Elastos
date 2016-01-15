
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_CPOINTCLOUD_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_CPOINTCLOUD_H__

#include "_Elastos_Droid_Internal_Widget_Multiwaveview_CPointCloud.h"
#include "elastos/droid/internal/widget/multiwaveview/PointCloud.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {
namespace Multiwaveview {

CarClass(CPointCloud)
    , public PointCloud
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Multiwaveview
} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_CPOINTCLOUD_H__
