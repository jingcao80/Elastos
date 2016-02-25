
#include "Elastos.Droid.Core_server.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::SystemUI::StatusBar::Policy::IRotationLockCallbacks;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class AutoRotateController
    : public ElRefBase
    , public ICompoundButtonOnCheckedChangeListener
{
private:

    // private final RotationPolicy.RotationPolicyListener mRotationPolicyListener =
    //         new RotationPolicy.RotationPolicyListener() {
    //     @Override
    //     public void onChange() {
    //         updateState();
    //     }
    // };
public:
    CAR_INTERFACE_DECL()

    AutoRotateController(
        /* [in] */ IContext* context,
        /* [in] */ ICompoundButton* checkbox,
        /* [in] */ IRotationLockCallbacks* callbacks);

    CARAPI OnCheckedChanged(
        /* [in] */ ICompoundButton* view,
        /* [in] */ Boolean checked);

    CARAPI UnRegister(); /* Release() */

    CARAPI UpdateState();

private:
    AutoPtr<IContext> mContext;
    AutoPtr<ICompoundButton> mCheckbox;
    AutoPtr<IRotationLockCallbacks> mCallbacks;

    Boolean mAutoRotation;
};


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
