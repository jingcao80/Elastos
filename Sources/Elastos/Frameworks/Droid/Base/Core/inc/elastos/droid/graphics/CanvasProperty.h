
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
    CAR_INTERFACE_DECL();

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
