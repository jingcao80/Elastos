
#include "CTSTInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {


ECode CTSTInfo::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetAccuracy(
    /* [out, callee] */ ArrayOf<Int32> ** ppAccuracy)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetExtensions(
    /* [out] */ Org::Apache::Harmony::Security::X509::IExtensions ** ppExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetGenTime(
    /* [out] */ Elastos::Utility::IDate ** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetMessageImprint(
    /* [out] */ Org::Apache::Harmony::Security::X509::Tsp::IMessageImprint ** ppMessageImprint)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetNonce(
    /* [out] */ Elastos::Math::IBigInteger ** ppNonce)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetOrdering(
    /* [out] */ Boolean * pOrdering)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetPolicy(
    /* [out] */ String * pPolicy)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetSerialNumber(
    /* [out] */ Elastos::Math::IBigInteger ** ppSerialNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetTsa(
    /* [out] */ Org::Apache::Harmony::Security::X509::IGeneralName ** ppTsa)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::GetVersion(
    /* [out] */ Int32 * pVer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTSTInfo::constructor(
    /* [in] */ Int32 ver,
    /* [in] */ const String& policy,
    /* [in] */ Org::Apache::Harmony::Security::X509::Tsp::IMessageImprint * pMessageImprint,
    /* [in] */ Elastos::Math::IBigInteger * pSerialNumber,
    /* [in] */ Elastos::Utility::IDate * pGenTime,
    /* [in] */ ArrayOf<Int32> * pAccuracy,
    /* [in] */ Boolean ordering,
    /* [in] */ Elastos::Math::IBigInteger * pNonce,
    /* [in] */ Org::Apache::Harmony::Security::X509::IGeneralName * pTsa,
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

