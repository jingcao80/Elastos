
#ifndef __ELASTOS_DROID_SETTINGS_CEDITPINPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_CEDITPINPREFERENCE_H__

#include "_Elastos_Droid_Settings_CEditPinPreference.h"
#include "elastos/droid/preference/EditTextPreference.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::EditTextPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * TODO: Add a soft dialpad for PIN entry.
 */
CarClass(CEditPinPreference)
    , public EditTextPreference
    , public IEditPinPreference
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CEditPinPreference();

    virtual ~CEditPinPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    virtual CARAPI SetOnPinEnteredListener(
        /* [in] */ IEditPinPreferenceOnPinEnteredListener* listener);

    virtual CARAPI_(Boolean) IsDialogOpen();

    virtual CARAPI ShowPinDialog();

protected:
    //@Override
    CARAPI OnBindDialogView(
        /* [in] */ IView* view);

    //@Override
    CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);

private:
    AutoPtr<IEditPinPreferenceOnPinEnteredListener> mPinListener;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CEDITPINPREFERENCE_H__