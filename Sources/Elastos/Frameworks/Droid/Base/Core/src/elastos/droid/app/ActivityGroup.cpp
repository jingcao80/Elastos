
#include "elastos/droid/app/ActivityGroup.h"
#include "elastos/droid/app/CLocalActivityManager.h"

namespace Elastos {
namespace Droid {
namespace App {

const String ActivityGroup::PARENT_NON_CONFIG_INSTANCE_KEY("android:parent_non_config_instance");
const String ActivityGroup::STATES_KEY("android:states");

CAR_INTERFACE_IMPL(ActivityGroup, Activity, IActivityGroup)

ActivityGroup::ActivityGroup()
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
    CLocalActivityManager::New(IActivity::Probe(this),
        singleActivityMode, (ILocalActivityManager**)&mLocalActivityManager);
    return NOERROR;
}

ECode ActivityGroup:: OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Activity::OnCreate(savedInstanceState))
    AutoPtr<IBundle> states;
    if (savedInstanceState != NULL)
        savedInstanceState->GetBundle(STATES_KEY, (IBundle**)&states);
    return mLocalActivityManager->DispatchCreate(states);
}

ECode ActivityGroup:: OnResume()
{
    FAIL_RETURN(Activity::OnResume())
    return mLocalActivityManager->DispatchResume();
}

ECode ActivityGroup:: OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    FAIL_RETURN(Activity::OnSaveInstanceState(outState))
    AutoPtr<IBundle> state;
    mLocalActivityManager->SaveInstanceState((IBundle**)&state);
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
    return mLocalActivityManager->DispatchPause(finish);
}

ECode ActivityGroup:: OnStop()
{
    FAIL_RETURN(Activity::OnStop())
    return mLocalActivityManager->DispatchStop();
}

ECode ActivityGroup:: OnDestroy()
{
    FAIL_RETURN(Activity::OnDestroy())
    Boolean bval;
    IsFinishing(&bval);
    return mLocalActivityManager->DispatchDestroy(bval);
}

ECode ActivityGroup::OnRetainNonConfigurationChildInstances(
    /* [out] */ IHashMap** map)
{
    return mLocalActivityManager->DispatchRetainNonConfigurationInstance(map);
}

ECode ActivityGroup::GetCurrentActivity(
    /* [out] */ IActivity** activity)
{
    return mLocalActivityManager->GetCurrentActivity(activity);
}

ECode ActivityGroup::GetLocalActivityManager(
    /* [out] */ ILocalActivityManager** mgr)
{
    VALIDATE_NOT_NULL(mgr)
    *mgr = mLocalActivityManager;
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
        mLocalActivityManager->GetActivity(who, (IActivity**)&act);
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
