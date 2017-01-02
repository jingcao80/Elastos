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

#include "elastos/droid/media/CMediaFileType.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaFileType, Object, IMediaFileType)

CAR_OBJECT_IMPL(CMediaFileType)

CMediaFileType::CMediaFileType()
    : mFileType(0)
{
}

CMediaFileType::~CMediaFileType()
{
}

ECode CMediaFileType::constructor()
{
    return NOERROR;
}

ECode CMediaFileType::constructor(
    /* [in] */ Int32 fileType,
    /* [in] */ const String& mimeType)
{
    mFileType = fileType;
    mMimeType = mimeType;
    return NOERROR;
}

ECode CMediaFileType::SetFileType(
    /* [in] */ Int32 result)
{
    mFileType = result;
    return NOERROR;
}

ECode CMediaFileType::SetMimeType(
    /* [in] */ const String& result)
{
    mMimeType = result;
    return NOERROR;
}

ECode CMediaFileType::GetFileType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mFileType;
    return NOERROR;
}

ECode CMediaFileType::GetMimeType(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mMimeType;
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
