#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__

#include "_SystemUI.h"
#include <elastos/droid/os/Runnable.h>
#include <elastos/droid/widget/LinearLayout.h>

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class IconMerger
    : public LinearLayout
    , public IIconMerger
{
private:
    class IconRunnable : public Runnable
    {
    public:
        IconRunnable(
            /* [in] */ IconMerger* host,
            /* [in] */ Boolean moreRequired);

        // @Override
        CARAPI Run();

    private:
        IconMerger* mHost;
        Boolean mMoreRequired;
    };

public:
    CAR_INTERFACE_DECL();

    IconMerger();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetOverflowIndicator(
        /* [in] */ IView* v);

protected:
    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

private:
    CARAPI_(void) CheckOverflow(
        /* [in] */ Int32 width);

private:
    static const String TAG;
    static const Boolean DEBUG;

    Int32 mIconSize;
    AutoPtr<IView> mMoreView;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__
