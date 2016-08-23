#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CUSERDICTIONARYLOCALEPICKER_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CUSERDICTIONARYLOCALEPICKER_H__

#include "_Elastos_Droid_Settings_Inputmethod_CUserDictionaryLocalePicker.h"
#include "elastos/droid/internal/app/LocalePicker.h"

using Elastos::Droid::Internal::App::LocalePicker;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

CarClass(CUserDictionaryLocalePicker)
    , public LocalePicker
{
public:
    CAR_OBJECT_DECL();

    CUserDictionaryLocalePicker();

    virtual ~CUserDictionaryLocalePicker();

    CARAPI constructor(
        /* [in] */ IUserDictionaryAddWordFragment* parent);
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_CUSERDICTIONARYLOCALEPICKER_H__