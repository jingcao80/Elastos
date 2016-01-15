
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CSMILEYSHELPER_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CSMILEYSHELPER_H__

#include "_Elastos_Droid_Internal_Widget_CSmileysHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Internal::Widget::ISmileysHelper;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CSmileysHelper)
    , public Singleton
    , public ISmileysHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetSmileyResource(
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CSMILEYSHELPER_H__
