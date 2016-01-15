
#ifndef __ELASTOS_DROID_WIDGET_CREMOTEVIEWSFACTORYADAPTER_H__
#define __ELASTOS_DROID_WIDGET_CREMOTEVIEWSFACTORYADAPTER_H__

#include "_Elastos_Droid_Widget_CRemoteViewsFactoryAdapter.h"
#include "elastos/droid/widget/RemoteViewsService.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CRemoteViewsFactoryAdapter)
    , public RemoteViewsService::RemoteViewsFactoryAdapter
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CREMOTEVIEWSFACTORYADAPTER_H__
