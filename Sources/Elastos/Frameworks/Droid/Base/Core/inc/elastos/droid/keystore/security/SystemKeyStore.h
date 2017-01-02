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

#ifndef _ELASTOS_DROID_KEYSTORE_SECURITY_SYSTEMKEYSTORE_H_
#define _ELASTOS_DROID_KEYSTORE_SECURITY_SYSTEMKEYSTORE_H_

#include "Elastos.Droid.KeyStore.h"
#include "elastos/core/Object.h"

using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

class SystemKeyStore
    : public Object
    , public ISystemKeyStore
{
public:
    CAR_INTERFACE_DECL();

    static CARAPI_(AutoPtr<ISystemKeyStore>) GetInstance();

    static CARAPI_(String) ToHexString(
        /* [in] */ ArrayOf<Byte>* keyData);

    CARAPI GenerateNewKeyHexString(
        /* [in] */ Int32 numBits,
        /* [in] */ const String& algName,
        /* [in] */ const String& keyName,
        /* [out] */ String* hex);

    CARAPI GenerateNewKey(
        /* [in] */ Int32 numBits,
        /* [in] */ const String& algName,
        /* [in] */ const String& keyName,
        /* [out, callee] */ ArrayOf<Byte>** key);


    CARAPI RetrieveKeyHexString(
        /* [in] */ const String& keyName,
        /* [out] */ String* hex);

    CARAPI RetrieveKey(
        /* [in] */ const String& keyName,
        /* [out, callee] */ ArrayOf<Byte>** key);

    CARAPI DeleteKey(
        /* [in] */ const String& keyName);

    CARAPI constructor();
private:

    CARAPI_(AutoPtr<IFile>) GetKeyFile(
        /* [in] */ const String& keyName);

private:
    static const String SYSTEM_KEYSTORE_DIRECTORY;// = "misc/systemkeys";
    static const String KEY_FILE_EXTENSION;// = ".sks";
    static AutoPtr<ISystemKeyStore> mInstance;// = new SystemKeyStore();
};

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security

#endif // _ELASTOS_DROID_KEYSTORE_SECURITY_SYSTEMKEYSTORE_H_
