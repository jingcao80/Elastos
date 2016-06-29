#ifndef __ELASTOS_APPS_DIALER_LIST_REMOVEVIEW_H__
#define __ELASTOS_APPS_DIALER_LIST_REMOVEVIEW_H__

#include "_Elastos.Apps.Dialer.h"
#include <elastos/droid/widget/FrameLayout.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IDragEvent;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

class RemoveView
    : public FrameLayout
    , public IRemoveView
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetDragDropController(
        /* [in] */ IDragDropController* controller);

    // @Override
    CARAPI OnDragEvent(
        /* [in] */ IDragEvent* event,
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(void) SetAppearanceNormal();

    CARAPI_(void) SetAppearanceHighlighted();

public:
    AutoPtr<IDragDropController> mDragDropController;
    AutoPtr<ITextView> mRemoveText;
    AutoPtr<IImageView> mRemoveIcon;
    Int32 mUnhighlightedColor;
    Int32 mHighlightedColor;
    AutoPtr<IDrawable> mRemoveDrawable;
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_REMOVEVIEW_H__
