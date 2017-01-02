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

#ifndef __ELASTOS_UTILITY_ZIP_H__
#define __ELASTOS_UTILITY_ZIP_H__

#include "Object.h"
#include "zlib.h"
#include "zutil.h"// For DEF_WBITS and DEF_MEM_LEVEL.

using Elastos::Core::Object;

namespace Elastos {
namespace Utility {
namespace Zip {

class NativeZipStream
    : public Object
{
public:
    NativeZipStream()
        : mInput(NULL)
        , mInCap(0)
        , mDict(NULL)
    {
        // Let zlib use its default allocator.
        mStream.opaque = Z_NULL;
        mStream.zalloc = Z_NULL;
        mStream.zfree = Z_NULL;
    }

    ~NativeZipStream()
    {
    }

    CARAPI_(void) SetDictionary(
        /* [in] */ ArrayOf<Byte>* dictionary,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [in] */ Boolean inflate)
    {
        AutoPtr<ArrayOf<Byte> > dictionaryBytes = ArrayOf<Byte>::Alloc(len);
        if (dictionaryBytes == NULL) {
//            jniThrowOutOfMemoryError(env, NULL);
            return;
        }
        dictionaryBytes->Copy(dictionary, off, len);
        const Bytef* zDictionary = reinterpret_cast<const Bytef*>(dictionaryBytes->GetPayload());
        Int32 err;
        if (inflate) {
            err = inflateSetDictionary(&mStream, zDictionary, len);
        }
        else {
            err = deflateSetDictionary(&mStream, zDictionary, len);
        }
        if (err != Z_OK) {
//            throwExceptionForZlibError(env, "java/lang/IllegalArgumentException", err);
            return;
        }
        mDict = dictionaryBytes;
    }

    CARAPI SetInput(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len)
    {
        mInput = ArrayOf<Byte>::Alloc(len);
        if (mInput == NULL) {
            mInCap = 0;
            return E_OUT_OF_MEMORY_ERROR;
        }

        mInCap = len;
        if (buf != NULL) {
            mInput->Copy((Byte*)(buf->GetPayload() + off), len);
        }
        mStream.next_in = reinterpret_cast<Bytef*>(mInput->GetPayload());
        mStream.avail_in = len;
        return NOERROR;
    }

private:
    // Disallow copy and assignment.
    NativeZipStream(const NativeZipStream&);
    void operator=(const NativeZipStream&);

public:
    AutoPtr<ArrayOf<Byte> > mInput;
    Int32 mInCap;
    z_stream mStream;

private:
    AutoPtr<ArrayOf<Byte> > mDict;
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif  //__ELASTOS_UTILITY_ZIP_H__
