
#include "CAboutActivity.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Core::CStringWrapper;
using Elastos::Utility::Logging::Logger;

namespace MusicPlayer {

//=======================================================================
//              CAboutActivity::MyListener
//=======================================================================
CAR_INTERFACE_IMPL(CAboutActivity::MyListener, IViewOnClickListener)

CAboutActivity::MyListener::MyListener(
    /* [in] */ CAboutActivity* host)
    : mActivity(host)
{
}

ECode CAboutActivity::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 viewId;
    v->GetId(&viewId);

    switch (viewId)
    {
    case R::about::imgview_back:
        mActivity->Finish();
        break;
    }

    return NOERROR;
}

//=======================================================================
//              CAboutActivity
//=======================================================================
ECode CAboutActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_about);
    mActionListener = new MyListener(this);

    mImgViewBack = FindViewById(R::about::imgview_back);
    assert(mImgViewBack != NULL);
    mImgViewBack->SetOnClickListener(mActionListener);

    String packageName;
    GetPackageName(&packageName);

    AutoPtr<IPackageManager> packageMgr;
    GetPackageManager((IPackageManager**)&packageMgr);
    AutoPtr<IPackageInfo> packageInfo;
    packageMgr->GetPackageInfo(packageName, 0, (IPackageInfo**)&packageInfo);
    String versionName;
    packageInfo->GetVersionName(&versionName);

    Logger::V("MuiscPlayer CAboutActivity versionName", versionName.string());

    AutoPtr<IView> view = FindViewById(R::about::tv_app_version);
    assert(view != NULL);
    mTextViewVersion = ITextView::Probe(view);
    assert(mTextViewVersion != NULL);
    AutoPtr<ICharSequence> mCharSeq;
    CStringWrapper::New(versionName, (ICharSequence**)&mCharSeq);
    mTextViewVersion->SetText(mCharSeq);

    return NOERROR;
}

} // namespace MusicPlayer
