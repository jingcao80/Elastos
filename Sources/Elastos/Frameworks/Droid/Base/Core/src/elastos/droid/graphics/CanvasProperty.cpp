
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/NativePaint.h"
#include "elastos/droid/graphics/CanvasProperty.h"
#include "elastos/droid/graphics/Paint.h"
#include "elastos/droid/internal/utility/CVirtualRefBasePtr.h"

using Elastos::Droid::Internal::Utility::CVirtualRefBasePtr;

namespace Elastos {
namespace Droid {
namespace Graphics {

class CanvasPropertyPaint : public VirtualLightRefBase {
    PREVENT_COPY_AND_ASSIGN(CanvasPropertyPaint);
public:
    CanvasPropertyPaint(const SkPaint& initialValue) : value(initialValue) {}

    SkPaint value;
};

CAR_INTERFACE_IMPL(CanvasProperty, Object, ICanvasProperty);
AutoPtr<CanvasProperty> CanvasProperty::CreateFloat(
    /* [in] */ Float initialValue)
{
    return new CanvasProperty(nCreateFloat(initialValue));
}

AutoPtr<CanvasProperty> CanvasProperty::CreatePaint(
    /* [in] */ IPaint* initialValue)
{
    Paint* paint = (Paint*)initialValue;
    return new CanvasProperty(nCreatePaint(paint->mNativePaint));
}

ECode CanvasProperty::GetNativeContainer(
    /* [out] */ Int64* native)
{
    VALIDATE_NOT_NULL(native);
    return mProperty->Get(native);
}

CanvasProperty::CanvasProperty(
    /* [in] */ Int64 nativeContainer)
{
    CVirtualRefBasePtr::New(nativeContainer, (IVirtualRefBasePtr**)&mProperty);
}

Int64 CanvasProperty::nCreateFloat(
    /* [in] */ Float initialValue)
{
    return reinterpret_cast<Int64>(new CanvasPropertyPrimitive(initialValue));
}

Int64 CanvasProperty::nCreatePaint(
    /* [in] */ Int64 initialValuePaintPtr)
{
    const NativePaint* paint = reinterpret_cast<const NativePaint*>(initialValuePaintPtr);
    return reinterpret_cast<Int64>(new CanvasPropertyPaint(*paint));
}

} // namespace Graphics
} // namespace Droid
} // namespace Elastos
