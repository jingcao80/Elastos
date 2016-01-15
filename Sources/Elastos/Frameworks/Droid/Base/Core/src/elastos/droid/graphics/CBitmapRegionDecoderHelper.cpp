
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CBitmapRegionDecoderHelper.h"
#include "elastos/droid/graphics/BitmapRegionDecoder.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_SINGLETON_IMPL(CBitmapRegionDecoderHelper);
CAR_INTERFACE_IMPL(CBitmapRegionDecoderHelper, Singleton, IBitmapRegionDecoderHelper);
/**
* Create a BitmapRegionDecoder from the specified byte array.
* Currently only the JPEG and PNG formats are supported.
*
* @param data byte array of compressed image data.
* @param offset offset into data for where the decoder should begin
*               parsing.
* @param length the number of bytes, beginning at offset, to parse
* @param isShareable If this is true, then the BitmapRegionDecoder may keep a
*                    shallow reference to the input. If this is false,
*                    then the BitmapRegionDecoder will explicitly make a copy of the
*                    input data, and keep that. Even if sharing is allowed,
*                    the implementation may still decide to make a deep
*                    copy of the input data. If an image is progressively encoded,
*                    allowing sharing may degrade the decoding speed.
* @param ppIBrd, or null if the image data could not be decoded.
*
*/
ECode CBitmapRegionDecoderHelper::NewInstance(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ Boolean isShareable,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    VALIDATE_NOT_NULL(decoder);

    return BitmapRegionDecoder::NewInstance(data, offset, length, isShareable, decoder);
}
/**
* Create a BitmapRegionDecoder from the file descriptor.
* The position within the descriptor will not be changed when
* this returns, so the descriptor can be used again as is.
* Currently only the JPEG and PNG formats are supported.
*
* @param fd The file descriptor containing the data to decode
* @param isShareable If this is true, then the BitmapRegionDecoder may keep a
*                    shallow reference to the input. If this is false,
*                    then the BitmapRegionDecoder will explicitly make a copy of the
*                    input data, and keep that. Even if sharing is allowed,
*                    the implementation may still decide to make a deep
*                    copy of the input data. If an image is progressively encoded,
*                    allowing sharing may degrade the decoding speed.
* @param ppIBrd, or null if the image data could not be decoded.
*/
ECode CBitmapRegionDecoderHelper::NewInstance(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Boolean isShareable,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    VALIDATE_NOT_NULL(decoder);

    return BitmapRegionDecoder::NewInstance(fd, isShareable, decoder);
}

/**
* Create a BitmapRegionDecoder from an input stream.
* The stream's position will be where ever it was after the encoded data
* was read.
* Currently only the JPEG and PNG formats are supported.
*
* @param is The input stream that holds the raw data to be decoded into a
*           BitmapRegionDecoder.
* @param isShareable If this is true, then the BitmapRegionDecoder may keep a
*                    shallow reference to the input. If this is false,
*                    then the BitmapRegionDecoder will explicitly make a copy of the
*                    input data, and keep that. Even if sharing is allowed,
*                    the implementation may still decide to make a deep
*                    copy of the input data. If an image is progressively encoded,
*                    allowing sharing may degrade the decoding speed.
* @param ppIBrd, or null if the image data could not be decoded.
*/
ECode CBitmapRegionDecoderHelper::NewInstance(
    /* [in] */ IInputStream* is,
    /* [in] */ Boolean isShareable,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    VALIDATE_NOT_NULL(decoder);

    return BitmapRegionDecoder::NewInstance(is, isShareable, decoder);
}


/**
* Create a BitmapRegionDecoder from a file path.
* Currently only the JPEG and PNG formats are supported.
*
* @param pathName complete path name for the file to be decoded.
* @param isShareable If this is true, then the BitmapRegionDecoder may keep a
*                    shallow reference to the input. If this is false,
*                    then the BitmapRegionDecoder will explicitly make a copy of the
*                    input data, and keep that. Even if sharing is allowed,
*                    the implementation may still decide to make a deep
*                    copy of the input data. If an image is progressively encoded,
*                    allowing sharing may degrade the decoding speed.
* @param ppIBrd, or null if the image data could not be decoded.
*/
ECode CBitmapRegionDecoderHelper::NewInstance(
    /* [in] */ const String& pathName,
    /* [in] */ Boolean isShareable,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    VALIDATE_NOT_NULL(decoder);

    return BitmapRegionDecoder::NewInstance(pathName, isShareable, decoder);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
