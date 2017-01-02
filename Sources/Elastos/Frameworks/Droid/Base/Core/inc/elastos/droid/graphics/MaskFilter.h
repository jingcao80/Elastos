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

#ifndef __ELASTOS_DROID_GRAPHICS_MASKFILTER_H__
#define __ELASTOS_DROID_GRAPHICS_MASKFILTER_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

extern const InterfaceID EIID_MaskFilter;

/**
 * MaskFilter is the base class for object that perform transformations on
 * an alpha-channel mask before drawing it. A subclass of MaskFilter may be
 * installed into a Paint. Blur and emboss are implemented as subclasses of MaskFilter.
 */
class MaskFilter
    : public Object
    , public IMaskFilter
{
    friend class Paint;

public:
    CAR_INTERFACE_DECL()

    virtual ~MaskFilter();

private:
    static CARAPI_(void) NativeDestructor(
        /* [in] */ Int64 nativeInstance);

protected:
    Int64 mNativeInstance;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_MASKFILTER_H__
