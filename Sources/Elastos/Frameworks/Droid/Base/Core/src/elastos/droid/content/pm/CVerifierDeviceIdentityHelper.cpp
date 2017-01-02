//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

