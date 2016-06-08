#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_IMAGEDESCRIPTOR_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_IMAGEDESCRIPTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * {@hide}
  */
class ImageDescriptor
    : public Object
    , public IImageDescriptor
{
public:
    CAR_INTERFACE_DECL();

    // public static final int ID_LENGTH = 9;
    // ID_LENGTH substituted by IccFileHandlerBase.GET_RESPONSE_EF_IMG_SIZE_BYTES
    ImageDescriptor();

    /**
      * Extract descriptor information about image instance.
      *
      * @param rawData
      * @param valueIndex
      * @return ImageDescriptor
      */
    static CARAPI_(AutoPtr<IImageDescriptor>) Parse(
        /* [in] */ ArrayOf<Byte>* rawData,
        /* [in] */ Int32 valueIndex);

public:
    // members
    Int32 mWidth;
    Int32 mHeight;
    Int32 mCodingScheme;
    Int32 mImageId;
    Int32 mHighOffset;
    Int32 mLowOffset;
    Int32 mLength;
    // constants
    static const Int32 CODING_SCHEME_BASIC = 0x11;
    static const Int32 CODING_SCHEME_COLOUR = 0x21;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_IMAGEDESCRIPTOR_H__
