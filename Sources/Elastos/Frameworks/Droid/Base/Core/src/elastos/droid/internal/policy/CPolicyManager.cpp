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
#include "elastos/droid/internal/policy/CPolicyManager.h"
#include "elastos/droid/internal/policy/impl/CPolicy.h"

using Elastos::Droid::Internal::Policy::Impl::CPolicy;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {

CAR_INTERFACE_IMPL(CPolicyManager, Singleton, IPolicyManager)

CAR_SINGLETON_IMPL(CPolicyManager)


CPolicyManager::CPolicyManager()
{
    ASSERT_SUCCEEDED(CPolicy::New((IIPolicy**)&mPolicy));
}

ECode CPolicyManager::MakeNewWindow(
    /* [in] */ IContext* ctx,
    /* [out] */ IWindow** window)
{
    return mPolicy->MakeNewWindow(ctx, window);
}

ECode CPolicyManager::MakeNewLayoutInflater(
    /* [in] */ IContext* ctx,
    /* [out] */ ILayoutInflater** inflater)
{
    return mPolicy->MakeNewLayoutInflater(ctx, inflater);
}

ECode CPolicyManager::MakeNewWindowManager(
    /* [out] */ IWindowManagerPolicy** wm)
{
    return mPolicy->MakeNewWindowManager(wm);
}

ECode CPolicyManager::MakeNewFallbackEventHandler(
    /* [in] */ IContext* context,
    /* [out] */ IFallbackEventHandler** handler)
{
    return mPolicy->MakeNewFallbackEventHandler(context, handler);
}

} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos
