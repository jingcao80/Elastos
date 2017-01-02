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

#ifndef __ELASTOS_DROID_GRAPHICS_CTABLEMASKFILTER_H__
#define __ELASTOS_DROID_GRAPHICS_CTABLEMASKFILTER_H__

#include "_Elastos_Droid_Graphics_CTableMaskFilter.h"
#include "elastos/droid/graphics/MaskFilter.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CTableMaskFilter)
    , public MaskFilter
    , public ITableMaskFilter
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const ArrayOf<Byte>& table);

    CARAPI constructor(
        /* [in] */ Int64 ni);

    static CARAPI CreateClipTable(
        /* [in] */ Int32 min,
        /* [in] */ Int32 max,
        /* [out] */ ITableMaskFilter** tf);

    static CARAPI CreateGammaTable(
        /* [in] */ Float gamma,
        /* [out] */ ITableMaskFilter** tf);

private:
    static CARAPI_(Int64) NativeNewTable(
        /* [in] */ const ArrayOf<Byte>& table);

    static CARAPI_(Int64) NativeNewClip(
        /* [in] */ Int32 min,
        /* [in] */ Int32 max);

    static CARAPI_(Int64) NativeNewGamma(
        /* [in] */ Float gamma);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CTABLEMASKFILTER_H__
