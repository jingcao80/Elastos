
#include "elastos/droid/server/am/LaunchWarningWindow.h"
#include "elastos/droid/R.h"

using Elastos::Core::CString;
using Elastos::Droid::App::EIID_IDialog;
using Elastos::Droid::View::EIID_IWindowCallback;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::EIID_IViewOnCreateContextMenuListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Content::EIID_IDialogInterface;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;


namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

IDIALOG_METHODS_IMPL(LaunchWarningWindow, Dialog);

IWINDOWCALLBACK_METHODS_IMPL(LaunchWarningWindow, Dialog);

ECode LaunchWarningWindow::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Dialog::OnKeyDown(keyCode, event);
    return NOERROR;
}

ECode LaunchWarningWindow::OnKeyLongPress(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Dialog::OnKeyLongPress(keyCode, event);
    return NOERROR;
}

ECode LaunchWarningWindow::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Dialog::OnKeyUp(keyCode, event);
    return NOERROR;
}

ECode LaunchWarningWindow::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 count,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Dialog::OnKeyMultiple(keyCode, count, event);
    return NOERROR;
}

LaunchWarningWindow::LaunchWarningWindow(
    /* [in] */ IContext* context,
    /* [in] */ ActivityRecord* cur,
    /* [in] */ ActivityRecord* next)
    : Dialog(context, R::style::Theme_Toast)
{
    Boolean result;
    Dialog::RequestWindowFeature(IWindow::FEATURE_LEFT_ICON);
    Dialog::GetWindow()->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    Dialog::GetWindow()->AddFlags(IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
            | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE);

    Dialog::SetContentView(R::layout::launch_warning);

    AutoPtr<ICharSequence> title;
    context->GetText(R::string::launch_warning_title,
            (ICharSequence**)&title);
    Dialog::SetTitle(title);

    AutoPtr<ITypedValue> out;
    CTypedValue::New((ITypedValue**)&out);
    AutoPtr<IResourcesTheme> theme;
    Dialog::GetContext()->GetTheme((IResourcesTheme**)&theme);
    theme->ResolveAttribute(R::attr::alertDialogIcon, out, TRUE, &result);
    Int32 resourceId;
    out->GetResourceId(&resourceId);
    Dialog::GetWindow()->SetFeatureDrawableResource(IWindow::FEATURE_LEFT_ICON, resourceId);

    AutoPtr<IView> view = Dialog::FindViewById(R::id::replace_app_icon);
    AutoPtr<IImageView> icon = IImageView::Probe(view);
    AutoPtr<IPackageManager> pkgManager;
    context->GetPackageManager((IPackageManager**)&pkgManager);
    AutoPtr<IApplicationInfo> appInfo;
    next->mInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
    AutoPtr<IDrawable> drawable;
    appInfo->LoadIcon(pkgManager, (IDrawable**)&drawable);
    icon->SetImageDrawable(drawable);

    view = Dialog::FindViewById(R::id::replace_message);
    AutoPtr<ITextView> text = ITextView::Probe(view);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ICharSequence> label;
    appInfo->LoadLabel(pkgManager, (ICharSequence**)&label);
    AutoPtr<ArrayOf<IInterface*> > labelcs = ArrayOf<IInterface*>::Alloc(1);
    labelcs->Set(0, label);
    String s;
    res->GetString(R::string::launch_warning_replace, labelcs, &s);
    AutoPtr<ICharSequence> ss;
    CString::New(s, (ICharSequence**)&ss);
    text->SetText(ss);

    view = Dialog::FindViewById(R::id::original_app_icon);
    icon = IImageView::Probe(view);
    AutoPtr<IApplicationInfo> curAppInfo;
    cur->mInfo->GetApplicationInfo((IApplicationInfo**)&curAppInfo);
    AutoPtr<IDrawable> curDraw;
    curAppInfo->LoadIcon(pkgManager, (IDrawable**)&curDraw);
    icon->SetImageDrawable(curDraw);
    view = Dialog::FindViewById(R::id::original_message);
    text = ITextView::Probe(view);
    AutoPtr<ICharSequence> curLabel;
    curAppInfo->LoadLabel(pkgManager, (ICharSequence**)&curLabel);
    AutoPtr<ArrayOf<IInterface*> > curLabelcs = ArrayOf<IInterface*>::Alloc(1);
    curLabelcs->Set(0, curLabel);
    res->GetString(R::string::launch_warning_original, curLabelcs, &s);
    ss = NULL;
    CString::New(s, (ICharSequence**)&ss);
    text->SetText(ss);
}

PInterface LaunchWarningWindow::Probe(
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

UInt32 LaunchWarningWindow::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 LaunchWarningWindow::Release()
{
    return ElRefBase::Release();
}

ECode LaunchWarningWindow::GetInterfaceID(
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

ECode LaunchWarningWindow::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    return Dialog::OnCreateContextMenu(menu, v, menuInfo);
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
