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

#ifndef __ELASTOS_NET_CHTTPCOOKIEHELPER_H__
#define __ELASTOS_NET_CHTTPCOOKIEHELPER_H__

#include "_Elastos_Net_CHttpCookieHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::Utility::IList;

namespace Elastos {
namespace Net {

CarClass(CHttpCookieHelper)
    , public Singleton
    , public IHttpCookieHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsDomainMatches(
        /* [in] */ const String& domainPattern,
        /* [in] */ const String& host,
        /* [out] */ Boolean* match);

    CARAPI Parse(
        /* [in] */ const String& header,
        /* [out] */ IList** cookies);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CHTTPCOOKIEHELPER_H__
