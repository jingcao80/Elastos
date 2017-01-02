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

#include "elastos/droid/dialer/util/EmptyLoader.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::App::EIID_ILoaderManagerLoaderCallbacks;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Util {

//=================================================================
// EmptyLoader::Callback
//=================================================================
CAR_INTERFACE_IMPL(EmptyLoader::Callback, Object, ILoaderManagerLoaderCallbacks);

EmptyLoader::Callback::Callback(
    /* [in] */ IContext* context)
{
    context->GetApplicationContext((IContext**)&mContext);
}

ECode EmptyLoader::Callback::OnCreateLoader(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* args,
    /* [out] */ ILoader** loader)
{
    VALIDATE_NOT_NULL(loader)
    AutoPtr<EmptyLoader> emptyLoader = new EmptyLoader();
    emptyLoader->constructor(mContext);
    *loader = ILoader::Probe(emptyLoader);
    REFCOUNT_ADD(*loader);
    return NOERROR;
}

ECode EmptyLoader::Callback::OnLoadFinished(
    /* [in] */ ILoader* loader,
    /* [in] */ IInterface* data)
{
    return NOERROR;
}

ECode EmptyLoader::Callback::OnLoaderReset(
    /* [in] */ ILoader* loader)
{
    return NOERROR;
}

//=================================================================
// EmptyLoader
//=================================================================
ECode EmptyLoader::constructor(
    /* [in] */ IContext* context)
{
    return Loader::constructor(context);
}

} // Util
} // Dialer
} // Droid
} // Elastos
