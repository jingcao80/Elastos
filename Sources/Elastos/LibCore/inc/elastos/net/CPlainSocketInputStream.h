
#ifndef __ELASTOS_NET_CSOCKETINPUTSTREAM_H__
#define __ELASTOS_NET_CSOCKETINPUTSTREAM_H__

#include "_Elastos_Net_CPlainSocketInputStream.h"
#include "InputStream.h"

using Elastos::IO::InputStream;

namespace Elastos {
namespace Net {

CarClass(CPlainSocketInputStream)
    , public InputStream
{
public:
    CPlainSocketInputStream();

    CARAPI constructor(
        /* [in] */ ISocketImpl* impl);

    CARAPI Available(
        /* [out] */ Int32* number);

    CARAPI Close();

    CARAPI Read(
        /* [out] */ Int32* value);

    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [out] */ Int32* number);

    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* number);

private:
    AutoPtr<ISocketImpl> mSocket;
};

} // namespace Net
} // namespace Elastos

#endif // __ELASTOS_NET_CSOCKETINPUTSTREAM_H__
