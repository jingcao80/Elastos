
#include "CSecureRandom.h"
#include "CSecurity.h"
#include "AutoLock.h"
#include "libcore/io/Memory.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Libcore.h"
#include "org/apache/harmony/security/fortress/CEngine.h"
#include "org/apache/harmony/security/fortress/Services.h"
#include <elastos/utility/logging/Logger.h>

using Libcore::IO::ISizeOf;
using Libcore::IO::Memory;
using Org::Apache::Harmony::Security::Fortress::CEngine;
using Org::Apache::Harmony::Security::Fortress::Services;
using Org::Apache::Harmony::Security::Fortress::IServices;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Security {

static AutoPtr<IEngine> InitEngine()
{
    AutoPtr<CEngine> e;
    CEngine::NewByFriend(String("SecureRandom")/*SERVICE*/, (CEngine**)&e);
    return e;
}

const String CSecureRandom::SERVICE("SecureRandom");
// Used to access common engine functionality
AutoPtr<IEngine> CSecureRandom::ENGINE = InitEngine();
volatile AutoPtr<ISecureRandom> CSecureRandom::internalSecureRandom;

CAR_OBJECT_IMPL(CSecureRandom)
CAR_INTERFACE_IMPL(CSecureRandom, Random, ISecureRandom)
ECode CSecureRandom::constructor()
{
    Random::constructor(0);

    AutoPtr<IProviderService> service;
    Services::GetSecureRandomService((IProviderService**)&service);
    if (service == NULL) {
        mProvider = NULL;
        Logger::D("CSecureRandom", "TODO: Need SHA1PRNG_SecureRandomImpl");
        // mSecureRandomSpi = new SHA1PRNG_SecureRandomImpl();
        mAlgorithm = String("SHA1PRNG");
    }
    else {
        if (FAILED(service->GetProvider((IProvider**)&mProvider))) {
            return E_RUNTIME_EXCEPTION;
        }
        AutoPtr<IInterface> obj;
        if (FAILED(service->NewInstance(NULL, (IInterface**)&obj))) {
            return E_RUNTIME_EXCEPTION;
        }
        mSecureRandomSpi = ISecureRandomSpi::Probe(obj);
        if (FAILED(service->GetAlgorithm(&mAlgorithm))) {
            return E_RUNTIME_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CSecureRandom::constructor(
    /* [in] */ ArrayOf<Byte>* seed)
{
    constructor();
    SetSeed(seed);
    return NOERROR;
}

ECode CSecureRandom::constructor(
    /* [in] */ ISecureRandomSpi* secureRandomSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ String algorithm)
{
    Random::constructor(0);
    mProvider = provider;
    mAlgorithm = algorithm;
    mSecureRandomSpi = secureRandomSpi;
    return NOERROR;
}

ECode CSecureRandom::constructor(
    /* [in] */ ISecureRandomSpi* secureRandomSpi,
    /* [in] */ IProvider* provider)
{
    return constructor(secureRandomSpi, provider, String("unknown"));
}

ECode CSecureRandom::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException*/
{
    VALIDATE_NOT_NULL(result);
    if (algorithm == NULL) {
        // throw new NullPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISpiAndProvider> sap;
    ENGINE->GetInstance(algorithm, NULL, (ISpiAndProvider**)&sap);
    AutoPtr<IInterface> spi;
    sap->GetSpi((IInterface**)&spi);
    AutoPtr<IProvider> p;
    sap->GetProvider((IProvider**)&p);
    return CSecureRandom::New(ISecureRandomSpi::Probe(spi), p, algorithm, result);
}

ECode CSecureRandom::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException, NoSuchProviderException*/
{
    VALIDATE_NOT_NULL(result);
    if (provider == NULL || provider.IsEmpty()) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    AutoPtr<IProvider> p;
    security->GetProvider(provider, (IProvider**)&p);
    if (p == NULL) {
        // throw new NoSuchProviderException(provider);
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(algorithm, p, result);
}

ECode CSecureRandom::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException*/
{
    VALIDATE_NOT_NULL(result);
    if (provider == NULL) {
        // throw new IllegalArgumentException("provider == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (algorithm == NULL) {
        // throw new NullPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IInterface> spi;
    ENGINE->GetInstance(algorithm, provider, NULL, (IInterface**)&spi);
    return CSecureRandom::New(ISecureRandomSpi::Probe(spi), provider, algorithm, result);
}

ECode CSecureRandom::GetProvider(
    /* [out] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    *provider = mProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

/**
 * Returns the name of the algorithm of this {@code SecureRandom}.
 *
 * @return the name of the algorithm of this {@code SecureRandom}.
 */
ECode CSecureRandom::GetAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAlgorithm;
    return NOERROR;
}

/**
 * Seeds this {@code SecureRandom} instance with the specified {@code
 * seed}. <a href="#insecure_seed">Seeding {@code SecureRandom} may be
 * insecure</a>.
 */
ECode CSecureRandom::SetSeed(
    /* [in] */ ArrayOf<Byte>* seed)
{
    AutoLock lock(THIS);
    if (mSecureRandomSpi != NULL) {
        mSecureRandomSpi->EngineSetSeed(seed);
    }
    return NOERROR;
}

/**
 * Seeds this {@code SecureRandom} instance with the specified eight-byte
 * {@code seed}. <a href="#insecure_seed">Seeding {@code SecureRandom} may
 * be insecure</a>.
 */
// @Override
ECode CSecureRandom::SetSeed(
    /* [in] */ Int64 seed)
{
    if (seed == 0) {    // skip call from Random
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > byteSeed = ArrayOf<Byte>::Alloc(ISizeOf::INT64);
    Memory::PokeInt64(byteSeed, 0, seed, Elastos::IO::ByteOrder_BIG_ENDIAN);
    SetSeed(byteSeed);
    return NOERROR;
}

/**
 * Generates and stores random bytes in the given {@code byte[]} for each
 * array element.
 *
 * @param bytes
 *            the {@code byte[]} to be filled with random bytes.
 */
// @Override
ECode CSecureRandom::NextBytes(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    AutoLock lock(THIS);
    if (mSecureRandomSpi != NULL) {
        mSecureRandomSpi->EngineNextBytes(bytes);
    }
    return NOERROR;
}

/**
 * Generates and returns an {@code Int32} containing the specified number of
 * random bits (right justified, with leading zeros).
 *
 * @param numBits
 *            number of bits to be generated. An input value should be in
 *            the range [0, 32].
 * @return an {@code Int32} containing the specified number of random bits.
 */
// @Override
Int32 CSecureRandom::Next(
    /* [in] */ Int32 numBits)
{
    if (numBits < 0) {
        numBits = 0;
    }
    else {
        if (numBits > 32) {
            numBits = 32;
        }
    }
    Int32 bytes = (numBits+7)/8;
    AutoPtr<ArrayOf<Byte> > next = ArrayOf<Byte>::Alloc(bytes);
    Int32 ret = 0;

    NextBytes(next);
    for (Int32 i = 0; i < bytes; i++) {
        ret = ((*next)[i] & 0xFF) | (ret << 8);
    }
    ret = ((UInt32)ret) >> (bytes*8 - numBits);
    return ret;
}

/**
 * Generates and returns the specified number of seed bytes, computed using
 * the seed generation algorithm used by this {@code SecureRandom}.
 *
 * @param numBytes
 *            the number of seed bytes.
 * @return the seed bytes
 */
ECode CSecureRandom::GetSeed(
    /* [in] */ Int32 numBytes,
    /* [out] */ ArrayOf<Byte>** seed)
{
    VALIDATE_NOT_NULL(seed);
    AutoPtr<ISecureRandom> result = internalSecureRandom;
    if (result == NULL) {
        // single-check idiom
        CSecureRandom::New((ISecureRandom**)&result);
        internalSecureRandom = result;
    }
    return result->GenerateSeed(numBytes, seed);
}

/**
 * Generates and returns the specified number of seed bytes, computed using
 * the seed generation algorithm used by this {@code SecureRandom}.
 *
 * @param numBytes
 *            the number of seed bytes.
 * @return the seed bytes.
 */
ECode CSecureRandom::GenerateSeed(
    /* [in] */ Int32 numBytes,
    /* [out] */ ArrayOf<Byte>** seed)
{
    VALIDATE_NOT_NULL(seed);
    *seed = NULL;
    if (mSecureRandomSpi != NULL) {
        return mSecureRandomSpi->EngineGenerateSeed(numBytes, seed);
    }
    return NOERROR;
}

}
}
