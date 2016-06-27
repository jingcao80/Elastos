
#include "elastos/droid/teleservice/phone/CDeleteFdnContactScreen.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/StringBuilder.h>
#include "R.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CDeleteFdnContactScreen::QueryHandler::QueryHandler(
    /* [in] */ IContentResolver* cr,
    /* [in] */ CDeleteFdnContactScreen* host)
    : mHost(host)
{
    AsyncQueryHandler::constructor(cr);
}

ECode CDeleteFdnContactScreen::QueryHandler::OnQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICursor* c)
{
    return NOERROR;
}

ECode CDeleteFdnContactScreen::QueryHandler::OnInsertComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ IUri* uri)
{
    return NOERROR;
}

ECode CDeleteFdnContactScreen::QueryHandler::OnUpdateComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ Int32 result)
{
    return NOERROR;
}

ECode CDeleteFdnContactScreen::QueryHandler::OnDeleteComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ Int32 result)
{
    if (DBG) mHost->Log(String("onDeleteComplete"));
    mHost->DisplayProgress(FALSE);
    mHost->HandleResult(result > 0);
    return NOERROR;
}

ECode CDeleteFdnContactScreen::MyRunnable::Run()
{
    return mHost->Finish();
}

const String CDeleteFdnContactScreen::TAG("CDeleteFdnContactScreen");// = PhoneGlobals.TAG;
const Boolean CDeleteFdnContactScreen::DBG = FALSE;

const String CDeleteFdnContactScreen::INTENT_EXTRA_NAME("name");
const String CDeleteFdnContactScreen::INTENT_EXTRA_NUMBER("number");

const Int32 CDeleteFdnContactScreen::PIN2_REQUEST_CODE = 100;

CAR_OBJECT_IMPL(CDeleteFdnContactScreen)

CDeleteFdnContactScreen::CDeleteFdnContactScreen()
    : mName(NULL)
    , mNumber(NULL)
    , mPin2(NULL)
{
    mHandler = new Handler();
}

ECode CDeleteFdnContactScreen::constructor()
{
    return Activity::constructor();
}

ECode CDeleteFdnContactScreen::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    ResolveIntent();

    AuthenticatePin2();

    AutoPtr<IWindow> window = GetWindow();
    Boolean result;
    window->RequestFeature(IWindow::FEATURE_INDETERMINATE_PROGRESS, &result);
    return SetContentView(Elastos::Droid::TeleService::R::layout::delete_fdn_contact_screen);
}

ECode CDeleteFdnContactScreen::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* intent)
{
    if (DBG) Log(String("onActivityResult"));

    switch (requestCode) {
        case PIN2_REQUEST_CODE:
            AutoPtr<IBundle> extras;
            if (intent != NULL){
                intent->GetExtras((IBundle**)&extras);
            }
            if (extras != NULL) {
                extras->GetString(String("pin2"), &mPin2);
                AutoPtr<IResources> resource;
                GetResources((IResources**)&resource);
                AutoPtr<ICharSequence> csq;
                resource->GetText(Elastos::Droid::TeleService::R::string::deleting_fdn_contact,
                        (ICharSequence**)&csq);
                ShowStatus(csq);
                DeleteContact();
            }
            else {
                // if they cancelled, then we just cancel too.
                if (DBG) Log(String("onActivityResult: CANCELLED"));
                DisplayProgress(FALSE);
                Finish();
            }
            break;
    }
    return NOERROR;
}

void CDeleteFdnContactScreen::ResolveIntent()
{
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);

    intent->GetStringExtra(INTENT_EXTRA_NAME, &mName);
    intent->GetStringExtra(INTENT_EXTRA_NUMBER, &mNumber);

    if (TextUtils::IsEmpty(mNumber)) {
        Finish();
    }
}

ECode CDeleteFdnContactScreen::DeleteContact()
{
    StringBuilder buf;
    if (TextUtils::IsEmpty(mName)) {
        buf += "number='";
    }
    else {
        buf += "tag='";
        buf += mName;
        buf += "' AND number='";
    }
    buf += mNumber;
    buf += "' AND pin2='";
    buf += mPin2;
    buf += "'";

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(String("content://icc/fdn"), (IUri**)&uri);

    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    mQueryHandler = new QueryHandler(resolver, this);
    mQueryHandler->StartDelete(0, NULL, uri, buf.ToString(), NULL);
    return DisplayProgress(TRUE);
}

ECode CDeleteFdnContactScreen::AuthenticatePin2()
{
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);

    assert(0);
    //intent->SetClass(this, GetPin2Screen.class);
    return StartActivityForResult(intent, PIN2_REQUEST_CODE);
}

ECode CDeleteFdnContactScreen::DisplayProgress(
    /* [in] */ Boolean flag)
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);

    return window->SetFeatureInt(
            IWindow::FEATURE_INDETERMINATE_PROGRESS,
            flag ? IWindow::PROGRESS_VISIBILITY_ON : IWindow::PROGRESS_VISIBILITY_OFF);
}

void CDeleteFdnContactScreen::ShowStatus(
    /* [in] */ ICharSequence* statusMsg)
{
    if (statusMsg != NULL) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText(this, statusMsg, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
    }
}

ECode CDeleteFdnContactScreen::HandleResult(
    /* [in] */ Boolean success)
{
    if (success) {
        if (DBG) Log(String("handleResult: success!"));

        AutoPtr<IResources> resource;
        GetResources((IResources**)&resource);
        AutoPtr<ICharSequence> csq;
        resource->GetText(Elastos::Droid::TeleService::R::string::pin2_invalid, (ICharSequence**)&csq);
        ShowStatus(csq);
    }
    else {
        if (DBG) Log(String("handleResult: failed!"));

        AutoPtr<IResources> resource;
        GetResources((IResources**)&resource);
        AutoPtr<ICharSequence> csq;
        resource->GetText(Elastos::Droid::TeleService::R::string::pin2_invalid, (ICharSequence**)&csq);
        ShowStatus(csq);
    }

    AutoPtr<IRunnable> r = new MyRunnable(this);
    Boolean res;
    return mHandler->PostDelayed(r, 2000, &res);
}

void CDeleteFdnContactScreen::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, String("[DeleteFdnContact] ") + msg);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos