
#include "CMainActivityListener.h"
#include "CMainActivity.h"

using Elastos::Droid::App::EIID_IActivity;

namespace MusicPlayer {

ECode CMainActivityListener::constructor(
    /* [in] */ IWeakReference* host)
{
    mHost = host;
    return  NOERROR;
}

ECode CMainActivityListener::OnTick(
    /* [in] */ Int32 duration)
{
    AutoPtr<IActivity> activity;
    mHost->Resolve(EIID_IActivity, (IInterface**)&activity);
    if (activity) {
        CMainActivity* mainActivity = (CMainActivity*)activity.Get();
        return mainActivity->OnTick(duration);

    }
    return  NOERROR;
}

ECode CMainActivityListener::OnCompletion()
{
    AutoPtr<IActivity> activity;
    mHost->Resolve(EIID_IActivity, (IInterface**)&activity);
    if (activity) {
        CMainActivity* mainActivity = (CMainActivity*)activity.Get();
        return mainActivity->OnCompletion();

    }
    return  NOERROR;
}

ECode CMainActivityListener::Run()
{
    AutoPtr<IActivity> activity;
    mHost->Resolve(EIID_IActivity, (IInterface**)&activity);
    if (activity) {
        CMainActivity* mainActivity = (CMainActivity*)activity.Get();
        return mainActivity->Run();

    }
    return  NOERROR;
}

} // namespace MusicPlayer

