
#include "CMonkeyCommandReturn.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkeyCommandReturn::HasMessage(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result);
    return MonkeyCommandReturn::HasMessage(result);
}

ECode CMonkeyCommandReturn::GetMessage(
    /* [out] */ String *message)
{
    VALIDATE_NOT_NULL(message);
    return GetMessage(message);
}

ECode CMonkeyCommandReturn::WasSuccessful(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result);
    return MonkeyCommandReturn::WasSuccessful(result);
}

ECode CMonkeyCommandReturn::constructor(
    /* [in] */ Boolean success)
{
    return MonkeyCommandReturn::Init(success);
}

ECode CMonkeyCommandReturn::constructor(
    /* [in] */ Boolean success,
    /* [in] */ const String& message)
{
    return MonkeyCommandReturn::Init(success, message);
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos