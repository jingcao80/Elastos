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
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CTableMaskFilter.h"
#include <skia/effects/SkTableMaskFilter.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CTableMaskFilter);
CAR_INTERFACE_IMPL(CTableMaskFilter, MaskFilter, ITableMaskFilter);
ECode CTableMaskFilter::constructor(
    /* [in] */ const ArrayOf<Byte>& table)
{
    if (table.GetLength() < 256) {
        // throw new RuntimeException("table.length must be >= 256");
        return E_RUNTIME_EXCEPTION;
    }
    mNativeInstance = NativeNewTable(table);
    return NOERROR;
}

ECode CTableMaskFilter::constructor(
    /* [in] */ Int64 ni)
{
    mNativeInstance = ni;
    return NOERROR;
}

ECode CTableMaskFilter::CreateClipTable(
    /* [in] */ Int32 min,
    /* [in] */ Int32 max,
    /* [out] */ ITableMaskFilter** tf)
{
    VALIDATE_NOT_NULL(tf);

    return CTableMaskFilter::New(NativeNewClip(min, max), tf);
}

ECode CTableMaskFilter::CreateGammaTable(
    /* [in] */ Float gamma,
    /* [out] */ ITableMaskFilter** tf)
{
    VALIDATE_NOT_NULL(tf);

    return CTableMaskFilter::New(NativeNewGamma(gamma), tf);
}

Int64 CTableMaskFilter::NativeNewTable(
    /* [in] */ const ArrayOf<Byte>& table)
{
    // AutoJavaByteArray autoTable(env, jtable, 256);
    assert(table.GetLength() >= 256);
    SkMaskFilter* filter = SkTableMaskFilter::Create((const uint8_t*)table.GetPayload());
    return reinterpret_cast<Int64>(filter);
}

Int64 CTableMaskFilter::NativeNewClip(
    /* [in] */ Int32 min,
    /* [in] */ Int32 max)
{
    SkMaskFilter* filter = SkTableMaskFilter::CreateClip(min, max);
    return reinterpret_cast<Int64>(filter);
}

Int64 CTableMaskFilter::NativeNewGamma(
    /* [in] */ Float gamma)
{
    SkMaskFilter* filter = SkTableMaskFilter::CreateGamma(gamma);
    return reinterpret_cast<Int64>(filter);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
