
#include "elastos/droid/internal/app/AlertActivity.h"
#include "elastos/droid/internal/app/CAlertControllerAlertParams.h"
#include "elastos/droid/internal/app/CAlertController.h"

using Elastos::Droid::Content::EIID_IDialogInterface;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL_2(AlertActivity, Activity, IAlertActivity, IDialogInterface)

ECode AlertActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    CAlertController::New(this, this, GetWindow(), (IAlertController**)&mAlert);
    CAlertControllerAlertParams::New(this, (IAlertControllerAlertParams**)&mAlertParams);
    return NOERROR;
}

ECode AlertActivity::Cancel()
{
    return Finish();
}

ECode AlertActivity::Dismiss()
{
    // This is called after the click, since we finish when handling the
    // click, don't do that again here.
    Boolean finished;
    IsFinishing(&finished);
    if (!finished) {
        Finish();
    }
    return NOERROR;
}

void AlertActivity::SetupAlert()
{
    mAlertParams->Apply(mAlert);
    mAlert->InstallContent();
}

ECode AlertActivity::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* keyDown)
{
    Boolean result;
    mAlert->OnKeyDown(keyCode, event, &result);
    if (result) {
        *keyDown = TRUE;
        return NOERROR;
    }
    return Activity::OnKeyDown(keyCode, event, keyDown);
}

ECode AlertActivity::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* keyUp)
{
    Boolean result;
    mAlert->OnKeyUp(keyCode, event, &result);
    if (result) {
        *keyUp = TRUE;
        return NOERROR;
    }
    return Activity::OnKeyUp(keyCode, event, keyUp);
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
