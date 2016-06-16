#ifndef  __ELASTOS_DROID_PHONE_CGSMUTSCALLOPTIONS_H__
#define  __ELASTOS_DROID_PHONE_CGSMUTSCALLOPTIONS_H__

#include "_Elastos_Droid_Phone_CGsmUmtsCallOptions.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CGsmUmtsCallOptions)
    , public PreferenceActivity
    , public IGsmUmtsCallOptions
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CGsmUmtsCallOptions();

    CARAPI constructor();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

private:
    static const String LOG_TAG = "GsmUmtsCallOptions";
    const Boolean DBG;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CGSMUTSCALLOPTIONS_H__