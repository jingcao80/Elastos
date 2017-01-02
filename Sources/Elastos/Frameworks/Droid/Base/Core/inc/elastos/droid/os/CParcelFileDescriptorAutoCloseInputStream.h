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

#ifndef __ELASTOS_DROID_OS_CPARCELFILEDESCRIPTORAUTOCLOSEINPUTSTREAM_H__
#define __ELASTOS_DROID_OS_CPARCELFILEDESCRIPTORAUTOCLOSEINPUTSTREAM_H__

#include "_Elastos_Droid_Os_CParcelFileDescriptorAutoCloseInputStream.h"
#include "elastos/droid/os/ParcelFileDescriptor.h"

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * An InputStream you can create on a ParcelFileDescriptor, which will
 * take care of calling {@link ParcelFileDescriptor#close
 * ParcelFileDescriptor.close()} for you when the stream is closed.
 */
CarClass(CParcelFileDescriptorAutoCloseInputStream)
    , public ParcelFileDescriptor::AutoCloseInputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CPARCELFILEDESCRIPTORAUTOCLOSEINPUTSTREAM_H__
