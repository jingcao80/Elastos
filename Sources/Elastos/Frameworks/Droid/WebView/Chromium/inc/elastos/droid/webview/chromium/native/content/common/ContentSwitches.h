// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_COMMON_CONTENTSWITCHES_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_COMMON_CONTENTSWITCHES_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Common {

/**
 * Contains all of the command line switches that are specific to the content/
 * portion of Chromium on Android.
 */
class ContentSwitches
{
public:
    // Tell Java to use the official command line, loaded from the
    // official-command-line.xml files.  WARNING this is not done
    // immediately on startup, so early running Java code will not see
    // these flags.
    static const String ADD_OFFICIAL_COMMAND_LINE;

    // Enables test intent handling.
    static const String ENABLE_TEST_INTENTS;

    // Dump frames-per-second to the log
    static const String LOG_FPS;

    // Whether Chromium should use a mobile user agent.
    static const String USE_MOBILE_UA;

    // Change the url of the JavaScript that gets injected when accessibility mode is enabled.
    static const String ACCESSIBILITY_JAVASCRIPT_URL;

    // Whether to ignore signature mismatches when connecting to BrailleBack's
    // SelfBrailleService.
    static const String ACCESSIBILITY_DEBUG_BRAILLE_SERVICE;

    // Disables exposing web content using Android's accessibility
    // framework and enables the (older) javascript injection accessibility.
    // TODO(dmazzoni): remove this after native accessibility is in stable.
    // http://crbug.com/356775
    static const String ENABLE_ACCESSIBILITY_SCRIPT_INJECTION;

    // Sets the ISO country code that will be used for phone number detection.
    static const String NETWORK_COUNTRY_ISO;

    // Whether to enable the auto-hiding top controls.
    static const String ENABLE_TOP_CONTROLS_POSITION_CALCULATION;

    // The height of the movable top controls.
    static const String TOP_CONTROLS_HEIGHT;

    // How much of the top controls need to be shown before they will auto show.
    static const String TOP_CONTROLS_SHOW_THRESHOLD;

    // How much of the top controls need to be hidden before they will auto hide.
    static const String TOP_CONTROLS_HIDE_THRESHOLD;

    // Native switch - chrome_switches::kEnableInstantExtendedAPI
    static const String ENABLE_INSTANT_EXTENDED_API;

    // Native switch - content_switches::kEnableSpeechRecognition
    static const String ENABLE_SPEECH_RECOGNITION;

    // Native switch - shell_switches::kDumpRenderTree
    static const String DUMP_RENDER_TREE;

    // Native switch - chrome_switches::kDisablePopupBlocking
    static const String DISABLE_POPUP_BLOCKING;

    // Native switch - content_switches::kDisableOverlayFullscreenVideoSubtitle
    static const String DISABLE_OVERLAY_FULLSCREEN_VIDEO_SUBTITLE;

    // Native switch - gfx_switches::kForceDeviceScaleFactor
    static const String FORCE_DEVICE_SCALE_FACTOR;

private:
    // Prevent instantiation.
    ContentSwitches();
};

} // namespace Common
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_COMMON_CONTENTSWITCHES_H__
