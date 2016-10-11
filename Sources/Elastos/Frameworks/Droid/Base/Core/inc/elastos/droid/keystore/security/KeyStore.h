#ifndef _ELASTOS_DROID_KEYSTORE_SECURITY_KEYSTORE_H_
#define _ELASTOS_DROID_KEYSTORE_SECURITY_KEYSTORE_H_

#include "Elastos.Droid.Security.h"
#include "Elastos.Droid.KeyStore.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Security::IIKeystoreService;
using Elastos::Core::IArrayOf;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

/**
 * @hide This should not be made public in its present form because it
 * assumes that private and secret key bytes are available and would
 * preclude the use of hardware crypto.
 */
class KeyStore
    : public Object
    , public IKeyStore
{
public:
    // States
    //enum KeyStore_State { UNLOCKED, LOCKED, UNINITIALIZED };

    CARAPI constructor(
        /* [in] */ IIKeystoreService* binder);

public:
    CAR_INTERFACE_DECL();

    static CARAPI_(AutoPtr<KeyStore>) GetInstance();

    static CARAPI_(Int32) GetKeyTypeForAlgorithm(
        /* [in] */ const String& keyType);

    CARAPI State(
        /* [out] */ KeyStoreState* result);

    CARAPI IsUnlocked(
        /* [in] */ Boolean* result);

    CARAPI Get(
        /* [in] */ const String& key,
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* value,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* result);

    CARAPI Delete(
        /* [in] */ const String& key,
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    CARAPI Delete(
        /* [in] */ const String& key,
        /* [out] */ Boolean* result);

    CARAPI Contains(
        /* [in] */ const String& key,
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    CARAPI Contains(
        /* [in] */ const String& key,
        /* [out] */ Boolean* result);

    CARAPI Saw(
        /* [in] */ const String& prefix,
        /* [in] */ Int32 uid,
        /* [out] */ ArrayOf<String>** result);

    CARAPI Saw(
        /* [in] */ const String& prefix,
        /* [out] */ ArrayOf<String>** result);

    CARAPI Reset(
        /* [out] */ Boolean* result);

    CARAPI Password(
        /* [in] */ const String& password,
        /* [out] */ Boolean* result);

    CARAPI Lock(
        /* [out] */ Boolean* result);

    CARAPI Unlock(
        /* [in] */ const String& password,
        /* [out] */ Boolean* result);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI Generate(
        /* [in] */ const String& key,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 keyType,
        /* [in] */ Int32 keySize,
        /* [in] */ Int32 flags,
        /* [in] */ ArrayOf<IArrayOf*>* args,
        /* [out] */ Boolean* result);

    CARAPI ImportKey(
        /* [in] */ const String& keyName,
        /* [in] */ ArrayOf<Byte>* key,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* result);

    CARAPI GetPubkey(
        /* [in] */ const String& key,
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI DelKey(
        /* [in] */ const String& key,
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    CARAPI DelKey(
        /* [in] */ const String& key,
        /* [out] */ Boolean* result);

    CARAPI Sign(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI Verify(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<Byte>* signature,
        /* [out] */ Boolean* result);

    CARAPI Grant(
        /* [in] */ const String& key,
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    CARAPI Ungrant(
        /* [in] */ const String& key,
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    /**
     * Returns the last modification time of the key in milliseconds since the
     * epoch. Will return -1L if the key could not be found or other error.
     */
    CARAPI Getmtime(
        /* [in] */ const String& key,
        /* [out] */ Int64* result);

    CARAPI Duplicate(
        /* [in] */ const String& srcKey,
        /* [in] */ Int32 srcUid,
        /* [in] */ const String& destKey,
        /* [in] */ Int32 destUid,
        /* [out] */ Boolean* result);

    // TODO remove this when it's removed from Settings
    CARAPI IsHardwareBacked(
        /* [out] */ Boolean* result);

    CARAPI IsHardwareBacked(
        /* [in] */ const String& keyType,
        /* [out] */ Boolean* result);

    CARAPI ClearUid(
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    CARAPI ResetUid(
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    CARAPI SyncUid(
        /* [in] */ Int32 sourceUid,
        /* [in] */ Int32 targetUid,
        /* [out] */ Boolean* result);

    CARAPI PasswordUid(
        /* [in] */ const String& password,
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    CARAPI GetLastError(
        /* [out] */ Int32* result);

public:
    // ResponseCodes
    static const Int32 NO_ERROR = 1;
    static const Int32 LOCKED = 2;
    static const Int32 UNINITIALIZED = 3;
    static const Int32 SYSTEM_ERROR = 4;
    static const Int32 PROTOCOL_ERROR = 5;
    static const Int32 PERMISSION_DENIED = 6;
    static const Int32 KEY_NOT_FOUND = 7;
    static const Int32 VALUE_CORRUPTED = 8;
    static const Int32 UNDEFINED_ACTION = 9;
    static const Int32 WRONG_PASSWORD = 10;

    // Used for UID field to indicate the calling UID.
    static const Int32 UID_SELF = -1;

    // Flags for "put" "import" and "generate"
    static const Int32 FLAG_NONE = 0;
    static const Int32 FLAG_ENCRYPTED = 1;

private:
    static const String TAG;// = "KeyStore";

    Int32 mError;// = NO_ERROR;

    AutoPtr<IIKeystoreService> mBinder;

};

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security

#endif // _ELASTOS_DROID_KEYSTORE_SECURITY_KEYSTORE_H_
