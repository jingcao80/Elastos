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

#include "elastos/droid/server/firewall/NotFilter.h"
#include "elastos/droid/server/firewall/IntentFirewall.h"
#include "elastos/droid/internal/utility/XmlUtils.h"

using Elastos::Droid::Internal::Utility::XmlUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// NotFilter::FACTORY_FilterFactory
//------------------------------------------------------------------------------

NotFilter::FACTORY_FilterFactory::FACTORY_FilterFactory(
    /* [in] */ const String& tag)
{
    FilterFactory::constructor(tag);
}

ECode NotFilter::FACTORY_FilterFactory::NewFilter(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IFilter** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IFilter> child;
    Int32 outerDepth = 0;
    parser->GetDepth(&outerDepth);

    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        AutoPtr<IFilter> filter;
        IntentFirewall::ParseFilter(parser, (IFilter**)&filter);
        if (child == NULL) {
            child = filter;
        }
        else {
            //throw new XmlPullParserException(
            //        "<not> tag can only contain a single child filter.", parser, null);
            return E_XML_PULL_PARSER_EXCEPTION;
        }
    }
    AutoPtr<NotFilter> notFilter = new NotFilter(child);
    *result = IFilter::Probe(notFilter);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=======================================================================================
// NotFilter
//=======================================================================================

CAR_INTERFACE_IMPL(NotFilter, Object, IFilter);

INIT_PROI_3 const AutoPtr<NotFilter::FACTORY_FilterFactory> NotFilter::FACTORY = new NotFilter::FACTORY_FilterFactory(String("not"));

NotFilter::NotFilter(
    /* in */ IFilter* child)
    : mChild(child)
{}

ECode NotFilter::Matches(
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
    mChild->Matches(ifw, resolvedComponent, intent, callerUid, callerPid, resolvedType,
                receivingUid, ret);
    *ret = !*ret;
    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
