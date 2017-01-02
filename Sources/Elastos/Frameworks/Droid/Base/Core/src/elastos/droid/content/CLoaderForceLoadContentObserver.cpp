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

#include "elastos/droid/content/CLoaderForceLoadContentObserver.h"
#include "elastos/droid/os/CHandler.h"

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CLoaderForceLoadContentObserver, ContentObserver, ILoaderForceLoadContentObserver)

CAR_OBJECT_IMPL(CLoaderForceLoadContentObserver)

ECode CLoaderForceLoadContentObserver::constructor(
    /* [in] */ ILoader* loader)
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    FAIL_RETURN(ContentObserver::constructor(handler))

    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(loader);
    wrs->GetWeakReference((IWeakReference**)&mWeakLoader);
    return NOERROR;
}

ECode CLoaderForceLoadContentObserver::DeliverSelfNotifications(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode CLoaderForceLoadContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoPtr<ILoader> loader;
    mWeakLoader->Resolve(EIID_ILoader, (IInterface**)&loader);
    if (loader) {
        return loader->OnContentChanged();
    }
    return NOERROR;
}


}
}
}

