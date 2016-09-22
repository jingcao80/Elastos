
#include "MessageDigest.h"
#include "core/Object.h"
#include "security/CSecurity.h"
#include "utility/logging/Logger.h"
#include "org/apache/harmony/security/fortress/CEngine.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Security::CSecurity;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Harmony::Security::Fortress::CEngine;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;

namespace Elastos {
namespace Security {

class MessageDigestImpl
    : public MessageDigest
    , public ICloneable
{
    friend class MessageDigest;

public:
    CAR_INTERFACE_DECL()

    CARAPI Clone(
        /* [out] */ IInterface** object);

protected:
    // engineReset() implementation
    //@Override
    CARAPI EngineReset();

    // engineDigest() implementation
    //@Override
    CARAPI EngineDigest(
        /* [out, callee] */ ArrayOf<Byte>** ed);

    // engineGetDigestLength() implementation
    //@Override
    CARAPI EngineGetDigestLength(
        /* [out] */ Int32* len);

    // engineUpdate() implementation
    //@Override
    CARAPI EngineUpdate(
        /* [in] */ Byte arg0);

    // engineUpdate() implementation
    //@Override
    CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte>* arg0,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

private:
    // MessageDigestImpl ctor
    MessageDigestImpl(
        /* [in] */ MessageDigestSpi* messageDigestSpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& algorithm);

private:
    // MessageDigestSpi implementation
    AutoPtr<MessageDigestSpi> mSpiImpl;
};

CAR_INTERFACE_IMPL(MessageDigestImpl, MessageDigest, ICloneable)
MessageDigestImpl::MessageDigestImpl(
    /* [in] */ MessageDigestSpi* messageDigestSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
    : MessageDigest(algorithm)
    , mSpiImpl(messageDigestSpi)
{
    mProvider = provider;
}

ECode MessageDigestImpl::EngineReset()
{
    return mSpiImpl->EngineReset();
}

ECode MessageDigestImpl::EngineDigest(
    /* [out, callee] */ ArrayOf<Byte>** ed)
{
    return mSpiImpl->EngineDigest(ed);
}

ECode MessageDigestImpl::EngineGetDigestLength(
    /* [out] */ Int32* len)
{
    return mSpiImpl->EngineGetDigestLength(len);
}

ECode MessageDigestImpl::EngineUpdate(
    /* [in] */ Byte arg0)
{
    return mSpiImpl->EngineUpdate(arg0);
}

ECode MessageDigestImpl::EngineUpdate(
    /* [in] */ ArrayOf<Byte>* arg0,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2)
{
    return mSpiImpl->EngineUpdate(arg0, arg1, arg2);
}

ECode MessageDigestImpl::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IInterface> cloneObj;
    ICloneable::Probe(mSpiImpl)->Clone((IInterface**)&cloneObj);
    MessageDigestSpi* spi = (MessageDigestSpi*)IObject::Probe(cloneObj);
    AutoPtr<IProvider> provider;
    GetProvider((IProvider**)&provider);
    String algorithm;
    GetAlgorithm(&algorithm);
    *object = (IMessageDigest*)new MessageDigestImpl(spi, provider, algorithm);
    REFCOUNT_ADD(*object)
    return NOERROR;
}


//---------------------------------------------------------------------
//    MessageDigest
//---------------------------------------------------------------------
const AutoPtr<IEngine> MessageDigest::ENGINE = Init_ENGINE();

CAR_INTERFACE_IMPL(MessageDigest, MessageDigestSpi, IMessageDigest)
MessageDigest::MessageDigest(
    /* [in] */ const String& algorithm)
    : mAlgorithm(algorithm)
{}

ECode MessageDigest::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IMessageDigest** instance)
{
    VALIDATE_NOT_NULL(instance)
    if (algorithm.IsNull()) {
        Logger::E("MessageDigest", "algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISpiAndProvider> sap;
    ENGINE->GetInstance(algorithm, NULL, (ISpiAndProvider**)&sap);
    AutoPtr<IInterface> spi;
    sap->GetSpi((IInterface**)&spi);
    AutoPtr<IProvider> provider;
    sap->GetProvider((IProvider**)&provider);
    if (IMessageDigest::Probe(spi) != NULL) {
        AutoPtr<IMessageDigest> result = IMessageDigest::Probe(spi);
        result->SetAlgorithm(algorithm);
        result->SetProvider(provider);
        *instance = result;
        REFCOUNT_ADD(*instance);
        return NOERROR;
    }
    *instance = new MessageDigestImpl((MessageDigestSpi*)(IObject*)spi.Get(), provider, algorithm);
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode MessageDigest::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IMessageDigest** instance)
{
    VALIDATE_NOT_NULL(instance)
    if (provider.IsNullOrEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ISecurity> sec;
    CSecurity::AcquireSingleton((ISecurity**)&sec);
    AutoPtr<IProvider> p;
    sec->GetProvider(provider, (IProvider**)&p);
    if (p == NULL) {
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(algorithm, p, instance);
}

ECode MessageDigest::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ IMessageDigest** instance)
{
    VALIDATE_NOT_NULL(instance);
    if (provider == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (algorithm.IsNull()) {
        Logger::E("MessageDigest", "algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IInterface> spi;
    ENGINE->GetInstance(algorithm, provider, NULL, (IInterface**)&spi);
    if (IMessageDigest::Probe(spi) != NULL) {
        AutoPtr<IMessageDigest> result = IMessageDigest::Probe(spi);
        result->SetAlgorithm(algorithm);
        result->SetProvider(provider);
        *instance = result;
        REFCOUNT_ADD(*instance);
        return NOERROR;
    }
    *instance = new MessageDigestImpl((MessageDigestSpi*)(IObject*)spi.Get(), provider, algorithm);
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode MessageDigest::Reset()
{
    return EngineReset();
}

ECode MessageDigest::Update(
    /* [in] */ Byte arg0)
{
    return EngineUpdate(arg0);
}

ECode MessageDigest::Update(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len)
{
    if (input == NULL ||
        // offset < 0 || len < 0 ||
        // checks for negative values are commented out intentionally
        // see HARMONY-1120 for details
        offset + len > input->GetLength()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return EngineUpdate(input, offset, len);
}

ECode MessageDigest::Update(
    /* [in] */ ArrayOf<Byte>* input)
{
    if (input == NULL) {
        Logger::E("MessageDigest", "input == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    return EngineUpdate(input, 0, input->GetLength());
}

ECode MessageDigest::Digest(
    /* [out, callee] */ ArrayOf<Byte>** hashValue)
{
    return EngineDigest(hashValue);
}

ECode MessageDigest::Digest(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    if (buf == NULL ||
        // offset < 0 || len < 0 ||
        // checks for negative values are commented out intentionally
        // see HARMONY-1148 for details
        offset + len > buf->GetLength()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return EngineDigest(buf, offset, len, number);
}

ECode MessageDigest::Digest(
    /* [in] */ ArrayOf<Byte>* input,
    /* [out, callee] */ ArrayOf<Byte>** hashValue)
{
    VALIDATE_NOT_NULL(hashValue)
    Update(input);
    return Digest(hashValue);
}

ECode MessageDigest::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "MESSAGE DIGEST ";
    *str += mAlgorithm;
    return NOERROR;
}

ECode MessageDigest::IsEqual(
    /* [in] */ ArrayOf<Byte>* digesta,
    /* [in] */ ArrayOf<Byte>* digestb,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (digesta->GetLength() != digestb->GetLength()) {
        *result = FALSE;
        return NOERROR;
    }
    // Perform a constant time comparison to avoid timing attacks.
    Int32 v = 0;
    for (Int32 i = 0; i < digesta->GetLength(); i++) {
        v |= ((*digesta)[i] ^ (*digestb)[i]);
    }
    *result = v == 0;
    return NOERROR;
}

ECode MessageDigest::GetAlgorithm(
    /* [out] */ String* algorithm)
{
    VALIDATE_NOT_NULL(algorithm)
    *algorithm = mAlgorithm;
    return NOERROR;
}

ECode MessageDigest::GetProvider(
    /* [out, callee] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider)
    *provider = mProvider;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode MessageDigest::GetDigestLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    *length = 0;
    Int32 l;
    EngineGetDigestLength(&l);
    if (l != 0) {
        *length = l;
        return NOERROR;
    }
    ICloneable* cloneObj = ICloneable::Probe(this);
    if (cloneObj == NULL) {
        return NOERROR;
    }

    AutoPtr<IInterface> clone;
    FAIL_RETURN(cloneObj->Clone((IInterface**)&clone))
    AutoPtr<ArrayOf<Byte> > hashValue;
    IMessageDigest::Probe(clone)->Digest((ArrayOf<Byte>**)&hashValue);
    *length = hashValue->GetLength();
    return NOERROR;
}

ECode MessageDigest::Update(
    /* [in] */ IByteBuffer* input)
{
    return EngineUpdate(input);
}

ECode MessageDigest::SetProvider(
    /* [in] */ IProvider* provider)
{
    mProvider = provider;
    return NOERROR;
}

ECode MessageDigest::SetAlgorithm(
    /* [in] */ const String& algorithm)
{
    mAlgorithm = algorithm;
    return NOERROR;
}

AutoPtr<IEngine> MessageDigest::Init_ENGINE()
{
    AutoPtr<CEngine> engine;
    CEngine::NewByFriend(String("MessageDigest"), (CEngine**)&engine);
    return engine;
}

} // namespace Security
} // namespace Elastos