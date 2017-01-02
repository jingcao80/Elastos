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

#include "elastos/droid/app/CFragmentManagerImplHelper.h"
#include "elastos/droid/app/FragmentManagerImpl.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CFragmentManagerImplHelper, Singleton, IFragmentManagerImplHelper)

CAR_SINGLETON_IMPL(CFragmentManagerImplHelper)

ECode CFragmentManagerImplHelper::ReverseTransit(
    /* [in] */ Int32 transit,
    /* [out] */ Int32* reverse)
{
    return FragmentManagerImpl::ReverseTransit(transit, reverse);
}

ECode CFragmentManagerImplHelper::TransitToStyleIndex(
    /* [in] */ Int32 transit,
    /* [in] */ Boolean enter,
    /* [out] */ Int32* index)
{
    return FragmentManagerImpl::TransitToStyleIndex(transit, enter, index);
}

ECode CFragmentManagerImplHelper::SetDEBUG(
    /* [in] */ Boolean debug)
{
    FragmentManagerImpl::DEBUG = debug;
    return NOERROR;
}

ECode CFragmentManagerImplHelper::GetDEBUG(
    /* [out] */ Boolean* debug)
{
    VALIDATE_NOT_NULL(debug);
    *debug = FragmentManagerImpl::DEBUG;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
