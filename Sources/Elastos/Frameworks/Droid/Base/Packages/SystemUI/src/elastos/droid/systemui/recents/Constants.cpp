
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
