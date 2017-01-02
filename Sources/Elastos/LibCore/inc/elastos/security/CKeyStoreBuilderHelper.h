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

#ifndef __ELASTOS_SECURITY_CKEYSTOREBUILDERHELPER_H__
#define __ELASTOS_SECURITY_CKEYSTOREBUILDERHELPER_H__

#include "_Elastos_Security_CKeyStoreBuilderHelper.h"

using Elastos::IO::IFile;

namespace Elastos {
namespace Security {

CarClass(CKeyStoreBuilderHelper)
    , public Singleton
    , public IKeyStoreBuilderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewInstance(
        /* [in] */ IKeyStore* keyStore,
        /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
        /* [out] */ IKeyStoreBuilder** instance);

    CARAPI NewInstance(
        /* [in] */ const String& type,
        /* [in] */ IProvider* provider,
        /* [in] */ IFile* file,
        /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
        /* [out] */ IKeyStoreBuilder** instance);

    CARAPI NewInstance(
        /* [in] */ const String& type,
        /* [in] */ IProvider* provider,
        /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
        /* [out] */ IKeyStoreBuilder** instance);
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_CKEYSTOREBUILDERHELPER_H__
