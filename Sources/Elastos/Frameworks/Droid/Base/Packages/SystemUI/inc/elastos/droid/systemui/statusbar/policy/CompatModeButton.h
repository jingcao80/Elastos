
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_COMPATMODEBUTTON_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_COMPATMODEBUTTON_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/widget/ImageView.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class CompatModeButton : public ImageView
{
public:
    CompatModeButton();

    CompatModeButton(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CompatModeButton(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI Refresh();

protected:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI InitImpl(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);
private:
    static const Boolean DEBUG;
    AutoPtr<IActivityManager> mAM;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_COMPATMODEBUTTON_H__
