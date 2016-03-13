
#include "elastos/droid/launcher2/InfoDropTarget.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(InfoDropTarget, ButtonDropTarget, IInfoDropTarget);

ECode InfoDropTarget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode InfoDropTarget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return ButtonDropTarget::constructor(context, attrs, defStyle);
}

ECode InfoDropTarget::OnFinishInflate()
{
    ButtonDropTarget::OnFinishInflate();

    GetTextColors(&mOriginalTextColor);

    // Get the hover color
    AutoPtr<IResources> r;
    GetResources((IResources**)&r);
    r->GetColor(Elastos::Droid::Launcher2::R::color::info_target_hover_tint,
            &mHoverColor);
    AutoPtr<IDrawable> d = GetCurrentDrawable();
    mDrawable = ITransitionDrawable::Probe(d);
    if (NULL != mDrawable) {
        mDrawable->SetCrossFadeEnabled(TRUE);
    }

    // Remove the text in the Phone UI in landscape
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    Int32 orientation;
    config->GetOrientation(&orientation);
    if (orientation == IConfiguration::ORIENTATION_LANDSCAPE) {
        Boolean res;
        LauncherApplication->IsScreenLarge(7res);
        if (!res) {
            SetText("");
        }
    }
    return NOERROR;
}

Boolean InfoDropTarget::IsFromAllApps(
    /* [in] */ IDragSource* source)
{
    return IAppsCustomizePagedView::Probe(source) != NULL;
}

ECode InfoDropTarget::AcceptDrop(
    /* [in] */ DragObject* d,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // acceptDrop is called just before onDrop. We do the work here, rather than
    // in onDrop, because it allows us to reject the drop (by returning false)
    // so that the object being dragged isn't removed from the drag source.
    AutoPtr<IComponentName> componentName;
    if (IApplicationInfo::Probe(d->mDragInfo) != NULL) {
        AutoPtr<IApplicationInfo> info = IApplicationInfo::Probe(d->mDragInfo);
        info->GetComponentName((IComponentName**)&componentName);
    }
    else if (IShortcutInfo::Probe(d->mDragInfo) != NULL) {
        AutoPtr<IShortcutInfo> info = IShortcutInfo::Probe(d->mDragInfo);
        AutoPtr<IIntent> intent;
        info->GetIntent((IIntent**)&intent);
        intent->GetComponent((IComponentName**)&componentName);
    }
    else if (IPendingAddItemInfo::Probe(d->mDragInfo) != NULL) {
        AutoPtr<IPendingAddItemInfo> info = IPendingAddItemInfo::Probe(d->mDragInfo);
        info->GetComponentName((IComponentName**)&componentName);
    }
    AutoPtr<IUserHandle> user;
    if (IItemInfo::Probe(d->mDragInfo) != NULL) {
        AutoPtr<IItemInfo> info = IItemInfo::Probe(d->mDragInfo);
        info->GetUser((IUserHandle**)&user);
    }
    else {
        Process::MyUserHandle((IUserHandle**)&user);
    }
    if (componentName != NULL) {
        mLauncher->StartApplicationDetailsActivity(componentName, user);
    }

    // There is no post-drop animation, so clean up the DragView now
    d->mDeferDragViewCleanupPostAnimation = FALSE;
    *result = FALSE;
    return NOERROR;
}

ECode InfoDropTarget::OnDragStart(
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* info,
    /* [in] */ Int32 dragAction)
{
    Boolean isVisible = TRUE;

    // Hide this button unless we are dragging something from AllApps
    if (!IsFromAllApps(source)) {
        isVisible = FALSE;
    }

    mActive = isVisible;
    mDrawable->ResetTransition();
    SetTextColor(mOriginalTextColor);

    AutoPtr<IViewParent> parent;
    GetParent((IViewParent**)&parent);
    AutoPtr<IViewGroup> group = IViewGroup::Probe(parent);
    return group->SetVisibility(isVisible ? IView::VISIBLE : IView::GONE);
}

ECode InfoDropTarget::OnDragEnd()
{
    ButtonDropTarget::OnDragEnd();
    mActive = FALSE;
    return NOERROR;
}

ECode InfoDropTarget::OnDragEnter(
    /* [in] */ DragObject* d)
{
    ButtonDropTarget::OnDragEnter(d);

    mDrawable->StartTransition(mTransitionDuration);
    return SetTextColor(mHoverColor);
}

ECode InfoDropTarget::OnDragExit(
    /* [in] */ DragObject* d)
{
    ButtonDropTarget::onDragExit(d);

    if (!d->mDragComplete) {
        mDrawable->ResetTransition();
        return SetTextColor(mOriginalTextColor);
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos