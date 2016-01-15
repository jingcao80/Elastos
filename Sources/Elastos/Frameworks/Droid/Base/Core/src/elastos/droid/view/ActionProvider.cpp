
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/ActionProvider.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(ActionProvider, Object, IActionProvider)

ActionProvider::ActionProvider()
{
}

ActionProvider::~ActionProvider()
{}

ECode ActionProvider::constructor(
    /* [in] */ IContext* ctx)
{
    return NOERROR;
}

ECode ActionProvider::OnCreateActionView(
    /* [in] */ IMenuItem* forItem,
    /* [out] */ IView** view)
{
    return OnCreateActionView(view);
}

ECode ActionProvider::OverridesItemVisibility(
    /* [out] */ Boolean* visibility)
{
    VALIDATE_NOT_NULL(visibility)
    *visibility = FALSE;
    return NOERROR;
}

ECode ActionProvider::IsVisible(
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible)
    *visible = TRUE;
    return NOERROR;
}

ECode ActionProvider::OnPerformDefaultAction(
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = FALSE;
    return NOERROR;
}

ECode ActionProvider::HasSubMenu(
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = FALSE;
    return NOERROR;
}

ECode ActionProvider::RefreshVisibility()
{
    Boolean bval;
    OverridesItemVisibility(&bval);
    if (mVisibilityListener != NULL && bval) {
        IsVisible(&bval);
        mVisibilityListener->OnActionProviderVisibilityChanged(bval);
    }
    return NOERROR;
}

ECode ActionProvider::OnPrepareSubMenu(
    /* [in] */ ISubMenu* subMenu)
{
    return NOERROR;
}

ECode ActionProvider::SubUiVisibilityChanged(
    /* [in] */ Boolean isVisible)
{
    if (mSubUiVisibilityListener != NULL) {
        mSubUiVisibilityListener->OnSubUiVisibilityChanged(isVisible);
    }
    return NOERROR;
}

ECode ActionProvider::SetSubUiVisibilityListener(
    /* [in] */ ISubUiVisibilityListener* listener)
{
    mSubUiVisibilityListener = listener;
    return NOERROR;
}

ECode ActionProvider::SetVisibilityListener(
    /* [in] */ IVisibilityListener* listener)
{
    if (mVisibilityListener != NULL) {
        // Log.w(TAG, "setVisibilityListener: Setting a new ActionProvider.VisibilityListener " +
        //         "when one is already set. Are you reusing this " + getClass().getSimpleName() +
        //         " instance while it is still in use somewhere else?");
    }
    mVisibilityListener = listener;
    return NOERROR;
}


} // namespace View
} // namespace Droid
} // namespace Elastos
