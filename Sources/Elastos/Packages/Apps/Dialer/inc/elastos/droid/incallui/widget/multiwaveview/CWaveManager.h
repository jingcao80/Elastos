
#ifndef __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_CWAVEMANAGER_H__
#define __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_CWAVEMANAGER_H__

#include "_Elastos_Droid_InCallUI_Widget_MultiwaveView_CWaveManager.h"
#include "elastos/droid/incallui/widget/multiwaveview/PointCloud.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {
namespace Widget {
namespace MultiwaveView {

CarClass(CWaveManager)
    , public PointCloud::WaveManager
{
public:
    CAR_OBJECT_DECL();
};

} // namespace MultiwaveView
} // namespace Widget
} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_CWAVEMANAGER_H__
