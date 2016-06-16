
#include "elastos/droid/phone/CDeleteFdnContactScreen.h"

namespace Elastos {
namespace Droid {
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
}

ECode CDeleteFdnContactScreen::MyRunnable::Run()
{
    return mHost->Finish();
}

const String CDeleteFdnContactScreen::LOG_TAG = PhoneGlobals.LOG_TAG;
const Boolean CDeleteFdnContactScreen::DBG = FALSE;

const String CDeleteFdnContactScreen::INTENT_EXTRA_NAME("name");
const String CDeleteFdnContactScreen::INTENT_EXTRA_NUMBER("number");

const Int32 CDeleteFdnContactScreen::PIN2_REQUEST_CODE = 100;

CAR_INTERFACE_IMPL(CDeleteFdnContactScreen, Activity, IDeleteFdnContactScreen)

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

ECode CDeleteFdnContactScreen::OnCreate()
{
    Activity::OnCreate(icicle);

    ResolveIntent();

    AuthenticatePin2();

    AutoPtr<IWindow> window = GetWindow();
    Boolean result;
    window->RequestFeature(IWindow::FEATURE_INDETERMINATE_PROGRESS, &result);
    return SetContentView(R.layout.delete_fdn_contact_screen);
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
                AutoPtr<IResource> resource;
                GetResources((IResource**)&resource);
                AutoPtr<ICharSequence> csq;
                resource->GetText(R.string.deleting_fdn_contact, (ICharSequence**)&csq);
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
    getContentResolver((IContentResolver**)&resolver);
    mQueryHandler = new QueryHandler(resolver, this);
    mQueryHandler->StartDelete(0, NULL, uri, buf.ToString(), NULL);
    return DisplayProgress(TRUE);
}

ECode CDeleteFdnContactScreen::AuthenticatePin2()
{
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);

    intent->SetClass(this, GetPin2Screen.class);
    return StartActivityForResult(intent, PIN2_REQUEST_CODE);
}

ECode CDeleteFdnContactScreen::DisplayProgress(
    /* [in] */ Boolean flag)
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);

    return window->SetFeatureInt(
            IWindow::FEATURE_INDETERMINATE_PROGRESS,
            flag ? PROGRESS_VISIBILITY_ON : PROGRESS_VISIBILITY_OFF);
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

void CDeleteFdnContactScreen::HandleResult(
    /* [in] */ Boolean success)
{
    if (success) {
        if (DBG) Log(String("handleResult: success!"));

        AutoPtr<IResource> resource;
        GetResources((IResource**)&resource);
        AutoPtr<ICharSequence> csq;
        resource->GetText(R.string.pin2_invalid, (ICharSequence**)&csq);
        ShowStatus(csq);
    }
    else {
        if (DBG) Log(String("handleResult: failed!"));

        AutoPtr<IResource> resource;
        GetResources((IResource**)&resource);
        AutoPtr<ICharSequence> csq;
        resource->GetText(R.string.pin2_invalid, (ICharSequence**)&csq);
        ShowStatus(csq);
    }

    AutoPtr<IRunnable> r = new MyRunnable(this);
    return mHandler->PostDelayed(r, 2000);
}

void CDeleteFdnContactScreen::Log(
    /* [in] */ const String& msg)
{
    Logger::D(LOG_TAG, String("[DeleteFdnContact] ") + msg);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos