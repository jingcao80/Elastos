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

#ifndef __ELASTOS_DROID_OS_CPERSISTABLEBUNDLEHELPER_H__
#define __ELASTOS_DROID_OS_CPERSISTABLEBUNDLEHELPER_H__

#include "_Elastos_Droid_Os_CPersistableBundleHelper.h"
#include <elastos/core/Singleton.h>

using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CPersistableBundleHelper)
    , public Singleton
    , public IPersistableBundleHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetEMPTY(
        /* [out] */ IPersistableBundle** pb);

    CARAPI ForPair(
        /* [in] */ const String& key,
        /* [in] */ const String& value,
        /* [out] */ IPersistableBundle** pb);

    CARAPI RestoreFromXml(
        /* [in] */ IXmlPullParser* in,
        /* [out] */ IPersistableBundle** pb);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_OS_CPERSISTABLEBUNDLEHELPER_H__
