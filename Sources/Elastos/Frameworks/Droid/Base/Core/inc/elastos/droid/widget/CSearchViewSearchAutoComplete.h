#ifndef __ELASTOS_DROID_WIDGET_SEARCHVIEWSEARCHAUTOCOMPLETE_H__
#define __ELASTOS_DROID_WIDGET_SEARCHVIEWSEARCHAUTOCOMPLETE_H__

#include "_Elastos_Droid_Widget_CSearchViewSearchAutoComplete.h"
#include "elastos/droid/widget/SearchView.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CSearchViewSearchAutoComplete), public SearchView::_SearchAutoComplete
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_SEARCHVIEWSEARCHAUTOCOMPLETE_H__
