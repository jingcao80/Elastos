
#include "elastos/coredef.h"
#include "elastos/net/CURI.h"
#include "elastos/net/CURIHelper.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CURIHelper, Singleton, IURIHelper)

CAR_SINGLETON_IMPL(CURIHelper)

ECode CURIHelper::Create(
    /* [in] */ const String& uri,
    /* [out] */ IURI** obj)
{
    return URI::Create(uri, obj);
}

ECode CURIHelper::GetEffectivePort(
    /* [in] */ const String& scheme,
    /* [in] */ Int32 specifiedPort,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = URI::GetEffectivePort(scheme, specifiedPort);
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
