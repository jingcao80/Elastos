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

#ifndef __ELASTOS_DROID_TELEPHONY_CDMA_CCDMACELLLOCATIONHELPER_H__
#define __ELASTOS_DROID_TELEPHONY_CDMA_CCDMACELLLOCATIONHELPER_H__

#include "_Elastos_Droid_Telephony_Cdma_CCdmaCellLocationHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Cdma {

CarClass(CCdmaCellLocationHelper)
    , public Singleton
    , public ICdmaCellLocationHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ConvertQuartSecToDecDegrees(
        /* [in] */ Int32 quartSec,
        /* [out] */ Double* val);
};

} // namespace Cdma
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CDMA_CCDMACELLLOCATIONHELPER_H__
