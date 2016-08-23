
#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_USERDICTIONARYSETTINGSUTILS_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_USERDICTIONARYSETTINGSUTILS_H__

#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

/**
 * Utilities of the user dictionary settings
 */
class UserDictionarySettingsUtils
{
public:
    static CARAPI_(String) GetLocaleDisplayName(
        /* [in] */ IContext* context,
        /* [in] */ const String& localeStr);
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_USERDICTIONARYSETTINGSUTILS_H__