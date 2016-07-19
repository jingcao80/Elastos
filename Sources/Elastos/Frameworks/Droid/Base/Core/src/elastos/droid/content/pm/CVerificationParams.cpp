
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/pm/CVerificationParams.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(CVerificationParams, Object, IVerificationParams, IParcelable)

CAR_OBJECT_IMPL(CVerificationParams)

CVerificationParams::CVerificationParams()
    : mOriginatingUid(0)
    , mInstallerUid(0)
{}

CVerificationParams::~CVerificationParams()
{}

ECode CVerificationParams::constructor()
{
    return NOERROR;
}

ECode CVerificationParams::constructor(
    /* [in] */ Elastos::Droid::Net::IUri* verificationURI,
    /* [in] */ Elastos::Droid::Net::IUri* originatingURI,
    /* [in] */ Elastos::Droid::Net::IUri* referrer,
    /* [in] */ Int32 originatingUid,
    /* [in] */ Elastos::Droid::Content::Pm::IManifestDigest* manifestDigest)
{
    mVerificationURI = verificationURI;
    mOriginatingURI = originatingURI;
    mReferrer = referrer;
    mOriginatingUid = originatingUid;
    mManifestDigest = manifestDigest;
    mInstallerUid = IVerificationParams::NO_UID;

    return NOERROR;
}

ECode CVerificationParams::constructor(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    source->ReadInterfacePtr((Handle32*)&mVerificationURI);
    source->ReadInterfacePtr((Handle32*)&mOriginatingURI);
    source->ReadInterfacePtr((Handle32*)&mReferrer);
    source->ReadInt32(&mOriginatingUid);
    source->ReadInt32(&mInstallerUid);
    source->ReadInterfacePtr((Handle32*)&mManifestDigest);

    return NOERROR;
}

ECode CVerificationParams::GetVerificationURI(
    /* [out] */ Elastos::Droid::Net::IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = mVerificationURI;
    return NOERROR;
}

ECode CVerificationParams::GetOriginatingURI(
    /* [out] */ Elastos::Droid::Net::IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = mOriginatingURI;
    return NOERROR;
}

ECode CVerificationParams::GetReferrer(
    /* [out] */ Elastos::Droid::Net::IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = mReferrer;
    return NOERROR;
}

ECode CVerificationParams::GetOriginatingUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);
    *uid = mOriginatingUid;
    return NOERROR;
}

ECode CVerificationParams::GetManifestDigest(
    /* [out] */ Elastos::Droid::Content::Pm::IManifestDigest** digest)
{
    VALIDATE_NOT_NULL(digest);
    *digest = mManifestDigest;
    REFCOUNT_ADD(*digest);
    return NOERROR;
}

ECode CVerificationParams::GetInstallerUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);
    *uid = mInstallerUid;
    return NOERROR;
}

ECode CVerificationParams::SetInstallerUid(
    /* [in] */ Int32 uid)
{
    mInstallerUid = uid;
    return NOERROR;
}

ECode CVerificationParams::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* isEquals)
{
    VALIDATE_NOT_NULL(isEquals)

    if ((IObject*)this == IObject::Probe(o)) {
        *isEquals = TRUE;
        return NOERROR;
    }

    if (IVerificationParams::Probe(o) == NULL) {
        *isEquals = FALSE;
        return NOERROR;
    }

    CVerificationParams* other = (CVerificationParams*)IVerificationParams::Probe(o);

    if (mVerificationURI == NULL) {
        if (other->mVerificationURI != NULL) {
            *isEquals = FALSE;
            return NOERROR;
        }
    }
    else if (!Object::Equals(mVerificationURI, other->mVerificationURI)) {
        *isEquals = FALSE;
        return NOERROR;
    }

    if (mOriginatingURI == NULL) {
        if (other->mOriginatingURI != NULL) {
            *isEquals = FALSE;
            return NOERROR;
        }
    }
    else if (!Object::Equals(mOriginatingURI, other->mOriginatingURI)) {
        *isEquals = FALSE;
        return NOERROR;
    }

    if (mReferrer == NULL) {
        if (other->mReferrer != NULL) {
            *isEquals = FALSE;
            return NOERROR;
        }
    }
    else if (!Object::Equals(mReferrer, other->mReferrer)) {
        *isEquals = FALSE;
        return NOERROR;
    }

    if (mOriginatingUid != other->mOriginatingUid) {
        *isEquals = FALSE;
        return NOERROR;
    }

    if (mManifestDigest == NULL) {
        if (other->mManifestDigest != NULL) {
            *isEquals = FALSE;
            return NOERROR;
        }
    }
    else if (!Object::Equals(mManifestDigest, other->mManifestDigest)) {
        *isEquals = FALSE;
        return NOERROR;
    }

    if (mInstallerUid != other->mInstallerUid) {
        *isEquals = FALSE;
        return NOERROR;
    }

    *isEquals = TRUE;
    return NOERROR;
}

ECode CVerificationParams::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    Int32 hash = 3;
    Int32 temp;
    hash += 5 * (mVerificationURI == NULL ? 1 : (IObject::Probe(mVerificationURI)->GetHashCode(&temp), temp));
    hash += 7 * (mOriginatingURI == NULL ? 1 : (IObject::Probe(mOriginatingURI)->GetHashCode(&temp), temp));
    hash += 11 * (mReferrer == NULL ? 1 : (IObject::Probe(mReferrer)->GetHashCode(&temp), temp));
    hash += 13 * mOriginatingUid;
    hash += 17 * (mManifestDigest == NULL ? 1 : (IObject::Probe(mManifestDigest)->GetHashCode(&temp), temp));
    hash += 19 * mInstallerUid;

    *hashCode = hash;
    return NOERROR;
}

ECode CVerificationParams::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("VerificationParams{");

    sb.Append("mVerificationURI=");
    sb.Append(TO_STR(mVerificationURI));
    sb.Append(",mOriginatingURI=");
    sb.Append(TO_STR(mOriginatingURI));
    sb.Append(",mReferrer=");
    sb.Append(TO_STR(mReferrer));
    sb.Append(",mOriginatingUid=");
    sb.Append(mOriginatingUid);
    sb.Append(",mManifestDigest=");
    sb.Append(TO_STR(mManifestDigest));
    sb.Append(",mInstallerUid=");
    sb.Append(mInstallerUid);
    sb.AppendChar('}');

    return sb.ToString(str);
}

ECode CVerificationParams::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    mVerificationURI = NULL;
    source->ReadInterfacePtr((Handle32*)&mVerificationURI);
    mOriginatingURI = NULL;
    source->ReadInterfacePtr((Handle32*)&mOriginatingURI);
    mReferrer = NULL;
    source->ReadInterfacePtr((Handle32*)&mReferrer);
    source->ReadInt32(&mOriginatingUid);
    source->ReadInt32(&mInstallerUid);
    mManifestDigest = NULL;
    source->ReadInterfacePtr((Handle32*)&mManifestDigest);

    return NOERROR;
}

ECode CVerificationParams::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    dest->WriteInterfacePtr(mVerificationURI);
    dest->WriteInterfacePtr(mOriginatingURI);
    dest->WriteInterfacePtr(mReferrer);
    dest->WriteInt32(mOriginatingUid);
    dest->WriteInt32(mInstallerUid);
    dest->WriteInterfacePtr(mManifestDigest);

    return NOERROR;
}

}
}
}
}

