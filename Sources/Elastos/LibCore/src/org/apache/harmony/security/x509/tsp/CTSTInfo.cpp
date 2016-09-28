
#include "org/apache/harmony/security/x509/tsp/CTSTInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_OBJECT_IMPL(CTSTInfo)

CAR_INTERFACE_IMPL(CTSTInfo, Object, ITSTInfo)

ECode CTSTInfo::ToString(
    /* [out] */ String* pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetAccuracy(
    /* [out, callee] */ ArrayOf<Int32>** ppAccuracy)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetExtensions(
    /* [out] */ IExtensions** ppExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetGenTime(
    /* [out] */ IDate** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetMessageImprint(
    /* [out] */ IMessageImprint** ppMessageImprint)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetNonce(
    /* [out] */ IBigInteger** ppNonce)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetOrdering(
    /* [out] */ Boolean* pOrdering)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetPolicy(
    /* [out] */ String* pPolicy)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetSerialNumber(
    /* [out] */ IBigInteger** ppSerialNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetTsa(
    /* [out] */ IGeneralName** ppTsa)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetVersion(
    /* [out] */ Int32* pVer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::constructor(
    /* [in] */ Int32 ver,
    /* [in] */ const String& policy,
    /* [in] */ IMessageImprint* pMessageImprint,
    /* [in] */ IBigInteger* pSerialNumber,
    /* [in] */ IDate* pGenTime,
    /* [in] */ ArrayOf<Int32>* pAccuracy,
    /* [in] */ Boolean ordering,
    /* [in] */ IBigInteger* pNonce,
    /* [in] */ IGeneralName* pTsa,
    /* [in] */ IExtensions* pExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org