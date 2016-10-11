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
