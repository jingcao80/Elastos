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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CRegionIterator.h"
#include "elastos/droid/graphics/CRegion.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include <skia/core/SkRegion.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

struct RgnIterPair
{
    RgnIterPair(const SkRegion& rgn) : fRgn(rgn)
    {
        // have our iterator reference our copy (fRgn), so we know it will be
        // unchanged for the lifetime of the iterator
        fIter.reset(fRgn);
    }

    SkRegion fRgn;   // a copy of the caller's region
    SkRegion::Iterator fIter;  // an iterator acting upon the copy (fRgn)
};

CAR_OBJECT_IMPL(CRegionIterator);
CAR_INTERFACE_IMPL(CRegionIterator, Object, IRegionIterator);
CRegionIterator::~CRegionIterator()
{
    NativeDestructor(mNativeIter);
}

ECode CRegionIterator::constructor(
    /* [in] */ IRegion* region)
{
    mNativeIter = NativeConstructor(((CRegion*)region)->mNativeRegion);
    return NOERROR;
}

ECode CRegionIterator::Next(
    /* [in] */ IRect* r,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (r == NULL) {
        // throw new NullPointerException("The Rect must be provided");
        return E_NULL_POINTER_EXCEPTION;
    }
    *result = NativeNext(mNativeIter, r);
    return NOERROR;
}

Int64 CRegionIterator::NativeConstructor(
    /* [in] */ Int64 regionHandle)
{
    const SkRegion* region = reinterpret_cast<SkRegion*>(regionHandle);
    SkASSERT(region);
    return reinterpret_cast<Int64>(new RgnIterPair(*region));
}

void CRegionIterator::NativeDestructor(
    /* [in] */ Int64 pairHandle)
{
    RgnIterPair* pair = reinterpret_cast<RgnIterPair*>(pairHandle);
    SkASSERT(pair);
    delete pair;
}

Boolean CRegionIterator::NativeNext(
    /* [in] */ Int64 pairHandle,
    /* [in] */ IRect* r)
{
    RgnIterPair* pair = reinterpret_cast<RgnIterPair*>(pairHandle);
    // the caller has checked that rectObject is not nul
    SkASSERT(pair);
    SkASSERT(r);

    if (!pair->fIter.done()) {
        GraphicsNative::SkIRect2IRect(pair->fIter.rect(), r);
        pair->fIter.next();
        return TRUE;
    }
    return FALSE;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
