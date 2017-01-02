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

#include "elastos/droid/systemui/recents/Constants.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

const Boolean Constants::DebugFlags::Verbose;
const Boolean Constants::DebugFlags::App::EnableScreenshotAppTransition;
const Boolean Constants::DebugFlags::App::EnableTransitionThumbnailDebugMode;
const Boolean Constants::DebugFlags::App::EnableTaskFiltering;
const Boolean Constants::DebugFlags::App::EnableTaskStackClipping;
const Boolean Constants::DebugFlags::App::EnableTaskBarTouchEvents;
const Boolean Constants::DebugFlags::App::EnableDevAppInfoOnLongPress;
const Boolean Constants::DebugFlags::App::EnableDebugMode;
const Boolean Constants::DebugFlags::App::EnableSearchLayout;
const Boolean Constants::DebugFlags::App::EnableThumbnailAlphaOnFrontmost;
const Boolean Constants::DebugFlags::App::DisableBackgroundCache;
const Boolean Constants::DebugFlags::App::EnableSimulatedTaskGroups;
const Int32 Constants::DebugFlags::App::TaskAffiliationsGroupCount;
const Boolean Constants::DebugFlags::App::EnableSystemServicesProxy;
const Int32 Constants::DebugFlags::App::SystemServicesProxyMockPackageCount;
const Int32 Constants::DebugFlags::App::SystemServicesProxyMockTaskCount;

Int32 Constants::Values::App::AppWidgetHostId = 1024;
String Constants::Values::App::Key_SearchAppWidgetId("searchAppWidgetId");
String Constants::Values::App::Key_DebugModeEnabled("debugModeEnabled");
String Constants::Values::App::DebugModeVersion("A");

const Int32 Constants::Values::RecentsTaskLoader::PreloadFirstTasksCount;

const Int32 Constants::Values::TaskStackView::TaskStackOverscrollRange;
const Int32 Constants::Values::TaskStackView::FilterStartDelay;
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
