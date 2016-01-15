
#include "elastos/droid/content/res/CObbInfo.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL_2(CObbInfo, Object, IObbInfo, IParcelable)

CAR_OBJECT_IMPL(CObbInfo)

CObbInfo::CObbInfo()
    : mVersion(0)
    , mFlags(0)
{}

ECode CObbInfo::GetFilename(
    /* [out] */ String* filename)
{
    VALIDATE_NOT_NULL(filename)
    *filename = mFilename;
    return NOERROR;
}

ECode CObbInfo::SetFilename(
    /* [in] */ const String& filename)
{
    VALIDATE_NOT_NULL(filename)
    mFilename = filename;
    return NOERROR;
}

ECode CObbInfo::GetPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mPackageName;
    return NOERROR;
}

ECode CObbInfo::SetPackageName(
    /* [in] */ const String& name)
{
    mPackageName = name;
    return NOERROR;
}

ECode CObbInfo::GetVersion(
    /* [out] */ Int32* ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = mVersion;
    return NOERROR;
}

ECode CObbInfo::SetVersion(
    /* [in] */ Int32 ver)
{
    mVersion = ver;
    return NOERROR;
}

ECode CObbInfo::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags)
    *flags = mFlags;
    return NOERROR;
}

ECode CObbInfo::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode CObbInfo::GetSalt(
    /* [out, callee] */ ArrayOf<Byte>** salt)
{
    VALIDATE_NOT_NULL(salt)
    *salt = mSalt;
    REFCOUNT_ADD(*salt)
    return NOERROR;
}

ECode CObbInfo::SetSalt(
    /* [in] */ ArrayOf<Byte>* salt)
{
    mSalt = salt;
    return NOERROR;
}

ECode CObbInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadString(&mFilename))
    FAIL_RETURN(source->ReadString(&mPackageName))
    FAIL_RETURN(source->ReadInt32(&mVersion))
    FAIL_RETURN(source->ReadInt32(&mFlags))
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&mSalt))
    return NOERROR;
}

ECode CObbInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteString(mFilename))
    FAIL_RETURN(dest->WriteString(mPackageName))
    FAIL_RETURN(dest->WriteInt32(mVersion))
    FAIL_RETURN(dest->WriteInt32(mFlags))
    FAIL_RETURN(dest->WriteArrayOf((Handle32)mSalt.Get()))
    return NOERROR;
}

ECode CObbInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    Int32 hash;
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->IdentityHashCode(TO_IINTERFACE(this), &hash);
    StringBuilder sb;
    sb.Append("ObbInfo{");
    sb.Append(StringUtils::ToHexString(hash));
    sb.Append(" packageName=");
    sb.Append(mPackageName);
    sb.Append(",version=");
    sb.Append(mVersion);
    sb.Append(",flags=");
    sb.Append(mFlags);
    sb.AppendChar('}');
    return sb.ToString(str);
}

ECode CObbInfo::constructor()
{
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos
