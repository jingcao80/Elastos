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

#include "elastos/droid/net/CPathSegments.h"

class CStatic
{
public:

    CStatic()
    {
        CPathSegments::New(
            NULL_ARRAYOF(String), 9, (IPathSegments**)&PathSegments::EMPTY);
    }
};

static CStatic _static;

AutoPtr<IPathSegments> PathSegments::EMPTY;

PathSegments::PathSegments()
    : mSize(0)
{
}

ECode PathSegments::Init(
    /* [in] */ const ArrayOf<String>& segments,
    /* [in] */ Int32 size)
{
    if (segments.GetLength() > 0) {
        for (Int32 i = 0; i < segments.GetLength(); ++i) {
            mSegments.PushBack(segments[i]);
        }
    }

    mSize = size;

    return NOERROR;
}

ECode PathSegments::Get(
    /* [in] */ Int32 index,
    /* [out] */ String* encoded)
{
    VALIDATE_NOT_NULL(encoded);

    if (index >= mSize) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *encoded = mSegments[index];

    return NOERROR;
}

ECode PathSegments::Size(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mSize;

    return NOERROR;
}

ECode CPathSegments::Get(
    /* [in] */ Int32 index,
    /* [out] */ String* value)
{
    return PathSegments::Get(index, value);
}

ECode CPathSegments::Size(
    /* [out] */ Int32* size)
{
    return PathSegments::Size(size);
}

ECode CPathSegments::constructor(
    /* [in] */ const ArrayOf<String>& segments,
    /* [in] */ Int32 size)
{
    return PathSegments::Init(segments, size);
}

