#ifndef __ELASTOS_DROID_WIDGET_CEXPANDABLELISTCONTEXTMENUINFO_H__
#define __ELASTOS_DROID_WIDGET_CEXPANDABLELISTCONTEXTMENUINFO_H__

#include "_Elastos_Droid_Widget_CExpandableListContextMenuInfo.h"
#include "elastos/droid/widget/ExpandableListView.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CExpandableListContextMenuInfo)
    , public ExpandableListView::ExpandableListContextMenuInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CEXPANDABLELISTCONTEXTMENUINFO_H__
