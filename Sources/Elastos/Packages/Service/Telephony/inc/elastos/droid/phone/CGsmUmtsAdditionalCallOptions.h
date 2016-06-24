#ifndef  __ELASTOS_DROID_PHONE_CGSMUMTSADDITIONALCALLOPTIONS_H__
#define  __ELASTOS_DROID_PHONE_CGSMUMTSADDITIONALCALLOPTIONS_H__

#include "_Elastos_Droid_Phone_CGsmUmtsAdditionalCallOptions.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CGsmUmtsAdditionalCallOptions)
    , public TimeConsumingPreferenceActivity
    , public IGsmUmtsAdditionalCallOptions
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CGsmUmtsAdditionalCallOptions();

    CARAPI constructor();

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

private:
    static const String TAG;
    const Boolean DBG;

    static const String BUTTON_CLIR_KEY;
    static const String BUTTON_CW_KEY;

    AutoPtr<ICLIRListPreference> mCLIRButton;
    AutoPtr<ICallWaitingCheckBoxPreference> mCWButton;

    AutoPtr<IArrayList> mPreferences;
    Int32 mInitIndex;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CGSMUMTSADDITIONALCALLOPTIONS_H__