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

#ifndef __ELASTOS_DROID_SYSTEMUI_SETTINGS_CTOGGLESEEKBAR_H__
#define __ELASTOS_DROID_SYSTEMUI_SETTINGS_CTOGGLESEEKBAR_H__

#include "_Elastos_Droid_SystemUI_Settings_CToggleSeekBar.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/droid/widget/SeekBar.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::SeekBar;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

CarClass(CToggleSeekBar), public SeekBar
{
public:
    CAR_OBJECT_DECL()

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);
};

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SETTINGS_CTOGGLESEEKBAR_H__
