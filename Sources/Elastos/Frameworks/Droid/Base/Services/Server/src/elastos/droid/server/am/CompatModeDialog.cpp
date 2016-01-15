
#include "elastos/droid/server/am/CompatModeDialog.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::App::EIID_IDialog;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::EIID_IWindowCallback;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::EIID_IViewOnCreateContextMenuListener;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Content::EIID_IDialogInterface;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

IDIALOG_METHODS_IMPL(CompatModeDialog, Dialog);

IWINDOWCALLBACK_METHODS_IMPL(CompatModeDialog, Dialog);

ECode CompatModeDialog::CompatEnabledListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    AutoLock lock(mHost->mService->mLock);
    String appInfoPkgName;
    mHost->mAppInfo->GetPackageName(&appInfoPkgName);
    Boolean checked;
    ICheckable::Probe(mHost->mCompatEnabled)->IsChecked(&checked);
    mHost->mService->mCompatModePackages->SetPackageScreenCompatModeLocked(
            appInfoPkgName,
            checked ? IActivityManager::COMPAT_MODE_ENABLED
                    : IActivityManager::COMPAT_MODE_DISABLED);
    mHost->UpdateControls();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CompatModeDialog::CompatEnabledListener, ICompoundButtonOnCheckedChangeListener);

ECode CompatModeDialog::AlwaysShowListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    AutoLock lock(mHost->mService->mLock);
    String appInfoPkgName;
    mHost->mAppInfo->GetPackageName(&appInfoPkgName);
    Boolean checked;
    ICheckable::Probe(mHost->mAlwaysShow)->IsChecked(&checked);
    mHost->mService->mCompatModePackages->SetPackageAskCompatModeLocked(
            appInfoPkgName, checked);
    mHost->UpdateControls();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CompatModeDialog::AlwaysShowListener, ICompoundButtonOnCheckedChangeListener);


CompatModeDialog::CompatModeDialog(
    /* [in] */ CActivityManagerService* service,
    /* [in] */ IContext* context,
    /* [in] */ IApplicationInfo* appInfo)
    : Dialog(context, R::style::Theme_Holo_Dialog_MinWidth)
    , mService(service)
    , mAppInfo(appInfo)
{
    SetCancelable(TRUE);
    SetCanceledOnTouchOutside(TRUE);
    Boolean hasFeature;
    Dialog::GetWindow()->RequestFeature(IWindow::FEATURE_NO_TITLE, &hasFeature);
    Dialog::GetWindow()->SetType(IWindowManagerLayoutParams::TYPE_PHONE);
    Dialog::GetWindow()->SetGravity(IGravity::BOTTOM | IGravity::CENTER_HORIZONTAL);

    SetContentView(R::layout::am_compat_mode_dialog);
    AutoPtr<IView> view = Dialog::FindViewById(R::id::compat_checkbox);
    mCompatEnabled = ISwitch::Probe(view);
    AutoPtr<ICompoundButtonOnCheckedChangeListener> enabledListener = new CompatEnabledListener(this);
    mCompatEnabled->SetOnCheckedChangeListener(enabledListener);

    view = Dialog::FindViewById(R::id::ask_checkbox);
    mAlwaysShow = ICheckBox::Probe(view);
    AutoPtr<ICompoundButtonOnCheckedChangeListener> showListener = new AlwaysShowListener(this);
    mAlwaysShow->SetOnCheckedChangeListener(showListener);
    mHint = Dialog::FindViewById(R::id::reask_hint);

    UpdateControls();
}

PInterface CompatModeDialog::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IDialog) {
        return (PInterface)(IDialog*)this;
    }
    else if (riid == EIID_IDialogInterface) {
        return (PInterface)(IDialogInterface*)this;
    }
    else if (riid == EIID_IWindowCallback) {
        return (PInterface)(IWindowCallback*)this;
    }
    if (riid == EIID_IKeyEventCallback) {
        return (PInterface)(IKeyEventCallback*)this;
    }
    else if (riid == EIID_IViewOnCreateContextMenuListener) {
       return (PInterface)(IViewOnCreateContextMenuListener*)this;
    }

    return NULL;
}

UInt32 CompatModeDialog::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CompatModeDialog::Release()
{
    return ElRefBase::Release();
}

ECode CompatModeDialog::GetInterfaceID(
    /* [in] */ IInterface *object,
    /* [out] */ InterfaceID *iid)
{
    VALIDATE_NOT_NULL(iid);
    if (object == (IInterface*)(IDialog *)this) {
        *iid = EIID_IDialog;
    }
    else if (object == (IInterface*)(IDialogInterface *)this) {
        *iid = EIID_IDialogInterface;
    }
    else if (object == (IInterface*)(IWindowCallback *)this) {
        *iid = EIID_IWindowCallback;
    }
    else if (object == (IInterface*)(IKeyEventCallback *)this) {
        *iid = EIID_IKeyEventCallback;
    }
    else if (object == (IInterface*)(IViewOnCreateContextMenuListener *)this) {
        *iid = EIID_IViewOnCreateContextMenuListener;
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;

}

ECode CompatModeDialog::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Dialog::OnKeyDown(keyCode, event);
    return NOERROR;
}

ECode CompatModeDialog::OnKeyLongPress(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Dialog::OnKeyLongPress(keyCode, event);
    return NOERROR;
}

ECode CompatModeDialog::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Dialog::OnKeyUp(keyCode, event);
    return NOERROR;
}

ECode CompatModeDialog::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 count,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Dialog::OnKeyMultiple(keyCode, count, event);
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
    AutoLock lock(mService->mLock);
    Int32 mode = mService->mCompatModePackages->ComputeCompatModeLocked(mAppInfo);
    ICheckable::Probe(mCompatEnabled)->SetChecked(mode == IActivityManager::COMPAT_MODE_ENABLED);
    String appInfoPkgName;
    mAppInfo->GetPackageName(&appInfoPkgName);
    Boolean ask = mService->mCompatModePackages->GetPackageAskCompatModeLocked(appInfoPkgName);
    ICheckable::Probe(mAlwaysShow)->SetChecked(ask);
    mHint->SetVisibility(ask ? IView::INVISIBLE : IView::VISIBLE);
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
