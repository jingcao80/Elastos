
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/pm/CVerificationParams.h"

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
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEquals)
{
    // if (this == o) {
    //     return true;
    // }

    // if (!(o instanceof VerificationParams)) {
    //     return false;
    // }

    // final VerificationParams other = (VerificationParams) o;

    // if (mVerificationURI == null) {
    //     if (other.mVerificationURI != null) {
    //         return false;
    //     }
    // } else if (!mVerificationURI.equals(other.mVerificationURI)) {
    //     return false;
    // }

    // if (mOriginatingURI == null) {
    //     if (other.mOriginatingURI != null) {
    //         return false;
    //     }
    // } else if (!mOriginatingURI.equals(other.mOriginatingURI)) {
    //     return false;
    // }

    // if (mReferrer == null) {
    //     if (other.mReferrer != null) {
    //         return false;
    //     }
    // } else if (!mReferrer.equals(other.mReferrer)) {
    //     return false;
    // }

    // if (mOriginatingUid != other.mOriginatingUid) {
    //     return false;
    // }

    // if (mManifestDigest == null) {
    //     if (other.mManifestDigest != null) {
    //         return false;
    //     }
    // } else if (!mManifestDigest.equals(other.mManifestDigest)) {
    //     return false;
    // }

    // if (mInstallerUid != other.mInstallerUid) {
    //     return false;
    // }

    // return true;
    return E_NOT_IMPLEMENTED;
}

ECode CVerificationParams::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CVerificationParams::ToString(
    /* [out] */ String* str)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
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

