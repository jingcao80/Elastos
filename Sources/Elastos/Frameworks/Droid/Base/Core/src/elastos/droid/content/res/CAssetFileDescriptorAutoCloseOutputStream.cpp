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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/res/CAssetFileDescriptorAutoCloseOutputStream.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_OBJECT_IMPL(CAssetFileDescriptorAutoCloseOutputStream)

CAssetFileDescriptorAutoCloseOutputStream::CAssetFileDescriptorAutoCloseOutputStream()
    : mRemaining(0)
{}

ECode CAssetFileDescriptorAutoCloseOutputStream::constructor(
    /* [in] */ IAssetFileDescriptor* fd)
{
    AutoPtr<IParcelFileDescriptor> pfd;
    fd->GetParcelFileDescriptor((IParcelFileDescriptor**)&pfd);
    ParcelFileDescriptor::AutoCloseOutputStream::constructor(pfd);

    Int64 startOffset = 0;
    fd->GetStartOffset(&startOffset);
    Int64 toOffset = 0;
    FAIL_RETURN(pfd->SeekTo(startOffset, &toOffset));
    if (toOffset < 0) {
        //throw new IOException("Unable to seek");
        return E_IO_EXCEPTION;
    }

    fd->GetLength(&mRemaining);
    return NOERROR;
}

ECode CAssetFileDescriptorAutoCloseOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    if (mRemaining >= 0) {
        if (mRemaining == 0) return NOERROR;
        if (count > mRemaining) count = (Int32)mRemaining;
        FAIL_RETURN(ParcelFileDescriptor::AutoCloseOutputStream::Write(buffer, offset, count));
        mRemaining -= count;
        return NOERROR;
    }

    return ParcelFileDescriptor::AutoCloseOutputStream::Write(buffer, offset, count);
}

ECode CAssetFileDescriptorAutoCloseOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    if (mRemaining >= 0) {
        if (mRemaining == 0) return NOERROR;
        Int32 count = buffer->GetLength();
        if (count > mRemaining) count = (Int32)mRemaining;
        FAIL_RETURN(ParcelFileDescriptor::AutoCloseOutputStream::Write(buffer));
        mRemaining -= count;
        return NOERROR;
    }

    return ParcelFileDescriptor::AutoCloseOutputStream::Write(buffer);
}

ECode CAssetFileDescriptorAutoCloseOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    if (mRemaining >= 0) {
        if (mRemaining == 0) return NOERROR;
        FAIL_RETURN(ParcelFileDescriptor::AutoCloseOutputStream::Write(oneByte));
        mRemaining--;
        return NOERROR;
    }

    return ParcelFileDescriptor::AutoCloseOutputStream::Write(oneByte);
}

}
}
}
}
