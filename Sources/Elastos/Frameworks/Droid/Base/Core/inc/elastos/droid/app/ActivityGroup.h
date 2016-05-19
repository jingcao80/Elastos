
#ifndef __ELASTOS_DROID_APP_ACTIVITY_GROUP_H__
#define __ELASTOS_DROID_APP_ACTIVITY_GROUP_H__

#include "elastos/droid/app/Activity.h"

namespace Elastos {
namespace Droid {
namespace App {

/**
 * A screen that contains and runs multiple embedded activities.
 *
 * @deprecated Use the new {@link Fragment} and {@link FragmentManager} APIs
 * instead; these are also
 * available On older platforms through the Android compatibility package.
 */
class ECO_PUBLIC ActivityGroup
    : public Activity
    , public IActivityGroup
{
public:
    CAR_INTERFACE_DECL()

    ActivityGroup();

    virtual ~ActivityGroup();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Boolean singleActivityMode);

    /**
     * Returns a HashMap mapping from child activity ids to the return values
     * from calls to their OnRetainNonConfigurationInstance methods.
     *
     * {@hide}
     */
    //@Override
    CARAPI OnRetainNonConfigurationChildInstances(
        /* [out] */ IHashMap** map);

    CARAPI GetCurrentActivity(
        /* [out] */ IActivity** activity);

    CARAPI GetLocalActivityManager(
        /* [out] */ ILocalActivityManager** mgr);

    //@Override
    CARAPI DispatchActivityResult(
        /* [in] */ const String& who,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnStop();

    //@Override
    CARAPI OnDestroy();

    AutoPtr<ILocalActivityManager> GetLocalActivityManager();

private:
    /**
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    AutoPtr<ILocalActivityManager> mLocalActivityManager;
    Boolean mSingleActivityMode;

private:
    static const String STATES_KEY;// = "android:states";
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ACTIVITY_GROUP_H__
