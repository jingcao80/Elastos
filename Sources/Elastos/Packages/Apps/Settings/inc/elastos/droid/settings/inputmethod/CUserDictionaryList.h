
#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CUSERDICTIONARYLIST_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CUSERDICTIONARYLIST_H__

#include "_Elastos_Droid_Settings_Inputmethod_CUserDictionaryList.h"
#include "elastos/droid/settings/inputmethod/UserDictionaryList.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

CarClass(CUserDictionaryList)
    , public UserDictionaryList
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_CUSERDICTIONARYLIST_H__
