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

#ifndef __ELASTOS_DROID_SUPPORT_V13_APP_FRAGMENTCOMPATICS_H__
#define __ELASTOS_DROID_SUPPORT_V13_APP_FRAGMENTCOMPATICS_H__

#include "Elastos.Droid.Support.V13.h"
#include "Elastos.Droid.App.h"

using Elastos::Droid::App::IFragment;

namespace Elastos {
namespace Droid {
namespace Support {
namespace V13 {
namespace App {

class FragmentCompatICS
{
public:
    static CARAPI_(void) SetMenuVisibility(
        /* [in] */ IFragment* f,
        /* [in] */ Boolean visible);
};

} // namespace App
} // namespace V13
} // namespace Support
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SUPPORT_V13_APP_FRAGMENTCOMPATICS_H__
