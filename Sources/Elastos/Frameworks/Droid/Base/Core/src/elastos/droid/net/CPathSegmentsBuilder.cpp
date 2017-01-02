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
#include "elastos/droid/net/CPathSegmentsBuilder.h"

PathSegmentsBuilder::PathSegmentsBuilder()
    : mSize(0)
{
}

ECode PathSegmentsBuilder::Add(
    /* [in] */ const String& segment)
{
    mSegments.PushBack(segment);
    mSize++;

    return NOERROR;
}

ECode PathSegmentsBuilder::Build(
    /* [out] */ IPathSegments** pathSegments)
{
    VALIDATE_NOT_NULL(pathSegments);

    if (mSegments.IsEmpty()) {
        *pathSegments = PathSegments::EMPTY;
        REFCOUNT_ADD(*pathSegments);
        return NOERROR;
    }

    // try {
    AutoPtr<ArrayOf<String> > segments = ArrayOf<String>::Alloc(mSegments.GetSize());
    if (!segments)
        return E_OUT_OF_MEMORY;

    List<String>::Iterator itor;
    Int32 i = 0;
    for (itor = mSegments.Begin(); itor != mSegments.End(); ++itor, ++i) {
        (*segments)[i] = *itor;
    }

    ECode ec = CPathSegments::New(*segments, mSize, pathSegments);
    if (FAILED(ec)) {
        return ec;
    }

    if (*pathSegments == NULL) {
        return NOERROR;
    }

    // } finally {
    // Makes sure this doesn't get reused.
    mSegments.Clear();
    // }

    return NOERROR;
}

ECode CPathSegmentsBuilder::Add(
    /* [in] */ const String& segment)
{
    PathSegmentsBuilder::Add(segment);

    return NOERROR;
}

ECode CPathSegmentsBuilder::Build(
    /* [out] */ IPathSegments** pathSegments)
{
    return PathSegmentsBuilder::Build(pathSegments);
}

