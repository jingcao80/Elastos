
#include "elastos/droid/dialer/util/DialerUtils.h"
#include "R.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/contacts/common/interactions/TouchPointManager.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Contacts::Common::Interactions::ITouchPointManager;
using Elastos::Droid::Contacts::Common::Interactions::TouchPointManager;
using Elastos::Core::CoreUtils;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IIterable;

namespace Elastos {
namespace Droid {
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
        TouchPointManager::GetInstance()->GetPoint((IPoint**)&touchPoint);
        Int32 x, y;
        touchPoint->GetX(&x);
        touchPoint->GetY(&y);
        if (x != 0 || y != 0) {
            AutoPtr<IBundle> extras;
            CBundle::New((IBundle**)&extras);
            extras->PutParcelable(ITouchPointManager::TOUCH_POINT, IParcelable::Probe(touchPoint));
            intent->PutExtra(ITelecomManager::EXTRA_OUTGOING_CALL_EXTRAS, extras);
        }

        ec = IActivity::Probe(context)->StartActivityForResult(intent, 0);
    }
    else {
        ec = context->StartActivity(intent);
    }
    // } catch (ActivityNotFoundException e) {
    //     Toast.makeText(context, msgId, Toast.LENGTH_SHORT).show();
    // }
    if (FAILED(ec)) {
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
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        // helper->FromParts(IContactsUtils::SCHEME_SMSTO, String(""), NULL, (IUri**)&uri);
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_SENDTO, uri, (IIntent**)&intent);
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
    res->GetDrawable(imageResId, (IDrawable**)&drawable);
    emptyListViewImage->SetImageDrawable(drawable);
    String str;
    res->GetString(strResId, &str);
    view->SetContentDescription(CoreUtils::Convert(str));

    AutoPtr<IView> emptyListViewMessage;
    emptyListView->FindViewById(R::id::emptyListViewMessage,
            (IView**)&emptyListViewMessage);
    ITextView::Probe(emptyListViewMessage)->SetText(CoreUtils::Convert(str));
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
    if (service != NULL) {
        Boolean result;
        IInputMethodManager::Probe(service)->ShowSoftInput(view, 0, &result);
    }
}

void DialerUtils::HideInputMethod(
    /* [in] */ IView* view)
{
    AutoPtr<IContext> context;
    view->GetContext((IContext**)&context);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&service);
    if (service != NULL) {
        AutoPtr<IBinder> token;
        view->GetWindowToken((IBinder**)&token);
        Boolean result;
        IInputMethodManager::Probe(service)->HideSoftInputFromWindow(token, 0, &result);
    }
}

} // Util
} // Dialer
} // Droid
} // Elastos
