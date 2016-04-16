#include "elastos/droid/webkit/webview/chromium/native/net/ElastosKeyStore.h"
#include "elastos/droid/webkit/webview/chromium/native/net/api/ElastosKeyStore_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/net/ElastosPrivateKey.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

AutoPtr<ArrayOf<Byte> > ElastosKeyStore::GetRSAKeyModulus(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* key)
{
    ElastosKeyStore* mObj = (ElastosKeyStore*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ElastosKeyStore", "ElastosKeyStore::GetRSAKeyModulus, mObj is NULL");
        return NULL;
    }
    ElastosPrivateKey* apk = (ElastosPrivateKey*)(IObject::Probe(key));
    return mObj->GetRSAKeyModulus(apk);
}

AutoPtr<ArrayOf<Byte> > ElastosKeyStore::GetDSAKeyParamQ(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* key)
{
    ElastosKeyStore* mObj = (ElastosKeyStore*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ElastosKeyStore", "ElastosKeyStore::GetDSAKeyParamQ, mObj is NULL");
        return NULL;
    }
    ElastosPrivateKey* apk = (ElastosPrivateKey*)(IObject::Probe(key));
    return mObj->GetDSAKeyParamQ(apk);
}

AutoPtr<ArrayOf<Byte> > ElastosKeyStore::GetECKeyOrder(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* key)
{
    ElastosKeyStore* mObj = (ElastosKeyStore*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ElastosKeyStore", "ElastosKeyStore::GetECKeyOrder, mObj is NULL");
        return NULL;
    }
    ElastosPrivateKey* apk = (ElastosPrivateKey*)(IObject::Probe(key));
    return mObj->GetECKeyOrder(apk);
}

AutoPtr<ArrayOf<Byte> > ElastosKeyStore::GetPrivateKeyEncodedBytes(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* key)
{
    ElastosKeyStore* mObj = (ElastosKeyStore*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ElastosKeyStore", "ElastosKeyStore::GetPrivateKeyEncodedBytes, mObj is NULL");
        return NULL;
    }
    ElastosPrivateKey* apk = (ElastosPrivateKey*)(IObject::Probe(key));
    return mObj->GetPrivateKeyEncodedBytes(apk);
}

AutoPtr<ArrayOf<Byte> > ElastosKeyStore::RawSignDigestWithPrivateKey(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* key,
    /* [in] */ ArrayOf<Byte>* message)
{
    ElastosKeyStore* mObj = (ElastosKeyStore*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ElastosKeyStore", "ElastosKeyStore::RawSignDigestWithPrivateKey, mObj is NULL");
        return NULL;
    }
    ElastosPrivateKey* apk = (ElastosPrivateKey*)(IObject::Probe(key));
    return mObj->RawSignDigestWithPrivateKey(apk, message);
}

Int32 ElastosKeyStore::GetPrivateKeyType(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* key)
{
    ElastosKeyStore* mObj = (ElastosKeyStore*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ElastosKeyStore", "ElastosKeyStore::GetPrivateKeyType, mObj is NULL");
        return 0;
    }
    ElastosPrivateKey* apk = (ElastosPrivateKey*)(IObject::Probe(key));
    return mObj->GetPrivateKeyType(apk);
}

Int64 ElastosKeyStore::GetOpenSSLHandleForPrivateKey(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* key)
{
    ElastosKeyStore* mObj = (ElastosKeyStore*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ElastosKeyStore", "ElastosKeyStore::GetOpenSSLHandleForPrivateKey, mObj is NULL");
        return 0;
    }
    ElastosPrivateKey* apk = (ElastosPrivateKey*)(IObject::Probe(key));
    return mObj->GetOpenSSLHandleForPrivateKey(apk);
}

void ElastosKeyStore::ReleaseKey(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* key)
{
    ElastosKeyStore* mObj = (ElastosKeyStore*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ElastosKeyStore", "ElastosKeyStore::ReleaseKey, mObj is NULL");
        return;
    }
    ElastosPrivateKey* apk = (ElastosPrivateKey*)(IObject::Probe(key));
    mObj->ReleaseKey(apk);
}

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


