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

        Int32 high = ((*rawData)[valueIndex++] & 0xff);
        Int32 low = ((*rawData)[valueIndex++] & 0xff);
        d->mLength = (high << 8 | low);
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
