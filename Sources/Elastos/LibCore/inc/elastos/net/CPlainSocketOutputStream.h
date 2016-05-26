
#ifndef __ELASTOS_NET_CSOCKETOUTPUTSTREAM_H__
#define __ELASTOS_NET_CSOCKETOUTPUTSTREAM_H__

#include "_Elastos_Net_CPlainSocketOutputStream.h"
#include "OutputStream.h"

using Elastos::IO::OutputStream;

namespace Elastos {
namespace Net {

CarClass(CPlainSocketOutputStream)
    , public OutputStream
{
public:
    CAR_OBJECT_DECL()

    CPlainSocketOutputStream();

    CARAPI constructor(
        /* [in] */ ISocketImpl * pImpl);

    CARAPI Close();


    CARAPI Write(
        /* [in] */ Int32 oneByte);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte> * buffer);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte> * buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

private:
    AutoPtr<ISocketImpl> mSocket;
};

} // namespace Net
} // namespace Elastos

#endif // __ELASTOS_NET_CSOCKETOUTPUTSTREAM_H__
