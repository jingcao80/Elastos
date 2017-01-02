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

#ifndef __ELASTOS_DROID_GRAPHICS_CANVASPROPERTY_H__
#define __ELASTOS_DROID_GRAPHICS_CANVASPROPERTY_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <utils/RefBase.h>

using Elastos::Droid::Internal::Utility::IVirtualRefBasePtr;
using android::VirtualLightRefBase;

namespace Elastos {
namespace Droid {
namespace Graphics {

class CanvasPropertyPrimitive : public VirtualLightRefBase {
    PREVENT_COPY_AND_ASSIGN(CanvasPropertyPrimitive);
public:
    CanvasPropertyPrimitive(Float initialValue) : value(initialValue) {}

    Float value;
};

/**
 * TODO: Make public?
 * @hide
 */
class ECO_PUBLIC CanvasProperty/*<T>*/
    : public Object
    , public ICanvasProperty
{
public:
    CAR_INTERFACE_DECL()

    static AutoPtr<CanvasProperty> CreateFloat(
        /* [in] */ Float initialValue);

    static AutoPtr<CanvasProperty> CreatePaint(
        /* [in] */ IPaint* initialValue);

    /** @hide */
    CARAPI GetNativeContainer(
        /* [out] */ Int64* native);

private:
    CanvasProperty(
        /* [in] */ Int64 nativeContainer);

    static Int64 nCreateFloat(
        /* [in] */ Float initialValue);

    static Int64 nCreatePaint(
        /* [in] */ Int64 initialValuePaintPtr);

private:
    AutoPtr<IVirtualRefBasePtr> mProperty;
};

} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CANVASPROPERTY_H__
