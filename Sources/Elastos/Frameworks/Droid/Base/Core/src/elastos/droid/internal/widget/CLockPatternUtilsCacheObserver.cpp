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

#include "elastos/droid/internal/widget/CLockPatternUtilsCacheObserver.h"
#include "elastos/droid/internal/widget/CLockPatternUtilsCache.h"

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_OBJECT_IMPL(CLockPatternUtilsCacheObserver)
CAR_INTERFACE_IMPL_2(CLockPatternUtilsCacheObserver, Object, IILockSettingsObserver, IBinder);

ECode CLockPatternUtilsCacheObserver::constructor()
{
    return NOERROR;
}

ECode CLockPatternUtilsCacheObserver::constructor(
    /* [in] */ ILockPatternUtilsCache* service)
{
    mService = service;
    return NOERROR;
}

ECode CLockPatternUtilsCacheObserver::OnLockSettingChanged(
    /* [in] */ const String& key,
    /* [in] */ Int32 userId)
{
    ((CLockPatternUtilsCache*)mService.Get())->InvalidateCache(key, userId);
    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
