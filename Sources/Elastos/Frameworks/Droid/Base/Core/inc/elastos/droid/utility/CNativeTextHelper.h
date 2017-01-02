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

#ifndef __ELASTOS_DROID_UTILITY_CNATIVETEXTHELPER_H__
#define __ELASTOS_DROID_UTILITY_CNATIVETEXTHELPER_H__

#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_Utility_CNativeTextHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::INativeTextHelper;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CNativeTextHelper)
    , public Singleton
    , public INativeTextHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * parse the string to current language string in public resources.
     *
     * @param context base context of the application
     * @param originalString original string
     * @param originNamesId the id of the original string array.
     * @param localNamesId the id of the local string keys.
     * @return local language string
     */
    CARAPI GetLocalString(
        /* [in] */ IContext* context,
        /* [in] */ const String& originalString,
        /* [in] */ Int32 originNamesId,
        /* [in] */ Int32 localNamesId,
        /* [out] */ String* result);

    /**
     * parse the string to current language string in current resources.
     *
     * @param context base context of the application
     * @param originalString original string
     * @param originNamesId the id of the original string array.
     * @param localNamesId the id of the local string keys.
     * @return local language string
     */
    CARAPI GetInternalLocalString(
        /* [in] */ IContext* context,
        /* [in] */ const String& originalString,
        /* [in] */ Int32 originNamesId,
        /* [in] */ Int32 localNamesId,
        /* [out] */ String* result);
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CNATIVETEXTHELPER_H__
