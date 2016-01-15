
#include "elastos/droid/webkit/native/content/common/ContentSwitches.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Common {

// Prevent instantiation.
ContentSwitches::ContentSwitches()
{
}

// Tell Java to use the official command line, loaded from the
// official-command-line.xml files.  WARNING this is not done
// immediately on startup, so early running Java code will not see
// these flags.
const String ContentSwitches::ADD_OFFICIAL_COMMAND_LINE("add-official-command-line");

// Enables test intent handling.
const String ContentSwitches::ENABLE_TEST_INTENTS("enable-test-intents");

// Dump frames-per-second to the log
const String ContentSwitches::LOG_FPS("log-fps");

// Whether Chromium should use a mobile user agent.
const String ContentSwitches::USE_MOBILE_UA("use-mobile-user-agent");

// Change the url of the JavaScript that gets injected when accessibility mode is enabled.
const String ContentSwitches::ACCESSIBILITY_JAVASCRIPT_URL("accessibility-js-url");

// Whether to ignore signature mismatches when connecting to BrailleBack's
// SelfBrailleService.
const String ContentSwitches::ACCESSIBILITY_DEBUG_BRAILLE_SERVICE("debug-braille-service");

// Disables exposing web content using Android's accessibility
// framework and enables the (older) javascript injection accessibility.
// TODO(dmazzoni): remove this after native accessibility is in stable.
// http://crbug.com/356775
const String ContentSwitches::ENABLE_ACCESSIBILITY_SCRIPT_INJECTION("enable-accessibility-script-injection");

// Sets the ISO country code that will be used for phone number detection.
const String ContentSwitches::NETWORK_COUNTRY_ISO("network-country-iso");

// Whether to enable the auto-hiding top controls.
const String ContentSwitches::ENABLE_TOP_CONTROLS_POSITION_CALCULATION("enable-top-controls-position-calculation");

// The height of the movable top controls.
const String ContentSwitches::TOP_CONTROLS_HEIGHT("top-controls-height");

// How much of the top controls need to be shown before they will auto show.
const String ContentSwitches::TOP_CONTROLS_SHOW_THRESHOLD("top-controls-show-threshold");

// How much of the top controls need to be hidden before they will auto hide.
const String ContentSwitches::TOP_CONTROLS_HIDE_THRESHOLD("top-controls-hide-threshold");

// Native switch - chrome_switches::kEnableInstantExtendedAPI
const String ContentSwitches::ENABLE_INSTANT_EXTENDED_API("enable-instant-extended-api");

// Native switch - content_switches::kEnableSpeechRecognition
const String ContentSwitches::ENABLE_SPEECH_RECOGNITION("enable-speech-recognition");

// Native switch - shell_switches::kDumpRenderTree
const String ContentSwitches::DUMP_RENDER_TREE("dump-render-tree");

// Native switch - chrome_switches::kDisablePopupBlocking
const String ContentSwitches::DISABLE_POPUP_BLOCKING("disable-popup-blocking");

// Native switch - content_switches::kDisableOverlayFullscreenVideoSubtitle
const String ContentSwitches::DISABLE_OVERLAY_FULLSCREEN_VIDEO_SUBTITLE("disable-overlay-fullscreen-video-subtitle");

// Native switch - gfx_switches::kForceDeviceScaleFactor
const String ContentSwitches::FORCE_DEVICE_SCALE_FACTOR("force-device-scale-factor");

} // namespace Common
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
