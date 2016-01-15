
#include "elastos/droid/widget/CToastHelper.h"
#include "elastos/droid/widget/Toast.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(CToastHelper, Singleton, IToastHelper);

CAR_SINGLETON_IMPL(CToastHelper);

ECode CToastHelper::MakeText(
    /* [in] */ IContext* context,
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 duration,
    /* [out] */ IToast** toast)
{
    VALIDATE_NOT_NULL(toast);
    return Toast::MakeText(context, text, duration, toast);
}

ECode CToastHelper::MakeText(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId,
    /* [in] */ Int32 duration,
    /* [out] */ IToast** toast)
{
    VALIDATE_NOT_NULL(toast);
    return Toast::MakeText(context, resId, duration, toast);
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
