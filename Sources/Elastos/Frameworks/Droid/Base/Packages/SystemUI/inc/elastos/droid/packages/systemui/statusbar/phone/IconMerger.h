#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__

#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class IconMerger;

class SetVisibilityRunnable : public Runnable
{
public:
    SetVisibilityRunnable(
        /* [in] */ Boolean mNoreRequired,
        /* [in] */ IconMerger* host);

    virtual CARAPI Run();
private:
    Boolean mMoreRequired;
    IconMerger* mHost;
};

class IconMerger : public LinearLayout
{
public:
    IconMerger();

    IconMerger(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetOverflowIndicator(
        /* [in] */ IView* v);

    //@Override
    virtual CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI_(void) OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

protected:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI InitIconSize(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);
private:
    CARAPI_(void) CheckOverflow(
        /* [in] */ Int32 width);

private:
    friend class SetVisibilityRunnable;

    Int32 mIconSize;
    AutoPtr<IView> mMoreView;
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__
