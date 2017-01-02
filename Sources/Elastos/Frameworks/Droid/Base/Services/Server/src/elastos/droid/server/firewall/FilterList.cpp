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

#include "elastos/droid/server/firewall/FilterList.h"
#include "elastos/droid/server/firewall/IntentFirewall.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Internal::Utility::XmlUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

CAR_INTERFACE_IMPL(FilterList, Object, IFilter);

ECode FilterList::ReadFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 outerDepth;

    parser->GetDepth(&outerDepth);

    while (XmlUtils::NextElementWithin(parser, outerDepth)) {
        ReadChild(parser);
    }
    return NOERROR;
}

ECode FilterList::ReadChild(
    /* [in] */ IXmlPullParser* parser)
{
    AutoPtr<IFilter> filter;
    IntentFirewall::ParseFilter(parser, (IFilter**)&filter);
    mChildren->Add(TO_IINTERFACE(filter));
    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos

