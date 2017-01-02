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

#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIATHUMBREQUESTHELPER_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIATHUMBREQUESTHELPER_H__

#include "_Elastos_Droid_Providers_Media_CMediaThumbRequestHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::IComparator;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CarClass(CMediaThumbRequestHelper)
    , public Singleton
    , public IMediaThumbRequestHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetComparator(
        /* [out] */ IComparator** result);
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIATHUMBREQUESTHELPER_H__