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

#ifndef __ELASTOS_DROID_SUPPORT_V4_VIEW_CVIEWPAGER_H__
#define __ELASTOS_DROID_SUPPORT_V4_VIEW_CVIEWPAGER_H__

#include "_Elastos_Droid_Support_V4_View_CViewPager.h"
#include "elastos/droid/support/v4/view/ViewPager.h"

namespace Elastos {
namespace Droid {
namespace Support {
namespace V4 {
namespace View {

CarClass(CViewPager)
    , public ViewPager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace View
} // namespace V4
} // namespace Support
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SUPPORT_V4_VIEW_CVIEWPAGER_H__
