
#include "CActivityOne.h"
#include "R.h"
#include <elautoptr.h>
#include <elastos/utility/logging/Slogger.h>


using Elastos::Utility::Logging::Slogger;
using Elastos::Core::ICharSequence;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Net::IHttpURLConnection;
using Elastos::Net::IURLConnection;
using Elastos::Net::IURL;
using Elastos::Net::CURL;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Interacting {

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host) :
    mHost(host),
    mRef(0)
{
}

PInterface CActivityOne::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IViewOnClickListener*)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }

    return NULL;
}

UInt32 CActivityOne::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IViewOnClickListener*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 id;
    v->GetId(&id);
    if (id == R::id::startotherapp) {
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);

        AutoPtr<IUri> webpage;
        helper->Parse(String("http://www.elastos.org"), (IUri**)&webpage);
        AutoPtr<IIntent> webIntent;
        CIntent::New(IIntent::ACTION_VIEW, webpage, (IIntent**)&webIntent);
        mHost->StartActivity(webIntent);
    }
    else if (id == R::id::startforresult) {
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->SetClassName(String("Interacting"), String("Interacting.CActivityTwo"));
        Int32 requestCode = 10;
        if (FAILED(mHost->StartActivityForResult(intent, requestCode))) {
            Slogger::E("CActivityOne", "StartActivityForResult---CActivityTwo---failed");
        }
    }
    else if (id == R::id::startanotherapp) {
        AutoPtr<IIntent> intent;
        ECode ec = CIntent::New(String("com.elastos.ActivityTest.CActivityTwo"), (IIntent**)&intent);
Slogger::D("CActivityOne", "=======================OnClick, ec=[0x%08x]=========1", ec);
        ec = mHost->StartActivity(intent);
Slogger::D("CActivityOne", "=======================OnClick, ec=[0x%08x]=========2", ec);
    }

    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<MyListener> l = new MyListener(this);
    IViewOnClickListener* clickListener = (IViewOnClickListener*)(l->Probe(EIID_IViewOnClickListener));

    mStartNewApp = FindViewById(R::id::startotherapp);
    assert(mStartNewApp != NULL);
    mStartNewApp->SetOnClickListener(clickListener);

    mStartForRes = FindViewById(R::id::startforresult);
    assert(mStartForRes != NULL);
    mStartForRes->SetOnClickListener(clickListener);

    mStartOtherApp = FindViewById(R::id::startanotherapp);
    assert(mStartOtherApp != NULL);
    mStartOtherApp->SetOnClickListener(clickListener);

    mResValue = ITextView::Probe(FindViewById(R::id::showntext));
    assert(mResValue != NULL);

    // AutoPtr<IURL> url;
    // String urlStr("http://mmd.minisocials.com/remoting/getevents.php?appkey=key525c4ade");
    // CURL::New(urlStr, (IURL**)&url);
    // assert(url != NULL);
    // AutoPtr<IURLConnection> urlConn;
    // url->OpenConnection((IURLConnection**)&urlConn);
    // AutoPtr<IHttpURLConnection> httpConn = IHttpURLConnection::Probe(urlConn);
    // assert(httpConn != NULL);
    // Int32 code = 0;
    // httpConn->GetResponseCode(&code);

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
Slogger::D("CActivityOne", "=============================================OnActivityResult");
    if (resultCode == 10) {
        AutoPtr<IBundle> extras;
        data->GetExtras((IBundle**)&extras);
        assert(extras != NULL);
        String value1, value2;
        extras->GetString(String("Interacting.at.key1"), &value1);
        extras->GetString(String("Interacting.at.key2"), &value2);
Slogger::D("CActivityOne", "============OnActivityResult, s1=[%s], s2=[%s]", value1.string(),value2.string());
        AutoPtr<ICharSequence> text;
        CStringWrapper::New(String("Result value: [") + value2 + String("]"), (ICharSequence**)&text);
        mResValue->SetText(text);
    }
    return NOERROR;
}

} // namespace Interacting
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
