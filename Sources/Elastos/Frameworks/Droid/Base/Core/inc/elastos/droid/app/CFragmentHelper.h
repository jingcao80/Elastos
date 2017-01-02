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

#ifndef __ELASTOS_DROID_APP_CFRAGMENTHELPER_H__
#define __ELASTOS_DROID_APP_CFRAGMENTHELPER_H__

#include "_Elastos_Droid_App_CFragmentHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CFragmentHelper)
    , public Singleton
    , public IFragmentHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Instantiate(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname,
        /* [out] */ IFragment** fragment);

    CARAPI Instantiate(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname,
        /* [in] */ IBundle* args,
        /* [out] */ IFragment** fragment);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CFRAGMENTHELPER_H__
