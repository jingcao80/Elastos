#ifndef  __ELASTOS_DROID_PHONE_CGSMUMTSCALLFORWARDOPTIONS_H__
#define  __ELASTOS_DROID_PHONE_CGSMUMTSCALLFORWARDOPTIONS_H__

#include "_Elastos_Droid_Phone_CGsmUmtsCallForwardOptions.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CGsmUmtsCallForwardOptions)
    , public TimeConsumingPreferenceActivity
    , public IGsmUmtsCallForwardOptions
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CGsmUmtsCallForwardOptions() {}

    CARAPI constructor();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnFinished(
        /* [in] */ IPreference* preference,
        /* [in] */ Boolean reading);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

private:
    static const String LOG_TAG;
    const Boolean DBG;

    static const AutoPtr<ArrayOf<String> > NUM_PROJECTION;

    static const String BUTTON_CFU_KEY;
    static const String BUTTON_CFB_KEY;
    static const String BUTTON_CFNRY_KEY;
    static const String BUTTON_CFNRC_KEY;

    static const String KEY_TOGGLE;
    static const String KEY_STATUS;
    static const String KEY_NUMBER;

    AutoPtr<ICallForwardEditPreference> mButtonCFU;
    AutoPtr<ICallForwardEditPreference> mButtonCFB;
    AutoPtr<ICallForwardEditPreference> mButtonCFNRy;
    AutoPtr<ICallForwardEditPreference> mButtonCFNRc;

    AutoPtr<IArrayList> mPreferences;
    Int32 mInitIndex;

    Boolean mFirstResume;
    AutoPtr<IBundle> mIcicle;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CGSMUMTSCALLFORWARDOPTIONS_H__