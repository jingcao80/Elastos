
#ifndef __ELASTOS_DROID_OPENGL_CETC1_H__
#define __ELASTOS_DROID_OPENGL_CETC1_H__

#include "_Elastos_Droid_Opengl_CETC1.h"
#include <cmdef.h>

using Elastos::IO::IBuffer;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CETC1)
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
