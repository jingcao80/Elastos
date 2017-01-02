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

#ifndef __ELASTOS_DROID_Server_Firewall_FilterList_H__
#define __ELASTOS_DROID_Server_Firewall_FilterList_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Server::Firewall::IFilter;

using Elastos::Utility::IArrayList;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

class FilterList
    : public Object
    , public IFilter
{
public:
    CAR_INTERFACE_DECL()

    // throws IOException, XmlPullParserException
    CARAPI ReadFromXml(
        /* [in] */ IXmlPullParser* parser);

    // throws IOException, XmlPullParserException
    CARAPI ReadChild(
        /* [in] */ IXmlPullParser* parser);

protected:
    AutoPtr<IArrayList> mChildren;
};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_FilterList_H__
