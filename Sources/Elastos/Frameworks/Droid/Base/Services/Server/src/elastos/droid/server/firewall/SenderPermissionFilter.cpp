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

#include "elastos/droid/server/firewall/SenderPermissionFilter.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Os::UserHandle;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// SenderPermissionFilter::FACTORY_FilterFactory
//------------------------------------------------------------------------------

SenderPermissionFilter::FACTORY_FilterFactory::FACTORY_FilterFactory(
    /* [in] */ const String& tag)
{
    FilterFactory::constructor(tag);
}

ECode SenderPermissionFilter::FACTORY_FilterFactory::NewFilter(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IFilter** result)
{
    VALIDATE_NOT_NULL(result)
    String permission;
    parser->GetAttributeValue(String(NULL), ATTR_NAME, &permission);
    if (permission.IsNull()) {
        Slogger::I("SenderPermissionFilter",
            "A package name must be specified. %p", parser);
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    AutoPtr<SenderPermissionFilter> spFilter = new SenderPermissionFilter(permission);
    *result = IFilter::Probe(spFilter);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=======================================================================================
// SenderPermissionFilter
//=======================================================================================

INIT_PROI_3 const AutoPtr<SenderPermissionFilter::FACTORY_FilterFactory> SenderPermissionFilter::FACTORY = new SenderPermissionFilter::FACTORY_FilterFactory(String("sender-permission"));
const String SenderPermissionFilter::ATTR_NAME("name");

CAR_INTERFACE_IMPL(SenderPermissionFilter, Object, IFilter);

SenderPermissionFilter::SenderPermissionFilter(
    /* [in] */ const String& permission)
    : mPermission(permission)
{}

ECode SenderPermissionFilter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    // We assume the component is exported here. If the component is not exported, then
    // ActivityManager would only resolve to this component for callers from the same uid.
    // In this case, it doesn't matter whether the component is exported or not.
    ifw->CheckComponentPermission(mPermission, callerPid, callerUid, receivingUid, TRUE, ret);

    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
