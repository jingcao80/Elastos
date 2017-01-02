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

#ifndef __ELASTOS_DROID_SYSTEMUI_SCREENSHOT_SAVEIMAGEINBACKGROUNDDATA_H__
#define __ELASTOS_DROID_SYSTEMUI_SCREENSHOT_SAVEIMAGEINBACKGROUNDDATA_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/systemui/screenshot/GlobalScreenshot.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Core::Runnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Screenshot {

/**
 * POD used in the AsyncTask which saves an image in the background.
 */
class SaveImageInBackgroundData
    : public Object
{
public:
    SaveImageInBackgroundData();

    CARAPI_(void) ClearImage();

    CARAPI_(void) ClearContext();

public:
    AutoPtr<IContext> mContext;
    AutoPtr<IBitmap> mImage;
    AutoPtr<IUri> mImageUri;
    AutoPtr<IRunnable> mFinisher;
    Int32 mIconSize;
    Int32 mResult;
    Int32 mPreviewWidth;
    Int32 mPreviewheight;
};

} // namespace Screenshot
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

template <>
struct Conversion<Elastos::Droid::SystemUI::Screenshot::SaveImageInBackgroundData*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif //__ELASTOS_DROID_SYSTEMUI_SCREENSHOT_SAVEIMAGEINBACKGROUNDDATA_H__