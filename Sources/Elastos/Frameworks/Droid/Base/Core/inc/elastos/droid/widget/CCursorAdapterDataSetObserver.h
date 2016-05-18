
#ifndef __ELASTOS_DROID_WIDGET_CCURSORADAPTER_DATASET_OBSERVER_H__
#define __ELASTOS_DROID_WIDGET_CCURSORADAPTER_DATASET_OBSERVER_H__

#include "_Elastos_Droid_Widget_CCursorAdapterDataSetObserver.h"
#include "elastos/droid/widget/CursorAdapter.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CCursorAdapterDataSetObserver)
    , public CursorAdapter::DataSetObserver
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif // __ELASTOS_DROID_WIDGET_CCURSORADAPTER_DATASET_OBSERVER_H__
