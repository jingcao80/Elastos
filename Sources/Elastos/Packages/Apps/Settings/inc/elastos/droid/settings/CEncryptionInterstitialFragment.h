
#ifndef __ELASTOS_DROID_SETTINGS_CENCRYPTIONINTERSTITIALFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_CENCRYPTIONINTERSTITIALFRAGMENT_H__

#include "_Elastos_Droid_Settings_CEncryptionInterstitialFragment.h"
#include "elastos/droid/settings/EncryptionInterstitial.h"

using Elastos::Droid::Settings::EncryptionInterstitial;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CEncryptionInterstitialFragment)
    , public EncryptionInterstitial::EncryptionInterstitialFragment
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CENCRYPTIONINTERSTITIALFRAGMENT_H__