#ifndef  __ELASTOS_DROID_PHONE_CGSMUTSCALLOPTIONS_H__
#define  __ELASTOS_DROID_PHONE_CGSMUTSCALLOPTIONS_H__

#include "_Elastos_Droid_Phone_CGsmUmtsCallOptions.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/PreferenceActivity.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::PreferenceActivity;

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CGsmUmtsCallOptions)
    , public PreferenceActivity
{
public:
    CAR_OBJECT_DECL();

    CGsmUmtsCallOptions();

    CARAPI constructor();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

private:
    static const String TAG;
    const Boolean DBG;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CGSMUTSCALLOPTIONS_H__