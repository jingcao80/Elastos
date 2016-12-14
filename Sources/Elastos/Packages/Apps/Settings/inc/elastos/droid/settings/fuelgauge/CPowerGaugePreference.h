
#ifndef __ELASTOS_DROID_SETTINGS_FUELGAUGE_CPOWERGAUGEPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_FUELGAUGE_CPOWERGAUGEPREFERENCE_H__

#include "_Elastos_Droid_Settings_Fuelgauge_CPowerGaugePreference.h"
#include "elastos/droid/preference/Preference.h"
#include "elastos/droid/settings/fuelgauge/BatteryEntry.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

/**
 * Custom preference for displaying power consumption as a bar and an icon on
 * the left for the subsystem/app type.
 */
CarClass(CPowerGaugePreference)
    , public Elastos::Droid::Preference::Preference
    , public IPowerGaugePreference
{
public:
    TO_STRING_IMPL("CPowerGaugePreference")

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPowerGaugePreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IDrawable* icon,
        /* [in] */ ICharSequence* contentDescription,
        /* [in] */ IBatteryEntry* info);

    CARAPI SetPercent(
        /* [in] */ Double percentOfMax,
        /* [in] */ Double percentOfTotal);

    CARAPI_(AutoPtr<BatteryEntry>) GetInfo();

protected:
    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

private:
    AutoPtr<BatteryEntry> mInfo;
    Int32 mProgress;
    AutoPtr<ICharSequence> mProgressText;
    AutoPtr<ICharSequence> mContentDescription;
};

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_FUELGAUGE_CPOWERGAUGEPREFERENCE_H__