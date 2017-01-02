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

#ifndef  __ELASTOS_DROID_SYSTEMUI_FONTSIZEUTILS_H__
#define  __ELASTOS_DROID_SYSTEMUI_FONTSIZEUTILS_H__

#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {

/**
 * Utility class to update the font size when the configuration has changed.
 */
class FontSizeUtils
{
public:
    static CARAPI_(void) UpdateFontSize(
        /* [in] */ IView* parent,
        /* [in] */ Int32 viewId,
        /* [in] */ Int32 dimensId);

    static CARAPI_(void) UpdateFontSize(
        /* [in] */ ITextView* v,
        /* [in] */ Int32 dimensId);

public:
    static const Float LARGE_TEXT_SCALE;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_FONTSIZEUTILS_H__
