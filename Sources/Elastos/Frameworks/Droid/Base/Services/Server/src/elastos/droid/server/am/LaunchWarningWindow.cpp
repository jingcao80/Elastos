#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/server/am/LaunchWarningWindow.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

ECode LaunchWarningWindow::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Dialog::OnKeyDown(keyCode, event, result);
    return NOERROR;
}

ECode LaunchWarningWindow::OnKeyLongPress(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Dialog::OnKeyLongPress(keyCode, event, result);
    return NOERROR;
}

ECode LaunchWarningWindow::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Dialog::OnKeyUp(keyCode, event, result);
    return NOERROR;
}

ECode LaunchWarningWindow::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 count,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Dialog::OnKeyMultiple(keyCode, count, event, result);
    return NOERROR;
}

LaunchWarningWindow::LaunchWarningWindow(
    /* [in] */ IContext* context,
    /* [in] */ ActivityRecord* cur,
    /* [in] */ ActivityRecord* next)
{
    Dialog::constructor(context, R::style::Theme_Toast);
    Boolean result;
    Boolean requestWF;
    Dialog::RequestWindowFeature(IWindow::FEATURE_LEFT_ICON, &requestWF);
    AutoPtr<IWindow> window;
    Dialog::GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    window->AddFlags(IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
            | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE);

    Dialog::SetContentView(R::layout::launch_warning);

    AutoPtr<ICharSequence> title;
    context->GetText(R::string::launch_warning_title,
            (ICharSequence**)&title);
    Dialog::SetTitle(title);

    AutoPtr<ITypedValue> out;
    CTypedValue::New((ITypedValue**)&out);
    AutoPtr<IResourcesTheme> theme;
    AutoPtr<IContext> dialogContext;
    dialogContext->GetTheme((IResourcesTheme**)&theme);
    theme->ResolveAttribute(R::attr::alertDialogIcon, out, TRUE, &result);
    Int32 resourceId;
    out->GetResourceId(&resourceId);
    window->SetFeatureDrawableResource(IWindow::FEATURE_LEFT_ICON, resourceId);

    AutoPtr<IView> view;
    Dialog::FindViewById(R::id::replace_app_icon, (IView**)&view);
    AutoPtr<IImageView> icon = IImageView::Probe(view);
    AutoPtr<IPackageManager> pkgManager;
    context->GetPackageManager((IPackageManager**)&pkgManager);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(next->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    AutoPtr<IDrawable> drawable;
    IPackageItemInfo::Probe(appInfo)->LoadIcon(pkgManager, (IDrawable**)&drawable);
    icon->SetImageDrawable(drawable);

    view = NULL;
    Dialog::FindViewById(R::id::replace_message, (IView**)&view);
    AutoPtr<ITextView> text = ITextView::Probe(view);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ICharSequence> label;
    IPackageItemInfo::Probe(appInfo)->LoadLabel(pkgManager, (ICharSequence**)&label);
    AutoPtr<ArrayOf<IInterface*> > labelcs = ArrayOf<IInterface*>::Alloc(1);
    labelcs->Set(0, label);
    String s;
    res->GetString(R::string::launch_warning_replace, labelcs, &s);
    AutoPtr<ICharSequence> ss;
    CString::New(s, (ICharSequence**)&ss);
    text->SetText(ss);

    view = NULL;
    Dialog::FindViewById(R::id::original_app_icon, (IView**)&view);
    icon = IImageView::Probe(view);
    AutoPtr<IApplicationInfo> curAppInfo;
    IComponentInfo::Probe(cur->mInfo)->GetApplicationInfo((IApplicationInfo**)&curAppInfo);
    AutoPtr<IDrawable> curDraw;
    IPackageItemInfo::Probe(curAppInfo)->LoadIcon(pkgManager, (IDrawable**)&curDraw);
    icon->SetImageDrawable(curDraw);
    view = NULL;
    Dialog::FindViewById(R::id::original_message, (IView**)&view);
    text = ITextView::Probe(view);
    AutoPtr<ICharSequence> curLabel;
    IPackageItemInfo::Probe(curAppInfo)->LoadLabel(pkgManager, (ICharSequence**)&curLabel);
    AutoPtr<ArrayOf<IInterface*> > curLabelcs = ArrayOf<IInterface*>::Alloc(1);
    curLabelcs->Set(0, curLabel);
    res->GetString(R::string::launch_warning_original, curLabelcs, &s);
    ss = NULL;
    CString::New(s, (ICharSequence**)&ss);
    text->SetText(ss);
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
