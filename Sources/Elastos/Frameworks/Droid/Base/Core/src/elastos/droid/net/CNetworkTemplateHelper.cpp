//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
