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

#include "CDSAKeyFactoryImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CAR_OBJECT_IMPL(CDSAKeyFactoryImpl)

ECode CDSAKeyFactoryImpl::EngineGeneratePublic(
    /* [in] */ Elastos::Security::Spec::IKeySpec * pKeySpec,
    /* [out] */ Elastos::Security::IPublicKey ** ppPubKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAKeyFactoryImpl::EngineGeneratePrivate(
    /* [in] */ Elastos::Security::Spec::IKeySpec * pKeySpec,
    /* [out] */ Elastos::Security::IPrivateKey ** ppPriKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAKeyFactoryImpl::EngineGetKeySpec(
    /* [in] */ Elastos::Security::IKey * pKey,
    /* [in] */ const ClassID & keySpec,
    /* [out] */ Elastos::Security::Spec::IKeySpec ** ppRetkeySpec)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAKeyFactoryImpl::EngineTranslateKey(
    /* [in] */ Elastos::Security::IKey * pKey,
    /* [out] */ Elastos::Security::IKey ** ppTranslatedKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

