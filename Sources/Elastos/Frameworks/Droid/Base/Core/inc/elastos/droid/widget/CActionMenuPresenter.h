#ifndef __ELASTOS_DROID_WIDGET_CACTIONMENUPRESENTER_H__
#define __ELASTOS_DROID_WIDGET_CACTIONMENUPRESENTER_H__

#include "_Elastos_Droid_Widget_CActionMenuPresenter.h"
#include "elastos/droid/widget/ActionMenuPresenter.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CActionMenuPresenter) , public ActionMenuPresenter
{
public:
    CAR_OBJECT_DECL()

};

} // namespace Widget
} // namespace Droid
} // namespace Elastos
#endif
