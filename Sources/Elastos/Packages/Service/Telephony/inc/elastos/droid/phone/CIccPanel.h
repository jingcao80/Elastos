#ifndef  __ELASTOS_DROID_PHONE_CICCPANEL_H__
#define  __ELASTOS_DROID_PHONE_CICCPANEL_H__

#include "_Elastos_Droid_Phone_CIccPanel.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Base class for ICC-related panels in the Phone UI.
 */
CarClass(CIccPanel)
    , public Dialog
    , public IIccPanel
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnStop();

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnStart();

protected:
    static const String TAG = PhoneGlobals.LOG_TAG;

private:
    AutoPtr<IStatusBarManager> mStatusBarManager;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CICCPANEL_H__