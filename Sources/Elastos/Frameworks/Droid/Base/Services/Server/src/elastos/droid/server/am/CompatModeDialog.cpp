
#include "elastos/droid/server/am/CompatModeDialog.h"
#include "elastos/droid/R.h"
#include "elastos/core/AutoLock.h"

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::R;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL(CompatModeDialog::CompatEnabledListener, Object, ICompoundButtonOnCheckedChangeListener)

ECode CompatModeDialog::CompatEnabledListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    AutoLock lock(mHost->mService);
    String appInfoPkgName;
    IPackageInfo::Probe(mHost->mAppInfo)->GetPackageName(&appInfoPkgName);
    Boolean checked;
    ICheckable::Probe(mHost->mCompatEnabled)->IsChecked(&checked);
    mHost->mService->mCompatModePackages->SetPackageScreenCompatModeLocked(
            appInfoPkgName,
            checked ? IActivityManager::COMPAT_MODE_ENABLED
                    : IActivityManager::COMPAT_MODE_DISABLED);
    mHost->UpdateControls();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CompatModeDialog::AlwaysShowListener, Object, ICompoundButtonOnCheckedChangeListener)

ECode CompatModeDialog::AlwaysShowListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    AutoLock lock(mHost->mService);
    String appInfoPkgName;
    IPackageInfo::Probe(mHost->mAppInfo)->GetPackageName(&appInfoPkgName);
    Boolean checked;
    ICheckable::Probe(mHost->mAlwaysShow)->IsChecked(&checked);
    mHost->mService->mCompatModePackages->SetPackageAskCompatModeLocked(
            appInfoPkgName, checked);
    mHost->UpdateControls();
    return NOERROR;
}

CompatModeDialog::CompatModeDialog(
    /* [in] */ CActivityManagerService* service,
    /* [in] */ IContext* context,
    /* [in] */ IApplicationInfo* appInfo)
    : mService(service)
    , mAppInfo(appInfo)
{
    Dialog::constructor(context, R::style::Theme_Holo_Dialog_MinWidth);
    SetCancelable(TRUE);
    SetCanceledOnTouchOutside(TRUE);
    Boolean hasFeature;
    AutoPtr<IWindow> window;
    Dialog::GetWindow((IWindow**)&window);
    window->RequestFeature(IWindow::FEATURE_NO_TITLE, &hasFeature);
    window->SetType(IWindowManagerLayoutParams::TYPE_PHONE);
    window->SetGravity(IGravity::BOTTOM | IGravity::CENTER_HORIZONTAL);

    SetContentView(R::layout::am_compat_mode_dialog);
    AutoPtr<IView> view;
    Dialog::FindViewById(R::id::compat_checkbox, (IView**)&view);
    mCompatEnabled = ISwitch::Probe(view);
    AutoPtr<ICompoundButtonOnCheckedChangeListener> enabledListener = new CompatEnabledListener(this);
    ICompoundButton::Probe(mCompatEnabled)->SetOnCheckedChangeListener(enabledListener);

    view = NULL;
    Dialog::FindViewById(R::id::ask_checkbox, (IView**)&view);
    mAlwaysShow = ICheckBox::Probe(view);
    AutoPtr<ICompoundButtonOnCheckedChangeListener> showListener = new AlwaysShowListener(this);
    ICompoundButton::Probe(mAlwaysShow)->SetOnCheckedChangeListener(showListener);
    Dialog::FindViewById(R::id::reask_hint, (IView**)&mHint);

    UpdateControls();
}

ECode CompatModeDialog::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Dialog::OnKeyDown(keyCode, event, result);
    return NOERROR;
}

ECode CompatModeDialog::OnKeyLongPress(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Dialog::OnKeyLongPress(keyCode, event, result);
    return NOERROR;
}

ECode CompatModeDialog::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Dialog::OnKeyUp(keyCode, event, result);
    return NOERROR;
}

ECode CompatModeDialog::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 count,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Dialog::OnKeyMultiple(keyCode, count, event, result);
    return NOERROR;
}

ECode CompatModeDialog::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    return Dialog::OnCreateContextMenu(menu, v, menuInfo);
}

void CompatModeDialog::UpdateControls()
{
    AutoLock lock(mService);
    Int32 mode = mService->mCompatModePackages->ComputeCompatModeLocked(mAppInfo);
    ICheckable::Probe(mCompatEnabled)->SetChecked(mode == IActivityManager::COMPAT_MODE_ENABLED);
    String appInfoPkgName;
    IPackageInfo::Probe(mAppInfo)->GetPackageName(&appInfoPkgName);
    Boolean ask = mService->mCompatModePackages->GetPackageAskCompatModeLocked(appInfoPkgName);
    ICheckable::Probe(mAlwaysShow)->SetChecked(ask);
    mHint->SetVisibility(ask ? IView::INVISIBLE : IView::VISIBLE);
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
