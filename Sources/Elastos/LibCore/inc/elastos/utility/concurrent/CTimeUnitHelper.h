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

#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTIMEUNITHELPER_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTIMEUNITHELPER_H__

#include "_Elastos_Utility_Concurrent_CTimeUnitHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CTimeUnitHelper)
    , public Singleton
    , public ITimeUnitHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetNANOSECONDS(
        /* [out] */ ITimeUnit** unit);

    CARAPI GetMICROSECONDS(
        /* [out] */ ITimeUnit** unit);

    CARAPI GetMILLISECONDS(
        /* [out] */ ITimeUnit** unit);

    CARAPI GetSECONDS(
        /* [out] */ ITimeUnit** unit);

    CARAPI GetMINUTES(
        /* [out] */ ITimeUnit** unit);

    CARAPI GetHOURS(
        /* [out] */ ITimeUnit** unit);

    CARAPI GetDAYS(
        /* [out] */ ITimeUnit** unit);
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTIMEUNITHELPER_H__
