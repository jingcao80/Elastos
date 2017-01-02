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

#ifndef __ELASTOS_DROID_NET_WIFI_CSCANRESULTINFORMATIONELEMENT_H__
#define __ELASTOS_DROID_NET_WIFI_CSCANRESULTINFORMATIONELEMENT_H__

#include "_Elastos_Droid_Wifi_CScanResultInformationElement.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CScanResultInformationElement)
    , public Object
    , public IScanResultInformationElement
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IScanResultInformationElement* rhs);

    CARAPI GetId(
        /* [out] */ Int32* result);

    CARAPI SetId(
        /* [in] */ Int32 id);

    CARAPI GetBytes(
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI SetBytes(
        /* [in] */ ArrayOf<Byte>* bytes);

private:
    Int32 mId;
    AutoPtr< ArrayOf<Byte> > mBytes;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CSCANRESULTINFORMATIONELEMENT_H__
