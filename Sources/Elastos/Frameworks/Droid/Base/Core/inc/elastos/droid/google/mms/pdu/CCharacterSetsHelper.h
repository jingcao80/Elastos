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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_CCHARACTERSETSHELPER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_CCHARACTERSETSHELPER_H__

#include "_Elastos_Droid_Google_Mms_Pdu_CCharacterSetsHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

CarClass(CCharacterSetsHelper)
    , public Singleton
    , public ICharacterSetsHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI GetMimeName(
        /* [in] */ Int32 mibEnumValue,
        /* [out] */ String* result);

    CARAPI GetMibEnumValue(
        /* [in] */ const String& mimeName,
        /* [out] */ Int32* result);

};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_CCHARACTERSETSHELPER_H__
