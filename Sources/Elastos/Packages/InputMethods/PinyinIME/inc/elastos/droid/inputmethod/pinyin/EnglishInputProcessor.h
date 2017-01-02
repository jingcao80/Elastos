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

#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_ENGLISHINPUTPROCESSOR_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_ENGLISHINPUTPROCESSOR_H__

#include <elastos/core/Object.h>

using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Class to handle English input.
 */
class EnglishInputProcessor: public Object
{
public:
    EnglishInputProcessor();

    CARAPI_(Boolean) ProcessKey(
        /* [in] */ IInputConnection* inputContext,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean upperCase,
        /* [in] */ Boolean realAction);

private:
    Int32 mLastKeyCode;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_ENGLISHINPUTPROCESSOR_H__
