
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CPorterDuffColorFilter.h"
#include <skia/core/SkColorFilter.h>
#include <skia/effects/SkPorterDuff.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CPorterDuffColorFilter);
CAR_INTERFACE_IMPL(CPorterDuffColorFilter, ColorFilter, IPorterDuffColorFilter);
CPorterDuffColorFilter::CPorterDuffColorFilter()
    : mColor(0)
    , mMode(-1)
{}

ECode CPorterDuffColorFilter::constructor(
    /* [in] */ Int32 srcColor,
    /* [in] */ PorterDuffMode mode)
{
    mColor = srcColor;
    mMode = mode;
    Update();
    return NOERROR;
}

ECode CPorterDuffColorFilter::GetColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    *color = mColor;
    return NOERROR;
}

ECode CPorterDuffColorFilter::SetColor(
    /* [in] */ Int32 color)
{
    mColor = color;
    Update();
    return NOERROR;
}

ECode CPorterDuffColorFilter::GetMode(
    /* [out] */ PorterDuffMode* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mMode;
    return NOERROR;
}

ECode CPorterDuffColorFilter::SetMode(
    /* [in] */ PorterDuffMode mode)
{
    mMode = mode;
    Update();
    return NOERROR;
}

void CPorterDuffColorFilter::Update()
{
    DestroyFilter(mNativeInstance);
    mNativeInstance = NativeCreatePorterDuffFilter(mColor, mMode);
}

ECode CPorterDuffColorFilter::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* equals)
{
    VALIDATE_NOT_NULL(equals);
    *equals = TRUE;
    if (TO_IINTERFACE(this) == IInterface::Probe(object)) {
        return NOERROR;
    }
    ClassID id1, id2;
    GetClassID(&id1);
    if (object == NULL || id1 != (IObject::Probe(object)->GetClassID(&id2), id2)) {
        *equals = FALSE;
        return NOERROR;
    }
    AutoPtr<IPorterDuffColorFilter> other = IPorterDuffColorFilter::Probe(object);
    if (mColor != ((CPorterDuffColorFilter*)other.Get())->mColor || mMode != ((CPorterDuffColorFilter*)other.Get())->mMode) {
        *equals = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode CPorterDuffColorFilter::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);
    // return 31 *  mMode.hashCode() + mColor;
    *code = 31 *  mMode + mColor;
    return NOERROR;
}

Int64 CPorterDuffColorFilter::NativeCreatePorterDuffFilter(
    /* [in] */ Int32 srcColor,
    /* [in] */ Int32 porterDuffMode)
{
    SkPorterDuff::Mode mode = (SkPorterDuff::Mode) porterDuffMode;
    return reinterpret_cast<Int64>(SkColorFilter::CreateModeFilter(srcColor, SkPorterDuff::ToXfermodeMode(mode)));
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
