
#include "FilterReader.h"
#include "AutoLock.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(FilterReader, Reader, IFilterReader)

FilterReader::FilterReader()
{
}

FilterReader::~FilterReader()
{
}

ECode FilterReader::constructor(
    /* [in] */ IReader* in)
{
    VALIDATE_NOT_NULL(in);

    FAIL_RETURN(Reader::constructor(ISynchronize::Probe(in)))

    mIn = in;

    return NOERROR;
}

ECode FilterReader::Close()
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return ICloseable::Probe(mIn)->Close();
}

ECode FilterReader::Mark(
    /* [in] */ Int32 readLimit)
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return mIn->Mark(readLimit);
}

ECode FilterReader::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported)
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return mIn->IsMarkSupported(supported);
}

ECode FilterReader::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return mIn->Read(value);
}

ECode FilterReader::Read(
    /* [out] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    VALIDATE_NOT_NULL(buffer)
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return mIn->Read(buffer, offset, count, number);
}

ECode FilterReader::IsReady(
    /* [out] */ Boolean* ready)
{
    VALIDATE_NOT_NULL(ready)
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return mIn->IsReady(ready);
}

ECode FilterReader::Reset()
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return mIn->Reset();
}

ECode FilterReader:: Skip(
    /* [in] */ Int64 count,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return mIn->Skip(count, number);
}

} // namespace IO
} // namespace Elastos
