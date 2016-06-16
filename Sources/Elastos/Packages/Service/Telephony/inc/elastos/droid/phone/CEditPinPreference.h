#ifndef  __ELASTOS_DROID_PHONE_CEDITPINPREFRENCE_H__
#define  __ELASTOS_DROID_PHONE_CEDITPINPREFRENCE_H__

#include "_Elastos_Droid_Phone_CEditPinPreference.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Class similar to the com.android.settings.EditPinPreference
 * class, with a couple of modifications, including a different layout
 * for the dialog.
 */
CarClass(CEditPinPreference)
    , public EditTextPreference
    , public IEditPinPreference
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CEditPinPreference();

    // CARAPI constructor();

    // CARAPI constructor(
    //     /* [in] */ IContext* context,
    //     /* [in] */ IAttributeSet* attrs);

    // CARAPI constructor(
    //     /* [in] */ IContext* context,
    //     /* [in] */ IAttributeSet* attrs,
    //     /* [in] */ Int32 defStyle);

    CARAPI SetOnPinEnteredListener(
        /* [in] */ IEditPinPreferenceOnPinEnteredListener* listener);

    /**
     * Externally visible method to bring up the dialog to
     * for multi-step / multi-dialog requests (like changing
     * the SIM pin).
     */
    CARAPI ShowPinDialog()

protected:
    /**
     * Overridden to setup the correct dialog layout, as well as setting up
     * other properties for the pin / puk entry field.
     */
    //@Override
    CARAPI OnCreateDialogView(
        /* [out] */ IView** view);

    //@Override
    CARAPI OnBindDialogView(
        /* [in] */ IView* view);

    //@Override
    CARAPI OnPrepareDialogBuilder(
        /* [in] */ IAlertDialogBuilder* builder);

    //@Override
    CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);

private:
    Boolean mShouldHideButtons;

    AutoPtr<IEditPinPreferenceOnPinEnteredListener> mPinListener;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CEDITPINPREFRENCE_H__