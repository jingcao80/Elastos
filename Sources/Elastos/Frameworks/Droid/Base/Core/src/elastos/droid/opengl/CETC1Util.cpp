
#include "elastos/droid/opengl/CETC1Util.h"
#include "elastos/droid/opengl/CETC1.h"
#include "elastos/droid/opengl/CETC1Texture.h"
#include "elastos/droid/opengl/CGLES10.h"

#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::IO::IBuffer;
using Elastos::IO::IByteBuffer;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::CByteOrderHelper;
using Elastos::IO::ByteOrder;

namespace Elastos {
namespace Droid {
namespace Opengl {

CAR_INTERFACE_IMPL(CETC1Util, Singleton, IETC1Util)

CAR_SINGLETON_IMPL(CETC1Util)

ECode CETC1Util::LoadTexture(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 border,
    /* [in] */ Int32 fallbackFormat,
    /* [in] */ Int32 fallbackType,
    /* [in] */ Elastos::IO::IInputStream* input)
{
    AutoPtr<IETC1Texture> texture;
    FAIL_RETURN(CreateTexture(input, (IETC1Texture**)&texture));
    FAIL_RETURN(LoadTexture(target, level, border, fallbackFormat, fallbackType, texture));
    return NOERROR;
}

ECode CETC1Util::LoadTexture(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 border,
    /* [in] */ Int32 fallbackFormat,
    /* [in] */ Int32 fallbackType,
    /* [in] */ IETC1Texture* texture)
{
    if (fallbackFormat != IGLES10::_GL_RGB) {
        SLOGGERE("CETC1Util", "fallbackFormat must be GL_RGB")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (! (fallbackType == IGLES10::_GL_UNSIGNED_SHORT_5_6_5
            || fallbackType == IGLES10::_GL_UNSIGNED_BYTE)) {
        SLOGGERE("CETC1Util", "Unsupported fallbackType")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 width;
    texture->GetWidth(&width);
    Int32 height;
    texture->GetHeight(&height);
    AutoPtr<IByteBuffer> data;
    texture->GetData((IByteBuffer**)&data);
    Boolean supported;
    IsETC1Supported(&supported);
    AutoPtr<IGLES10> gles10;
    CGLES10::AcquireSingleton((IGLES10**)&gles10);
    if (supported) {
        Int32 imageSize;
        IBuffer::Probe(data)->GetRemaining(&imageSize);
        gles10->GlCompressedTexImage2D(target, level, IETC1::_ETC1_RGB8_OES, width, height,
                border, imageSize, IBuffer::Probe(data));
    } else {
        Boolean useShorts = fallbackType != IGLES10::_GL_UNSIGNED_BYTE;
        Int32 pixelSize = useShorts ? 2 : 3;
        Int32 stride = pixelSize * width;
        AutoPtr<IByteBufferHelper> helper;
        CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
        AutoPtr<IByteBuffer> decodedData;
        helper->AllocateDirect(stride * height, (IByteBuffer**)&decodedData);
        ByteOrder order;
        AutoPtr<IByteOrderHelper> orderHelper;
        CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
        orderHelper->GetNativeOrder(&order);
        decodedData->SetOrder(order);
        AutoPtr<IETC1> etc1;
        CETC1::AcquireSingleton((IETC1**)&etc1);
        etc1->DecodeImage(IBuffer::Probe(data), IBuffer::Probe(decodedData), width, height, pixelSize, stride);
        gles10->GlTexImage2D(target, level, fallbackFormat, width, height, border,
                fallbackFormat, fallbackType, IBuffer::Probe(decodedData));
    }
    return NOERROR;
}

ECode CETC1Util::IsETC1Supported(
    /* [out] */ Boolean* isSupported)
{
    VALIDATE_NOT_NULL(isSupported)

    AutoPtr<ArrayOf<Int32> > results = ArrayOf<Int32>::Alloc(20);
    AutoPtr<IGLES10> gles10;
    CGLES10::AcquireSingleton((IGLES10**)&gles10);
    gles10->GlGetIntegerv(IGLES10::_GL_NUM_COMPRESSED_TEXTURE_FORMATS, results, 0);
    Int32 numFormats = (*results)[0];
    if (numFormats > results->GetLength()) {
        results = ArrayOf<Int32>::Alloc(numFormats);
    }
    gles10->GlGetIntegerv(IGLES10::_GL_COMPRESSED_TEXTURE_FORMATS, results, 0);
    for (int i = 0; i < numFormats; i++) {
        if ((*results)[i] == IETC1::_ETC1_RGB8_OES) {
            *isSupported = TRUE;
            return NOERROR;
        }
    }
    *isSupported = FALSE;
    return NOERROR;
}

ECode CETC1Util::CreateTexture(
    /* [in] */ Elastos::IO::IInputStream* input,
    /* [out] */ IETC1Texture** texture)
{
    VALIDATE_NOT_NULL(texture)

    Int32 width = 0;
    Int32 height = 0;
    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    AutoPtr<IETC1> etc1;
    CETC1::AcquireSingleton((IETC1**)&etc1);
    AutoPtr<IByteOrderHelper> orderHelper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
    AutoPtr<ArrayOf<Byte> > ioBuffer = ArrayOf<Byte>::Alloc(4096);
    {
        Int32 readRst;
        input->Read(ioBuffer, 0, IETC1::_ETC_PKM_HEADER_SIZE, &readRst);
        if (readRst !=IETC1::_ETC_PKM_HEADER_SIZE) {
            SLOGGERE("CETC1Util", "Unable to read PKM file header.")
            return E_IO_EXCEPTION;
        }

        AutoPtr<IByteBuffer> headerBuffer;
        helper->AllocateDirect(IETC1::_ETC_PKM_HEADER_SIZE, (IByteBuffer**)&headerBuffer);

        ByteOrder order;
        orderHelper->GetNativeOrder(&order);

        headerBuffer->SetOrder(order);
        headerBuffer->Put(ioBuffer, 0, IETC1::_ETC_PKM_HEADER_SIZE);
        IBuffer::Probe(headerBuffer)->SetPosition(0);

        Boolean isValid;
        if (etc1->IsValid(IBuffer::Probe(headerBuffer), &isValid), !isValid) {
            SLOGGERE("CETC1Util", "Not a PKM file.")
            return E_IO_EXCEPTION;
        }
        etc1->GetWidth(IBuffer::Probe(headerBuffer), &width);
        etc1->GetHeight(IBuffer::Probe(headerBuffer), &height);
    }
    Int32 encodedSize;
    etc1->GetEncodedDataSize(width, height, &encodedSize);
    AutoPtr<IByteBuffer> dataBuffer;
    helper->AllocateDirect(encodedSize, (IByteBuffer**)&dataBuffer);
    ByteOrder order;
    orderHelper->GetNativeOrder(&order);
    dataBuffer->SetOrder(order);
    for (Int32 i = 0; i < encodedSize; ) {
        Int32 chunkSize = Elastos::Core::Math::Min(ioBuffer->GetLength(), encodedSize - i);
        Int32 readRst;
        input->Read(ioBuffer, 0, chunkSize, &readRst);
        if (readRst != chunkSize) {
            return E_IO_EXCEPTION;
        }
        dataBuffer->Put(ioBuffer, 0, chunkSize);
        i += chunkSize;
    }
    IBuffer::Probe(dataBuffer)->SetPosition(0);
    CETC1Texture::New(width, height, dataBuffer, texture);
    return NOERROR;
}

ECode CETC1Util::CompressTexture(
    /* [in] */ Elastos::IO::IBuffer* input,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 pixelSize,
    /* [in] */ Int32 stride,
    /* [out] */ IETC1Texture** texture)
{
    VALIDATE_NOT_NULL(texture)

    AutoPtr<IETC1> etc1;
    CETC1::AcquireSingleton((IETC1**)&etc1);
    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    Int32 encodedImageSize;
    etc1->GetEncodedDataSize(width, height, &encodedImageSize);
    AutoPtr<IByteBuffer> compressedImage;
    helper->AllocateDirect(encodedImageSize, (IByteBuffer**)&compressedImage);
    AutoPtr<IByteOrderHelper> orderHelper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
    ByteOrder order;
    orderHelper->GetNativeOrder(&order);
    compressedImage->SetOrder(order);
    etc1->EncodeImage(input, width, height, pixelSize, stride, IBuffer::Probe(compressedImage));
    CETC1Texture::New(width, height, compressedImage, texture);
    return NOERROR;
}

ECode CETC1Util::WriteTexture(
    /* [in] */ IETC1Texture* texture,
    /* [in] */ Elastos::IO::IOutputStream* output)
{
    AutoPtr<IByteBuffer> dataBuffer;
    texture->GetData((IByteBuffer**)&dataBuffer);
    Int32 originalPosition;
    IBuffer::Probe(dataBuffer)->GetPosition(&originalPosition);
    ECode ec = NOERROR;
    // try {
        Int32 width;
        texture->GetWidth(&width);
        Int32 height;
        texture->GetHeight(&height);
        AutoPtr<IByteBuffer> header;
        AutoPtr<IByteBufferHelper> helper;
        CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
        AutoPtr<ArrayOf<Byte> > ioBuffer = ArrayOf<Byte>::Alloc(4096);
        AutoPtr<IETC1> etc1;
        CETC1::AcquireSingleton((IETC1**)&etc1);
        ec = helper->AllocateDirect(IETC1::_ETC_PKM_HEADER_SIZE, (IByteBuffer**)&header);
        AutoPtr<IByteOrderHelper> orderHelper;
        CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
        if(FAILED(ec))
            goto finally;
        ByteOrder order;
        orderHelper->GetNativeOrder(&order);
        header->SetOrder(order);

        ec = etc1->FormatHeader(IBuffer::Probe(header), width, height);
        if(FAILED(ec))
            goto finally;
        ec = header->Get(ioBuffer, 0, IETC1::_ETC_PKM_HEADER_SIZE);
        if(FAILED(ec))
            goto finally;
        ec = output->Write(ioBuffer, 0, IETC1::_ETC_PKM_HEADER_SIZE);
        if(FAILED(ec))
            goto finally;
        Int32 encodedSize;
        ec = etc1->GetEncodedDataSize(width, height, &encodedSize);
        if(FAILED(ec))
            goto finally;
        for (Int32 i = 0; i < encodedSize; ) {
            Int32 chunkSize = Elastos::Core::Math::Min(ioBuffer->GetLength(), encodedSize - i);
            ec = dataBuffer->Get(ioBuffer, 0, chunkSize);
            if(FAILED(ec))
                goto finally;
            ec = output->Write(ioBuffer, 0, chunkSize);
            if(FAILED(ec))
                goto finally;
            i += chunkSize;
        }
    finally:
        IBuffer::Probe(dataBuffer)->SetPosition(originalPosition);
    // }
    return ec;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

