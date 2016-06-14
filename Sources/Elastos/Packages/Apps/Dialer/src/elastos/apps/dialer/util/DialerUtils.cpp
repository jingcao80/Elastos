
#include "DialerUtils.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Util {

void DialerUtils::StartActivityWithErrorToast(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    StartActivityWithErrorToast(context, intent, R::string::activity_not_available);
}

void DialerUtils::StartActivityWithErrorToast(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 msgId)
{
    // try {
    ECode ec = NOERROR;
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_CALL.Equals(action)
            || IIntent::ACTION_CALL_PRIVILEGED.Equals(action)) {
        // All dialer-initiated calls should pass the touch point to the InCallUI
        AutoPtr<IPoint> touchPoint;
        assert(0 && "TODO");
        // TouchPointManager::GetInstance()->GetPoint((IPoint**)&touchPoint);
        Int32 x, y;
        touchPoint->GetX(&x);
        touchPoint->GetY(&y);
        if (x != 0 || y != 0) {
            AutoPtr<IBundle> extras;
            CBundle::New((IBundle**)&extras);
            extras->PutParcelable(ITouchPointManager::TOUCH_POINT, touchPoint);
            intent->PutExtra(ITelecomManager::EXTRA_OUTGOING_CALL_EXTRAS, extras);
        }

        ec = IActivity::Probe(context)->StartActivityForResult(intent, 0);
        if (FAILED(ec)) goto exit;
    }
    else {
        ec = context->StartActivity(intent);
        if (FAILED(ec)) goto exit;
    }
    // } catch (ActivityNotFoundException e) {
    //     Toast.makeText(context, msgId, Toast.LENGTH_SHORT).show();
    // }
exit:
    if (ec == E_ACTIVITY_NOT_FOUND_EXCEPTION) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText(context, msgId, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
    }
}

AutoPtr<IComponentName> DialerUtils::GetSmsComponent(
    /* [in] */ IContext* context)
{
    String smsPackage;
    assert(0 && "TODO");
    // Telephony.Sms.getDefaultSmsPackage(context);
    if (!smsPackage.IsNull()) {
        AutoPtr<IPackageManager> packageManager;
        context->GetPackageManager((IPackageManager**)&packageManager);
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_SENDTO,
                Uri::FromParts(IContactsUtils::SCHEME_SMSTO, String(""), NULL),
                (IIntent**)&intent);
        AutoPtr<IList> resolveInfos;
        packageManager->QueryIntentActivities(intent, 0, (IList**)&resolveInfos);

        AutoPtr<IIterator> it;
        resolveInfos->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while(it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> resolveInfo;
            it->GetNext((IInterface**)&resolveInfo);
            AutoPtr<IActivityInfo> activityInfo;
            IResolveInfo::Probe(resolveInfo)->GetActivityInfo(
                    (IActivityInfo**)&activityInfo);
            String packageName;
            IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
            if (smsPackage.Equals(packageName)) {
                String name;
                IPackageItemInfo::Probe(activityInfo)->GetName(&name);
                AutoPtr<IComponentName> componentName;
                CComponentName::New(smsPackage, name, (IComponentName**)&componentName);
                return componentName;
            }
        }
    }
    return NULL;
}

void DialerUtils::ConfigureEmptyListView(
    /* [in] */ IView* emptyListView,
    /* [in] */ Int32 imageResId,
    /* [in] */ Int32 strResId,
    /* [in] */ IResources* res)
{
    AutoPtr<IView> view;
    emptyListView->FindViewById(R::id::emptyListViewImage, (IView**)&view);
    IImageView* emptyListViewImage = IImageView::Probe(view);

    AutoPtr<IDrawable> drawable;
    res->GetDrawable(imageResId, (IDrawable**)&drawable)
    emptyListViewImage->SetImageDrawable(drawable);
    String str;
    res->GetString(strResId, &str);
    emptyListViewImage->SetContentDescription(str);

    AutoPtr<IView> emptyListViewMessage;
    emptyListView->FindViewById(R::id::emptyListViewMessage,
            (IView**)&emptyListViewMessage);
    ITextView::Probe(emptyListViewMessage)->SetText(str);
}

// TODO:
// ECode DialerUtils::CloseQuietly(
//     /* [in] */ IAutoCloseable closeable)
// {
//     if (closeable != NULL) {
//         // try {
//         ECode ec = closeable->Close();
//         if (ec == (ECode)E_RUNTIME_EXCEPTION) {
//             return ec;
//         }
//         // } catch (RuntimeException rethrown) {
//         //     throw rethrown;
//         // } catch (Exception ignored) {
//         // }
//     }
//     return NOERROR;
// }

AutoPtr<ICharSequence> DialerUtils::Join(
    /* [in] */ IResources* resources,
    /* [in] */ IIterable* list)
{
    String separator;
    resources->GetString(R::string::list_delimeter, &separator);
    return CoreUtils::Convert(TextUtils::Join(
            CoreUtils::Convert(separator), list));
}

Boolean DialerUtils::IsRtl()
{
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> defaultLocale;
    helper->GetDefault((ILocale**)&defaultLocale);
    return TextUtils::GetLayoutDirectionFromLocale(defaultLocale) ==
            IView::LAYOUT_DIRECTION_RTL;
}

void DialerUtils::ShowInputMethod(
    /* [in] */ IView* view)
{
    AutoPtr<IContext> context;
    view->GetContext((IContext**)&context);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&service);
    if (imm != NULL) {
        IInputMethodManager::Probe(imm)->ShowSoftInput(view, 0);
    }
}

void DialerUtils::HideInputMethod(
    /* [in] */ IView* view)
{
    AutoPtr<IContext> context;
    view->GetContext((IContext**)&context);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&service);
    if (imm != NULL) {
        AutoPtr<IBinder> token;
        view->GetWindowToken((IBinder**)&token);
        IInputMethodManager::Probe(imm)->HideSoftInputFromWindow(token, 0);
    }
}

} // Util
} // Dialer
} // Apps
} // Elastos
