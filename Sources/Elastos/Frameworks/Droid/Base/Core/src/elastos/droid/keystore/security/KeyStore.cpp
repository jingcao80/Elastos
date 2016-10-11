#include "elastos/droid/keystore/security/KeyStore.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/utility/logging/Logger.h>


//import com.android.org.conscrypt.NativeCrypto;
//
//import android.os.RemoteException;
//import android.os.ServiceManager;
using Elastos::Droid::Os::ServiceManager;
//import android.util.Log;
//
//import java.util.Locale;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

const String KeyStore::TAG("KeyStore");

const Int32 KeyStore::NO_ERROR;// = 1;
const Int32 KeyStore::LOCKED;// = 2;
const Int32 KeyStore::UNINITIALIZED;// = 3;
const Int32 KeyStore::SYSTEM_ERROR;// = 4;
const Int32 KeyStore::PROTOCOL_ERROR;// = 5;
const Int32 KeyStore::PERMISSION_DENIED;// = 6;
const Int32 KeyStore::KEY_NOT_FOUND;// = 7;
const Int32 KeyStore::VALUE_CORRUPTED;// = 8;
const Int32 KeyStore::UNDEFINED_ACTION;// = 9;
const Int32 KeyStore::WRONG_PASSWORD;// = 10;
const Int32 KeyStore::UID_SELF;// = -1;
const Int32 KeyStore::FLAG_NONE;// = 0;
const Int32 KeyStore::FLAG_ENCRYPTED;// = 1;

CAR_INTERFACE_IMPL(KeyStore, Object, IKeyStore);

ECode KeyStore::constructor(
    /* [in] */ IIKeystoreService* binder)
{
    mError = NO_ERROR;
    mBinder = binder;
    return NOERROR;
}

AutoPtr<KeyStore> KeyStore::GetInstance()
{
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("android.security.keystore"));
    AutoPtr<IIKeystoreService> keystore = IIKeystoreService::Probe(obj);

    AutoPtr<KeyStore> ks = new KeyStore();
    ks->constructor(keystore);
    return ks;
}

Int32 KeyStore::GetKeyTypeForAlgorithm(
    /* [in] */ const String& keyType)
{
    if (String("RSA").EqualsIgnoreCase(keyType)) {
        return 6/*TODO INativeCrypto::EVP_PKEY_RSA*/;
    } else if (String("DSA").EqualsIgnoreCase(keyType)) {
        return 116/*TODO INativeCrypto::EVP_PKEY_DSA*/;
    } else if (String("EC").EqualsIgnoreCase(keyType)) {
        return 408/*TODO INativeCrypto::EVP_PKEY_EC*/;
    } else {
        //throw new IllegalArgumentException("Unsupported key type: " + keyType);
        Logger::E("KeyStore", "GetKeyTypeForAlgorithm, Unsupported key type: %s", keyType.string());
        assert(0);
        return -1;
    }
}

ECode KeyStore::State(
    /* [out] */ KeyStoreState* result)
{
    Int32 ret;
    //try {
    mBinder->Test(&ret);
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    throw new AssertionError(e);
    //}

    switch (ret) {
        case NO_ERROR: return KeyStoreState_UNLOCKED;
        case LOCKED: return KeyStoreState_LOCKED;
        case UNINITIALIZED: return KeyStoreState_UNINITIALIZED;
        default: //throw new AssertionError(mError);
            Logger::E("KeyStore", "State");
            assert(0);
            return KeyStoreState_UNLOCKED;
    }
}

ECode KeyStore::IsUnlocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    KeyStoreState kState;
    State(&kState);
    *result = kState == KeyStoreState_UNLOCKED;
    return NOERROR;
}

ECode KeyStore::Get(
    /* [in] */ const String& key,
    /* [out] */ ArrayOf<Byte>** result)
{
    //try {
    return mBinder->Get(key, result);
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return null;
    //}
}

ECode KeyStore::Put(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Byte>* value,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Insert(key, value, uid, flags, &ret);
    *result = (ret == NO_ERROR);
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::Delete(
    /* [in] */ const String& key,
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Del(key, uid, &ret);
    *result = ret == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::Delete(
    /* [in] */ const String& key,
    /* [out] */ Boolean* result)
{
    return Delete(key, UID_SELF, result);
}

ECode KeyStore::Contains(
    /* [in] */ const String& key,
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Exist(key, uid, &ret);
    *result = ret == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::Contains(
    /* [in] */ const String& key,
    /* [out] */ Boolean* result)
{
    return Contains(key, UID_SELF, result);
}

ECode KeyStore::Saw(
    /* [in] */ const String& prefix,
    /* [in] */ Int32 uid,
    /* [out] */ ArrayOf<String>** result)
{
    //try {
    return mBinder->Saw(prefix, uid, result);
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return null;
    //}
}

ECode KeyStore::Saw(
    /* [in] */ const String& prefix,
    /* [out] */ ArrayOf<String>** result)
{
    return Saw(prefix, UID_SELF, result);
}

ECode KeyStore::Reset(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 ret;
    //try {
    mBinder->Reset(&ret);
    *result = ret == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::Password(
    /* [in] */ const String& password,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Password(password, &ret);
    *result = ret == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::Lock(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Lock(&ret);
    *result = ret == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::Unlock(
    /* [in] */ const String& password,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    mBinder->Unlock(password, &mError);
    *result = mError == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Zero(&ret);
    *result = ret == KEY_NOT_FOUND;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::Generate(
    /* [in] */ const String& key,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 keyType,
    /* [in] */ Int32 keySize,
    /* [in] */ Int32 flags,
    /* [in] */ ArrayOf<IArrayOf*>* args,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Generate(key, uid, keyType, keySize, flags, args, &ret);
    *result = ret == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::ImportKey(
    /* [in] */ const String& keyName,
    /* [in] */ ArrayOf<Byte>* key,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Import_key(keyName, key, uid, flags, &ret);
    *result = ret == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::GetPubkey(
    /* [in] */ const String& key,
    /* [out] */ ArrayOf<Byte>** result)
{
    //try {
    return mBinder->Get_pubkey(key, result);
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return null;
    //}
}

ECode KeyStore::DelKey(
    /* [in] */ const String& key,
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Del_key(key, uid, &ret);
    *result = ret == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::DelKey(
    /* [in] */ const String& key,
    /* [out] */ Boolean* result)
{
    return DelKey(key, UID_SELF, result);
}

ECode KeyStore::Sign(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    return mBinder->Sign(key, data, result);
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return null;
    //}
}

ECode KeyStore::Verify(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* signature,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Verify(key, data, signature, &ret);
    *result = ret == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::Grant(
    /* [in] */ const String& key,
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Grant(key, uid, &ret);
    *result = ret == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::Ungrant(
    /* [in] */ const String& key,
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Ungrant(key, uid, &ret);
    *result = ret == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::Getmtime(
    /* [in] */ const String& key,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int64 millis;
    mBinder->Getmtime(key, &millis);
    if (millis == -1L) {
        *result = -1L;
        return NOERROR;
    }

    *result = millis * 1000L;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return -1L;
    //}
}

ECode KeyStore::Duplicate(
    /* [in] */ const String& srcKey,
    /* [in] */ Int32 srcUid,
    /* [in] */ const String& destKey,
    /* [in] */ Int32 destUid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Duplicate(srcKey, srcUid, destKey, destUid, &ret);
    *result = ret == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::IsHardwareBacked(
    /* [out] */ Boolean* result)
{
    return IsHardwareBacked(String("RSA"), result);
}

ECode KeyStore::IsHardwareBacked(
    /* [in] */ const String& keyType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Is_hardware_backed(keyType.ToUpperCase(/*Locale.US*/), &ret);
    *result = ret == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::ClearUid(
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    Int32 ret;
    mBinder->Clear_uid(uid, &ret);
    *result = ret == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::ResetUid(
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    mBinder->Reset_uid(uid, &mError);
    *result = mError == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::SyncUid(
    /* [in] */ Int32 sourceUid,
    /* [in] */ Int32 targetUid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    mBinder->Sync_uid(sourceUid, targetUid, &mError);
    *result = mError == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::PasswordUid(
    /* [in] */ const String& password,
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    mBinder->Password_uid(password, uid, &mError);
    *result = mError == NO_ERROR;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.w(TAG, "Cannot connect to keystore", e);
    //    return false;
    //}
}

ECode KeyStore::GetLastError(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mError;
    return NOERROR;
}

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security

