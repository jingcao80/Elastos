
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGLISTITEMVIEW_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGLISTITEMVIEW_H__

#include "_Elastos_Droid_Dialer_CallLog_CCallLogListItemView.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos{
namespace Droid{
namespace Dialer {
namespace CallLog {

/**
 * An entry in the call log.
 */
CarClass(CCallLogListItemView)
    , public LinearLayout
    , public ICallLogListItemView
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGLISTITEMVIEW_H__
