
#include "Elastos.CoreLibrary.Net.h"
#include "FtpURLInputStream.h"
#include "IoUtils.h"
#include "AutoLock.h"

using Elastos::Core::AutoLock;
using Libcore::IO::IoUtils;

namespace Libcore {
namespace Net {
namespace Url {

CAR_INTERFACE_IMPL(FtpURLInputStream, InputStream, IFtpURLInputStream)

ECode FtpURLInputStream::constructor(
    /* [in] */ IInputStream* is,
    /* [in] */ ISocket* controlSocket)
{
    mIs = is;
    mControlSocket = controlSocket;
    return NOERROR;
}

ECode FtpURLInputStream::Available(
    /* [out] */ Int32* number)
{
    return mIs->Available(number);
}

ECode FtpURLInputStream::Close()
{
    IoUtils::CloseQuietly(ICloseable::Probe(mIs));
    IoUtils::CloseQuietly(mControlSocket);
    return NOERROR;
}

ECode FtpURLInputStream::Mark(
    /* [in] */ Int32 readLimit)
{
    AutoLock lock(mLock);
    return mIs->Mark(readLimit);;
}

ECode FtpURLInputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    return mIs->IsMarkSupported(supported);
}

ECode FtpURLInputStream::Read(
    /* [out] */ Int32* value)
{
    return mIs->Read(value);
}

ECode FtpURLInputStream::ReadBytes(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* number)
{
    return mIs->Read(buffer, offset, length, number);
}

ECode FtpURLInputStream::Reset()
{
    AutoLock lock(mLock);
    return mIs->Reset();;
}

ECode FtpURLInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    return mIs->Skip(byteCount, number);
}

} // namespace Url
} // namespace Net
} // namespace Libcore
