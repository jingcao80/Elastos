
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/CPackageCleanItem.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(CPackageCleanItem, Object, IPackageCleanItem, IParcelable)

CAR_OBJECT_IMPL(CPackageCleanItem)

CPackageCleanItem::CPackageCleanItem()
    : mUserId(0)
    , mAndCode(FALSE)
{}

CPackageCleanItem::~CPackageCleanItem()
{}

ECode CPackageCleanItem::constructor()
{
    return NOERROR;
}

ECode CPackageCleanItem::constructor(
    /* [in] */ Int32 userId,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean andCode)
{
    mUserId = userId;
    mPackageName = packageName;
    mAndCode = andCode;
    return NOERROR;
}

ECode CPackageCleanItem::Equals(
    /* [in] */ IPackageCleanItem* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FALSE;

    if (this == obj) {
        *result = TRUE;
        return NOERROR;
    }

    // try {
    if (obj != NULL) {
        Int32 userId;
        obj->GetUserId(&userId);
        String packageName;
        obj->GetPackageName(&packageName);
        Boolean andCode;
        obj->GetAndCode(&andCode);

        *result = (userId == mUserId && packageName.Equals(mPackageName) && andCode == mAndCode);
    }
    // } catch (ClassCastException e) {
    // }

    return NOERROR;
}

ECode CPackageCleanItem::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return Equals(IPackageCleanItem::Probe(obj), result);
}

ECode CPackageCleanItem::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);
    // int result = 17;
    //     result = 31 * result + userId;
    //     result = 31 * result + packageName.hashCode();
    //     result = 31 * result + (andCode ? 1 : 0);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CPackageCleanItem::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(source != NULL);
    source->ReadInt32(&mUserId);
    source->ReadString(&mPackageName);
    source->ReadBoolean(&mAndCode);
    return NOERROR;
}

ECode CPackageCleanItem::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(dest != NULL);
    dest->WriteInt32(mUserId);
    dest->WriteString(mPackageName);
    dest->WriteBoolean(mAndCode);
    return NOERROR;
}

ECode CPackageCleanItem::GetUserId(
    /* [out] */ Int32* userId)
{
    VALIDATE_NOT_NULL(userId);
    *userId = mUserId;
    return NOERROR;
}

ECode CPackageCleanItem::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = mPackageName;
    return NOERROR;
}

ECode CPackageCleanItem::GetAndCode(
    /* [out] */ Boolean* andCode)
{
    VALIDATE_NOT_NULL(andCode);
    *andCode = mAndCode;
    return NOERROR;
}


ECode CPackageCleanItem::SetUserId(
    /* [in] */ Int32 userId)
{
    mUserId = userId;
    return NOERROR;
}

ECode CPackageCleanItem::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode CPackageCleanItem::SetAndCode(
    /* [in] */ Boolean andCode)
{
    mAndCode = andCode;
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
