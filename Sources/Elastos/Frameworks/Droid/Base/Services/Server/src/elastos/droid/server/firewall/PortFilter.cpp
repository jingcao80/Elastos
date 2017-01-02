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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/server/firewall/PortFilter.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// PortFilter::FACTORY_FilterFactory
//------------------------------------------------------------------------------

PortFilter::FACTORY_FilterFactory::FACTORY_FilterFactory(
    /* [in] */ const String& tag)
{
    FilterFactory::constructor(tag);
}

ECode PortFilter::FACTORY_FilterFactory::NewFilter(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IFilter** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 lowerBound = NO_BOUND;
    Int32 upperBound = NO_BOUND;

    String equalsValue(NULL);
    parser->GetAttributeValue(String(NULL), ATTR_EQUALS, &equalsValue);
    if (!equalsValue.IsNull()) {
        Int32 value = StringUtils::ParseInt32(equalsValue);

        lowerBound = value;
        upperBound = value;
    }

    String lowerBoundString(NULL);
    String upperBoundString(NULL);
    parser->GetAttributeValue(String(NULL), ATTR_MIN, &lowerBoundString);
    parser->GetAttributeValue(String(NULL), ATTR_MAX, &upperBoundString);
    if (!lowerBoundString.IsNull() || !upperBoundString.IsNull()) {
        if (!equalsValue.IsNull()) {
            Slogger::I("PortFilter", "Port filter cannot use both equals and range filtering %p", parser);
            return E_XML_PULL_PARSER_EXCEPTION;
        }

        if (!lowerBoundString.IsNull()) {
            lowerBound = StringUtils::ParseInt32(lowerBoundString);
            // throw new XmlPullParserException(
            //        "Invalid minimum port value: " + lowerBoundString,
            //        parser, NULL);
        }

        if (!upperBoundString.IsNull()) {
            upperBound = StringUtils::ParseInt32(upperBoundString);

            //throw new XmlPullParserException(
            //        "Invalid maximum port value: " + upperBoundString,
            //        parser, NULL);
        }
    }

    // an empty port filter is explicitly allowed, and checks for the existence of a port
    AutoPtr<PortFilter> portFilter = new PortFilter(lowerBound, upperBound);
    *result = IFilter::Probe(portFilter);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=======================================================================================
// PortFilter
//=======================================================================================

CAR_INTERFACE_IMPL(PortFilter, Object, IFilter);

INIT_PROI_3 const AutoPtr<PortFilter::FACTORY_FilterFactory> PortFilter::FACTORY = new PortFilter::FACTORY_FilterFactory(String("port"));
const String PortFilter::ATTR_EQUALS("equals");
const String PortFilter::ATTR_MIN("min");
const String PortFilter::ATTR_MAX("max");
const Int32 PortFilter::NO_BOUND = -1;

PortFilter::PortFilter(
    /* [in] */ Int32 lowerBound,
    /* [in] */ Int32 upperBound)
{
    mLowerBound = lowerBound;
    mUpperBound = upperBound;
}

ECode PortFilter::Matches(
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
    Int32 port = -1;
    AutoPtr<IUri> uri;
    intent->GetData((IUri**)&uri);

    if (uri != NULL) {
        uri->GetPort(&port);
    }
    *ret = (port != -1 &&
            (mLowerBound == NO_BOUND || mLowerBound <= port) &&
            (mUpperBound == NO_BOUND || mUpperBound >= port));

    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
