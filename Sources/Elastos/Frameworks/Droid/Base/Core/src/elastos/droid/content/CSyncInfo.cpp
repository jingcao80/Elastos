
#include "Elastos.Droid.Accounts.h"
#include "elastos/droid/content/CSyncInfo.h"
//#include "elastos/droid/accounts/CAccount.h"

//using Elastos::Droid::Accouts::CAccount;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_2(CSyncInfo, Object, ISyncInfo, IParcelable)

CAR_OBJECT_IMPL(CSyncInfo)

CSyncInfo::CSyncInfo()
    : mAuthorityId(0)
    , mStartTime(0)
{}

CSyncInfo::~CSyncInfo()
{}

ECode CSyncInfo::constructor()
{
    return NOERROR;
}

ECode CSyncInfo::constructor(
    /* [in] */ ISyncInfo* other)
{
    VALIDATE_NOT_NULL(other)
    CSyncInfo* si = (CSyncInfo*)other;

    mAuthorityId = si->mAuthorityId;
    AutoPtr<IAccount> oa;
    si->GetAccount((IAccount**)&oa);
    String name, type;
    oa->GetName(&name);
    oa->GetType(&type);
    assert(0 && "TODO");
    // CAccount::New(name, type, (IAccount**)&mAccount);
    mAuthority = si->mAuthority;
    mStartTime = si->mStartTime;
    return NOERROR;
}

ECode CSyncInfo::constructor(
    /* [in] */ Int32 authorityId,
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ Int64 startTime)
{
    mAuthorityId = authorityId;
    mAccount = account;
    mAuthority = authority;
    mStartTime = startTime;
    return NOERROR;
}

ECode CSyncInfo::GetAuthorityId(
    /* [out] */ Int32* authorityId)
{
    VALIDATE_NOT_NULL(authorityId)
    *authorityId = mAuthorityId;
    return NOERROR;
}

ECode CSyncInfo::SetAuthorityId(
    /* [in] */ Int32 authorityId)
{
    mAuthorityId = authorityId;
    return NOERROR;
}

ECode CSyncInfo::GetAccount(
    /* [out] */ IAccount** account)
{
    VALIDATE_NOT_NULL(account)
    *account = mAccount;
    REFCOUNT_ADD(*account);
    return NOERROR;
}

ECode CSyncInfo::SetAccount(
    /* [in] */ IAccount* account)
{
    mAccount = account;
    return NOERROR;
}

ECode CSyncInfo::GetAuthority(
    /* [out] */ String* authority)
{
    VALIDATE_NOT_NULL(authority)
    *authority = mAuthority;
    return NOERROR;
}

ECode CSyncInfo::SetAuthority(
    /* [in] */ const String& authority)
{
    mAuthority = authority;
    return NOERROR;
}

ECode CSyncInfo::GetStartTime(
    /* [out] */ Int64* startTime)
{
    VALIDATE_NOT_NULL(startTime)
    *startTime = mStartTime;
    return NOERROR;
}

ECode CSyncInfo::SetStartTime(
    /* [in] */ Int64 startTime)
{
    mStartTime = startTime;
    return NOERROR;
}

ECode CSyncInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mAuthorityId);
    assert(0 && "TODO");
    //CAccount::New((IAccount**)&mAccount);
    FAIL_RETURN(IParcelable::Probe(mAccount)->ReadFromParcel(source))
    source->ReadString(&mAuthority);
    source->ReadInt64(&mStartTime);
    return NOERROR;
}

ECode CSyncInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mAuthorityId);
    FAIL_RETURN(IParcelable::Probe(mAccount)->WriteToParcel(dest))
    dest->WriteString(mAuthority);
    dest->WriteInt64(mStartTime);
    return NOERROR;
}



}
}
}

