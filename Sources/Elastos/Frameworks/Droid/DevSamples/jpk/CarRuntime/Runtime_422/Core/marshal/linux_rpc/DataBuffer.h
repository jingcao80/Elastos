
#ifndef __DATABUFFER_H__
#define __DATABUFFER_H__

#include "eltypes.h"

namespace Elastos {
namespace RPC {

class DataBuffer : public ElLightRefBase
{
public:
    DataBuffer(
        /* [in] */ Int32 cap = 128);

    ~DataBuffer();

    CARAPI_(void) Append(
        /* [in] */ const void* buf,
        /* [in] */ Int32 size);

    CARAPI_(void*) Data();

    CARAPI_(Int32) DataSize();

    CARAPI_(void) Reset();

private:
    Int32 mSize;
    Int32 mCapacity;
    void* mBuf;
};

} // namespace RPC
} // namespace Elastos

#endif //__DATABUFFER_H__
