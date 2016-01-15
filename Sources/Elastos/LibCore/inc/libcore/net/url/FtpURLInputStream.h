
#ifndef __LIBCORE_NET_URL_LIBCORE_NET_URI_FTPURLINPUTSTREAM_H__
#define __LIBCORE_NET_URL_LIBCORE_NET_URI_FTPURLINPUTSTREAM_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "Elastos.CoreLibrary.Net.h"
#include "InputStream.h"

using Elastos::IO::InputStream;
using Elastos::IO::IInputStream;
using Elastos::Net::ISocket;

namespace Libcore {
namespace Net {
namespace Url {

class FtpURLInputStream
    : public InputStream
    , public IFtpURLInputStream
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IInputStream* is,
        /* [in] */ ISocket* controlSocket);

    CARAPI Available(
        /* [out] */ Int32* number);

    CARAPI Close();

    CARAPI Mark(
        /* [in] */ Int32 readLimit);

    CARAPI IsMarkSupported(
        /* [out] */ Boolean* supported);

    CARAPI Read(
        /* [out] */ Int32* value);

    CARAPI ReadBytes(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* number);

    CARAPI Reset();

    CARAPI Skip(
        /* [in] */ Int64 byteCount,
        /* [out] */ Int64* number);

private:
    AutoPtr<IInputStream> mIs; // Actual input stream

    AutoPtr<ISocket> mControlSocket;
    Object mLock;
};

} // namespace Url
} // namespace Net
} // namespace Libcore

#endif //__LIBCORE_NET_URL_LIBCORE_NET_URI_FTPURLINPUTSTREAM_H__
