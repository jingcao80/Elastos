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

#include "SelectorProvider.h"
#include "CSelectorProviderImpl.h"
#include "ServiceLoader.h"

using Elastos::Utility::ServiceLoader;
using Elastos::Utility::IServiceLoader;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace IO {
namespace Channels {
namespace Spi {

CAR_INTERFACE_IMPL(SelectorProvider, Object, ISelectorProvider)

AutoPtr<ISelectorProvider> SelectorProvider::sProvider;

SelectorProvider::SelectorProvider()
{
}

SelectorProvider::~SelectorProvider()
{
}

ECode SelectorProvider::GetProvider(
    /* [out] */ ISelectorProvider** provider)
{
    VALIDATE_NOT_NULL(provider)
    if (NULL == sProvider) {
        AutoPtr<CSelectorProviderImpl> impl;
        ASSERT_SUCCEEDED(CSelectorProviderImpl::NewByFriend((CSelectorProviderImpl**)&impl));
        sProvider = (ISelectorProvider*)impl.Get();
    }

    *provider = sProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode SelectorProvider::LoadProviderByJar(
    /* [out] */ ISelectorProvider** provider)
{
    VALIDATE_NOT_NULL(provider)

    AutoPtr<IServiceLoader> sl = ServiceLoader::Load(EIID_ISelectorProvider);
    if (sl) {
        AutoPtr<IIterator> it;
        ((ServiceLoader*)sl.Get())->GetIterator((IIterator**)&it);
        Boolean isflag = FALSE;
        while(it->HasNext(&isflag), isflag) {
            AutoPtr<IInterface> outface;
            it->GetNext((IInterface**)&outface);
            *provider = ISelectorProvider::Probe(outface);
            REFCOUNT_ADD(*provider)
            return NOERROR;
        }
    }

    *provider = NULL;
    return NOERROR;
}

ECode SelectorProvider::InheriteChannel(
    /* [out] */ IChannel** cannel)
{
    VALIDATE_NOT_NULL(cannel)

    // // Android never has stdin/stdout connected to a socket.
    *cannel = NULL;
    return NOERROR;
}

} // namespace Spi
} // namespace Channels
} // namespace IO
} // namespace Elastos
