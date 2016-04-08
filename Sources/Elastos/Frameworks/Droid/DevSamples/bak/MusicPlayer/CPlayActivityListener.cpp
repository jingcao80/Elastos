
#include "CPlayActivityListener.h"
#include "CPlayActivity.h"

using Elastos::Droid::App::EIID_IActivity;

namespace MusicPlayer {

ECode CPlayActivityListener::constructor(
    /* [in] */ IWeakReference* host)
{
    mHost = host;
    return  NOERROR;
}

ECode CPlayActivityListener::OnTick(
    /* [in] */ Int32 duration)
{
    AutoPtr<IActivity> activity;
    mHost->Resolve(EIID_IActivity, (IInterface**)&activity);
    if (activity) {
        CPlayActivity* playActivity = (CPlayActivity*)activity.Get();
        return playActivity->OnTick(duration);

    }
    return  NOERROR;
}

ECode CPlayActivityListener::OnCompletion()
{
    AutoPtr<IActivity> activity;
    mHost->Resolve(EIID_IActivity, (IInterface**)&activity);
    if (activity) {
        CPlayActivity* playActivity = (CPlayActivity*)activity.Get();
        return playActivity->OnCompletion();

    }
    return  NOERROR;
}

} // namespace MusicPlayer

