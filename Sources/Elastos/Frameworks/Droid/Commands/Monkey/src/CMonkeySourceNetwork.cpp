
#include "CMonkeySourceNetwork.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkeySourceNetwork::GetNextEvent(
    /* [out] */ IMonkeyEvent **event)
{
    // TODO: Add your code here
    VALIDATE_NOT_NULL(event)
    MonkeySourceNetwork::GetNextEvent(event);
    return NOERROR;
}

ECode CMonkeySourceNetwork::SetVerbose(
    /* [in] */ Int32 verbose)
{
    // TODO: Add your code here
    MonkeySourceNetwork::SetVerbose(verbose);
    return NOERROR;
}

ECode CMonkeySourceNetwork::Validate(
    /* [out] */ Boolean *result)
{
    // TODO: Add your code here
    VALIDATE_NOT_NULL(result)
    MonkeySourceNetwork::Validate(result);
    return NOERROR;
}

ECode CMonkeySourceNetwork::constructor(
    /* [in] */ Int32 port)
{
    // TODO: Add your code here
    MonkeySourceNetwork::Init(port);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
