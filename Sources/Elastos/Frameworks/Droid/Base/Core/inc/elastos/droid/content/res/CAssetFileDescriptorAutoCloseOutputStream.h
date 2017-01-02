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

#ifndef __ELASTOS_DROID_CONTENT_RES_CASSETFILEDESCRIPTORAUTOCLOSEOUTPUTSTREAM_H__
#define __ELASTOS_DROID_CONTENT_RES_CASSETFILEDESCRIPTORAUTOCLOSEOUTPUTSTREAM_H__

#include "_Elastos_Droid_Content_Res_CAssetFileDescriptorAutoCloseOutputStream.h"
#include "elastos/droid/os/ParcelFileDescriptor.h"

using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Os::ParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

/**
 * An OutputStream you can create on a ParcelFileDescriptor, which will
 * take care of calling {@link ParcelFileDescriptor#close
 * ParcelFileDescritor.close()} for you when the stream is closed.
 */
CarClass(CAssetFileDescriptorAutoCloseOutputStream)
    , public ParcelFileDescriptor::AutoCloseOutputStream
{
public:
    TO_STRING_IMPL("CAssetFileDescriptorAutoCloseOutputStream");

    CAR_OBJECT_DECL()

    CAssetFileDescriptorAutoCloseOutputStream();

    CARAPI constructor(
        /* [in] */ IAssetFileDescriptor* fd);

    CARAPI Write(
        /* [in] */ Int32 oneByte);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

private:
    Int64 mRemaining;
};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_RES_CASSETFILEDESCRIPTORAUTOCLOSEOUTPUTSTREAM_H__
