
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_CVIEWGROUP_ADAPTER_BRIDGE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_CVIEWGROUP_ADAPTER_BRIDGE_H__

#include "_Elastos_Droid_SystemUI_Qs_CViewGroupAdapterBridge.h"
#include <elastos/droid/systemui/qs/PseudoGridView.h>

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

class UsageTracker;

CarClass(CViewGroupAdapterBridge)
    , public PseudoGridView::ViewGroupAdapterBridge
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Qs
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_QS_CVIEWGROUP_ADAPTER_BRIDGE_H__
