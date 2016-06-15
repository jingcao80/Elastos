
#ifndef __ELASTOS_DROID_APPWIDGET_CPARCELABLESPARSEARRAY_H__
#define __ELASTOS_DROID_APPWIDGET_CPARCELABLESPARSEARRAY_H__

#include "_Elastos_Droid_AppWidget_CParcelableSparseArray.h"
#include "elastos/droid/appwidget/AppWidgetHostView.h"

namespace Elastos {
namespace Droid {
namespace AppWidget {

CarClass(CParcelableSparseArray)
    , public AppWidgetHostView::ParcelableSparseArray
{
public:
    CAR_OBJECT_DECL()
};

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APPWIDGET_CPARCELABLESPARSEARRAY_H__
