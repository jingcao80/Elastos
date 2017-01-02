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

#include "CCryptoProvider.h"
#include "CoreUtils.h"

using Elastos::Core::CoreUtils;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CAR_OBJECT_IMPL(CCryptoProvider)
ECode CCryptoProvider::constructor()
{
    Provider::constructor(String("Crypto"), 1.0,
        String("HARMONY (SHA1 digest; SecureRandom; SHA1withDSA signature)"));

    Put(CoreUtils::Convert(String("SecureRandom.SHA1PRNG")),
            CoreUtils::Convert(String("Org.Apache.Harmony.Security.Provider.Crypto.CSHA1PRNG_SecureRandomImpl")));
    Put(CoreUtils::Convert(String("SecureRandom.SHA1PRNG ImplementedIn")), CoreUtils::Convert(String("Software")));
    return NOERROR;
}

}
}
}
}
}
}
