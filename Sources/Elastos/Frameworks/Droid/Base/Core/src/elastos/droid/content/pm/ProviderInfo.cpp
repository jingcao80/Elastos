#include "elastos/droid/content/pm/ProviderInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(ProviderInfo, ComponentInfo, IProviderInfo, IParcelable)

ProviderInfo::ProviderInfo()
    : mGrantUriPermissions(FALSE)
    , mMultiprocess(FALSE)
    , mInitOrder(0)
    , mFlags(0)
    , mIsSyncable(FALSE)
{}

ProviderInfo::~ProviderInfo()
{}

ECode ProviderInfo::constructor()
{
    return ComponentInfo::constructor();
}

ECode ProviderInfo::constructor(
    /* [in] */ IProviderInfo* orig)
{
    VALIDATE_NOT_NULL(orig)
    FAIL_RETURN(ComponentInfo::constructor(IComponentInfo::Probe(orig)))

    orig->GetAuthority(&mAuthority);
    orig->GetReadPermission(&mReadPermission);
    orig->GetWritePermission(&mWritePermission);
    orig->GetGrantUriPermissions(&mGrantUriPermissions);
    orig->GetUriPermissionPatterns((ArrayOf<IPatternMatcher*>**)&mUriPermissionPatterns);
    orig->GetPathPermissions((ArrayOf<IPathPermission*>**)&mPathPermissions);
    orig->GetMultiprocess(&mMultiprocess);
    orig->GetInitOrder(&mInitOrder);
    orig->GetFlags(&mFlags);
    orig->GetIsSyncable(&mIsSyncable);
    return NOERROR;
}

ECode ProviderInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    ComponentInfo::ReadFromParcel(source);
    source->ReadString(&mAuthority);
    source->ReadString(&mReadPermission);
    source->ReadString(&mWritePermission);
    source->ReadBoolean(&mGrantUriPermissions);
    source->ReadArrayOf((Handle32*)&mUriPermissionPatterns);
    source->ReadArrayOf((Handle32*)&mPathPermissions);
    source->ReadBoolean(&mMultiprocess);
    source->ReadInt32(&mInitOrder);
    source->ReadInt32(&mFlags);
    source->ReadBoolean(&mIsSyncable);
    return NOERROR;
}

ECode ProviderInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    ComponentInfo::WriteToParcel(dest);
    dest->WriteString(mAuthority);
    dest->WriteString(mReadPermission);
    dest->WriteString(mWritePermission);
    dest->WriteBoolean(mGrantUriPermissions);
    dest->WriteArrayOf((Handle32)mUriPermissionPatterns.Get());
    dest->WriteArrayOf((Handle32)mPathPermissions.Get());
    dest->WriteBoolean(mMultiprocess);
    dest->WriteInt32(mInitOrder);
    dest->WriteInt32(mFlags);
    dest->WriteBoolean(mIsSyncable);
    return NOERROR;
}

ECode ProviderInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    // return "ContentProviderInfo{name=" + authority + " className=" + name + "}";
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode ProviderInfo::GetAuthority(
    /* [out] */ String* authority)
{
    VALIDATE_NOT_NULL(authority);
    *authority = mAuthority;
    return NOERROR;
}

ECode ProviderInfo::SetAuthority(
    /* [in] */ const String& authority)
{
    mAuthority = authority;
    return NOERROR;
}

ECode ProviderInfo::GetReadPermission(
    /* [out] */ String* readPermission)
{
    VALIDATE_NOT_NULL(readPermission);
    *readPermission = mReadPermission;
    return NOERROR;
}

ECode ProviderInfo::SetReadPermission(
    /* [in] */ const String& readPermission)
{
    mReadPermission = readPermission;
    return NOERROR;
}

ECode ProviderInfo::GetWritePermission(
    /* [out] */ String* writePermission)
{
    VALIDATE_NOT_NULL(writePermission);
    *writePermission = mWritePermission;
    return NOERROR;
}

ECode ProviderInfo::SetWritePermission(
    /* [in] */ const String& writePermission)
{
    mWritePermission = writePermission;
    return NOERROR;
}

ECode ProviderInfo::GetGrantUriPermissions(
    /* [out] */ Boolean* grantUriPermissions)
{
    VALIDATE_NOT_NULL(grantUriPermissions);
    *grantUriPermissions = mGrantUriPermissions;
    return NOERROR;
}

ECode ProviderInfo::SetGrantUriPermissions(
    /* [in] */ Boolean grantUriPermissions)
{
    mGrantUriPermissions = grantUriPermissions;
    return NOERROR;
}

ECode ProviderInfo::GetUriPermissionPatterns(
    /* [out, callee] */ ArrayOf<IPatternMatcher*>** patterns)
{
    VALIDATE_NOT_NULL(patterns);
    *patterns = mUriPermissionPatterns;
    REFCOUNT_ADD(*patterns);
    return NOERROR;
}

ECode ProviderInfo::SetUriPermissionPatterns(
    /* [in] */ ArrayOf<IPatternMatcher*>* patterns)
{
    mUriPermissionPatterns = patterns;
    return NOERROR;
}

ECode ProviderInfo::GetPathPermissions(
    /* [out, callee] */ ArrayOf<IPathPermission*>** pathPermissions)
{
    VALIDATE_NOT_NULL(pathPermissions);
    *pathPermissions = mPathPermissions;
    REFCOUNT_ADD(*pathPermissions);
    return NOERROR;
}

ECode ProviderInfo::SetPathPermissions(
    /* [in] */ ArrayOf<IPathPermission*>* pathPermissions)
{
    mPathPermissions = pathPermissions;
    return NOERROR;
}

ECode ProviderInfo::GetMultiprocess(
    /* [out] */ Boolean* multiprocess)
{
    VALIDATE_NOT_NULL(multiprocess);
    *multiprocess = mMultiprocess;
    return NOERROR;
}

ECode ProviderInfo::SetMultiprocess(
    /* [in] */ Boolean multiprocess)
{
    mMultiprocess = multiprocess;
    return NOERROR;
}

ECode ProviderInfo::GetInitOrder(
    /* [out] */ Int32* initOrder)
{
    VALIDATE_NOT_NULL(initOrder);
    *initOrder = mInitOrder;
    return NOERROR;
}

ECode ProviderInfo::SetInitOrder(
    /* [in] */ Int32 initOrder)
{
    mInitOrder = initOrder;
    return NOERROR;
}

ECode ProviderInfo::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

ECode ProviderInfo::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode ProviderInfo::GetIsSyncable(
    /* [out] */ Boolean* isSyncable)
{
    VALIDATE_NOT_NULL(isSyncable);
    *isSyncable = mIsSyncable;
    return NOERROR;
}

ECode ProviderInfo::SetIsSyncable(
    /* [in] */ Boolean isSyncable)
{
    mIsSyncable = isSyncable;
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
