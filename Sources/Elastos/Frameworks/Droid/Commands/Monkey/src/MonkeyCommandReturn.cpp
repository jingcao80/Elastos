#include "MonkeyCommandReturn.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyCommandReturn::MonkeyCommandReturn()
    : mMessage()
    , mSuccess(FALSE)
{
}

ECode MonkeyCommandReturn::HasMessage(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    *result = !const_cast<String*>(&mMessage)->IsNull();
    return NOERROR;
}

ECode MonkeyCommandReturn::GetMessage(
    /* [out] */ String *message)
{
    VALIDATE_NOT_NULL(message)
    *message = *const_cast<String*>(&mMessage);
    return NOERROR;
}

ECode MonkeyCommandReturn::WasSuccessful(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSuccess;
    return NOERROR;
}

ECode MonkeyCommandReturn::Init(
    /* [in] */ Boolean success,
    /* [in] */ const String& message)
{
    Boolean* suc = const_cast<Boolean*>(&mSuccess);
    *suc = success;
    String* msg = const_cast<String*>(&mMessage);
    *msg = message;
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
