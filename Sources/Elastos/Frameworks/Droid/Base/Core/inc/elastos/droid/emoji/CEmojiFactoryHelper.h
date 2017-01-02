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

#ifndef __ELASTOS_DROID_EMOJI_CEMOJIFACTORYHELPER_H__
#define __ELASTOS_DROID_EMOJI_CEMOJIFACTORYHELPER_H__

#include "_Elastos_Droid_Emoji_CEmojiFactoryHelper.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Emoji {

using Elastos::Droid::Emoji::IEmojiFactoryHelper;

CarClass(CEmojiFactoryHelper)
    , public Singleton
    , public IEmojiFactoryHelper
{
public:
    CAR_SINGLETON_DECL();
    CAR_INTERFACE_DECL()

    CEmojiFactoryHelper();

    virtual ~CEmojiFactoryHelper();

    /**
     * Constructs an instance of EmojiFactory corresponding to the name.
     *
     * @param class_name Name of the factory. This must include complete package name.
     * @return A concrete EmojiFactory instance corresponding to factory_name.
     * If factory_name is invalid, null is returned.
     */
    CARAPI NewInstance(
        /* [in] */ const String& className,
        /* [out] */ IEmojiFactory** EmojiFactory);

    /**
     * Constructs an instance of available EmojiFactory.
     *
     * @return A concrete EmojiFactory instance. If there are several available
     * EmojiFactory class, preferred one is chosen by the system. If there isn't, null
     * is returned.
     */
    CARAPI NewAvailableInstance(
        /* [out] */ IEmojiFactory** EmojiFactory);
};

} // namespace Emoji
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_EMOJI_CEMOJIFACTORYHELPER_H__
