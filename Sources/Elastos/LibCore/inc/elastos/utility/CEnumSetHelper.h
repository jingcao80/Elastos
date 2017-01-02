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

#ifndef __ELASTOS_UTILITY_CENUMSETHELPER_H__
#define __ELASTOS_UTILITY_CENUMSETHELPER_H__

#include "_Elastos_Utility_CEnumSetHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos{
namespace Utility{

CarClass (CEnumSetHelper)
    , public Singleton
    , public IEnumSetHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI NoneOf(
        /* [in] */ const InterfaceID& type,
        /* [out] */ IEnumSet** res);

    CARAPI AllOf(
        /* [in] */ const InterfaceID& type,
        /* [out] */ IEnumSet** res);

    CARAPI CopyOf(
        /* [in] */ IEnumSet* s,
        /* [out] */ IEnumSet** res);

    CARAPI CopyOf(
        /* [in] */ ICollection* c,
        /* [out] */ IEnumSet** set);

    CARAPI ComplementOf(
        /* [in] */ IEnumSet* s,
        /* [out] */ IEnumSet** res);

   CARAPI  Of(
        /* [in] */ IInterface* i,
        /* [out] */ IEnumSet** res);

    CARAPI Of(
        /* [in] */ IInterface* i1,
        /* [in] */ IInterface* i2,
        /* [out] */ IEnumSet** res);

    CARAPI Of(
        /* [in] */ IInterface* i1,
        /* [in] */ IInterface* i2,
        /* [in] */ IInterface* i3,
        /* [out] */ IEnumSet** res);

    CARAPI Of(
        /* [in] */ IInterface* i1,
        /* [in] */ IInterface* i2,
        /* [in] */ IInterface* i3,
        /* [in] */ IInterface* i4,
        /* [out] */ IEnumSet** res);

    CARAPI Of(
        /* [in] */ IInterface* i1,
        /* [in] */ IInterface* i2,
        /* [in] */ IInterface* i3,
        /* [in] */ IInterface* i4,
        /* [in] */ IInterface* i5,
        /* [out] */ IEnumSet** res);

    CARAPI Of(
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [out] */ IEnumSet** res);

    CARAPI Range(
        /* [in] */ IInterface* start,
        /* [in] */ IInterface* end,
        /* [out] */ IEnumSet** set);
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CENUMSETHELPER_H__