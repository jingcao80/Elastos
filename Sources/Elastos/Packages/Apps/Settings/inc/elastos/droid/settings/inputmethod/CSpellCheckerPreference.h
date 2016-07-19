#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CSPELLCHECKERSPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CSPELLCHECKERSPREFERENCE_H__

#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_Droid_Settings_Inputmethod_CSpellCheckerPreference.h"
#include "elastos/droid/preference/Preference.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::Preference;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::TextService::ISpellCheckerInfo;
using Elastos::Droid::Widget::IRadioButton;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

/**
 * Spell checker service preference.
 *
 * This preference represents a spell checker service. It is used for two purposes. 1) A radio
 * button on the left side is used to choose the current spell checker service. 2) A settings
 * icon on the right side is used to invoke the setting activity of the spell checker service.
 */
CarClass(CSpellCheckerPreference)
    , public Elastos::Droid::Preference::Preference
    , public ISpellCheckerPreference
    , public IViewOnClickListener
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSpellCheckerPreference();

    ~CSpellCheckerPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ISpellCheckerInfo* sci,
        /* [in] */ ISpellCheckerPreferenceOnRadioButtonPreferenceListener* onRadioButtonListener);

    //@Override
    CARAPI OnClick(
        /* [in] */ IView* v);

    CARAPI GetSpellCheckerInfo(
        /* [out] */ ISpellCheckerInfo** info);

    CARAPI SetSelected(
        /* [in] */ Boolean selected);

protected:
    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

private:
    CARAPI_(void) OnSettingsButtonClicked();

    CARAPI_(void) UpdateSelectedState(
        /* [in] */ Boolean selected);

    CARAPI_(void) EnableSettingsButton(
        /* [in] */ Boolean enabled);

private:
    AutoPtr<ISpellCheckerInfo> mSci;
    AutoPtr<ISpellCheckerPreferenceOnRadioButtonPreferenceListener> mOnRadioButtonListener;

    AutoPtr<IRadioButton> mRadioButton;
    AutoPtr<IView> mPrefLeftButton;
    AutoPtr<IView> mSettingsButton;
    Boolean mSelected;
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_CSPELLCHECKERSPREFERENCE_H__
