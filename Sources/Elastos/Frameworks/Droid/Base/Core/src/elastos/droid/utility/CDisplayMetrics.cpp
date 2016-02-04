
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/os/SystemProperties.h"

using Elastos::Droid::Os::SystemProperties;

namespace Elastos {
namespace Droid {
namespace Utility {

static Int32 InitDENSITY_DEVICE()
{
    Int32 ro;
    SystemProperties::GetInt32(String("ro.sf.lcd_density"),
            IDisplayMetrics::DENSITY_DEFAULT, &ro);
    Int32 result;
    SystemProperties::GetInt32(String("qemu.sf.lcd_density"), ro, &result);
    return result;
}

Int32 CDisplayMetrics::DENSITY_DEVICE = InitDENSITY_DEVICE();

CAR_INTERFACE_IMPL(CDisplayMetrics, Object, IDisplayMetrics)

CAR_OBJECT_IMPL(CDisplayMetrics)

CDisplayMetrics::CDisplayMetrics()
    : mWidthPixels(0)
    , mHeightPixels(0)
    , mDensity(0.0f)
    , mDensityDpi(0)
    , mScaledDensity(0.0f)
    , mXdpi(0.0f)
    , mYdpi(0.0f)
    , mNoncompatWidthPixels(0)
    , mNoncompatHeightPixels(0)
    , mNoncompatDensity(0.0f)
    , mNoncompatDensityDpi(0)
    , mNoncompatScaledDensity(0.0f)
    , mNoncompatXdpi(0.0f)
    , mNoncompatYdpi(0.0f)
{}

ECode CDisplayMetrics::SetTo(
    /* [in] */ IDisplayMetrics *o)
{
    assert(o != NULL);

    o->GetWidthPixels(&mWidthPixels);
    o->GetHeightPixels(&mHeightPixels);
    o->GetDensity(&mDensity);
    o->GetDensityDpi(&mDensityDpi);
    o->GetScaledDensity(&mScaledDensity);
    o->GetXdpi(&mXdpi);
    o->GetYdpi(&mYdpi);
    o->GetNoncompatWidthPixels(&mNoncompatWidthPixels);
    o->GetNoncompatHeightPixels(&mNoncompatHeightPixels);
    o->GetNoncompatDensity(&mNoncompatDensity);
    o->GetNoncompatDensityDpi(&mNoncompatDensityDpi);
    o->GetNoncompatScaledDensity(&mNoncompatScaledDensity);
    o->GetNoncompatXdpi(&mNoncompatXdpi);
    o->GetNoncompatYdpi(&mNoncompatYdpi);
    return NOERROR;
}

ECode CDisplayMetrics::SetToDefaults()
{
    mWidthPixels = 0;
    mHeightPixels = 0;
    mDensity = DENSITY_DEVICE / (Float) DENSITY_DEFAULT;
    mDensityDpi = DENSITY_DEVICE;
    mScaledDensity = mDensity;
    mXdpi = DENSITY_DEVICE;
    mYdpi = DENSITY_DEVICE;
    mNoncompatWidthPixels = mWidthPixels;
    mNoncompatHeightPixels = mHeightPixels;
    mNoncompatDensity = mDensity;
    mNoncompatDensityDpi = mDensityDpi;
    mNoncompatScaledDensity = mScaledDensity;
    mNoncompatXdpi = mXdpi;
    mNoncompatYdpi = mYdpi;
    return NOERROR;
}

ECode CDisplayMetrics::Equals(
    /* [in] */ IDisplayMetrics* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    CDisplayMetrics* otherObj = other != NULL ? (CDisplayMetrics*)other : NULL;
    EqualsPhysical(other, result);
    *result = *result
            && mScaledDensity == otherObj->mScaledDensity
            && mNoncompatScaledDensity == otherObj->mNoncompatScaledDensity;
    return NOERROR;
}

ECode CDisplayMetrics::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    return Equals(IDisplayMetrics::Probe(other), result);
}

ECode CDisplayMetrics::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = mWidthPixels * mHeightPixels * mDensityDpi;;
    return NOERROR;
}

ECode CDisplayMetrics::EqualsPhysical(
    /* [in] */ IDisplayMetrics* other,
    /* [out] */ Boolean* result)
{
    CDisplayMetrics* otherObj = other != NULL ? (CDisplayMetrics*)other : NULL;
    *result = otherObj != NULL
            && mWidthPixels == otherObj->mWidthPixels
            && mHeightPixels == otherObj->mHeightPixels
            && mDensity == otherObj->mDensity
            && mDensityDpi == otherObj->mDensityDpi
            && mXdpi == otherObj->mXdpi
            && mYdpi == otherObj->mYdpi
            && mNoncompatWidthPixels == otherObj->mNoncompatWidthPixels
            && mNoncompatHeightPixels == otherObj->mNoncompatHeightPixels
            && mNoncompatDensity == otherObj->mNoncompatDensity
            && mNoncompatDensityDpi == otherObj->mNoncompatDensityDpi
            && mNoncompatXdpi == otherObj->mNoncompatXdpi
            && mNoncompatYdpi == otherObj->mNoncompatYdpi;
    return NOERROR;
}

Int32 CDisplayMetrics::GetDeviceDensity()
{
    // qemu.sf.lcd_density can be used to override ro.sf.lcd_density
    // when running in the emulator, allowing for dynamic configurations.
    // The reason for this is that ro.sf.lcd_density is write-once and is
    // set by the init process when it parses build.prop before anything else.
    return InitDENSITY_DEVICE();
}

ECode CDisplayMetrics::GetWidthPixels(
    /* [out] */ Int32 *value)
{
    VALIDATE_NOT_NULL(value);
    *value = mWidthPixels;
    return NOERROR;
}

ECode CDisplayMetrics::SetWidthPixels(
    /* [in] */ Int32 value)
{
    mWidthPixels = value;
    return NOERROR;
}

ECode CDisplayMetrics::GetHeightPixels(
    /* [out] */ Int32 *value)
{
    VALIDATE_NOT_NULL(value);
    *value = mHeightPixels;
    return NOERROR;
}

ECode CDisplayMetrics::SetHeightPixels(
    /* [in] */ Int32 value)
{
    mHeightPixels = value;
    return NOERROR;
}

ECode CDisplayMetrics::GetDensity(
    /* [out] */ Float *density)
{
    VALIDATE_NOT_NULL(density);
    *density = mDensity;
    return NOERROR;
}

ECode CDisplayMetrics::SetDensity(
    /* [in] */ Float density)
{
    mDensity = density;
    return NOERROR;
}

ECode CDisplayMetrics::GetDensityDpi(
    /* [out] */ Int32 *value)
{
    VALIDATE_NOT_NULL(value);
    *value = mDensityDpi;
    return NOERROR;
}

ECode CDisplayMetrics::SetDensityDpi(
    /* [in] */ Int32 value)
{
    mDensityDpi = value;
    return NOERROR;
}

ECode CDisplayMetrics::GetScaledDensity(
    /* [out] */ Float *value)
{
    VALIDATE_NOT_NULL(value);
    *value = mScaledDensity;
    return NOERROR;
}

ECode CDisplayMetrics::SetScaledDensity(
    /* [in] */ Float value)
{
    mScaledDensity = value;
    return NOERROR;
}

ECode CDisplayMetrics::GetXdpi(
    /* [out] */ Float *value)
{
    VALIDATE_NOT_NULL(value);
    *value = mXdpi;
    return NOERROR;
}

ECode CDisplayMetrics::SetXdpi(
    /* [in] */ Float value)
{
    mXdpi = value;
    return NOERROR;
}

ECode CDisplayMetrics::GetYdpi(
    /* [out] */ Float *value)
{
    VALIDATE_NOT_NULL(value);
    *value = mYdpi;
    return NOERROR;
}

ECode CDisplayMetrics::SetYdpi(
    /* [in] */ Float value)
{
    mYdpi = value;
    return NOERROR;
}

ECode CDisplayMetrics::GetNoncompatWidthPixels(
    /* [out] */ Int32 *value)
{
    VALIDATE_NOT_NULL(value);
    *value = mNoncompatWidthPixels;
    return NOERROR;
}

ECode CDisplayMetrics::SetNoncompatWidthPixels(
    /* [in] */ Int32 value)
{
    mNoncompatWidthPixels = value;
    return NOERROR;
}

ECode CDisplayMetrics::GetNoncompatHeightPixels(
    /* [out] */ Int32 *value)
{
    VALIDATE_NOT_NULL(value);
    *value = mNoncompatHeightPixels;
    return NOERROR;
}

ECode CDisplayMetrics::SetNoncompatHeightPixels(
    /* [in] */ Int32 value)
{
    mNoncompatHeightPixels = value;
    return NOERROR;
}

ECode CDisplayMetrics::GetNoncompatDensity(
    /* [out] */ Float *value)
{
    VALIDATE_NOT_NULL(value);
    *value = mNoncompatDensity;
    return NOERROR;
}

ECode CDisplayMetrics::SetNoncompatDensity(
    /* [in] */ Float value)
{
    mNoncompatDensity = value;
    return NOERROR;
}

ECode CDisplayMetrics::GetNoncompatDensityDpi(
    /* [out] */ Int32 *value)
{
    VALIDATE_NOT_NULL(value);
    *value = mNoncompatDensityDpi;
    return NOERROR;
}

ECode CDisplayMetrics::SetNoncompatDensityDpi(
    /* [in] */ Int32 value)
{
    mNoncompatDensityDpi = value;
    return NOERROR;
}

ECode CDisplayMetrics::GetNoncompatScaledDensity(
    /* [out] */ Float *value)
{
    VALIDATE_NOT_NULL(value);
    *value = mNoncompatScaledDensity;
    return NOERROR;
}

ECode CDisplayMetrics::SetNoncompatScaledDensity(
    /* [in] */ Float value)
{
    mNoncompatScaledDensity = value;
    return NOERROR;
}

ECode CDisplayMetrics::GetNoncompatXdpi(
    /* [out] */ Float *value)
{
    VALIDATE_NOT_NULL(value);
    *value = mNoncompatXdpi;
    return NOERROR;
}

ECode CDisplayMetrics::SetNoncompatXdpi(
    /* [in] */ Float value)
{
    mNoncompatXdpi = value;
    return NOERROR;
}

ECode CDisplayMetrics::GetNoncompatYdpi(
    /* [out] */ Float *value)
{
    VALIDATE_NOT_NULL(value);
    *value = mNoncompatYdpi;
    return NOERROR;
}

ECode CDisplayMetrics::SetNoncompatYdpi(
    /* [in] */ Float value)
{
    mNoncompatYdpi = value;
    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
