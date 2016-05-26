
#include "elastos/droid/launcher2/CInfoDropTarget.h"
#include "elastos/droid/launcher2/ApplicationInfo.h"
#include "elastos/droid/launcher2/PendingAddItemInfo.h"
#include "elastos/droid/launcher2/ShortcutInfo.h"
#include "elastos/droid/launcher2/ItemInfo.h"
#include "elastos/droid/launcher2/LauncherApplication.h"
#include "elastos/droid/os/Process.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(CInfoDropTarget, ButtonDropTarget, IInfoDropTarget);

CAR_OBJECT_IMPL(CInfoDropTarget)

ECode CInfoDropTarget::OnFinishInflate()
{
    ButtonDropTarget::OnFinishInflate();

    GetTextColors((IColorStateList**)&mOriginalTextColor);

    // Get the hover color
    AutoPtr<IResources> r;
    GetResources((IResources**)&r);
    r->GetColor(Elastos::Droid::Launcher2::R::color::info_target_hover_tint,
            &mHoverColor);
    AutoPtr<IDrawable> d;
    GetCurrentDrawable((IDrawable**)&d);
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
        LauncherApplication::IsScreenLarge(&res);
        if (!res) {
            AutoPtr<ICharSequence> text = CoreUtils::Convert(String(""));
            SetText(text);
        }
    }
    return NOERROR;
}

Boolean CInfoDropTarget::IsFromAllApps(
    /* [in] */ IDragSource* source)
{
    return IAppsCustomizePagedView::Probe(source) != NULL;
}

ECode CInfoDropTarget::AcceptDrop(
    /* [in] */ IDropTargetDragObject* d,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // acceptDrop is called just before onDrop. We do the work here, rather than
    // in onDrop, because it allows us to reject the drop (by returning false)
    // so that the object being dragged isn't removed from the drag source.
    AutoPtr<IComponentName> componentName;
    DragObject* _d = (DragObject*)d;
    if (IApplicationInfo::Probe(_d->mDragInfo) != NULL) {
        ApplicationInfo* info = (ApplicationInfo*)IApplicationInfo::Probe(_d->mDragInfo);
        componentName = info->mComponentName;
    }
    else if (IShortcutInfo::Probe(_d->mDragInfo) != NULL) {
        ShortcutInfo* info = (ShortcutInfo*)IShortcutInfo::Probe(_d->mDragInfo);
        info->mIntent->GetComponent((IComponentName**)&componentName);
    }
    else if (IPendingAddItemInfo::Probe(_d->mDragInfo) != NULL) {
        PendingAddItemInfo* info = (PendingAddItemInfo*)IPendingAddItemInfo::Probe(_d->mDragInfo);
        componentName = info->mComponentName;
    }
    AutoPtr<IUserHandle> user;
    if (IItemInfo::Probe(_d->mDragInfo) != NULL) {
        ItemInfo* info = (ItemInfo*)IItemInfo::Probe(_d->mDragInfo);
        user = info->mUser;
    }
    else {
        Process::MyUserHandle((IUserHandle**)&user);
    }
    if (componentName != NULL) {
        mLauncher->StartApplicationDetailsActivity(componentName, user);
    }

    // There is no post-drop animation, so clean up the DragView now
    _d->mDeferDragViewCleanupPostAnimation = FALSE;
    *result = FALSE;
    return NOERROR;
}

ECode CInfoDropTarget::OnDragStart(
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
    return IView::Probe(group)->SetVisibility(isVisible ? IView::VISIBLE : IView::GONE);
}

ECode CInfoDropTarget::OnDragEnd()
{
    ButtonDropTarget::OnDragEnd();
    mActive = FALSE;
    return NOERROR;
}

ECode CInfoDropTarget::OnDragEnter(
    /* [in] */ IDropTargetDragObject* d)
{
    ButtonDropTarget::OnDragEnter(d);

    mDrawable->StartTransition(mTransitionDuration);
    return SetTextColor(mHoverColor);
}

ECode CInfoDropTarget::OnDragExit(
    /* [in] */ IDropTargetDragObject* d)
{
    ButtonDropTarget::OnDragExit(d);

    DragObject* _d = (DragObject*)d;
    if (!_d->mDragComplete) {
        mDrawable->ResetTransition();
        return SetTextColor(mOriginalTextColor);
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos