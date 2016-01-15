
#include <DataBuffer.h>

namespace Elastos {
namespace RPC {

DataBuffer::DataBuffer(
    /* [in] */ Int32 cap)
    : mSize(0)
    , mCapacity(cap)
{
    mBuf = malloc(mCapacity);
}

DataBuffer::~DataBuffer()
{
    free(mBuf);
    mCapacity = mSize = 0;
}

void DataBuffer::Append(
    /* [in] */ const void* buf,
    /* [in] */ Int32 size)
{
    if (mSize + size > mCapacity) {
        Int32 newSize = ((mSize + size) * 3) / 2;
        void* data = realloc(mBuf, newSize);
        assert(data != NULL);
        mBuf = data;
        mCapacity = newSize;
    }
    memcpy((UInt8*)mBuf + mSize, buf, size);
    mSize += size;
}

void* DataBuffer::Data()
{
    return mBuf;
}

Int32 DataBuffer::DataSize()
{
    return mSize;
}

void DataBuffer::Reset()
{
    mSize = 0;
}

} // namespace RPC
} // namespace Elastos
