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

#ifndef __ELASTOS_DROID_OPENGL_CETC1_H__
#define __ELASTOS_DROID_OPENGL_CETC1_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Opengl.h"
#include "_Elastos_Droid_Opengl_CETC1.h"

#include <elastos/core/Singleton.h>

using Elastos::IO::IBuffer;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CETC1)
    , public Singleton
    , public IETC1
{
private:
    class BufferHelper
    {
    public:
        BufferHelper(
            /* [in] */ IBuffer* buffer) {
            mBuffer = buffer;
            mData = NULL;
            mRemaining = 0;
        }

        CARAPI CheckPointer(
            /* [in] */ const char* errorMessage,
            /* [in] */ Boolean* rst)
        {
            if (mBuffer) {
                Handle32 dataTmp;
                FAIL_RETURN(CETC1::GetPointer(mBuffer, &mRemaining, &dataTmp));
                mData = reinterpret_cast<void*>(dataTmp);
                if (mData == NULL) {
                    *rst = FALSE;
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
                *rst = (mData != NULL);
                return NOERROR;
            } else {
                *rst = FALSE;
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }

        inline void* GetData() {
            return mData;
        }

        inline Int32 Remaining() {
            return mRemaining;
        }

    private:
        AutoPtr<IBuffer> mBuffer;
        void* mData;
        Int32 mRemaining;
    };
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI EncodeBlock(
        /* [in] */ Elastos::IO::IBuffer* inBuf,
        /* [in] */ Int32 validPixelMask,
        /* [in] */ Elastos::IO::IBuffer* outBuf);

    CARAPI DecodeBlock(
        /* [in] */ Elastos::IO::IBuffer* inBuf,
        /* [in] */ Elastos::IO::IBuffer* outBuf);

    CARAPI GetEncodedDataSize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ Int32* size);

    CARAPI EncodeImage(
        /* [in] */ Elastos::IO::IBuffer* inBuf,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 pixelSize,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* outBuf);

    CARAPI DecodeImage(
        /* [in] */ Elastos::IO::IBuffer* inBuf,
        /* [in] */ Elastos::IO::IBuffer* outBuf,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 pixelSize,
        /* [in] */ Int32 stride);

    CARAPI FormatHeader(
        /* [in] */ Elastos::IO::IBuffer* header,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI IsValid(
        /* [in] */ Elastos::IO::IBuffer* header,
        /* [out] */ Boolean* isvalid);

    CARAPI GetWidth(
        /* [in] */ Elastos::IO::IBuffer* header,
        /* [out] */ Int32* width);

    CARAPI GetHeight(
        /* [in] */ Elastos::IO::IBuffer* header,
        /* [out] */ Int32* height);

private:
    static CARAPI GetPointer(
        /* [in] */ Elastos::IO::IBuffer* inBuf,
        /* [in, out] */ Int32 *remaining,
        /* [out] */ Handle32* rst);
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CETC1_H__
