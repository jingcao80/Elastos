
#include "elastos/droid/app/backup/CRestoreDescription.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

static AutoPtr<IRestoreDescription> NO_MORE_PACKAGES_init()
{
    AutoPtr<IRestoreDescription> res;
    CRestoreDescription::New(CRestoreDescription::NO_MORE_PACKAGES_SENTINEL, 0, (IRestoreDescription**)&res);
    return res;
}

AutoPtr<IRestoreDescription> CRestoreDescription::NO_MORE_PACKAGES = NO_MORE_PACKAGES_init();

const String CRestoreDescription::NO_MORE_PACKAGES_SENTINEL("");

CAR_INTERFACE_IMPL_2(CRestoreDescription, Object, IRestoreDescription, IParcelable)

CAR_OBJECT_IMPL(CRestoreDescription)

CRestoreDescription::CRestoreDescription()
    : mDataType(0)
{
}

CRestoreDescription::CRestoreDescription(
    /* [in] */ IParcel* in)
{
    in->ReadString(&mPackageName);
    in->ReadInt32(&mDataType);
}

ECode CRestoreDescription::constructor(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 dataType)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CRestoreDescription::GetPackageName(
    /* [out] */ String* name)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CRestoreDescription::GetDataType(
    /* [out] */ Int32* type)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode CRestoreDescription::ToString(
    /* [out] */ String* str)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

// ---------------------------------------
// Parcelable implementation - not used by transport

//@Override
ECode CRestoreDescription::DescribeContents(
    /* [out] */ Int32* contents)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode CRestoreDescription::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int32 flags)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CRestoreDescription::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CRestoreDescription::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
