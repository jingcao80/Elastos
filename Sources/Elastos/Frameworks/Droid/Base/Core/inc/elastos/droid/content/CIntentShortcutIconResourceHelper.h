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

#ifndef __ELASTOS_DROID_CONTENT_CINTENTSHORTCUTICONRESOURCEHELPER_H__
#define __ELASTOS_DROID_CONTENT_CINTENTSHORTCUTICONRESOURCEHELPER_H__

#include "_Elastos_Droid_Content_CIntentShortcutIconResourceHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntentShortcutIconResourceHelper)
    , public Singleton
    , public IIntentShortcutIconResourceHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Creates a new ShortcutIconResource for the specified context and resource
     * identifier.
     *
     * @param context The context of the application.
     * @param resourceId The resource idenfitier for the icon.
     * @return A new ShortcutIconResource with the specified's context package name
     *         and icon resource idenfitier.
     */
    CARAPI FromContext(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resourceId,
        /* [out] */ IIntentShortcutIconResource** shortcutIconRes);
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CINTENTSHORTCUTICONRESOURCEHELPER_H__
