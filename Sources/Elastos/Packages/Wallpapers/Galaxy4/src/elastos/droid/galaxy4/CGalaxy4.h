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

#ifndef __ELASTOS_DROID_GALAXY4_CGALAXY4_H__
#define __ELASTOS_DROID_GALAXY4_CGALAXY4_H__

#include "_Elastos_Droid_Galaxy4_CGalaxy4.h"
#include "CGalaxyView.h"
#include <Elastos.Droid.Os.h>
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

CarClass(CGalaxy4)
    , public Activity
{
public:
    CAR_OBJECT_DECL()

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    AutoPtr<CGalaxyView> mView;
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_CGALAXY4_H__
