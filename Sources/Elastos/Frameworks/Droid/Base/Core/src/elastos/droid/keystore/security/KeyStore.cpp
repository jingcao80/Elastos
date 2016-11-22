
#include "elastos/droid/keystore/security/KeyStore.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/text/TextUtils.h"
#include "_Org.Conscrypt.h"
#include <binder/Parcel.h>
#include <binder/IServiceManager.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Security::EIID_IIKeystoreService;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::IByte;
using Elastos::Utility::Logging::Logger;
using Org::Conscrypt::INativeCrypto;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL_2(KeyStore::KeystoreServiceWrapper, Object, IBinder, IIKeystoreService);
ECode KeyStore::KeystoreServiceWrapper::Init()
{
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    android::sp<android::IBinder> binder = sm->getService(android::String16("android.security.keystore"));

    if (binder == NULL) {
        // keystore service is now down, leave mCameraService as null
        Logger::W(TAG, "keystore service is now down");
        return NOERROR;
    }

    mKeystoreService = android::IKeystoreService::asInterface(binder);
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Test(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->test();
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Get(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes);
    *bytes = NULL;
    if (mKeystoreService.get() != NULL) {
        uint8_t* data = NULL;
        size_t len = 0;
        int32_t ret = mKeystoreService->get(android::String16(name), &data, &len);
        if (ret == 0 && len > 0) {
            *bytes = ArrayOf<Byte>::Alloc(len);
            REFCOUNT_ADD(*bytes);
            (*bytes)->Copy(data, len);
            free(data);
        }
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Insert(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Byte>* item,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 flags,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->insert(android::String16(name), item->GetPayload(),
                item->GetLength(), uid, flags);
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Del(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->del(android::String16(name), uid);
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Exist(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->exist(android::String16(name), uid);
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Saw(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [out, callee] */ ArrayOf<String>** ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = NULL;

    if (mKeystoreService.get() != NULL) {
        android::Vector<android::String16> matches;
        mKeystoreService->saw(android::String16(name), uid, &matches);
        Int32 size = matches.size();
        AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(size);

        if (size > 0) {
            android::Vector<android::String16>::const_iterator it = matches.begin();
            Int32 i = 0;
            for (; it != matches.end(); ++it) {
                array->Set(i++, TextUtils::String16ToString(*it));
            }
        }
        *ret = array;
        REFCOUNT_ADD(*ret);
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Reset(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->reset();
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Password(
    /* [in] */ const String& password,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->password(android::String16(password));
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Lock(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->lock();
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Unlock(
    /* [in] */ const String& password,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->unlock(android::String16(password));
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Zero(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->zero();
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Generate(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 keyType,
    /* [in] */ Int32 keySize,
    /* [in] */ Int32 flags,
    /* [in] */ ArrayOf<IArrayOf*>* _args,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        android::Vector<android::sp<android::KeystoreArg> > args;
        Int32 len = 0;
        if (_args != NULL && (len = _args->GetLength(), len > 0)) {
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                AutoPtr<IArrayOf> item = (*_args)[i];
                Int32 size = 0;
                item->GetLength(&size);
                if (size > 0) {
                    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(size);
                    for (Int32 j = 0; j < size; j++) {
                        AutoPtr<IInterface> o;
                        item->Get(j, (IInterface**)&o);
                        Byte v;
                        IByte::Probe(o)->GetValue(&v);
                        (*bytes)[j] = v;
                    }
                    android::sp<android::KeystoreArg> arg = new android::KeystoreArg(
                            bytes->GetPayload(), size);
                    args.push_back(arg);
                }
                else {
                    args.push_back(NULL);
                }

            }
        }
        *ret = mKeystoreService->generate(android::String16(name), uid, keyType, keySize
            , flags, &args);
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Import_key(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 flags,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->import(android::String16(name), data->GetPayload()
                , data->GetLength(), uid, flags);
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Sign(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out, callee] */ ArrayOf<Byte>** ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = NULL;
    if (mKeystoreService.get() != NULL) {
        void* out = NULL;
        size_t outSize = 0;
        mKeystoreService->sign(android::String16(name), data->GetPayload(), data->GetLength()
                , (uint8_t**) &out, &outSize);
        if (outSize > 0 && out != NULL) {
            *ret = ArrayOf<Byte>::Alloc(outSize);
            memcpy((*ret)->GetPayload(), out, outSize);
            REFCOUNT_ADD(*ret);
            free(out);
        }
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Verify(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* signature,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->verify(android::String16(name), data->GetPayload(), data->GetLength()
                , signature->GetPayload(), signature->GetLength());
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Get_pubkey(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<Byte>** ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = NULL;
    if (mKeystoreService.get() != NULL) {
        void* out = NULL;
        size_t outSize = 0;
        mKeystoreService->get_pubkey(android::String16(name), (unsigned char**) &out, &outSize);
        if (outSize > 0 && out != NULL) {
            *ret = ArrayOf<Byte>::Alloc(outSize);
            memcpy((*ret)->GetPayload(), out, outSize);
            REFCOUNT_ADD(*ret);
            free(out);
        }
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Del_key(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->del_key(android::String16(name), uid);
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Grant(
    /* [in] */ const String& name,
    /* [in] */ Int32 granteeUid,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->grant(android::String16(name), granteeUid);
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Ungrant(
    /* [in] */ const String& name,
    /* [in] */ Int32 granteeUid,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->ungrant(android::String16(name), granteeUid);
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Getmtime(
    /* [in] */ const String& name,
    /* [out] */ Int64* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->getmtime(android::String16(name));
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Duplicate(
    /* [in] */ const String& srcKey,
    /* [in] */ Int32 srcUid,
    /* [in] */ const String& destKey,
    /* [in] */ Int32 destUid,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->duplicate(android::String16(srcKey), srcUid
                , android::String16(destKey), destUid);
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Is_hardware_backed(
    /* [in] */ const String& keyType,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->is_hardware_backed(android::String16(keyType));
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Clear_uid(
    /* [in] */ Int64 uid,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->clear_uid(uid);
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Reset_uid(
    /* [in] */ Int32 uid,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->reset_uid(uid);
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Sync_uid(
    /* [in] */ Int32 srcUid,
    /* [in] */ Int32 dstUid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    if (mKeystoreService.get() != NULL) {
        *result = mKeystoreService->sync_uid(srcUid, dstUid);
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::Password_uid(
    /* [in] */ const String& password,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    if (mKeystoreService.get() != NULL) {
        *ret = mKeystoreService->password_uid(android::String16(password), uid);
    }
    return NOERROR;
}

ECode KeyStore::KeystoreServiceWrapper::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("KeyStore::KeystoreServiceWrapper");
    return NOERROR;
}

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
    AutoPtr<KeystoreServiceWrapper> keystore = new KeystoreServiceWrapper();
    keystore->Init();

    AutoPtr<KeyStore> ks = new KeyStore();
    ks->constructor(keystore);
    return ks;
}

Int32 KeyStore::GetKeyTypeForAlgorithm(
    /* [in] */ const String& keyType)
{
    if (String("RSA").EqualsIgnoreCase(keyType)) {
        return INativeCrypto::EVP_PKEY_RSA;
    }
    else if (String("DSA").EqualsIgnoreCase(keyType)) {
        return INativeCrypto::EVP_PKEY_DSA;
    }
    else if (String("EC").EqualsIgnoreCase(keyType)) {
        return INativeCrypto::EVP_PKEY_EC;
    }
    else {
        //throw new IllegalArgumentException("Unsupported key type: " + keyType);
        Logger::E("KeyStore", "GetKeyTypeForAlgorithm, Unsupported key type: %s", keyType.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
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
        case NO_ERROR: {
            *result = KeyStoreState_UNLOCKED;
            break;
        }
        case LOCKED: {
            *result = KeyStoreState_LOCKED;
            break;
        }
        case UNINITIALIZED: {
            *result = KeyStoreState_UNINITIALIZED;
            break;
        }
        default: //throw new AssertionError(mError);
            Logger::E("KeyStore", "State");
            *result = KeyStoreState_UNLOCKED;
    }
    return NOERROR;
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

