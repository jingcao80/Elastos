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

#ifndef __ELASTOS_DROID_DIALER_UTIL_ORIENTATIONUTIL_H__
#define __ELASTOS_DROID_DIALER_UTIL_ORIENTATIONUTIL_H__

#include <elastos/core/Object.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;

namespace Elastos{
namespace Droid{
namespace Dialer {
namespace Util {

/**
 * Static methods related to device orientation.
 */
class OrientationUtil
    : public Object
{
public:
    /**
     * @return if the context is in landscape orientation.
     */
    static CARAPI_(Boolean) IsLandscape(
        /* [in] */ IContext* context);
}

} // Util
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_UTIL_ORIENTATIONUTIL_H__
