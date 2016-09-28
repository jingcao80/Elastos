
#include "org/apache/harmony/security/x509/tsp/CTimeStampReq.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_OBJECT_IMPL(CTimeStampReq)

CAR_INTERFACE_IMPL(CTimeStampReq, Object, ITimeStampReq)

ECode CTimeStampReq::ToString(
    /* [out] */ String* pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::GetCertReq(
    /* [out] */ Boolean* pCertReq)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::GetExtensions(
    /* [out] */ IExtensions** ppExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::GetMessageImprint(
    /* [out] */ IMessageImprint** ppMessageImprint)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::GetNonce(
    /* [out] */ IBigInteger** ppNonce)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::GetReqPolicy(
    /* [out] */ String* pReqPolicy)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::GetVersion(
    /* [out] */ Int32* pVer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::constructor(
    /* [in] */ Int32 ver,
    /* [in] */ IMessageImprint* pMessageImprint,
    /* [in] */ const String& reqPolicy,
    /* [in] */ IBigInteger* pNonce,
    /* [in] */ Boolean certReq,
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