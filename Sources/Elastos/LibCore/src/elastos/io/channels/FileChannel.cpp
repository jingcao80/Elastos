//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "FileChannel.h"

namespace Elastos {
namespace IO {
namespace Channels {

CAR_INTERFACE_IMPL_4(FileChannel, AbstractInterruptibleChannel, IByteChannel, IScatteringByteChannel, IGatheringByteChannel, IFileChannel)

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
