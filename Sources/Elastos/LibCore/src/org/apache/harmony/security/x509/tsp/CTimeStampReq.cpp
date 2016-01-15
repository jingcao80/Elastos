
#include "CTimeStampReq.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {


ECode CTimeStampReq::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::GetCertReq(
    /* [out] */ Boolean * pCertReq)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::GetExtensions(
    /* [out] */ Org::Apache::Harmony::Security::X509::IExtensions ** ppExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::GetMessageImprint(
    /* [out] */ Org::Apache::Harmony::Security::X509::Tsp::IMessageImprint ** ppMessageImprint)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::GetNonce(
    /* [out] */ Elastos::Math::IBigInteger ** ppNonce)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::GetReqPolicy(
    /* [out] */ String * pReqPolicy)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::GetVersion(
    /* [out] */ Int32 * pVer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReq::constructor(
    /* [in] */ Int32 ver,
    /* [in] */ Org::Apache::Harmony::Security::X509::Tsp::IMessageImprint * pMessageImprint,
    /* [in] */ const String& reqPolicy,
    /* [in] */ Elastos::Math::IBigInteger * pNonce,
    /* [in] */ Boolean certReq,
    /* [in] */ Org::Apache::Harmony::Security::X509::IExtensions * pExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

