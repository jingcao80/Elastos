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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_CONSTANTS_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_CONSTANTS_H__

#include <elastos.h>

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

/**
 * Constants
 */
class Constants
{
public:
    class DebugFlags
    {
    public:
        // Enable this with any other debug flag to see more info
        static const Boolean Verbose = FALSE;

        class App
        {
        public:
            // Enables the screenshot app->Recents transition
            static const Boolean EnableScreenshotAppTransition = FALSE;
            // Enables debug drawing for the transition thumbnail
            static const Boolean EnableTransitionThumbnailDebugMode = FALSE;
            // Enables the filtering of tasks according to their grouping
            static const Boolean EnableTaskFiltering = FALSE;
            // Enables clipping of tasks against each other
            static const Boolean EnableTaskStackClipping = TRUE;
            // Enables tapping on the TaskBar to launch the task
            static const Boolean EnableTaskBarTouchEvents = TRUE;
            // Enables app-info pane on long-pressing the icon
            static const Boolean EnableDevAppInfoOnLongPress = TRUE;
            // Enables debug mode
            static const Boolean EnableDebugMode = FALSE;
            // Enables the search bar layout
            static const Boolean EnableSearchLayout = TRUE;
            // Enables the thumbnail alpha on the front-most task
            static const Boolean EnableThumbnailAlphaOnFrontmost = FALSE;
            // This disables the bitmap and icon caches
            static const Boolean DisableBackgroundCache = FALSE;
            // Enables the simulated task affiliations
            static const Boolean EnableSimulatedTaskGroups = FALSE;
            // Defines the number of mock task affiliations per group
            static const Int32 TaskAffiliationsGroupCount = 12;
            // Enables us to create mock recents tasks
            static const Boolean EnableSystemServicesProxy = FALSE;
            // Defines the number of mock recents packages to create
            static const Int32 SystemServicesProxyMockPackageCount = 3;
            // Defines the number of mock recents tasks to create
            static const Int32 SystemServicesProxyMockTaskCount = 100;
        };
    };

    class Values
    {
    public:
        class App
        {
        public:
            static Int32 AppWidgetHostId;
            static String Key_SearchAppWidgetId;
            static String Key_DebugModeEnabled;
            static String DebugModeVersion;
        };

        class RecentsTaskLoader
        {
        public:
            // XXX: This should be calculated on the first load
            static const Int32 PreloadFirstTasksCount = 6;
        };

        class TaskStackView
        {
        public:
            static const Int32 TaskStackOverscrollRange = 150;
            static const Int32 FilterStartDelay = 25;
        };
    };
};

} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_CONSTANTS_H__
