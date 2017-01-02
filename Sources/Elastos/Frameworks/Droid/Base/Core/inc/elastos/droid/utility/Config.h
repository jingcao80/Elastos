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

#ifndef __ELASTOS_DROID_UTILITY_CONFIG_H__
#define __ELASTOS_DROID_UTILITY_CONFIG_H__

#include <elastos.h>

using namespace Elastos;

namespace Elastos {
namespace Droid {
namespace Utility {

class ECO_PUBLIC Config
{
public:
    /**
     * If this is a debug build, this field will be true.
     */
    static const Boolean DEBUG = FALSE;

    /*
     * Deprecated fields
     * TODO: Remove platform references to these and @hide them.
     */

    /**
     * @deprecated Use {@link #DEBUG} instead.
     */
    static const Boolean RELEASE = TRUE;

    /**
     * @deprecated Always false.
     */
    static const Boolean PROFILE = FALSE;

    /**
     * @deprecated Always false.
     */
    static const Boolean LOGV = FALSE;

    /**
     * @deprecated Always true.
     */
    static const Boolean LOGD = TRUE;

private:
    ECO_LOCAL Config();
};

}
}
}

#endif // __ELASTOS_DROID_UTILITY_CONFIG_H__
