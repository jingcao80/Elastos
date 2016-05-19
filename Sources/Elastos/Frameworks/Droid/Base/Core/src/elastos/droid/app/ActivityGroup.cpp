
#include "elastos/droid/app/ActivityGroup.h"
#include "elastos/droid/app/CLocalActivityManager.h"

namespace Elastos {
namespace Droid {
namespace App {

const String ActivityGroup::STATES_KEY("android:states");

CAR_INTERFACE_IMPL(ActivityGroup, Activity, IActivityGroup)

ActivityGroup::ActivityGroup()
    : mSingleActivityMode(TRUE)
{}

ActivityGroup::~ActivityGroup()
{}

ECode ActivityGroup::constructor()
{
    return constructor(TRUE);
}

ECode ActivityGroup::constructor(
    /* [in] */ Boolean singleActivityMode)
{
    mSingleActivityMode = singleActivityMode;
    return Activity::constructor();
}

ECode ActivityGroup::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Activity::OnCreate(savedInstanceState))
    AutoPtr<IBundle> states;
    if (savedInstanceState != NULL)
        savedInstanceState->GetBundle(STATES_KEY, (IBundle**)&states);
    return GetLocalActivityManager()->DispatchCreate(states);
}

ECode ActivityGroup:: OnResume()
{
    FAIL_RETURN(Activity::OnResume())
    return GetLocalActivityManager()->DispatchResume();
}

ECode ActivityGroup:: OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    FAIL_RETURN(Activity::OnSaveInstanceState(outState))
    assert(GetLocalActivityManager() != NULL);
    AutoPtr<IBundle> state;
    GetLocalActivityManager()->SaveInstanceState((IBundle**)&state);
    if (state != NULL) {
        outState->PutBundle(STATES_KEY, state);
    }
    return NOERROR;
}

ECode ActivityGroup:: OnPause()
{
    FAIL_RETURN(Activity::OnPause())
    Boolean finish;
    IsFinishing(&finish);
    return GetLocalActivityManager()->DispatchPause(finish);
}

ECode ActivityGroup:: OnStop()
{
    FAIL_RETURN(Activity::OnStop())
    return GetLocalActivityManager()->DispatchStop();
}

ECode ActivityGroup:: OnDestroy()
{
    FAIL_RETURN(Activity::OnDestroy())
    Boolean bval;
    IsFinishing(&bval);
    return GetLocalActivityManager()->DispatchDestroy(bval);
}

ECode ActivityGroup::OnRetainNonConfigurationChildInstances(
    /* [out] */ IHashMap** map)
{
    return GetLocalActivityManager()->DispatchRetainNonConfigurationInstance(map);
}

ECode ActivityGroup::GetCurrentActivity(
    /* [out] */ IActivity** activity)
{
    return GetLocalActivityManager()->GetCurrentActivity(activity);
}

AutoPtr<ILocalActivityManager> ActivityGroup::GetLocalActivityManager()
{
    if (mLocalActivityManager == NULL) {
        CLocalActivityManager::New((IActivity*)this, mSingleActivityMode,
            (ILocalActivityManager**)&mLocalActivityManager);
    }
    return mLocalActivityManager;
}

ECode ActivityGroup::GetLocalActivityManager(
    /* [out] */ ILocalActivityManager** mgr)
{
    VALIDATE_NOT_NULL(mgr)
    AutoPtr<ILocalActivityManager> tmp = GetLocalActivityManager();
    *mgr = tmp;
    REFCOUNT_ADD(*mgr)
    return NOERROR;
}

ECode ActivityGroup::DispatchActivityResult(
    /* [in] */ const String& who,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    if (who != NULL) {
        AutoPtr<IActivity> act;
        GetLocalActivityManager()->GetActivity(who, (IActivity**)&act);
        /*
        if (false) Log.v(
            TAG, "Dispatching result: who=" + who + ", reqCode=" + requestCode
            + ", resCode=" + resultCode + ", data=" + data
            + ", rec=" + rec);
        */
        if (act != NULL) {
            Activity* activity = (Activity*)act.Get();
            return activity->OnActivityResult(requestCode, resultCode, data);
        }
    }

    return Activity::DispatchActivityResult(who, requestCode, resultCode, data);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
