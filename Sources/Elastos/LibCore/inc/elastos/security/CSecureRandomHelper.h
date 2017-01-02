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

#ifndef __ELASTOS_SECURITY_CSECURERANDOMHELPER_H__
#define __ELASTOS_SECURITY_CSECURERANDOMHELPER_H__

#include "_Elastos_Security_CSecureRandomHelper.h"
#include "Singleton.h"

namespace Elastos {
namespace Security {

CarClass(CSecureRandomHelper)
    , public Singleton
    , public ISecureRandomHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException*/;

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException, NoSuchProviderException*/;

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException*/;

    CARAPI GetSeed(
        /* [in] */ Int32 numBytes,
        /* [out] */ ArrayOf<Byte>** seed);
};

}
}

#endif // __ELASTOS_SECURITY_CSECURERANDOMHELPER_H__
