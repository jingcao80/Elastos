#include "elastos/droid/systemui/statusbar/policy/AutoRotateController.h"

using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_INTERFACE_IMPL(AutoRotateController, ICompoundButtonOnCheckedChangeListener)

AutoRotateController::AutoRotateController(
    /* [in] */ IContext* context,
    /* [in] */ ICompoundButton* checkbox,
    /* [in] */ IRotationLockCallbacks* callbacks)
    : mContext(context)
    , mCheckbox(checkbox)
    , mCallbacks(callbacks)
{
    mCheckbox->SetOnCheckedChangeListener(this);

    assert(0 && "TODO");
    // RotationPolicy::RegisterRotationPolicyListener(
    //     context,
    //     mRotationPolicyListener, UserHandle::USER_ALL);
    UpdateState();
}

ECode AutoRotateController::OnCheckedChanged(
    /* [in] */ ICompoundButton* view,
    /* [in] */ Boolean checked)
{
    if (checked != mAutoRotation) {
        mAutoRotation = checked;
        assert(0 && "TODO");
        // RotationPolicy::SetRotationLock(mContext, !checked);
    }
    return NOERROR;
}

ECode AutoRotateController::UnRegister() /* Release() */
{
    assert(0 && "TODO");
    // RotationPolicy.unregisterRotationPolicyListener(mContext,
    //         mRotationPolicyListener);
    return NOERROR;
}

ECode AutoRotateController::UpdateState()
{
    assert(0 && "TODO");
    // mAutoRotation = !RotationPolicy.isRotationLocked(mContext);
    // AutoPtr<ICheckable> checkable = ICheckable::Probe(mCheckbox.Get());
    // checkable->SetChecked(mAutoRotation);

    // Boolean visible = RotationPolicy.isRotationLockToggleVisible(mContext);
    // //mCallbacks->SetRotationLockControlVisibility(visible);
    // mCheckbox->SetEnabled(visible);
    return NOERROR;
}


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
