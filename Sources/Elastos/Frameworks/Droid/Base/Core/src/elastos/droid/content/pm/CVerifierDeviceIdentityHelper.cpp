
#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/content/pm/CVerifierDeviceIdentityHelper.h"
#include "elastos/droid/content/pm/CVerifierDeviceIdentity.h"

using Elastos::Utility::IRandom;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(CVerifierDeviceIdentityHelper, Singleton, IVerifierDeviceIdentityHelper)

CAR_SINGLETON_IMPL(CVerifierDeviceIdentityHelper)

ECode CVerifierDeviceIdentityHelper::Generate(
    /* [out] */ IVerifierDeviceIdentity** identity)
{
    AutoPtr<ISecureRandom> sr;
    assert(0 && "TODO");
    // CSecureRandom::New((Elastos::Security::ISecureRandom**)&sr);
    return Generate(sr, identity);
}

ECode CVerifierDeviceIdentityHelper::Generate(
    /* [in] */ ISecureRandom* rng,
    /* [out] */ IVerifierDeviceIdentity** identity)
{
    VALIDATE_NOT_NULL(identity)
    *identity = NULL;

    Int64 lIdentity;
    IRandom::Probe(rng)->NextInt64(&lIdentity);
    AutoPtr<CVerifierDeviceIdentity> cvdi;
    CVerifierDeviceIdentity::NewByFriend(lIdentity, (CVerifierDeviceIdentity**)&cvdi);
    *identity = (IVerifierDeviceIdentity*)cvdi.Get();
    REFCOUNT_ADD(*identity)
    return NOERROR;
}

ECode CVerifierDeviceIdentityHelper::Parse(
    /* [in] */ const String& deviceIdentity,
    /* [out] */ IVerifierDeviceIdentity** identity)
{
    VALIDATE_NOT_NULL(identity)
    *identity = NULL;

    AutoPtr<ArrayOf<Byte> > input = deviceIdentity.GetBytes();
    Int64 output;
    FAIL_RETURN(CVerifierDeviceIdentity::DecodeBase32(input, &output));

    AutoPtr<CVerifierDeviceIdentity> cvdi;
    CVerifierDeviceIdentity::NewByFriend(output, (CVerifierDeviceIdentity**)&cvdi);
    *identity = (IVerifierDeviceIdentity*)cvdi.Get();
    REFCOUNT_ADD(*identity)
    return NOERROR;
}


}
}
}
}

