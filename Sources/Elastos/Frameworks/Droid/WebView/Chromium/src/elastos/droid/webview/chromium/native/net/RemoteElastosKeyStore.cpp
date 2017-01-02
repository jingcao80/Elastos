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

#include "elastos/droid/webkit/webview/chromium/native/net/RemoteElastosKeyStore.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

//=====================================================================
//               RemoteElastosKeyStore::RemotePrivateKey
//=====================================================================
RemoteElastosKeyStore::RemotePrivateKey::RemotePrivateKey(
    /* [in] */ Int32 handle,
    /* [in] */ RemoteElastosKeyStore* store)
    : mHandle(handle)
    , mStore(store)
{
    // ==================before translated======================
    // mHandle = handle;
    // mStore = store;
}

Int32 RemoteElastosKeyStore::RemotePrivateKey::GetHandle()
{
    // ==================before translated======================
    // return mHandle;

    return mHandle;
}

AutoPtr<ElastosKeyStore> RemoteElastosKeyStore::RemotePrivateKey::GetKeyStore()
{
    // ==================before translated======================
    // return mStore;

    return mStore;
}

//=====================================================================
//                        RemoteElastosKeyStore
//=====================================================================
const String RemoteElastosKeyStore::TAG("ElastosKeyStoreRemoteImpl");

RemoteElastosKeyStore::RemoteElastosKeyStore(
    /* [in] */ IRemoteElastosKeyStore* manager)
    : mRemoteManager(manager)
{
    // ==================before translated======================
    // mRemoteManager = manager;
}

AutoPtr< ArrayOf<Byte> > RemoteElastosKeyStore::GetRSAKeyModulus(
    /* [in] */ ElastosPrivateKey* key)
{
    // ==================before translated======================
    // RemotePrivateKey remoteKey = (RemotePrivateKey) key;
    // try {
    //     Log.d(TAG, "getRSAKeyModulus");
    //     return mRemoteManager.getRSAKeyModulus(remoteKey.getHandle());
    // } catch (RemoteException e) {
    //     e.printStackTrace();
    //     return null;
    // }

    RemotePrivateKey* remoteKey = (RemotePrivateKey*)key;
    //try {
        Logger::D(TAG, "getRSAKeyModulus");
        AutoPtr< ArrayOf<Byte> > ret;
        mRemoteManager->GetRSAKeyModulus(remoteKey->GetHandle(), (ArrayOf<Byte>**)&ret);
        return ret;
    //} catch (RemoteException e) {
    //    e.printStackTrace();
    //    return null;
    //}
}

AutoPtr< ArrayOf<Byte> > RemoteElastosKeyStore::GetDSAKeyParamQ(
    /* [in] */ ElastosPrivateKey* key)
{
    // ==================before translated======================
    // RemotePrivateKey remoteKey = (RemotePrivateKey) key;
    // try {
    //     Log.d(TAG, "getDSAKeyParamQ");
    //     return mRemoteManager.getDSAKeyParamQ(remoteKey.getHandle());
    // } catch (RemoteException e) {
    //     e.printStackTrace();
    //     return null;
    // }

    RemotePrivateKey* remoteKey = (RemotePrivateKey*)key;
    //try {
        Logger::D(TAG, "getDSAKeyParamQ");
        AutoPtr< ArrayOf<Byte> > ret;
        mRemoteManager->GetDSAKeyParamQ(remoteKey->GetHandle(), (ArrayOf<Byte>**)&ret);
        return ret;
    //} catch (RemoteException e) {
    //    e.printStackTrace();
    //    return null;
    //}
}

AutoPtr< ArrayOf<Byte> > RemoteElastosKeyStore::GetECKeyOrder(
    /* [in] */ ElastosPrivateKey* key)
{
    // ==================before translated======================
    // RemotePrivateKey remoteKey = (RemotePrivateKey) key;
    // try {
    //     Log.d(TAG, "getECKeyOrder");
    //     return mRemoteManager.getECKeyOrder(remoteKey.getHandle());
    // } catch (RemoteException e) {
    //     e.printStackTrace();
    //     return null;
    // }

    RemotePrivateKey* remoteKey = (RemotePrivateKey*)key;
    //try {
        Logger::D(TAG, "getECKeyOrder");
        AutoPtr< ArrayOf<Byte> > ret;
        mRemoteManager->GetECKeyOrder(remoteKey->GetHandle(), (ArrayOf<Byte>**)&ret);
        return ret;
    //} catch (RemoteException e) {
    //    e.printStackTrace();
    //    return null;
    //}
}

AutoPtr< ArrayOf<Byte> > RemoteElastosKeyStore::RawSignDigestWithPrivateKey(
    /* [in] */ ElastosPrivateKey* key,
    /* [in] */ ArrayOf<Byte>* message)
{
    // ==================before translated======================
    // RemotePrivateKey remoteKey = (RemotePrivateKey) key;
    // try {
    //     Log.d(TAG, "rawSignDigestWithPrivateKey");
    //     return mRemoteManager.rawSignDigestWithPrivateKey(remoteKey.getHandle(), message);
    // } catch (RemoteException e) {
    //     e.printStackTrace();
    //     return null;
    // }

    RemotePrivateKey* remoteKey = (RemotePrivateKey*)key;
    //try {
        Logger::D(TAG, "rawSignDigestWithPrivateKey");
        AutoPtr< ArrayOf<Byte> > ret;
        mRemoteManager->RawSignDigestWithPrivateKey(remoteKey->GetHandle(), message, (ArrayOf<Byte>**)&ret);
        return ret;
    //} catch (RemoteException e) {
    //    e.printStackTrace();
    //    return null;
    //}
}

Int32 RemoteElastosKeyStore::GetPrivateKeyType(
    /* [in] */ ElastosPrivateKey* key)
{
    // ==================before translated======================
    // RemotePrivateKey remoteKey = (RemotePrivateKey) key;
    // try {
    //     Log.d(TAG, "getPrivateKeyType");
    //     return mRemoteManager.getPrivateKeyType(remoteKey.getHandle());
    // } catch (RemoteException e) {
    //     e.printStackTrace();
    //     return 0;
    // }

    RemotePrivateKey* remoteKey = (RemotePrivateKey*)key;
    //try {
        Logger::D(TAG, "getPrivateKeyType");
        Int32 result = 0;
        mRemoteManager->GetPrivateKeyType(remoteKey->GetHandle(), &result);
        return result;
    //} catch (RemoteException e) {
    //    e.printStackTrace();
    //    return 0;
    //}
}

AutoPtr< ArrayOf<Byte> > RemoteElastosKeyStore::GetPrivateKeyEncodedBytes(
    /* [in] */ ElastosPrivateKey* key)
{
    // ==================before translated======================
    // // This should not be called as it's only for older versions of Android.
    // assert false;
    // return null;

    // This should not be called as it's only for older versions of Android.
    assert (FALSE);
    return NULL;
}

Int64 RemoteElastosKeyStore::GetOpenSSLHandleForPrivateKey(
    /* [in] */ ElastosPrivateKey* privateKey)
{
    // ==================before translated======================
    // // This should not be called as it's only for older versions of Android.
    // assert false;
    // return 0;

    // This should not be called as it's only for older versions of Android.
    assert (FALSE);
    return 0;
}

AutoPtr<ElastosPrivateKey> RemoteElastosKeyStore::CreateKey(
    /* [in] */ const String& alias)
{
    // ==================before translated======================
    // try {
    //     int handle = mRemoteManager.getPrivateKeyHandle(alias);
    //     return new RemotePrivateKey(handle, this);
    // } catch (RemoteException e) {
    //     e.printStackTrace();
    //     return null;
    // }

    assert (0);
    //try {
        Int32 handle = 0;
        mRemoteManager->GetPrivateKeyHandle(alias, &handle);
        AutoPtr<ElastosPrivateKey> ret = new RemotePrivateKey(handle, this);
        return ret;
    //} catch (RemoteException e) {
    //    e.printStackTrace();
    //    return null;
    //}
}

ECode RemoteElastosKeyStore::ReleaseKey(
    /* [in] */ ElastosPrivateKey* key)
{
    VALIDATE_NOT_NULL(key);
    // ==================before translated======================
    // RemotePrivateKey remoteKey = (RemotePrivateKey) key;
    // try {
    //     Log.d(TAG, "releaseKey");
    //     mRemoteManager.releaseKey(remoteKey.getHandle());
    // } catch (RemoteException e) {
    //     e.printStackTrace();
    // }

    RemotePrivateKey* remoteKey = (RemotePrivateKey*) key;
    //try {
        Logger::D(TAG, "releaseKey");
        mRemoteManager->ReleaseKey(remoteKey->GetHandle());
    //} catch (RemoteException e) {
    //    e.printStackTrace();
    //}
    return NOERROR;
}

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


