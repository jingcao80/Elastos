
#ifndef __UTILS_H__
#define __UTILS_H__

#include "elastos/droid/app/Activity.h"

using Elastos::Droid::Widget::IToastHelper;

namespace Gallery {

class Utils
    : public ElRefBase
{
public:
    static void MakeToast(
        /* [in] */ IContext* context,
        /* [in] */ const String& text,
        /* [in] */ Int32 toastType);

private:
    static const String TAG;
    static AutoPtr<IToastHelper> sToastHelper;
};

} // namespace Gallery

#endif // __UTILS_H__
