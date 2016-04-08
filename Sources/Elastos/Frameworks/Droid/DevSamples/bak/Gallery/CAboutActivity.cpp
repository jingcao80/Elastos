
#include "CAboutActivity.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>
#include "src/data/AsyncImageLoader.h"

using Elastos::Core::CStringWrapper;
using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::App::EIID_IActivity;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String CAboutActivity::TAG("CAboutActivity");

CAboutActivity::MyListener::MyListener(
    /* [in] */ CAboutActivity* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(CAboutActivity::MyListener, IViewOnClickListener);

ECode CAboutActivity::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 viewId = 0;
    v->GetId(&viewId);

    switch(viewId) {
        case R::id::about_back: {
            Logger::D(TAG, "OnClick()---about_back");
            mHost->OnBackPressed();
            break;
        }
        default:
            break;
    }

    return NOERROR;
}

ECode CAboutActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::D(TAG, "OnCreate()---");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_about);
    AutoPtr<MyListener> l = new MyListener(this);
    AutoPtr<IView> view = FindViewById(R::id::about_back);
    mBackButton = IImageView::Probe(view);
    assert(mBackButton != NULL);
    mBackButton->SetOnClickListener(l.Get());
    view = FindViewById(R::id::about_version);
    AutoPtr<ITextView> versionView = ITextView::Probe(view);
    assert(versionView != NULL);

    String pkgName, pkgVersion;
    AutoPtr<IPackageManager> pkgMgr;
    AutoPtr<IPackageInfo> pkgInfo;

    GetPackageName(&pkgName);
    GetPackageManager((IPackageManager**)&pkgMgr);
    pkgMgr->GetPackageInfo(pkgName, 0, (IPackageInfo**)&pkgInfo);
    pkgInfo->GetVersionName(&pkgVersion);
    Logger::D(TAG, "OnCreate()---pkgVersion:%s", pkgVersion.string());
    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(pkgVersion, (ICharSequence**)&cs);
    versionView->SetText(cs);

    return NOERROR;
}

ECode CAboutActivity::OnStart()
{
    Logger::D(TAG, "OnStart()---");
    Activity::OnStart();
    return NOERROR;
}

ECode CAboutActivity::OnResume()
{
    Logger::D(TAG, "OnResume()---");
    Activity::OnResume();
    return NOERROR;
}

ECode CAboutActivity::OnPause()
{
    Logger::D(TAG, "OnPause()---");
    Activity::OnPause();
    return NOERROR;
}

ECode CAboutActivity::OnStop()
{
    Logger::D(TAG, "OnStop()---");
    Activity::OnStop();
    return NOERROR;
}

ECode CAboutActivity::OnDestroy()
{
    Logger::D(TAG, "OnDestroy()---");
    Activity::OnDestroy();
    return NOERROR;
}

} // namespace Gallery
