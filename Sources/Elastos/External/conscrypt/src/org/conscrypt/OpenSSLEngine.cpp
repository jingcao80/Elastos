#include "org/conscrypt/OpenSSLEngine.h"
#include "org/conscrypt/OpenSSLKey.h"
#include "org/conscrypt/NativeCrypto.h"
#include <elastos/core/AutoLock.h>

namespace Org {
namespace Conscrypt {

Object OpenSSLEngine::mLoadingLock;

Boolean OpenSSLEngine::Init()
{
    NativeCrypto::ENGINE_load_dynamic();
    return TRUE;
}

Boolean OpenSSLEngine::sIsInited = OpenSSLEngine::Init();

CAR_INTERFACE_IMPL(OpenSSLEngine, Object, IOpenSSLEngine)

OpenSSLEngine::OpenSSLEngine()
    : mCtx(0)
{
}

OpenSSLEngine::~OpenSSLEngine()
{
    // try {
    Int32 result;
    NativeCrypto::ENGINE_finish(mCtx, &result);
    NativeCrypto::ENGINE_free(mCtx, &result);
    // } finally {
    //     super.finalize();
    // }
}

ECode OpenSSLEngine::constructor(
    /* [in] */ Int64 engineCtx)
{
    mCtx = engineCtx;

    Int32 ret;
    NativeCrypto::ENGINE_init(engineCtx, &ret);
    if (ret == 0) {
        Int32 result;
        NativeCrypto::ENGINE_free(engineCtx, &result);
        // throw new IllegalArgumentException("Could not initialize engine");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode OpenSSLEngine::GetInstance(
    /* [in] */ const String& engine,
    /* [out] */ IOpenSSLEngine** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (engine.IsNull()) {
        // throw new NullPointerException("engine == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    Int64 engineCtx;
    {
        AutoLock lock(mLoadingLock);
        NativeCrypto::ENGINE_by_id(engine, &engineCtx);
        if (engineCtx == 0) {
            // throw new IllegalArgumentException("Unknown ENGINE id: " + engine);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        Int32 ret;
        NativeCrypto::ENGINE_add(engineCtx, &ret);
    }

    AutoPtr<OpenSSLEngine> ret = new OpenSSLEngine();
    ret->constructor(engineCtx);
    *result = IOpenSSLEngine::Probe(ret);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLEngine::GetPrivateKeyById(
    /* [in] */ const String& id,
    /* [out] */ IPrivateKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (id.IsNull()) {
        // throw new NullPointerException("id == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    Int64 keyRef;
    NativeCrypto::ENGINE_load_private_key(mCtx, id, &keyRef);
    if (keyRef == 0) {
        return NOERROR;
    }

    assert(0 && "TODO");
    AutoPtr<OpenSSLKey> pkey;// = new OpenSSLKey(keyRef, this, id);
    // try {
    return pkey->GetPrivateKey(result);
    // } catch (NoSuchAlgorithmException e) {
    //     throw new InvalidKeyException(e);
    // }
}

ECode OpenSSLEngine::GetSecretKeyById(
    /* [in] */ const String& id,
    /* [in] */ const String& algorithm,
    /* [out] */ ISecretKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (id.IsNull()) {
        // throw new NullPointerException("id == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    Int64 keyRef;
    NativeCrypto::ENGINE_load_private_key(mCtx, id, &keyRef);
    if (keyRef == 0) {
        return NOERROR;
    }

    assert(0 && "TODO");
    AutoPtr<OpenSSLKey> pkey;// = new OpenSSLKey(keyRef, this, id);
    // try {
    return pkey->GetSecretKey(algorithm, result);
    // } catch (NoSuchAlgorithmException e) {
    //     throw new InvalidKeyException(e);
    // }
}

ECode OpenSSLEngine::GetEngineContext(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCtx;
    return NOERROR;
}

ECode OpenSSLEngine::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (Int32)mCtx;
    return NOERROR;
}

ECode OpenSSLEngine::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (TO_IINTERFACE(this) == o) {
        *result = TRUE;
        return NOERROR;
    }

    if (IOpenSSLEngine::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<OpenSSLEngine> other = (OpenSSLEngine*)IOpenSSLEngine::Probe(o);

    Int64 ctx;
    other->GetEngineContext(&ctx);
    if (ctx == mCtx) {
        *result = TRUE;
        return NOERROR;
    }

    String id;
    NativeCrypto::ENGINE_get_id(mCtx, &id);
    if (id.IsNull()) {
        *result = FALSE;
        return NOERROR;
    }

    String str;
    NativeCrypto::ENGINE_get_id(ctx, &str);
    *result = id.Equals(str);
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
