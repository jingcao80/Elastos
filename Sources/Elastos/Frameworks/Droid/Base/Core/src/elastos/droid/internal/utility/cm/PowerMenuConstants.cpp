#include "elastos/droid/internal/utility/cm/PowerMenuConstants.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

static AutoPtr<ArrayOf<String> > InitStatic()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(10);

    array->Set(0, IPowerMenuConstants::GLOBAL_ACTION_KEY_POWER);
    array->Set(1, IPowerMenuConstants::GLOBAL_ACTION_KEY_REBOOT);
    array->Set(2, IPowerMenuConstants::GLOBAL_ACTION_KEY_SCREENSHOT);
    array->Set(3, IPowerMenuConstants::GLOBAL_ACTION_KEY_PROFILE);
    array->Set(4, IPowerMenuConstants::GLOBAL_ACTION_KEY_AIRPLANE);
    array->Set(5, IPowerMenuConstants::GLOBAL_ACTION_KEY_USERS);
    array->Set(6, IPowerMenuConstants::GLOBAL_ACTION_KEY_SETTINGS);
    array->Set(7, IPowerMenuConstants::GLOBAL_ACTION_KEY_LOCKDOWN);
    array->Set(8, IPowerMenuConstants::GLOBAL_ACTION_KEY_BUGREPORT);
    array->Set(9, IPowerMenuConstants::GLOBAL_ACTION_KEY_SILENT);

    return array;
}

AutoPtr<ArrayOf<String> > PowerMenuConstants::ALL_ACTIONS = InitStatic();

ECode PowerMenuConstants::GetAllActions(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ALL_ACTIONS;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
