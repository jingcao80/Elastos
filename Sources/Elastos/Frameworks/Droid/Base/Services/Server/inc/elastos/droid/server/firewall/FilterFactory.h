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

#ifndef __ELASTOS_DROID_Server_Firewall_FilterFactory_H__
#define __ELASTOS_DROID_Server_Firewall_FilterFactory_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Server::Firewall::IFilter;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

class FilterFactory
    : public Object
{
public:
    CARAPI_(String) GetTagName();

    virtual CARAPI NewFilter(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ IFilter** result) = 0;

protected:
    FilterFactory();

    CARAPI constructor(
        /* [in] */ const String& tag);

private:
    String mTag;
};

} // Firewall
} // Server
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Firewall::FilterFactory, IInterface)

#endif // __ELASTOS_DROID_Server_Firewall_FilterFactory_H__
