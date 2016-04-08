
#include "CActivityOne.h"
#include "R.h"
//#include "Uri.h"
#include <elautoptr.h>
#include <stdio.h>

using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::App::IIActivityManager;

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace InstrumentationDemo {

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
    PRINT_FILE_LINE

    AutoPtr<IServiceManager> sm;
    AutoPtr<IIActivityManager> am;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    sm->GetService(IContext::ACTIVITY_SERVICE, (IInterface**)&am);

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetPackage(String("MyImageDemo"));
    intent->SetAction(String("android.intent.action.MAIN"));
    Int32 status;
    PRINT_FILE_LINE
    if (FAILED(am->StartActivity(NULL, intent, String(NULL),
            NULL, String(NULL), 0, 0, String(NULL), NULL, NULL, &status))) {
        PRINT_FILE_LINE
    }
    PRINT_FILE_LINE

    return NOERROR;
}

// #include <unistd.h>

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<IView> view = FindViewById(R::id::ImageView);
    assert(view != NULL);
    IImageView::Probe(view)->SetImageResource(R::drawable::earth);
    IImageView::Probe(view)->SetScaleType(Elastos::Droid::Widget::ImageViewScaleType_FIT_CENTER);

    AutoPtr<MyListener> l = new MyListener(this);
    view->SetOnClickListener(l.Get());

//    AutoPtr<ILocationManager> lm;
//    GetSystemService(IContext::LOCATION_SERVICE, (IInterface**)&lm);
//
//    AutoPtr<ILocation> loc;
//    lm->GetLastKnownLocation(String("gps"), (ILocation**)&loc);
//    Double latitude, longitude;
//    loc->GetLatitude(&latitude);
//    loc->GetLongitude(&longitude);
//PRINT_FILE_LINE_EX("latitude: %f, longitude: %f ====\n", latitude, longitude);
//
//    AutoPtr<IPackageManager> cm;
//    GetPackageManager((IPackageManager**)&cm);
//
//    AutoPtr<IObjectContainer> caps;
//    cm->GetInstalledPackages(0, (IObjectContainer**)&caps);
//    AutoPtr<IObjectEnumerator> it;
//    caps->GetObjectEnumerator((IObjectEnumerator**)&it);
//    Boolean succeeded;
//    while (it->MoveNext(&succeeded), succeeded) {
//        AutoPtr<IInterface> obj;
//        it->Current((IInterface**)&obj);
//        String capName;
//        IPackageInfo::Probe(obj)->GetPackageName(&capName);
//       PRINT_FILE_LINE_EX("capName: %s ====\n", capName.string());
//    }

//    AutoPtr<IContentResolver> resolver;
//    ECode ec = GetContentResolver((IContentResolver**)&resolver);
//    PRINT_FILE_LINE_EX("ec: %d, resolver: %p", ec, resolver.Get());
//    FAIL_RETURN(ec);
//
//    AutoPtr<IUriHelper> uriHelper;
//    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
//    AutoPtr<IUri> uri;
//    ec = uriHelper->Parse(String("content://CContentProviderOne"), (IUri**)&uri);
//    PRINT_FILE_LINE_EX("ec: %d, uri: %p", ec, uri.Get());
//
//    AutoPtr<ICursor> cursor;
//    ec = resolver->Query(uri, NULL, String(""), NULL, String(""), (ICursor**)&cursor);
//    PRINT_FILE_LINE_EX("ec: %d, cursor: %p, pid: %d", ec, cursor.Get(), getpid());

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
    return NOERROR;
}

} // namespace InstrumentationDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
