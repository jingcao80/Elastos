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

#include "elastos/droid/server/firewall/SenderPackageFilter.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Os::UserHandle;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// SenderPackageFilter::FACTORY_FilterFactory
//------------------------------------------------------------------------------

SenderPackageFilter::FACTORY_FilterFactory::FACTORY_FilterFactory(
    /* [in] */ const String& tag)
{
    FilterFactory::constructor(tag);
}

ECode SenderPackageFilter::FACTORY_FilterFactory::NewFilter(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IFilter** result)
{
    VALIDATE_NOT_NULL(result)
    String packageName;
    parser->GetAttributeValue(String(NULL), ATTR_NAME, &packageName);
    if (packageName.IsNull()) {
        Slogger::I("SenderPackageFilter",
            "A package name must be specified. %p", parser);
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    AutoPtr<SenderPackageFilter> spFilter = new SenderPackageFilter(packageName);
    *result = IFilter::Probe(spFilter);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=======================================================================================
// SenderPackageFilter
//=======================================================================================

INIT_PROI_3 const AutoPtr<SenderPackageFilter::FACTORY_FilterFactory> SenderPackageFilter::FACTORY = new SenderPackageFilter::FACTORY_FilterFactory(String("sender-package"));
const String SenderPackageFilter::ATTR_NAME("name");

CAR_INTERFACE_IMPL(SenderPackageFilter, Object, IFilter);

SenderPackageFilter::SenderPackageFilter(
    /* in */ const String& packageName)
    :  mPackageName(packageName)
{}

ECode SenderPackageFilter::Matches(
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
    Int32 packageUid = -1;

    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    FAIL_RETURN(pm->GetPackageUid(mPackageName, UserHandle::USER_OWNER, &packageUid));

    if (packageUid == -1)  {
        *ret = FALSE;
    } else {
        *ret = UserHandle::IsSameApp(packageUid, callerUid);
    }
    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
