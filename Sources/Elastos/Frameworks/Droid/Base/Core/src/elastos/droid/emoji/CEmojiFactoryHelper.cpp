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

#include "elastos/droid/emoji/CEmojiFactoryHelper.h"
#include "elastos/droid/emoji/EmojiFactory.h"

namespace Elastos {
namespace Droid {
namespace Emoji {

CAR_SINGLETON_IMPL(CEmojiFactoryHelper);
CAR_INTERFACE_IMPL(CEmojiFactoryHelper, Singleton, IEmojiFactoryHelper);

CEmojiFactoryHelper::CEmojiFactoryHelper()
{}

CEmojiFactoryHelper::~CEmojiFactoryHelper()
{}

ECode CEmojiFactoryHelper::NewInstance(
    /* [in] */ const String& className,
    /* [out] */ IEmojiFactory** emojiFactory)
{
    return EmojiFactory::NewInstance(className, emojiFactory);
}

ECode CEmojiFactoryHelper::NewAvailableInstance(
    /* [out] */ IEmojiFactory** emojiFactory)
{
    return EmojiFactory::NewAvailableInstance(emojiFactory);
}

} // namespace Emoji
} // namepsace Droid
} // namespace Elastos
