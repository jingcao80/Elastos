#ifndef  __ELASTOS_DROID_LAUNCHER2_CPAGEDVIEWWIDGETIMAGEVIEW_H__
#define  __ELASTOS_DROID_LAUNCHER2_CPAGEDVIEWWIDGETIMAGEVIEW_H__

#include "_Elastos_Droid_Launcher2_CPagedViewWidgetImageView.h"
#include "elastos/droid/widget/ImageView.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Widget::ImageView;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

 CarClass(CPagedViewWidgetImageView)
    , public ImageView
    , public IPagedViewWidgetImageView
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CPagedViewWidgetImageView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI RequestLayout();

    CARAPI GetAllowRequestLayout(
        /* [out] */ Boolean* result);

    CARAPI SetAllowRequestLayout(
        /* [in] */ Boolean result);

protected:
    //@Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

public:
    Boolean mAllowRequestLayout;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CPAGEDVIEWWIDGETIMAGEVIEW_H__