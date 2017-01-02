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

#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_KEYMAPDREAM_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_KEYMAPDREAM_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Class used to map the symbols on Dream's hardware keyboard to corresponding
 * Chinese full-width symbols.
 */
class KeyMapDream
{
public:
    static CARAPI_(Char32) GetChineseLabel(
        /* [in] */ Int32 keyCode);

private:
    // Number of shift bits to store full-width symbols
    static const Int32 SHIFT_FWCH = 8;
    static const Int32 sKeyMap[];
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_KEYMAPDREAM_H__
