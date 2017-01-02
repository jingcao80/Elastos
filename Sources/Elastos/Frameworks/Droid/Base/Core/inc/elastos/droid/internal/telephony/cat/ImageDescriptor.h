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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_IMAGEDESCRIPTOR_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_IMAGEDESCRIPTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

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
