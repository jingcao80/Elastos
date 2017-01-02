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

#ifndef __ELASTOS_NET_COOKIEHANDLER_H__
#define __ELASTOS_NET_COOKIEHANDLER_H__

#include "Elastos.CoreLibrary.Net.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Net::ICookieHandler;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Net {

class CookieHandler
    : public Object
    , public ICookieHandler
{
public:
    CAR_INTERFACE_DECL()

    CookieHandler();

    virtual ~CookieHandler();

    static CARAPI GetDefault(
        /* [out] */ ICookieHandler** handler);

    static CARAPI SetDefault(
        /* [in] */ ICookieHandler* cHandler);

    CARAPI Get(
        /* [in] */ IURI* uri,
        /* [in] */ IMap* requestHeaders,
        /* [out]*/ IMap** cookiesMap);

    CARAPI Put(
        /* [in] */ IURI* uri,
        /* [in] */ IMap* requestHeaders);

private:
    static AutoPtr<ICookieHandler> sSystemWideCookieHandler;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_COOKIEHANDLER_H__
