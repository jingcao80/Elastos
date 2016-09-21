// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "InternalSettingsGenerated.h"

#include "core/page/Page.h"
#include "core/frame/Settings.h"

namespace WebCore {

InternalSettingsGenerated::InternalSettingsGenerated(Page* page)
    : m_page(page)
    , m_DOMPasteAllowed(page->settings().DOMPasteAllowed())
    , m_accelerated2dCanvasEnabled(page->settings().accelerated2dCanvasEnabled())
    , m_accelerated2dCanvasMSAASampleCount(page->settings().accelerated2dCanvasMSAASampleCount())
    , m_acceleratedCompositingEnabled(page->settings().acceleratedCompositingEnabled())
    , m_acceleratedCompositingForCanvasEnabled(page->settings().acceleratedCompositingForCanvasEnabled())
    , m_acceleratedCompositingForFiltersEnabled(page->settings().acceleratedCompositingForFiltersEnabled())
    , m_acceleratedCompositingForFixedPositionEnabled(page->settings().acceleratedCompositingForFixedPositionEnabled())
    , m_acceleratedCompositingForFixedRootBackgroundEnabled(page->settings().acceleratedCompositingForFixedRootBackgroundEnabled())
    , m_acceleratedCompositingForOverflowScrollEnabled(page->settings().acceleratedCompositingForOverflowScrollEnabled())
    , m_acceleratedCompositingForVideoEnabled(page->settings().acceleratedCompositingForVideoEnabled())
    , m_accessibilityFontScaleFactor(page->settings().accessibilityFontScaleFactor())
    , m_allowConnectingInsecureWebSocket(page->settings().allowConnectingInsecureWebSocket())
    , m_allowCustomScrollbarInMainFrame(page->settings().allowCustomScrollbarInMainFrame())
    , m_allowDisplayOfInsecureContent(page->settings().allowDisplayOfInsecureContent())
    , m_allowFileAccessFromFileURLs(page->settings().allowFileAccessFromFileURLs())
    , m_allowRunningOfInsecureContent(page->settings().allowRunningOfInsecureContent())
    , m_allowScriptsToCloseWindows(page->settings().allowScriptsToCloseWindows())
    , m_allowUniversalAccessFromFileURLs(page->settings().allowUniversalAccessFromFileURLs())
    , m_antialiased2dCanvasEnabled(page->settings().antialiased2dCanvasEnabled())
    , m_asynchronousSpellCheckingEnabled(page->settings().asynchronousSpellCheckingEnabled())
    , m_caretBrowsingEnabled(page->settings().caretBrowsingEnabled())
    , m_compositedScrollingForFramesEnabled(page->settings().compositedScrollingForFramesEnabled())
    , m_compositorDrivenAcceleratedScrollingEnabled(page->settings().compositorDrivenAcceleratedScrollingEnabled())
    , m_compositorTouchHitTesting(page->settings().compositorTouchHitTesting())
    , m_containerCullingEnabled(page->settings().containerCullingEnabled())
    , m_cookieEnabled(page->settings().cookieEnabled())
    , m_defaultFixedFontSize(page->settings().defaultFixedFontSize())
    , m_defaultFontSize(page->settings().defaultFontSize())
    , m_defaultTextEncodingName(page->settings().defaultTextEncodingName())
    , m_defaultVideoPosterURL(page->settings().defaultVideoPosterURL())
    , m_deferredFiltersEnabled(page->settings().deferredFiltersEnabled())
    , m_deviceScaleAdjustment(page->settings().deviceScaleAdjustment())
    , m_deviceSupportsMouse(page->settings().deviceSupportsMouse())
    , m_deviceSupportsTouch(page->settings().deviceSupportsTouch())
    , m_disallowFullscreenForNonMediaElements(page->settings().disallowFullscreenForNonMediaElements())
    , m_dnsPrefetchingEnabled(page->settings().dnsPrefetchingEnabled())
    , m_downloadableBinaryFontsEnabled(page->settings().downloadableBinaryFontsEnabled())
    , m_forceZeroLayoutHeight(page->settings().forceZeroLayoutHeight())
    , m_fullscreenSupported(page->settings().fullscreenSupported())
    , m_hyperlinkAuditingEnabled(page->settings().hyperlinkAuditingEnabled())
    , m_ignoreMainFrameOverflowHiddenQuirk(page->settings().ignoreMainFrameOverflowHiddenQuirk())
    , m_imagesEnabled(page->settings().imagesEnabled())
    , m_javaEnabled(page->settings().javaEnabled())
    , m_javaScriptCanAccessClipboard(page->settings().javaScriptCanAccessClipboard())
    , m_javaScriptCanOpenWindowsAutomatically(page->settings().javaScriptCanOpenWindowsAutomatically())
    , m_layerSquashingEnabled(page->settings().layerSquashingEnabled())
    , m_loadWithOverviewMode(page->settings().loadWithOverviewMode())
    , m_loadsImagesAutomatically(page->settings().loadsImagesAutomatically())
    , m_localStorageEnabled(page->settings().localStorageEnabled())
    , m_mainFrameClipsContent(page->settings().mainFrameClipsContent())
    , m_maxBeaconTransmission(page->settings().maxBeaconTransmission())
    , m_maxTouchPoints(page->settings().maxTouchPoints())
    , m_mediaControlsOverlayPlayButtonEnabled(page->settings().mediaControlsOverlayPlayButtonEnabled())
    , m_mediaPlaybackRequiresUserGesture(page->settings().mediaPlaybackRequiresUserGesture())
    , m_mediaTypeOverride(page->settings().mediaTypeOverride())
    , m_minimumAccelerated2dCanvasSize(page->settings().minimumAccelerated2dCanvasSize())
    , m_minimumFontSize(page->settings().minimumFontSize())
    , m_minimumLogicalFontSize(page->settings().minimumLogicalFontSize())
    , m_navigateOnDragDrop(page->settings().navigateOnDragDrop())
    , m_needsSiteSpecificQuirks(page->settings().needsSiteSpecificQuirks())
    , m_offlineWebApplicationCacheEnabled(page->settings().offlineWebApplicationCacheEnabled())
    , m_pageCacheSupportsPlugins(page->settings().pageCacheSupportsPlugins())
    , m_passwordEchoDurationInSeconds(page->settings().passwordEchoDurationInSeconds())
    , m_passwordEchoEnabled(page->settings().passwordEchoEnabled())
    , m_pinchOverlayScrollbarThickness(page->settings().pinchOverlayScrollbarThickness())
    , m_pinchVirtualViewportEnabled(page->settings().pinchVirtualViewportEnabled())
    , m_pluginsEnabled(page->settings().pluginsEnabled())
    , m_regionBasedColumnsEnabled(page->settings().regionBasedColumnsEnabled())
    , m_reportScreenSizeInPhysicalPixelsQuirk(page->settings().reportScreenSizeInPhysicalPixelsQuirk())
    , m_scriptEnabled(page->settings().scriptEnabled())
    , m_scrollAnimatorEnabled(page->settings().scrollAnimatorEnabled())
    , m_selectTrailingWhitespaceEnabled(page->settings().selectTrailingWhitespaceEnabled())
    , m_selectionIncludesAltImageText(page->settings().selectionIncludesAltImageText())
    , m_shouldClearDocumentBackground(page->settings().shouldClearDocumentBackground())
    , m_shouldPrintBackgrounds(page->settings().shouldPrintBackgrounds())
    , m_shouldRespectImageOrientation(page->settings().shouldRespectImageOrientation())
    , m_shrinksStandaloneImagesToFit(page->settings().shrinksStandaloneImagesToFit())
    , m_smartInsertDeleteEnabled(page->settings().smartInsertDeleteEnabled())
    , m_spatialNavigationEnabled(page->settings().spatialNavigationEnabled())
    , m_supportsMultipleWindows(page->settings().supportsMultipleWindows())
    , m_syncXHRInDocumentsEnabled(page->settings().syncXHRInDocumentsEnabled())
    , m_textAreasAreResizable(page->settings().textAreasAreResizable())
    , m_touchAdjustmentEnabled(page->settings().touchAdjustmentEnabled())
    , m_touchDragDropEnabled(page->settings().touchDragDropEnabled())
    , m_touchEditingEnabled(page->settings().touchEditingEnabled())
    , m_treatIPAddressAsDomain(page->settings().treatIPAddressAsDomain())
    , m_unifiedTextCheckerEnabled(page->settings().unifiedTextCheckerEnabled())
    , m_unsafePluginPastingEnabled(page->settings().unsafePluginPastingEnabled())
    , m_useLegacyBackgroundSizeShorthandBehavior(page->settings().useLegacyBackgroundSizeShorthandBehavior())
    , m_useSolidColorScrollbars(page->settings().useSolidColorScrollbars())
    , m_useWideViewport(page->settings().useWideViewport())
    , m_usesEncodingDetector(page->settings().usesEncodingDetector())
    , m_validationMessageTimerMagnification(page->settings().validationMessageTimerMagnification())
    , m_viewportEnabled(page->settings().viewportEnabled())
    , m_viewportMetaEnabled(page->settings().viewportMetaEnabled())
    , m_viewportMetaMergeContentQuirk(page->settings().viewportMetaMergeContentQuirk())
    , m_viewportMetaZeroValuesQuirk(page->settings().viewportMetaZeroValuesQuirk())
    , m_webAudioEnabled(page->settings().webAudioEnabled())
    , m_webGLEnabled(page->settings().webGLEnabled())
    , m_webGLErrorsToConsoleEnabled(page->settings().webGLErrorsToConsoleEnabled())
    , m_webSecurityEnabled(page->settings().webSecurityEnabled())
    , m_wideViewportQuirkEnabled(page->settings().wideViewportQuirkEnabled())
    , m_xssAuditorEnabled(page->settings().xssAuditorEnabled())
{
}

InternalSettingsGenerated::~InternalSettingsGenerated()
{
}

void InternalSettingsGenerated::resetToConsistentState()
{
    m_page->settings().setDOMPasteAllowed(m_DOMPasteAllowed);
    m_page->settings().setAccelerated2dCanvasEnabled(m_accelerated2dCanvasEnabled);
    m_page->settings().setAccelerated2dCanvasMSAASampleCount(m_accelerated2dCanvasMSAASampleCount);
    m_page->settings().setAcceleratedCompositingEnabled(m_acceleratedCompositingEnabled);
    m_page->settings().setAcceleratedCompositingForCanvasEnabled(m_acceleratedCompositingForCanvasEnabled);
    m_page->settings().setAcceleratedCompositingForFiltersEnabled(m_acceleratedCompositingForFiltersEnabled);
    m_page->settings().setAcceleratedCompositingForFixedPositionEnabled(m_acceleratedCompositingForFixedPositionEnabled);
    m_page->settings().setAcceleratedCompositingForFixedRootBackgroundEnabled(m_acceleratedCompositingForFixedRootBackgroundEnabled);
    m_page->settings().setAcceleratedCompositingForOverflowScrollEnabled(m_acceleratedCompositingForOverflowScrollEnabled);
    m_page->settings().setAcceleratedCompositingForVideoEnabled(m_acceleratedCompositingForVideoEnabled);
    m_page->settings().setAccessibilityFontScaleFactor(m_accessibilityFontScaleFactor);
    m_page->settings().setAllowConnectingInsecureWebSocket(m_allowConnectingInsecureWebSocket);
    m_page->settings().setAllowCustomScrollbarInMainFrame(m_allowCustomScrollbarInMainFrame);
    m_page->settings().setAllowDisplayOfInsecureContent(m_allowDisplayOfInsecureContent);
    m_page->settings().setAllowFileAccessFromFileURLs(m_allowFileAccessFromFileURLs);
    m_page->settings().setAllowRunningOfInsecureContent(m_allowRunningOfInsecureContent);
    m_page->settings().setAllowScriptsToCloseWindows(m_allowScriptsToCloseWindows);
    m_page->settings().setAllowUniversalAccessFromFileURLs(m_allowUniversalAccessFromFileURLs);
    m_page->settings().setAntialiased2dCanvasEnabled(m_antialiased2dCanvasEnabled);
    m_page->settings().setAsynchronousSpellCheckingEnabled(m_asynchronousSpellCheckingEnabled);
    m_page->settings().setCaretBrowsingEnabled(m_caretBrowsingEnabled);
    m_page->settings().setCompositedScrollingForFramesEnabled(m_compositedScrollingForFramesEnabled);
    m_page->settings().setCompositorDrivenAcceleratedScrollingEnabled(m_compositorDrivenAcceleratedScrollingEnabled);
    m_page->settings().setCompositorTouchHitTesting(m_compositorTouchHitTesting);
    m_page->settings().setContainerCullingEnabled(m_containerCullingEnabled);
    m_page->settings().setCookieEnabled(m_cookieEnabled);
    m_page->settings().setDefaultFixedFontSize(m_defaultFixedFontSize);
    m_page->settings().setDefaultFontSize(m_defaultFontSize);
    m_page->settings().setDefaultTextEncodingName(m_defaultTextEncodingName);
    m_page->settings().setDefaultVideoPosterURL(m_defaultVideoPosterURL);
    m_page->settings().setDeferredFiltersEnabled(m_deferredFiltersEnabled);
    m_page->settings().setDeviceScaleAdjustment(m_deviceScaleAdjustment);
    m_page->settings().setDeviceSupportsMouse(m_deviceSupportsMouse);
    m_page->settings().setDeviceSupportsTouch(m_deviceSupportsTouch);
    m_page->settings().setDisallowFullscreenForNonMediaElements(m_disallowFullscreenForNonMediaElements);
    m_page->settings().setDNSPrefetchingEnabled(m_dnsPrefetchingEnabled);
    m_page->settings().setDownloadableBinaryFontsEnabled(m_downloadableBinaryFontsEnabled);
    m_page->settings().setForceZeroLayoutHeight(m_forceZeroLayoutHeight);
    m_page->settings().setFullscreenSupported(m_fullscreenSupported);
    m_page->settings().setHyperlinkAuditingEnabled(m_hyperlinkAuditingEnabled);
    m_page->settings().setIgnoreMainFrameOverflowHiddenQuirk(m_ignoreMainFrameOverflowHiddenQuirk);
    m_page->settings().setImagesEnabled(m_imagesEnabled);
    m_page->settings().setJavaEnabled(m_javaEnabled);
    m_page->settings().setJavaScriptCanAccessClipboard(m_javaScriptCanAccessClipboard);
    m_page->settings().setJavaScriptCanOpenWindowsAutomatically(m_javaScriptCanOpenWindowsAutomatically);
    m_page->settings().setLayerSquashingEnabled(m_layerSquashingEnabled);
    m_page->settings().setLoadWithOverviewMode(m_loadWithOverviewMode);
    m_page->settings().setLoadsImagesAutomatically(m_loadsImagesAutomatically);
    m_page->settings().setLocalStorageEnabled(m_localStorageEnabled);
    m_page->settings().setMainFrameClipsContent(m_mainFrameClipsContent);
    m_page->settings().setMaxBeaconTransmission(m_maxBeaconTransmission);
    m_page->settings().setMaxTouchPoints(m_maxTouchPoints);
    m_page->settings().setMediaControlsOverlayPlayButtonEnabled(m_mediaControlsOverlayPlayButtonEnabled);
    m_page->settings().setMediaPlaybackRequiresUserGesture(m_mediaPlaybackRequiresUserGesture);
    m_page->settings().setMediaTypeOverride(m_mediaTypeOverride);
    m_page->settings().setMinimumAccelerated2dCanvasSize(m_minimumAccelerated2dCanvasSize);
    m_page->settings().setMinimumFontSize(m_minimumFontSize);
    m_page->settings().setMinimumLogicalFontSize(m_minimumLogicalFontSize);
    m_page->settings().setNavigateOnDragDrop(m_navigateOnDragDrop);
    m_page->settings().setNeedsSiteSpecificQuirks(m_needsSiteSpecificQuirks);
    m_page->settings().setOfflineWebApplicationCacheEnabled(m_offlineWebApplicationCacheEnabled);
    m_page->settings().setPageCacheSupportsPlugins(m_pageCacheSupportsPlugins);
    m_page->settings().setPasswordEchoDurationInSeconds(m_passwordEchoDurationInSeconds);
    m_page->settings().setPasswordEchoEnabled(m_passwordEchoEnabled);
    m_page->settings().setPinchOverlayScrollbarThickness(m_pinchOverlayScrollbarThickness);
    m_page->settings().setPinchVirtualViewportEnabled(m_pinchVirtualViewportEnabled);
    m_page->settings().setPluginsEnabled(m_pluginsEnabled);
    m_page->settings().setRegionBasedColumnsEnabled(m_regionBasedColumnsEnabled);
    m_page->settings().setReportScreenSizeInPhysicalPixelsQuirk(m_reportScreenSizeInPhysicalPixelsQuirk);
    m_page->settings().setScriptEnabled(m_scriptEnabled);
    m_page->settings().setScrollAnimatorEnabled(m_scrollAnimatorEnabled);
    m_page->settings().setSelectTrailingWhitespaceEnabled(m_selectTrailingWhitespaceEnabled);
    m_page->settings().setSelectionIncludesAltImageText(m_selectionIncludesAltImageText);
    m_page->settings().setShouldClearDocumentBackground(m_shouldClearDocumentBackground);
    m_page->settings().setShouldPrintBackgrounds(m_shouldPrintBackgrounds);
    m_page->settings().setShouldRespectImageOrientation(m_shouldRespectImageOrientation);
    m_page->settings().setShrinksStandaloneImagesToFit(m_shrinksStandaloneImagesToFit);
    m_page->settings().setSmartInsertDeleteEnabled(m_smartInsertDeleteEnabled);
    m_page->settings().setSpatialNavigationEnabled(m_spatialNavigationEnabled);
    m_page->settings().setSupportsMultipleWindows(m_supportsMultipleWindows);
    m_page->settings().setSyncXHRInDocumentsEnabled(m_syncXHRInDocumentsEnabled);
    m_page->settings().setTextAreasAreResizable(m_textAreasAreResizable);
    m_page->settings().setTouchAdjustmentEnabled(m_touchAdjustmentEnabled);
    m_page->settings().setTouchDragDropEnabled(m_touchDragDropEnabled);
    m_page->settings().setTouchEditingEnabled(m_touchEditingEnabled);
    m_page->settings().setTreatIPAddressAsDomain(m_treatIPAddressAsDomain);
    m_page->settings().setUnifiedTextCheckerEnabled(m_unifiedTextCheckerEnabled);
    m_page->settings().setUnsafePluginPastingEnabled(m_unsafePluginPastingEnabled);
    m_page->settings().setUseLegacyBackgroundSizeShorthandBehavior(m_useLegacyBackgroundSizeShorthandBehavior);
    m_page->settings().setUseSolidColorScrollbars(m_useSolidColorScrollbars);
    m_page->settings().setUseWideViewport(m_useWideViewport);
    m_page->settings().setUsesEncodingDetector(m_usesEncodingDetector);
    m_page->settings().setValidationMessageTimerMagnification(m_validationMessageTimerMagnification);
    m_page->settings().setViewportEnabled(m_viewportEnabled);
    m_page->settings().setViewportMetaEnabled(m_viewportMetaEnabled);
    m_page->settings().setViewportMetaMergeContentQuirk(m_viewportMetaMergeContentQuirk);
    m_page->settings().setViewportMetaZeroValuesQuirk(m_viewportMetaZeroValuesQuirk);
    m_page->settings().setWebAudioEnabled(m_webAudioEnabled);
    m_page->settings().setWebGLEnabled(m_webGLEnabled);
    m_page->settings().setWebGLErrorsToConsoleEnabled(m_webGLErrorsToConsoleEnabled);
    m_page->settings().setWebSecurityEnabled(m_webSecurityEnabled);
    m_page->settings().setWideViewportQuirkEnabled(m_wideViewportQuirkEnabled);
    m_page->settings().setXSSAuditorEnabled(m_xssAuditorEnabled);
}

void InternalSettingsGenerated::setDOMPasteAllowed(bool DOMPasteAllowed)
{
    m_page->settings().setDOMPasteAllowed(DOMPasteAllowed);
}

void InternalSettingsGenerated::setAccelerated2dCanvasEnabled(bool accelerated2dCanvasEnabled)
{
    m_page->settings().setAccelerated2dCanvasEnabled(accelerated2dCanvasEnabled);
}

void InternalSettingsGenerated::setAccelerated2dCanvasMSAASampleCount(int accelerated2dCanvasMSAASampleCount)
{
    m_page->settings().setAccelerated2dCanvasMSAASampleCount(accelerated2dCanvasMSAASampleCount);
}

void InternalSettingsGenerated::setAcceleratedCompositingEnabled(bool acceleratedCompositingEnabled)
{
    m_page->settings().setAcceleratedCompositingEnabled(acceleratedCompositingEnabled);
}

void InternalSettingsGenerated::setAcceleratedCompositingForCanvasEnabled(bool acceleratedCompositingForCanvasEnabled)
{
    m_page->settings().setAcceleratedCompositingForCanvasEnabled(acceleratedCompositingForCanvasEnabled);
}

void InternalSettingsGenerated::setAcceleratedCompositingForFiltersEnabled(bool acceleratedCompositingForFiltersEnabled)
{
    m_page->settings().setAcceleratedCompositingForFiltersEnabled(acceleratedCompositingForFiltersEnabled);
}

void InternalSettingsGenerated::setAcceleratedCompositingForFixedPositionEnabled(bool acceleratedCompositingForFixedPositionEnabled)
{
    m_page->settings().setAcceleratedCompositingForFixedPositionEnabled(acceleratedCompositingForFixedPositionEnabled);
}

void InternalSettingsGenerated::setAcceleratedCompositingForFixedRootBackgroundEnabled(bool acceleratedCompositingForFixedRootBackgroundEnabled)
{
    m_page->settings().setAcceleratedCompositingForFixedRootBackgroundEnabled(acceleratedCompositingForFixedRootBackgroundEnabled);
}

void InternalSettingsGenerated::setAcceleratedCompositingForOverflowScrollEnabled(bool acceleratedCompositingForOverflowScrollEnabled)
{
    m_page->settings().setAcceleratedCompositingForOverflowScrollEnabled(acceleratedCompositingForOverflowScrollEnabled);
}

void InternalSettingsGenerated::setAcceleratedCompositingForVideoEnabled(bool acceleratedCompositingForVideoEnabled)
{
    m_page->settings().setAcceleratedCompositingForVideoEnabled(acceleratedCompositingForVideoEnabled);
}

void InternalSettingsGenerated::setAccessibilityFontScaleFactor(double accessibilityFontScaleFactor)
{
    m_page->settings().setAccessibilityFontScaleFactor(accessibilityFontScaleFactor);
}

void InternalSettingsGenerated::setAllowConnectingInsecureWebSocket(bool allowConnectingInsecureWebSocket)
{
    m_page->settings().setAllowConnectingInsecureWebSocket(allowConnectingInsecureWebSocket);
}

void InternalSettingsGenerated::setAllowCustomScrollbarInMainFrame(bool allowCustomScrollbarInMainFrame)
{
    m_page->settings().setAllowCustomScrollbarInMainFrame(allowCustomScrollbarInMainFrame);
}

void InternalSettingsGenerated::setAllowDisplayOfInsecureContent(bool allowDisplayOfInsecureContent)
{
    m_page->settings().setAllowDisplayOfInsecureContent(allowDisplayOfInsecureContent);
}

void InternalSettingsGenerated::setAllowFileAccessFromFileURLs(bool allowFileAccessFromFileURLs)
{
    m_page->settings().setAllowFileAccessFromFileURLs(allowFileAccessFromFileURLs);
}

void InternalSettingsGenerated::setAllowRunningOfInsecureContent(bool allowRunningOfInsecureContent)
{
    m_page->settings().setAllowRunningOfInsecureContent(allowRunningOfInsecureContent);
}

void InternalSettingsGenerated::setAllowScriptsToCloseWindows(bool allowScriptsToCloseWindows)
{
    m_page->settings().setAllowScriptsToCloseWindows(allowScriptsToCloseWindows);
}

void InternalSettingsGenerated::setAllowUniversalAccessFromFileURLs(bool allowUniversalAccessFromFileURLs)
{
    m_page->settings().setAllowUniversalAccessFromFileURLs(allowUniversalAccessFromFileURLs);
}

void InternalSettingsGenerated::setAntialiased2dCanvasEnabled(bool antialiased2dCanvasEnabled)
{
    m_page->settings().setAntialiased2dCanvasEnabled(antialiased2dCanvasEnabled);
}

void InternalSettingsGenerated::setAsynchronousSpellCheckingEnabled(bool asynchronousSpellCheckingEnabled)
{
    m_page->settings().setAsynchronousSpellCheckingEnabled(asynchronousSpellCheckingEnabled);
}

void InternalSettingsGenerated::setCaretBrowsingEnabled(bool caretBrowsingEnabled)
{
    m_page->settings().setCaretBrowsingEnabled(caretBrowsingEnabled);
}

void InternalSettingsGenerated::setCompositedScrollingForFramesEnabled(bool compositedScrollingForFramesEnabled)
{
    m_page->settings().setCompositedScrollingForFramesEnabled(compositedScrollingForFramesEnabled);
}

void InternalSettingsGenerated::setCompositorDrivenAcceleratedScrollingEnabled(bool compositorDrivenAcceleratedScrollingEnabled)
{
    m_page->settings().setCompositorDrivenAcceleratedScrollingEnabled(compositorDrivenAcceleratedScrollingEnabled);
}

void InternalSettingsGenerated::setCompositorTouchHitTesting(bool compositorTouchHitTesting)
{
    m_page->settings().setCompositorTouchHitTesting(compositorTouchHitTesting);
}

void InternalSettingsGenerated::setContainerCullingEnabled(bool containerCullingEnabled)
{
    m_page->settings().setContainerCullingEnabled(containerCullingEnabled);
}

void InternalSettingsGenerated::setCookieEnabled(bool cookieEnabled)
{
    m_page->settings().setCookieEnabled(cookieEnabled);
}

void InternalSettingsGenerated::setDefaultFixedFontSize(int defaultFixedFontSize)
{
    m_page->settings().setDefaultFixedFontSize(defaultFixedFontSize);
}

void InternalSettingsGenerated::setDefaultFontSize(int defaultFontSize)
{
    m_page->settings().setDefaultFontSize(defaultFontSize);
}

void InternalSettingsGenerated::setDefaultTextEncodingName(const String& defaultTextEncodingName)
{
    m_page->settings().setDefaultTextEncodingName(defaultTextEncodingName);
}

void InternalSettingsGenerated::setDefaultVideoPosterURL(const String& defaultVideoPosterURL)
{
    m_page->settings().setDefaultVideoPosterURL(defaultVideoPosterURL);
}

void InternalSettingsGenerated::setDeferredFiltersEnabled(bool deferredFiltersEnabled)
{
    m_page->settings().setDeferredFiltersEnabled(deferredFiltersEnabled);
}

void InternalSettingsGenerated::setDeviceScaleAdjustment(double deviceScaleAdjustment)
{
    m_page->settings().setDeviceScaleAdjustment(deviceScaleAdjustment);
}

void InternalSettingsGenerated::setDeviceSupportsMouse(bool deviceSupportsMouse)
{
    m_page->settings().setDeviceSupportsMouse(deviceSupportsMouse);
}

void InternalSettingsGenerated::setDeviceSupportsTouch(bool deviceSupportsTouch)
{
    m_page->settings().setDeviceSupportsTouch(deviceSupportsTouch);
}

void InternalSettingsGenerated::setDisallowFullscreenForNonMediaElements(bool disallowFullscreenForNonMediaElements)
{
    m_page->settings().setDisallowFullscreenForNonMediaElements(disallowFullscreenForNonMediaElements);
}

void InternalSettingsGenerated::setDNSPrefetchingEnabled(bool dnsPrefetchingEnabled)
{
    m_page->settings().setDNSPrefetchingEnabled(dnsPrefetchingEnabled);
}

void InternalSettingsGenerated::setDownloadableBinaryFontsEnabled(bool downloadableBinaryFontsEnabled)
{
    m_page->settings().setDownloadableBinaryFontsEnabled(downloadableBinaryFontsEnabled);
}

void InternalSettingsGenerated::setForceZeroLayoutHeight(bool forceZeroLayoutHeight)
{
    m_page->settings().setForceZeroLayoutHeight(forceZeroLayoutHeight);
}

void InternalSettingsGenerated::setFullscreenSupported(bool fullscreenSupported)
{
    m_page->settings().setFullscreenSupported(fullscreenSupported);
}

void InternalSettingsGenerated::setHyperlinkAuditingEnabled(bool hyperlinkAuditingEnabled)
{
    m_page->settings().setHyperlinkAuditingEnabled(hyperlinkAuditingEnabled);
}

void InternalSettingsGenerated::setIgnoreMainFrameOverflowHiddenQuirk(bool ignoreMainFrameOverflowHiddenQuirk)
{
    m_page->settings().setIgnoreMainFrameOverflowHiddenQuirk(ignoreMainFrameOverflowHiddenQuirk);
}

void InternalSettingsGenerated::setImagesEnabled(bool imagesEnabled)
{
    m_page->settings().setImagesEnabled(imagesEnabled);
}

void InternalSettingsGenerated::setJavaEnabled(bool javaEnabled)
{
    m_page->settings().setJavaEnabled(javaEnabled);
}

void InternalSettingsGenerated::setJavaScriptCanAccessClipboard(bool javaScriptCanAccessClipboard)
{
    m_page->settings().setJavaScriptCanAccessClipboard(javaScriptCanAccessClipboard);
}

void InternalSettingsGenerated::setJavaScriptCanOpenWindowsAutomatically(bool javaScriptCanOpenWindowsAutomatically)
{
    m_page->settings().setJavaScriptCanOpenWindowsAutomatically(javaScriptCanOpenWindowsAutomatically);
}

void InternalSettingsGenerated::setLayerSquashingEnabled(bool layerSquashingEnabled)
{
    m_page->settings().setLayerSquashingEnabled(layerSquashingEnabled);
}

void InternalSettingsGenerated::setLoadWithOverviewMode(bool loadWithOverviewMode)
{
    m_page->settings().setLoadWithOverviewMode(loadWithOverviewMode);
}

void InternalSettingsGenerated::setLoadsImagesAutomatically(bool loadsImagesAutomatically)
{
    m_page->settings().setLoadsImagesAutomatically(loadsImagesAutomatically);
}

void InternalSettingsGenerated::setLocalStorageEnabled(bool localStorageEnabled)
{
    m_page->settings().setLocalStorageEnabled(localStorageEnabled);
}

void InternalSettingsGenerated::setMainFrameClipsContent(bool mainFrameClipsContent)
{
    m_page->settings().setMainFrameClipsContent(mainFrameClipsContent);
}

void InternalSettingsGenerated::setMaxBeaconTransmission(int maxBeaconTransmission)
{
    m_page->settings().setMaxBeaconTransmission(maxBeaconTransmission);
}

void InternalSettingsGenerated::setMaxTouchPoints(int maxTouchPoints)
{
    m_page->settings().setMaxTouchPoints(maxTouchPoints);
}

void InternalSettingsGenerated::setMediaControlsOverlayPlayButtonEnabled(bool mediaControlsOverlayPlayButtonEnabled)
{
    m_page->settings().setMediaControlsOverlayPlayButtonEnabled(mediaControlsOverlayPlayButtonEnabled);
}

void InternalSettingsGenerated::setMediaPlaybackRequiresUserGesture(bool mediaPlaybackRequiresUserGesture)
{
    m_page->settings().setMediaPlaybackRequiresUserGesture(mediaPlaybackRequiresUserGesture);
}

void InternalSettingsGenerated::setMediaTypeOverride(const String& mediaTypeOverride)
{
    m_page->settings().setMediaTypeOverride(mediaTypeOverride);
}

void InternalSettingsGenerated::setMinimumAccelerated2dCanvasSize(int minimumAccelerated2dCanvasSize)
{
    m_page->settings().setMinimumAccelerated2dCanvasSize(minimumAccelerated2dCanvasSize);
}

void InternalSettingsGenerated::setMinimumFontSize(int minimumFontSize)
{
    m_page->settings().setMinimumFontSize(minimumFontSize);
}

void InternalSettingsGenerated::setMinimumLogicalFontSize(int minimumLogicalFontSize)
{
    m_page->settings().setMinimumLogicalFontSize(minimumLogicalFontSize);
}

void InternalSettingsGenerated::setNavigateOnDragDrop(bool navigateOnDragDrop)
{
    m_page->settings().setNavigateOnDragDrop(navigateOnDragDrop);
}

void InternalSettingsGenerated::setNeedsSiteSpecificQuirks(bool needsSiteSpecificQuirks)
{
    m_page->settings().setNeedsSiteSpecificQuirks(needsSiteSpecificQuirks);
}

void InternalSettingsGenerated::setOfflineWebApplicationCacheEnabled(bool offlineWebApplicationCacheEnabled)
{
    m_page->settings().setOfflineWebApplicationCacheEnabled(offlineWebApplicationCacheEnabled);
}

void InternalSettingsGenerated::setPageCacheSupportsPlugins(bool pageCacheSupportsPlugins)
{
    m_page->settings().setPageCacheSupportsPlugins(pageCacheSupportsPlugins);
}

void InternalSettingsGenerated::setPasswordEchoDurationInSeconds(double passwordEchoDurationInSeconds)
{
    m_page->settings().setPasswordEchoDurationInSeconds(passwordEchoDurationInSeconds);
}

void InternalSettingsGenerated::setPasswordEchoEnabled(bool passwordEchoEnabled)
{
    m_page->settings().setPasswordEchoEnabled(passwordEchoEnabled);
}

void InternalSettingsGenerated::setPinchOverlayScrollbarThickness(int pinchOverlayScrollbarThickness)
{
    m_page->settings().setPinchOverlayScrollbarThickness(pinchOverlayScrollbarThickness);
}

void InternalSettingsGenerated::setPinchVirtualViewportEnabled(bool pinchVirtualViewportEnabled)
{
    m_page->settings().setPinchVirtualViewportEnabled(pinchVirtualViewportEnabled);
}

void InternalSettingsGenerated::setPluginsEnabled(bool pluginsEnabled)
{
    m_page->settings().setPluginsEnabled(pluginsEnabled);
}

void InternalSettingsGenerated::setRegionBasedColumnsEnabled(bool regionBasedColumnsEnabled)
{
    m_page->settings().setRegionBasedColumnsEnabled(regionBasedColumnsEnabled);
}

void InternalSettingsGenerated::setReportScreenSizeInPhysicalPixelsQuirk(bool reportScreenSizeInPhysicalPixelsQuirk)
{
    m_page->settings().setReportScreenSizeInPhysicalPixelsQuirk(reportScreenSizeInPhysicalPixelsQuirk);
}

void InternalSettingsGenerated::setScriptEnabled(bool scriptEnabled)
{
    m_page->settings().setScriptEnabled(scriptEnabled);
}

void InternalSettingsGenerated::setScrollAnimatorEnabled(bool scrollAnimatorEnabled)
{
    m_page->settings().setScrollAnimatorEnabled(scrollAnimatorEnabled);
}

void InternalSettingsGenerated::setSelectTrailingWhitespaceEnabled(bool selectTrailingWhitespaceEnabled)
{
    m_page->settings().setSelectTrailingWhitespaceEnabled(selectTrailingWhitespaceEnabled);
}

void InternalSettingsGenerated::setSelectionIncludesAltImageText(bool selectionIncludesAltImageText)
{
    m_page->settings().setSelectionIncludesAltImageText(selectionIncludesAltImageText);
}

void InternalSettingsGenerated::setShouldClearDocumentBackground(bool shouldClearDocumentBackground)
{
    m_page->settings().setShouldClearDocumentBackground(shouldClearDocumentBackground);
}

void InternalSettingsGenerated::setShouldPrintBackgrounds(bool shouldPrintBackgrounds)
{
    m_page->settings().setShouldPrintBackgrounds(shouldPrintBackgrounds);
}

void InternalSettingsGenerated::setShouldRespectImageOrientation(bool shouldRespectImageOrientation)
{
    m_page->settings().setShouldRespectImageOrientation(shouldRespectImageOrientation);
}

void InternalSettingsGenerated::setShrinksStandaloneImagesToFit(bool shrinksStandaloneImagesToFit)
{
    m_page->settings().setShrinksStandaloneImagesToFit(shrinksStandaloneImagesToFit);
}

void InternalSettingsGenerated::setSmartInsertDeleteEnabled(bool smartInsertDeleteEnabled)
{
    m_page->settings().setSmartInsertDeleteEnabled(smartInsertDeleteEnabled);
}

void InternalSettingsGenerated::setSpatialNavigationEnabled(bool spatialNavigationEnabled)
{
    m_page->settings().setSpatialNavigationEnabled(spatialNavigationEnabled);
}

void InternalSettingsGenerated::setSupportsMultipleWindows(bool supportsMultipleWindows)
{
    m_page->settings().setSupportsMultipleWindows(supportsMultipleWindows);
}

void InternalSettingsGenerated::setSyncXHRInDocumentsEnabled(bool syncXHRInDocumentsEnabled)
{
    m_page->settings().setSyncXHRInDocumentsEnabled(syncXHRInDocumentsEnabled);
}

void InternalSettingsGenerated::setTextAreasAreResizable(bool textAreasAreResizable)
{
    m_page->settings().setTextAreasAreResizable(textAreasAreResizable);
}

void InternalSettingsGenerated::setTouchAdjustmentEnabled(bool touchAdjustmentEnabled)
{
    m_page->settings().setTouchAdjustmentEnabled(touchAdjustmentEnabled);
}

void InternalSettingsGenerated::setTouchDragDropEnabled(bool touchDragDropEnabled)
{
    m_page->settings().setTouchDragDropEnabled(touchDragDropEnabled);
}

void InternalSettingsGenerated::setTouchEditingEnabled(bool touchEditingEnabled)
{
    m_page->settings().setTouchEditingEnabled(touchEditingEnabled);
}

void InternalSettingsGenerated::setTreatIPAddressAsDomain(bool treatIPAddressAsDomain)
{
    m_page->settings().setTreatIPAddressAsDomain(treatIPAddressAsDomain);
}

void InternalSettingsGenerated::setUnifiedTextCheckerEnabled(bool unifiedTextCheckerEnabled)
{
    m_page->settings().setUnifiedTextCheckerEnabled(unifiedTextCheckerEnabled);
}

void InternalSettingsGenerated::setUnsafePluginPastingEnabled(bool unsafePluginPastingEnabled)
{
    m_page->settings().setUnsafePluginPastingEnabled(unsafePluginPastingEnabled);
}

void InternalSettingsGenerated::setUseLegacyBackgroundSizeShorthandBehavior(bool useLegacyBackgroundSizeShorthandBehavior)
{
    m_page->settings().setUseLegacyBackgroundSizeShorthandBehavior(useLegacyBackgroundSizeShorthandBehavior);
}

void InternalSettingsGenerated::setUseSolidColorScrollbars(bool useSolidColorScrollbars)
{
    m_page->settings().setUseSolidColorScrollbars(useSolidColorScrollbars);
}

void InternalSettingsGenerated::setUseWideViewport(bool useWideViewport)
{
    m_page->settings().setUseWideViewport(useWideViewport);
}

void InternalSettingsGenerated::setUsesEncodingDetector(bool usesEncodingDetector)
{
    m_page->settings().setUsesEncodingDetector(usesEncodingDetector);
}

void InternalSettingsGenerated::setValidationMessageTimerMagnification(int validationMessageTimerMagnification)
{
    m_page->settings().setValidationMessageTimerMagnification(validationMessageTimerMagnification);
}

void InternalSettingsGenerated::setViewportEnabled(bool viewportEnabled)
{
    m_page->settings().setViewportEnabled(viewportEnabled);
}

void InternalSettingsGenerated::setViewportMetaEnabled(bool viewportMetaEnabled)
{
    m_page->settings().setViewportMetaEnabled(viewportMetaEnabled);
}

void InternalSettingsGenerated::setViewportMetaMergeContentQuirk(bool viewportMetaMergeContentQuirk)
{
    m_page->settings().setViewportMetaMergeContentQuirk(viewportMetaMergeContentQuirk);
}

void InternalSettingsGenerated::setViewportMetaZeroValuesQuirk(bool viewportMetaZeroValuesQuirk)
{
    m_page->settings().setViewportMetaZeroValuesQuirk(viewportMetaZeroValuesQuirk);
}

void InternalSettingsGenerated::setWebAudioEnabled(bool webAudioEnabled)
{
    m_page->settings().setWebAudioEnabled(webAudioEnabled);
}

void InternalSettingsGenerated::setWebGLEnabled(bool webGLEnabled)
{
    m_page->settings().setWebGLEnabled(webGLEnabled);
}

void InternalSettingsGenerated::setWebGLErrorsToConsoleEnabled(bool webGLErrorsToConsoleEnabled)
{
    m_page->settings().setWebGLErrorsToConsoleEnabled(webGLErrorsToConsoleEnabled);
}

void InternalSettingsGenerated::setWebSecurityEnabled(bool webSecurityEnabled)
{
    m_page->settings().setWebSecurityEnabled(webSecurityEnabled);
}

void InternalSettingsGenerated::setWideViewportQuirkEnabled(bool wideViewportQuirkEnabled)
{
    m_page->settings().setWideViewportQuirkEnabled(wideViewportQuirkEnabled);
}

void InternalSettingsGenerated::setXSSAuditorEnabled(bool xssAuditorEnabled)
{
    m_page->settings().setXSSAuditorEnabled(xssAuditorEnabled);
}

} // namespace WebCore
