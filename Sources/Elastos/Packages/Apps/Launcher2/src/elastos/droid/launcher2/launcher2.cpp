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

#include "elastos/droid/launcher2/CPagedViewWidget.h"
#include "elastos/droid/launcher2/CBubbleTextView.h"
#include "elastos/droid/launcher2/CInstallShortcutReceiver.h"
#include "elastos/droid/launcher2/CUninstallShortcutReceiver.h"
#include "elastos/droid/launcher2/CWallpaperChooserDialogFragment.h"
#include "elastos/droid/launcher2/CDragLayer.h"
#include "elastos/droid/launcher2/CDragLayerLayoutParams.h"
#include "elastos/droid/launcher2/CFolderIcon.h"
#include "elastos/droid/launcher2/CLauncherProvider.h"
#include "elastos/droid/launcher2/CCellLayout.h"
#include "elastos/droid/launcher2/CLauncherApplication.h"
#include "elastos/droid/launcher2/CFolder.h"
#include "elastos/droid/launcher2/CWorkspace.h"
#include "elastos/droid/launcher2/CLauncher.h"
#include "elastos/droid/launcher2/CLauncherModel.h"
#include "elastos/droid/launcher2/CLauncherBroadcastReceiver.h"
#include "elastos/droid/launcher2/CLauncherCloseSystemDialogsIntentReceiver.h"
#include "elastos/droid/launcher2/CLauncherAppWidgetResetObserver.h"
#include "elastos/droid/launcher2/CLauncherApplicationContentObserver.h"
#include "elastos/droid/launcher2/CShortcutAndWidgetContainer.h"
#include "elastos/droid/launcher2/CFastBitmapDrawable.h"
#include "elastos/droid/launcher2/CWallpaperDrawable.h"


namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_OBJECT_IMPL(CPagedViewWidget)
CAR_OBJECT_IMPL(CBubbleTextView)
CAR_OBJECT_IMPL(CInstallShortcutReceiver)
CAR_OBJECT_IMPL(CUninstallShortcutReceiver)
CAR_OBJECT_IMPL(CWallpaperChooserDialogFragment)
CAR_OBJECT_IMPL(CDragLayer)
CAR_OBJECT_IMPL(CDragLayerLayoutParams)
CAR_OBJECT_IMPL(CFolderIcon)
CAR_OBJECT_IMPL(CLauncherProvider)
CAR_OBJECT_IMPL(CCellLayout)
CAR_OBJECT_IMPL(CLauncherApplication)
CAR_OBJECT_IMPL(CFolder)
CAR_OBJECT_IMPL(CWorkspace)
CAR_OBJECT_IMPL(CLauncher)
CAR_OBJECT_IMPL(CLauncherModel)
CAR_OBJECT_IMPL(CLauncherBroadcastReceiver)
CAR_OBJECT_IMPL(CLauncherCloseSystemDialogsIntentReceiver)
CAR_OBJECT_IMPL(CLauncherAppWidgetResetObserver)
CAR_OBJECT_IMPL(CLauncherApplicationContentObserver)
CAR_OBJECT_IMPL(CShortcutAndWidgetContainer)
CAR_OBJECT_IMPL(CFastBitmapDrawable)
CAR_OBJECT_IMPL(CWallpaperDrawable)


} // namespace Launcher2
} // namespace Droid
} // namespace Elastos