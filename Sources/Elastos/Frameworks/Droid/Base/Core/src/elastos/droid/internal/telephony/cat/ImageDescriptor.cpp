
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/ImageDescriptor.h"
#include "elastos/droid/internal/telephony/cat/CatLog.h"

#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                           ImageDescriptor
//=====================================================================
CAR_INTERFACE_IMPL(ImageDescriptor, Object, IImageDescriptor);

const Int32 ImageDescriptor::CODING_SCHEME_BASIC;
const Int32 ImageDescriptor::CODING_SCHEME_COLOUR;

ImageDescriptor::ImageDescriptor()
{
    mWidth = 0;
    mHeight = 0;
    mCodingScheme = 0;
    mImageId = 0;
    mHighOffset = 0;
    mLowOffset = 0;
    mLength = 0;
}

AutoPtr<IImageDescriptor> ImageDescriptor::Parse(
    /* [in] */ ArrayOf<Byte>* rawData,
    /* [in] */ Int32 valueIndex)
{
    AutoPtr<ImageDescriptor> d = new ImageDescriptor();
    // try {
        d->mWidth = (*rawData)[valueIndex++] & 0xff;
        d->mHeight = (*rawData)[valueIndex++] & 0xff;
        d->mCodingScheme = (*rawData)[valueIndex++] & 0xff;

        // parse image id
        d->mImageId = ((*rawData)[valueIndex++] & 0xff) << 8;
        d->mImageId |= (*rawData)[valueIndex++] & 0xff;
        // parse offset
        d->mHighOffset = ((*rawData)[valueIndex++] & 0xff); // high byte offset
        d->mLowOffset = (*rawData)[valueIndex++] & 0xff; // low byte offset

        d->mLength = (((*rawData)[valueIndex++] & 0xff) << 8 | ((*rawData)[valueIndex++] & 0xff));
        CatLog::D(String("ImageDescriptor"), String("parse; Descriptor : ")
                + StringUtils::ToString(d->mWidth) + String(", ") + StringUtils::ToString(d->mHeight)
                + String(", ") + StringUtils::ToString(d->mCodingScheme) + String(", 0x")
                + StringUtils::ToString(d->mImageId) + String(", ")
                + StringUtils::ToString(d->mHighOffset) + String(", ")
                + StringUtils::ToString(d->mLowOffset) + String(", ")
                + StringUtils::ToString(d->mLength));
    // } catch (IndexOutOfBoundsException e) {
    //     CatLog.d("ImageDescriptor", "parse; failed parsing image descriptor");
    //     d = null;
    // }
    return d;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
