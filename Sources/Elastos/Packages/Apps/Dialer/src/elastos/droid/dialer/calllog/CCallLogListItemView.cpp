
#include "elastos/droid/dialer/calllog/CCallLogListItemView.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CCallLogListItemView, LinearLayout, ICallLogListItemView);

CAR_OBJECT_IMPL(CCallLogListItemView);

ECode CCallLogListItemView::constructor(
    /* [in] */ IContext* context)
{
    return LinearLayout::constructor(context);
}

ECode CCallLogListItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::constructor(context, attrs);
}

ECode CCallLogListItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return LinearLayout::constructor(context, attrs, defStyle);
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
