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

#include "elastos/droid/galaxy4/CGalaxy4.h"

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

CAR_OBJECT_IMPL(CGalaxy4)

ECode CGalaxy4::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Activity::OnCreate(savedInstanceState));
    CGalaxyView::NewByFriend((IContext*)this, (CGalaxyView**)&mView);
    SetContentView(mView);
    return NOERROR;
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
