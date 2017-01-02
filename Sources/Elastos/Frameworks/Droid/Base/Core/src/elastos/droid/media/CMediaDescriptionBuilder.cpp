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

#include "elastos/droid/media/CMediaDescriptionBuilder.h"
#include "elastos/droid/media/CMediaDescription.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaDescriptionBuilder, Object, IMediaDescriptionBuilder)

CAR_OBJECT_IMPL(CMediaDescriptionBuilder)

CMediaDescriptionBuilder::CMediaDescriptionBuilder()
{
}

CMediaDescriptionBuilder::~CMediaDescriptionBuilder()
{
}

ECode CMediaDescriptionBuilder::constructor()
{
    return NOERROR;
}

ECode CMediaDescriptionBuilder::SetMediaId(
    /* [in] */ const String& mediaId)
{
    mMediaId = mediaId;
    return NOERROR;
}

ECode CMediaDescriptionBuilder::SetTitle(
    /* [in] */ ICharSequence* title)
{
    mTitle = title;
    return NOERROR;
}

ECode CMediaDescriptionBuilder::SetSubtitle(
    /* [in] */ ICharSequence* subtitle)
{
    mSubtitle = subtitle;
    return NOERROR;
}

ECode CMediaDescriptionBuilder::SetDescription(
    /* [in] */ ICharSequence* description)
{
    mDescription = description;
    return NOERROR;
}

ECode CMediaDescriptionBuilder::SetIconBitmap(
    /* [in] */ IBitmap* icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode CMediaDescriptionBuilder::SetIconUri(
    /* [in] */ IUri* iconUri)
{
    mIconUri = iconUri;
    return NOERROR;
}

ECode CMediaDescriptionBuilder::SetExtras(
    /* [in] */ IBundle* extras)
{
    mExtras = extras;
    return NOERROR;
}

ECode CMediaDescriptionBuilder::Build(
    /* [out] */ IMediaDescription** result)
{
    VALIDATE_NOT_NULL(result)
    return CMediaDescription::New(mMediaId, mTitle, mSubtitle,
        mDescription, mIcon, mIconUri, mExtras, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
