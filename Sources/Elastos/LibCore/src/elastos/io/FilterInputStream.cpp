
#include "FilterInputStream.h"
#include "AutoLock.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(FilterInputStream, InputStream, IFilterInputStream)

FilterInputStream::FilterInputStream()
{}

FilterInputStream::~FilterInputStream()
{}

ECode FilterInputStream::constructor(
    /* [in] */ IInputStream* in)
{
    mIn = in;
    return NOERROR;
}

ECode FilterInputStream::Available(
    /* [out] */ Int32* number)
{
    return mIn->Available(number);
}

ECode FilterInputStream::Close()
{
    return ICloseable::Probe(mIn)->Close();
}

ECode FilterInputStream::Mark(
    /* [in] */ Int32 readLimit)
{
    AutoLock lock(this);
    return mIn->Mark(readLimit);
}

ECode FilterInputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    return mIn->IsMarkSupported(supported);
}

ECode FilterInputStream::Read(
    /* [out] */ Int32* value)
{
    return mIn->Read(value);
}

ECode FilterInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    return mIn->Read(buffer, byteOffset, byteCount, number);
}

ECode FilterInputStream::Reset()
{
    AutoLock lock(this);
    return mIn->Reset();
}

ECode FilterInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    return mIn->Skip(byteCount, number);
}

} // namespace IO
} // namespace Elastos
