
#ifndef __ELASTOS_DROID_WIDGET_CACTIVITYCHOOSERMODELHELPER_H__
#define __ELASTOS_DROID_WIDGET_CACTIVITYCHOOSERMODELHELPER_H__

#include "_Elastos_Droid_Widget_CActivityChooserModelHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Widget::IActivityChooserModelHelper;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CActivityChooserModelHelper)
    , public Singleton
    , public IActivityChooserModelHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Get(
        /* [in] */ IContext* context,
        /* [in] */ const String& historyFileName,
        /* [out] */ IActivityChooserModel** result);
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CACTIVITYCHOOSERMODELHELPER_H__
