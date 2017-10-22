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

#ifndef _ELASTOS_DROID_KEYSTORE_SECURITY_KEYSTORE_H_
#define _ELASTOS_DROID_KEYSTORE_SECURITY_KEYSTORE_H_

#include "Elastos.Droid.Security.h"
#include "Elastos.Droid.KeyStore.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
#include <keystore/IKeystoreService.h>

using Elastos::Droid::Os::IBinder;
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
private:
    class KeystoreServiceWrapper
        : public Object
        , public IBinder
        , public IIKeystoreService
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI Init();

        CARAPI Test(
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         */
        CARAPI Get(
            /* [in] */ const String& name,
            /* [out, callee] */ ArrayOf<Byte>** bytes);

        /**
         * throws RemoteException
         */
        CARAPI Insert(
            /* [in] */ const String& name,
            /* [in] */ ArrayOf<Byte>* item,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 flags,
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         */
        CARAPI Del(
            /* [in] */ const String& name,
            /* [in] */ Int32 uid,
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         */
        CARAPI Exist(
            /* [in] */ const String& name,
            /* [in] */ Int32 uid,
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         */
        CARAPI Saw(
            /* [in] */ const String& name,
            /* [in] */ Int32 uid,
            /* [out, callee] */ ArrayOf<String>** ret);

        /**
         * throws RemoteException
         * @Override
         */
        CARAPI Reset(
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         */
        CARAPI Password(
            /* [in] */ const String& password,
            /* [out] */ Int32* ret);

        using Object::Lock;

        /**
         * throws RemoteException
         */
        CARAPI Lock(
            /* [out] */ Int32* ret);

        using Object::Unlock;

        /**
         * throws RemoteException
         */
        CARAPI Unlock(
            /* [in] */ const String& password,
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         * @Override
         */
        CARAPI Zero(
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         */
        CARAPI Generate(
            /* [in] */ const String& name,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 keyType,
            /* [in] */ Int32 keySize,
            /* [in] */ Int32 flags,
            /* [in] */ ArrayOf<IArrayOf*>* args,
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         */
        CARAPI Import_key(
            /* [in] */ const String& name,
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 flags,
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         */
        CARAPI Sign(
            /* [in] */ const String& name,
            /* [in] */ ArrayOf<Byte>* data,
            /* [out, callee] */ ArrayOf<Byte>** ret);

        /**
         * throws RemoteException
         */
        CARAPI Verify(
            /* [in] */ const String& name,
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ ArrayOf<Byte>* signature,
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         */
        CARAPI Get_pubkey(
            /* [in] */ const String& name,
            /* [out, callee] */ ArrayOf<Byte>** ret);

        /**
         * throws RemoteException
         */
        CARAPI Del_key(
            /* [in] */ const String& name,
            /* [in] */ Int32 uid,
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         */
        CARAPI Grant(
            /* [in] */ const String& name,
            /* [in] */ Int32 granteeUid,
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         */
        CARAPI Ungrant(
            /* [in] */ const String& name,
            /* [in] */ Int32 granteeUid,
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         * @Override
         */
        CARAPI Getmtime(
            /* [in] */ const String& name,
            /* [out] */ Int64* ret);

        /**
         * throws RemoteException
         * @Override
         */
        CARAPI Duplicate(
            /* [in] */ const String& srcKey,
            /* [in] */ Int32 srcUid,
            /* [in] */ const String& destKey,
            /* [in] */ Int32 destUid,
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         * @Override
         */
        CARAPI Is_hardware_backed(
            /* [in] */ const String& keyType,
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         * @Override
         */
        CARAPI Clear_uid(
            /* [in] */ Int64 uid,
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         */
        CARAPI Reset_uid(
            /* [in] */ Int32 uid,
            /* [out] */ Int32* ret);

        /**
         * throws RemoteException
         */
        CARAPI Sync_uid(
            /* [in] */ Int32 srcUid,
            /* [in] */ Int32 dstUid,
            /* [out] */ Int32* result);

        /**
         * throws RemoteException
         */
        CARAPI Password_uid(
            /* [in] */ const String& password,
            /* [in] */ Int32 uid,
            /* [out] */ Int32* ret);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        android::sp<android::IKeystoreService> mKeystoreService;
    };

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

    using Object::Lock;

    CARAPI Lock(
        /* [out] */ Boolean* result);

    using Object::Unlock;

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
