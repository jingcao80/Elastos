#include "FileChannel.h"

namespace Elastos {
namespace IO {
namespace Channels {

CAR_INTERFACE_IMPL_4(FileChannel, Object, IByteChannel, IScatteringByteChannel, IGatheringByteChannel, IFileChannel)

FileChannel::FileChannel()
    : AbstractInterruptibleChannel()
{}

ECode FileChannel::Lock(
    /*[out]*/ IFileLock** lock)
{
    return this->Lock(0L, LONG_MAX, FALSE, lock);
}

ECode FileChannel::Read(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [out] */ Int64* number)
{
    return Read(buffers, 0, 0, number);
}

ECode FileChannel::TryLock(
    /* [out] */ IFileLock** lock)
{
    return this->TryLock(0L, LONG_MAX, FALSE, lock);
}

ECode FileChannel::Write(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [out] */ Int64* number)
{
    return this->Write(buffers, 0, buffers->GetLength(), number);
}

} // namespace Channels
} // namespace IO
} // namespace Elastos
