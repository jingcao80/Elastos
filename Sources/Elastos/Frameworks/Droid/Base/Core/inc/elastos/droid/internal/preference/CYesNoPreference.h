
#ifndef __ELASTOS_DROID_INTERNAL_PREFERENCE_CYESNOPREFERENCE_H__
#define __ELASTOS_DROID_INTERNAL_PREFERENCE_CYESNOPREFERENCE_H__

#include "_Elastos_Droid_Internal_Preference_CYesNoPreference.h"
#include "elastos/droid/internal/preference/YesNoPreference.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Preference {

CarClass(CYesNoPreference)
    , public YesNoPreference
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Preference
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_PREFERENCE_CYESNOPREFERENCE_H__
