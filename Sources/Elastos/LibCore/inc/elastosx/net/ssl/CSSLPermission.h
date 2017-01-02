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

#ifndef __ELASTOSX_NET_SSL_CSSLPERMISSION_H__
#define __ELASTOSX_NET_SSL_CSSLPERMISSION_H__

#include "_Elastosx_Net_Ssl_CSSLPermission.h"
//TODO #include "BasicPermission.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Security::IPermission;
//TODO using Elastos::Security::BasicPermission;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * Legacy security code; do not use.
 */
CarClass(CSSLPermission)
//TOD0    , public BasicPermission
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& actions);

    CARAPI GetActions(
        /* [out] */ String* action);

    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* result);
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CSSLPERMISSION_H__
