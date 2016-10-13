
#include "CSHA1PRNG_SecureRandomImpl.h"
#include "SHA1Impl.h"
#include "AutoLock.h"
#include "CFile.h"
#include "CFileInputStream.h"
#include "EmptyArray.h"
#include "BlockGuard.h"

using Elastos::Core::AutoLock;
using Elastos::Core::BlockGuard;
using Elastos::Core::IBlockGuardPolicy;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataOutput;
using Elastos::IO::IFile;
using Libcore::Utility::EmptyArray;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

static AutoPtr<IFileInputStream> InitDevURandom()
{
    AutoPtr<IFile> f;
    ASSERT_SUCCEEDED(CFile::New(String("/dev/urandom"), (IFile**)&f));
    AutoPtr<IFileInputStream> fs;
    ASSERT_SUCCEEDED(CFileInputStream::New(f, (IFileInputStream**)&fs));
    return fs;
}

AutoPtr<IFileInputStream> CSHA1PRNG_SecureRandomImpl::sDevURandom = InitDevURandom();
Int32 CSHA1PRNG_SecureRandomImpl::END_FLAGS[] = { 0x80000000, 0x800000, 0x8000, 0x80 };
const Int32 CSHA1PRNG_SecureRandomImpl::RIGHT1[] = { 0, 40, 48, 56 };
const Int32 CSHA1PRNG_SecureRandomImpl::RIGHT2[] = { 0, 8, 16, 24 };
const Int32 CSHA1PRNG_SecureRandomImpl::LEFT[] = { 0, 24, 16, 8 };
const Int32 CSHA1PRNG_SecureRandomImpl::MASK[] = { 0xFFFFFFFF, 0x00FFFFFF, 0x0000FFFF, 0x000000FF };
const Int32 CSHA1PRNG_SecureRandomImpl::HASHBYTES_TO_USE = 20;
const Int32 CSHA1PRNG_SecureRandomImpl::FRAME_LENGTH = 16;
const Int32 CSHA1PRNG_SecureRandomImpl::COUNTER_BASE = 0;
const Int32 CSHA1PRNG_SecureRandomImpl::HASHCOPY_OFFSET = 0;
const Int32 CSHA1PRNG_SecureRandomImpl::EXTRAFRAME_OFFSET = 5;
const Int32 CSHA1PRNG_SecureRandomImpl::FRAME_OFFSET = 21;
const Int32 CSHA1PRNG_SecureRandomImpl::MAX_BYTES = 48;
const Int32 CSHA1PRNG_SecureRandomImpl::UNDEFINED = 0;
const Int32 CSHA1PRNG_SecureRandomImpl::SET_SEED = 1;
const Int32 CSHA1PRNG_SecureRandomImpl::NEXT_BYTES = 2;
AutoPtr<CSHA1PRNG_SecureRandomImpl> CSHA1PRNG_SecureRandomImpl::sMyRandom;


CAR_OBJECT_IMPL(CSHA1PRNG_SecureRandomImpl)
CAR_INTERFACE_IMPL(CSHA1PRNG_SecureRandomImpl, SecureRandomSpi, ISHA1PRNG_SecureRandomImpl)
CSHA1PRNG_SecureRandomImpl::CSHA1PRNG_SecureRandomImpl()
    : mSeedLength(0)
    , mNextBIndex(0)
    , mCounter(0)
    , mState(0)
{}

ECode CSHA1PRNG_SecureRandomImpl::constructor()
{
    mSeed = ArrayOf<Int32>::Alloc(ISHA1Constants::HASH_OFFSET + EXTRAFRAME_OFFSET);
    (*mSeed)[ISHA1Constants::HASH_OFFSET] = ISHA1Constants::H0;
    (*mSeed)[ISHA1Constants::HASH_OFFSET + 1] = ISHA1Constants::H1;
    (*mSeed)[ISHA1Constants::HASH_OFFSET + 2] = ISHA1Constants::H2;
    (*mSeed)[ISHA1Constants::HASH_OFFSET + 3] = ISHA1Constants::H3;
    (*mSeed)[ISHA1Constants::HASH_OFFSET + 4] = ISHA1Constants::H4;

    mSeedLength = 0;
    mCopies = ArrayOf<Int32>::Alloc(2 * FRAME_LENGTH + EXTRAFRAME_OFFSET);
    mNextBytes = ArrayOf<Byte>::Alloc(ISHA1Constants::DIGEST_LENGTH);
    mNextBIndex = HASHBYTES_TO_USE;
    mCounter = COUNTER_BASE;
    mState = UNDEFINED;
    return NOERROR;
}

void CSHA1PRNG_SecureRandomImpl::UpdateSeed(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    // on call:   "seed" contains current bytes and current hash;
    // on return: "seed" contains new current bytes and possibly new current hash
    //            if after adding, seed bytes overfill its buffer
    SHA1Impl::UpdateHash(mSeed, bytes, 0, bytes->GetLength() - 1);

    mSeedLength += bytes->GetLength();
}

ECode CSHA1PRNG_SecureRandomImpl::EngineSetSeed(
    /* [in] */ ArrayOf<Byte>* seed)
{
    AutoLock lock(THIS);
    if (seed == NULL) {
        // throw new NullPointerException("seed == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (mState == NEXT_BYTES) { // first setSeed after NextBytes; restoring hash
        // System.arraycopy(mCopies, HASHCOPY_OFFSET, mSeed, ISHA1Constants::HASH_OFFSET, EXTRAFRAME_OFFSET);
        mSeed->Copy(ISHA1Constants::HASH_OFFSET, mCopies, HASHCOPY_OFFSET, EXTRAFRAME_OFFSET);
    }
    mState = SET_SEED;

    if (seed->GetLength() != 0) {
        UpdateSeed(seed);
    }
    return NOERROR;
}

ECode CSHA1PRNG_SecureRandomImpl::EngineGenerateSeed(
    /* [in] */ Int32 numBytes,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoLock lock(THIS);
    AutoPtr<ArrayOf<Byte> > myBytes; // Byte[] for bytes returned by "nextBytes()"

    if (numBytes < 0) {
        // throw new NegativeArraySizeException(Integer.toString(numBytes));
        return E_NEGATIVE_ARRAY_SIZE_EXCEPTION;
    }
    if (numBytes == 0) {
        *result = EmptyArray::BYTE;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    if (sMyRandom == NULL) {
        CSHA1PRNG_SecureRandomImpl::NewByFriend((CSHA1PRNG_SecureRandomImpl**)&sMyRandom);
        AutoPtr<ArrayOf<Byte> > bs;
        FAIL_RETURN(GetRandomBytes(ISHA1Constants::DIGEST_LENGTH, (ArrayOf<Byte>**)&bs));
        FAIL_RETURN(sMyRandom->EngineSetSeed(bs));
    }

    myBytes = ArrayOf<Byte>::Alloc(numBytes);
    FAIL_RETURN(sMyRandom->EngineNextBytes(myBytes));

    *result = myBytes;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSHA1PRNG_SecureRandomImpl::EngineNextBytes(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    AutoLock lock(THIS);
    Int32 i, n;

    Int64 bits; // number of bits required by Secure Hash Standard
    Int32 nextByteToReturn; // index of ready bytes in "bytes" array
    Int32 lastWord; // index of last word in frame containing bytes
    const Int32 extrabytes = 7;// # of bytes to add in order to computer # of 8 Byte words

    if (bytes == NULL) {
        // throw new NullPointerException("bytes == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    lastWord = (*mSeed)[ISHA1Constants::BYTES_OFFSET] == 0 ? 0
            : ((*mSeed)[ISHA1Constants::BYTES_OFFSET] + extrabytes) >> (3 - 1);

    if (mState == UNDEFINED) {
        // no seed supplied by user, hence it is generated thus randomizing internal state
        AutoPtr<ArrayOf<Byte> > bs;
        FAIL_RETURN(GetRandomBytes(ISHA1Constants::DIGEST_LENGTH, (ArrayOf<Byte>**)&bs));
        UpdateSeed(bs);
        mNextBIndex = HASHBYTES_TO_USE;

        // UpdateSeed(...) updates where the last word of the seed is, so we
        // have to read it again.
        lastWord = (*mSeed)[ISHA1Constants::BYTES_OFFSET] == 0 ? 0
                : ((*mSeed)[ISHA1Constants::BYTES_OFFSET] + extrabytes) >> (3 - 1);

    }
    else if (mState == SET_SEED) {
        // System.arraycopy(mSeed, ISHA1Constants::HASH_OFFSET, mCopies, HASHCOPY_OFFSET, EXTRAFRAME_OFFSET);
        mCopies->Copy(HASHCOPY_OFFSET, mSeed, ISHA1Constants::HASH_OFFSET, EXTRAFRAME_OFFSET);

        // possible cases for 64-Byte frame:
        //
        // seed bytes < 48      - remaining bytes are enough for all, 8 counter bytes,
        //                        0x80, and 8 seedLength bytes; no extra frame required
        // 48 < seed bytes < 56 - remaining 9 bytes are for 0x80 and 8 counter bytes
        //                        extra frame contains only seedLength value at the end
        // seed bytes > 55      - extra frame contains both counter's bytes
        //                        at the beginning and seedLength value at the end;
        //                        note, that beginning extra bytes are not more than 8,
        //                        that is, only 2 extra words may be used

        // no need to set to "0" 3 words after "lastWord" and
        // more than two words behind frame
        for (i = lastWord + 3; i < FRAME_LENGTH + 2; i++) {
            (*mSeed)[i] = 0;
        }

        bits = (mSeedLength << 3) + 64; // transforming # of bytes into # of bits

        // putting # of bits into two last words (14,15) of 16 word frame in
        // seed or copies array depending on total length after padding
        if ((*mSeed)[ISHA1Constants::BYTES_OFFSET] < MAX_BYTES) {
            (*mSeed)[14] = (Int32) (((UInt64)bits) >> 32);
            (*mSeed)[15] = (Int32) (bits & 0xFFFFFFFF);
        }
        else {
            (*mCopies)[EXTRAFRAME_OFFSET + 14] = (Int32) (((UInt64)bits) >> 32);
            (*mCopies)[EXTRAFRAME_OFFSET + 15] = (Int32) (bits & 0xFFFFFFFF);
        }

        mNextBIndex = HASHBYTES_TO_USE; // skipping remaining random bits
    }
    mState = NEXT_BYTES;

    if (bytes->GetLength() == 0) {
        return NOERROR;
    }

    nextByteToReturn = 0;

    // possibly not all of HASHBYTES_TO_USE bytes were used previous time
    n = (HASHBYTES_TO_USE - mNextBIndex) < (bytes->GetLength() - nextByteToReturn) ? HASHBYTES_TO_USE
            - mNextBIndex
            : bytes->GetLength() - nextByteToReturn;
    if (n > 0) {
        // System.arraycopy(mNextBytes, mNextBIndex, bytes, nextByteToReturn, n);
        bytes->Copy(nextByteToReturn, mNextBytes, mNextBIndex, n);
        mNextBIndex += n;
        nextByteToReturn += n;
    }

    if (nextByteToReturn >= bytes->GetLength()) {
        return NOERROR; // return because "bytes[]" are filled in
    }

    n = (*mSeed)[ISHA1Constants::BYTES_OFFSET] & 0x03;
    for (;;) {
        if (n == 0) {
            (*mSeed)[lastWord] = (Int32) (((UInt64)mCounter) >> 32);
            (*mSeed)[lastWord + 1] = (Int32) (mCounter & 0xFFFFFFFF);
            (*mSeed)[lastWord + 2] = END_FLAGS[0];

        }
        else {
            (*mSeed)[lastWord] |= (Int32) ((((UInt64)mCounter) >> RIGHT1[n]) & MASK[n]);
            (*mSeed)[lastWord + 1] = (Int32) ((((UInt64)mCounter) >> RIGHT2[n]) & 0xFFFFFFFF);
            (*mSeed)[lastWord + 2] = (Int32) ((mCounter << LEFT[n]) | END_FLAGS[n]);
        }
        if ((*mSeed)[ISHA1Constants::BYTES_OFFSET] > MAX_BYTES) {
            (*mCopies)[EXTRAFRAME_OFFSET] = (*mSeed)[FRAME_LENGTH];
            (*mCopies)[EXTRAFRAME_OFFSET + 1] = (*mSeed)[FRAME_LENGTH + 1];
        }

        SHA1Impl::ComputeHash(mSeed);

        if ((*mSeed)[ISHA1Constants::BYTES_OFFSET] > MAX_BYTES) {
            // System.arraycopy(mSeed, 0, mCopies, FRAME_OFFSET, FRAME_LENGTH);
            mCopies->Copy(FRAME_OFFSET, mSeed, 0, FRAME_LENGTH);
            // System.arraycopy(mCopies, EXTRAFRAME_OFFSET, mSeed, 0, FRAME_LENGTH);
            mSeed->Copy(0, mCopies, EXTRAFRAME_OFFSET, FRAME_LENGTH);

            SHA1Impl::ComputeHash(mSeed);
            // System.arraycopy(mCopies, FRAME_OFFSET, mSeed, 0, FRAME_LENGTH);
            mSeed->Copy(0, mCopies, FRAME_OFFSET, FRAME_LENGTH);
        }
        mCounter++;

        Int32 j = 0;
        for (i = 0; i < EXTRAFRAME_OFFSET; i++) {
            Int32 k = (*mSeed)[ISHA1Constants::HASH_OFFSET + i];
            (*mNextBytes)[j] = (Byte) (((UInt32)k) >> 24); // getting first  Byte from left
            (*mNextBytes)[j + 1] = (Byte) (((UInt32)k) >> 16); // getting second Byte from left
            (*mNextBytes)[j + 2] = (Byte) (((UInt32)k) >> 8); // getting third  Byte from left
            (*mNextBytes)[j + 3] = (Byte) (k); // getting fourth Byte from left
            j += 4;
        }

        mNextBIndex = 0;
        j = HASHBYTES_TO_USE < (bytes->GetLength() - nextByteToReturn) ? HASHBYTES_TO_USE
                : bytes->GetLength() - nextByteToReturn;

        if (j > 0) {
            // System.arraycopy(mNextBytes, 0, bytes, nextByteToReturn, j);
            bytes->Copy(nextByteToReturn, mNextBytes, 0, j);
            nextByteToReturn += j;
            mNextBIndex += j;
        }

        if (nextByteToReturn >= bytes->GetLength()) {
            break;
        }
    }
    return NOERROR;
}

ECode CSHA1PRNG_SecureRandomImpl::WriteObject(
    /* [in] */ IObjectOutputStream* oos) /*throws IOException*/
{
    AutoPtr<ArrayOf<Int32> > intData;

    const Int32 only_hash = EXTRAFRAME_OFFSET;
    const Int32 hashes_and_frame = EXTRAFRAME_OFFSET * 2 + FRAME_LENGTH;
    const Int32 hashes_and_frame_extra = EXTRAFRAME_OFFSET * 2 + FRAME_LENGTH * 2;

    FAIL_RETURN(IDataOutput::Probe(oos)->WriteInt64(mSeedLength));
    FAIL_RETURN(IDataOutput::Probe(oos)->WriteInt64(mCounter));
    FAIL_RETURN(IDataOutput::Probe(oos)->WriteInt32(mState));
    FAIL_RETURN(IDataOutput::Probe(oos)->WriteInt32((*mSeed)[ISHA1Constants::BYTES_OFFSET]));

    Int32 nRemaining = ((*mSeed)[ISHA1Constants::BYTES_OFFSET] + 3) >> 2; // converting bytes in words
    // result may be 0
    if (mState != NEXT_BYTES) {
        // either the state is UNDEFINED or previous method was "setSeed(..)"
        // so in "seed[]" to serialize are remaining bytes (seed[0-nRemaining]) and
        // current hash (seed[82-86])

        intData = ArrayOf<Int32>::Alloc(only_hash + nRemaining);

        // System.arraycopy(mSeed, 0, intData, 0, nRemaining);
        intData->Copy(0, mSeed, 0, nRemaining);
        // System.arraycopy(mSeed, ISHA1Constants::HASH_OFFSET, intData, nRemaining, EXTRAFRAME_OFFSET);
        intData->Copy(nRemaining, mSeed, ISHA1Constants::HASH_OFFSET, EXTRAFRAME_OFFSET);
    }
    else {
        // previous method was "nextBytes(..)"
        // so, data to serialize are all the above (two first are in "copies" array)
        // and current words in both frame and extra frame (as if)

        Int32 offset = 0;
        if ((*mSeed)[ISHA1Constants::BYTES_OFFSET] < MAX_BYTES) { // no extra frame
            intData = ArrayOf<Int32>::Alloc(hashes_and_frame + nRemaining);
        }
        else { // extra frame is used
            intData = ArrayOf<Int32>::Alloc(hashes_and_frame_extra + nRemaining);

            (*intData)[offset] = (*mSeed)[FRAME_LENGTH];
            (*intData)[offset + 1] = (*mSeed)[FRAME_LENGTH + 1];
            (*intData)[offset + 2] = (*mSeed)[FRAME_LENGTH + 14];
            (*intData)[offset + 3] = (*mSeed)[FRAME_LENGTH + 15];
            offset += 4;
        }

        // System.arraycopy(mSeed, 0, intData, offset, FRAME_LENGTH);
        intData->Copy(offset, mSeed, 0, FRAME_LENGTH);
        offset += FRAME_LENGTH;

        // System.arraycopy(mCopies, FRAME_LENGTH + EXTRAFRAME_OFFSET, intData, offset, nRemaining);
        intData->Copy(offset, mCopies, FRAME_LENGTH + EXTRAFRAME_OFFSET, nRemaining);
        offset += nRemaining;

        // System.arraycopy(mCopies, 0, intData, offset, EXTRAFRAME_OFFSET);
        intData->Copy(offset, mCopies, 0, EXTRAFRAME_OFFSET);
        offset += EXTRAFRAME_OFFSET;

        // System.arraycopy(mSeed, ISHA1Constants::HASH_OFFSET, intData, offset, EXTRAFRAME_OFFSET);
        intData->Copy(offset, mSeed, ISHA1Constants::HASH_OFFSET, EXTRAFRAME_OFFSET);
    }
    for (Int32 i = 0; i < intData->GetLength(); i++) {
        FAIL_RETURN(IDataOutput::Probe(oos)->WriteInt32((*intData)[i]));
    }

    FAIL_RETURN(IDataOutput::Probe(oos)->WriteInt32(mNextBIndex));
    return IDataOutput::Probe(oos)->Write(mNextBytes, mNextBIndex, HASHBYTES_TO_USE - mNextBIndex);
}

ECode CSHA1PRNG_SecureRandomImpl::ReadObject(
    /* [in] */ IObjectInputStream* ois) /*throws IOException, ClassNotFoundException*/
{
    mSeed = ArrayOf<Int32>::Alloc(ISHA1Constants::HASH_OFFSET + EXTRAFRAME_OFFSET);
    mCopies = ArrayOf<Int32>::Alloc(2 * FRAME_LENGTH + EXTRAFRAME_OFFSET);
    mNextBytes = ArrayOf<Byte>::Alloc(ISHA1Constants::DIGEST_LENGTH);

    FAIL_RETURN(IDataInput::Probe(ois)->ReadInt64(&mSeedLength));
    FAIL_RETURN(IDataInput::Probe(ois)->ReadInt64(&mCounter));
    FAIL_RETURN(IDataInput::Probe(ois)->ReadInt32(&mState));
    Int32 v = 0;
    FAIL_RETURN(IDataInput::Probe(ois)->ReadInt32(&v));
    (*mSeed)[ISHA1Constants::BYTES_OFFSET] = v;

    Int32 nRemaining = ((*mSeed)[ISHA1Constants::BYTES_OFFSET] + 3) >> 2; // converting bytes in words

    if (mState != NEXT_BYTES) {
        for (Int32 i = 0; i < nRemaining; i++) {
            FAIL_RETURN(IDataInput::Probe(ois)->ReadInt32(&((*mSeed)[i])));
        }
        for (Int32 i = 0; i < EXTRAFRAME_OFFSET; i++) {
            FAIL_RETURN(IDataInput::Probe(ois)->ReadInt32(&((*mSeed)[ISHA1Constants::HASH_OFFSET + i])));
        }
    }
    else {
        if ((*mSeed)[ISHA1Constants::BYTES_OFFSET] >= MAX_BYTES) {
            // reading next bytes in seed extra frame
            FAIL_RETURN(IDataInput::Probe(ois)->ReadInt32(&((*mSeed)[FRAME_LENGTH])));
            FAIL_RETURN(IDataInput::Probe(ois)->ReadInt32(&((*mSeed)[FRAME_LENGTH + 1])));
            FAIL_RETURN(IDataInput::Probe(ois)->ReadInt32(&((*mSeed)[FRAME_LENGTH + 14])));
            FAIL_RETURN(IDataInput::Probe(ois)->ReadInt32(&((*mSeed)[FRAME_LENGTH + 15])));
        }
        // reading next bytes in seed frame
        for (Int32 i = 0; i < FRAME_LENGTH; i++) {
            FAIL_RETURN(IDataInput::Probe(ois)->ReadInt32(&((*mSeed)[i])));
        }
        // reading remaining seed bytes
        for (Int32 i = 0; i < nRemaining; i++) {
            FAIL_RETURN(IDataInput::Probe(ois)->ReadInt32(&((*mCopies)[FRAME_LENGTH + EXTRAFRAME_OFFSET + i])));
        }
        // reading copy of current hash
        for (Int32 i = 0; i < EXTRAFRAME_OFFSET; i++) {
            FAIL_RETURN(IDataInput::Probe(ois)->ReadInt32(&((*mCopies)[i])));
        }
        // reading current hash
        for (Int32 i = 0; i < EXTRAFRAME_OFFSET; i++) {
            FAIL_RETURN(IDataInput::Probe(ois)->ReadInt32(&((*mSeed)[ISHA1Constants::HASH_OFFSET + i])));
        }
    }

    FAIL_RETURN(IDataInput::Probe(ois)->ReadInt32(&mNextBIndex));
    assert(0 && "TODO");
    // Streams::ReadFully(ois, mNextBytes, mNextBIndex, HASHBYTES_TO_USE - mNextBIndex);
    return NOERROR;
}

ECode CSHA1PRNG_SecureRandomImpl::GetRandomBytes(
    /* [in] */ Int32 byteCount,
    /* [out] */ ArrayOf<Byte>** retValue)
{
    VALIDATE_NOT_NULL(retValue);
    *retValue = NULL;
    if (byteCount <= 0) {
        // throw new IllegalArgumentException("Too few bytes requested: " + byteCount);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IBlockGuardPolicy> originalPolicy = BlockGuard::GetThreadPolicy();
    AutoPtr<ArrayOf<Byte> > result;
    ECode ec = NOERROR;
    do {
        assert(0 && "TODO");
        // BlockGuard::SetThreadPolicy(BlockGuard::LAX_POLICY);
        result = ArrayOf<Byte>::Alloc(byteCount);
        assert(0 && "TODO");
        // ec = Streams::ReadFully(sDevURandom, result, 0, byteCount);
    } while (0);

    BlockGuard::SetThreadPolicy(originalPolicy);
    if (FAILED(ec)) {
        // throw new ProviderException("Couldn't read " + byteCount + " random bytes", ex);
        return E_PROVIDER_EXCEPTION;
    }
    *retValue = result;
    REFCOUNT_ADD(*retValue);
    return NOERROR;
}

}
}
}
}
}
}
