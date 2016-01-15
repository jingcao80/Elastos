
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CNetworkTemplate.h"
#include "elastos/droid/net/CNetworkTemplateHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CNetworkTemplateHelper, Singleton, INetworkTemplateHelper)

CAR_SINGLETON_IMPL(CNetworkTemplateHelper)

ECode CNetworkTemplateHelper::ForceAllNetworkTypes()
{
    return CNetworkTemplate::ForceAllNetworkTypes();
}

ECode CNetworkTemplateHelper::BuildTemplateMobileAll(
    /* [in] */ const String& subscriberId,
    /* [out] */ INetworkTemplate** result)
{
    VALIDATE_NOT_NULL(result);
    return CNetworkTemplate::BuildTemplateMobileAll(subscriberId, result);
}

ECode CNetworkTemplateHelper::BuildTemplateMobile3gLower(
    /* [in] */ const String& subscriberId,
    /* [out] */ INetworkTemplate** result)
{
    VALIDATE_NOT_NULL(result);
    return CNetworkTemplate::BuildTemplateMobile3gLower(subscriberId, result);
}

ECode CNetworkTemplateHelper::BuildTemplateMobile4g(
    /* [in] */ const String& subscriberId,
    /* [out] */ INetworkTemplate** result)
{
    VALIDATE_NOT_NULL(result);
    return CNetworkTemplate::BuildTemplateMobile4g(subscriberId, result);
}

ECode CNetworkTemplateHelper::BuildTemplateMobileWildcard(
    /* [out] */ INetworkTemplate** result)
{
    VALIDATE_NOT_NULL(result);
    return CNetworkTemplate::BuildTemplateMobileWildcard(result);
}

ECode CNetworkTemplateHelper::BuildTemplateWifiWildcard(
    /* [out] */ INetworkTemplate** result)
{
    VALIDATE_NOT_NULL(result);
    return CNetworkTemplate::BuildTemplateWifiWildcard(result);
}

ECode CNetworkTemplateHelper::BuildTemplateWifi(
    /* [out] */ INetworkTemplate** result)
{
    VALIDATE_NOT_NULL(result);
    return CNetworkTemplate::BuildTemplateWifi(result);
}

ECode CNetworkTemplateHelper::BuildTemplateWifi(
    /* [in] */ const String& networkId,
    /* [out] */ INetworkTemplate** result)
{
    VALIDATE_NOT_NULL(result);
    return CNetworkTemplate::BuildTemplateWifi(networkId, result);
}

ECode CNetworkTemplateHelper::BuildTemplateEthernet(
    /* [out] */ INetworkTemplate** result)
{
    VALIDATE_NOT_NULL(result);
    return CNetworkTemplate::BuildTemplateEthernet(result);
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos
