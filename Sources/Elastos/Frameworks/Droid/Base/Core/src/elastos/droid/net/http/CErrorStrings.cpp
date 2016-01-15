
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/http/CErrorStrings.h"
#include "elastos/droid/net/http/ErrorStrings.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(CErrorStrings, Singleton, IErrorStrings)

CAR_SINGLETON_IMPL(CErrorStrings)

ECode CErrorStrings::GetString(
        /* [in] */ Int32 errorCode,
        /* [in] */ IContext* context,
        /* [out] */ String* result)
{
    return ErrorStrings::GetString(errorCode, context, result);
}

ECode CErrorStrings::GetResource(
        /* [in] */ Int32 errorCode,
        /* [out] */ Int32* result)
{
    return ErrorStrings::GetResource(errorCode, result);
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

