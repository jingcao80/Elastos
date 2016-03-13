#ifndef  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWWIDGETIMAGEVIEW_H__
#define  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWWIDGETIMAGEVIEW_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class PagedViewWidgetImageView
    : public ImageView
    , public IPagedViewWidgetImageView
{
public:
    CAR_INTERFACE_DECL();

    PagedViewWidgetImageView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI RequestLayout();

protected:
    //@Override
    CARAPI OnDraw(
        /* [in] */ ICanvas* canvas);

public:
    Boolean mAllowRequestLayout = TRUE;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWWIDGETIMAGEVIEW_H__