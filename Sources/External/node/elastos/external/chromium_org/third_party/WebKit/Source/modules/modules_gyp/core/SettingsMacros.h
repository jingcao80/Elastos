// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SettingsMacros_h
#define SettingsMacros_h

#define SETTINGS_GETTERS_AND_SETTERS \
    bool DOMPasteAllowed() const { return m_DOMPasteAllowed; } \
    void setDOMPasteAllowed(bool DOMPasteAllowed); \
    bool accelerated2dCanvasEnabled() const { return m_accelerated2dCanvasEnabled; } \
    void setAccelerated2dCanvasEnabled(bool accelerated2dCanvasEnabled); \
    int accelerated2dCanvasMSAASampleCount() const { return m_accelerated2dCanvasMSAASampleCount; } \
    void setAccelerated2dCanvasMSAASampleCount(int accelerated2dCanvasMSAASampleCount); \
    bool acceleratedCompositingEnabled() const { return m_acceleratedCompositingEnabled; } \
    void setAcceleratedCompositingEnabled(bool acceleratedCompositingEnabled); \
    bool acceleratedCompositingForCanvasEnabled() const { return m_acceleratedCompositingForCanvasEnabled; } \
    void setAcceleratedCompositingForCanvasEnabled(bool acceleratedCompositingForCanvasEnabled); \
    bool acceleratedCompositingForFiltersEnabled() const { return m_acceleratedCompositingForFiltersEnabled; } \
    void setAcceleratedCompositingForFiltersEnabled(bool acceleratedCompositingForFiltersEnabled); \
    bool acceleratedCompositingForFixedPositionEnabled() const { return m_acceleratedCompositingForFixedPositionEnabled; } \
    void setAcceleratedCompositingForFixedPositionEnabled(bool acceleratedCompositingForFixedPositionEnabled); \
    bool acceleratedCompositingForFixedRootBackgroundEnabled() const { return m_acceleratedCompositingForFixedRootBackgroundEnabled; } \
    void setAcceleratedCompositingForFixedRootBackgroundEnabled(bool acceleratedCompositingForFixedRootBackgroundEnabled); \
    bool acceleratedCompositingForOverflowScrollEnabled() const { return m_acceleratedCompositingForOverflowScrollEnabled; } \
    void setAcceleratedCompositingForOverflowScrollEnabled(bool acceleratedCompositingForOverflowScrollEnabled); \
    bool acceleratedCompositingForVideoEnabled() const { return m_acceleratedCompositingForVideoEnabled; } \
    void setAcceleratedCompositingForVideoEnabled(bool acceleratedCompositingForVideoEnabled); \
    double accessibilityFontScaleFactor() const { return m_accessibilityFontScaleFactor; } \
    void setAccessibilityFontScaleFactor(double accessibilityFontScaleFactor); \
    bool allowConnectingInsecureWebSocket() const { return m_allowConnectingInsecureWebSocket; } \
    void setAllowConnectingInsecureWebSocket(bool allowConnectingInsecureWebSocket); \
    bool allowCustomScrollbarInMainFrame() const { return m_allowCustomScrollbarInMainFrame; } \
    void setAllowCustomScrollbarInMainFrame(bool allowCustomScrollbarInMainFrame); \
    bool allowDisplayOfInsecureContent() const { return m_allowDisplayOfInsecureContent; } \
    void setAllowDisplayOfInsecureContent(bool allowDisplayOfInsecureContent); \
    bool allowFileAccessFromFileURLs() const { return m_allowFileAccessFromFileURLs; } \
    void setAllowFileAccessFromFileURLs(bool allowFileAccessFromFileURLs); \
    bool allowRunningOfInsecureContent() const { return m_allowRunningOfInsecureContent; } \
    void setAllowRunningOfInsecureContent(bool allowRunningOfInsecureContent); \
    bool allowScriptsToCloseWindows() const { return m_allowScriptsToCloseWindows; } \
    void setAllowScriptsToCloseWindows(bool allowScriptsToCloseWindows); \
    bool allowUniversalAccessFromFileURLs() const { return m_allowUniversalAccessFromFileURLs; } \
    void setAllowUniversalAccessFromFileURLs(bool allowUniversalAccessFromFileURLs); \
    bool antialiased2dCanvasEnabled() const { return m_antialiased2dCanvasEnabled; } \
    void setAntialiased2dCanvasEnabled(bool antialiased2dCanvasEnabled); \
    bool asynchronousSpellCheckingEnabled() const { return m_asynchronousSpellCheckingEnabled; } \
    void setAsynchronousSpellCheckingEnabled(bool asynchronousSpellCheckingEnabled); \
    bool caretBrowsingEnabled() const { return m_caretBrowsingEnabled; } \
    void setCaretBrowsingEnabled(bool caretBrowsingEnabled); \
    bool compositedScrollingForFramesEnabled() const { return m_compositedScrollingForFramesEnabled; } \
    void setCompositedScrollingForFramesEnabled(bool compositedScrollingForFramesEnabled); \
    bool compositorDrivenAcceleratedScrollingEnabled() const { return m_compositorDrivenAcceleratedScrollingEnabled; } \
    void setCompositorDrivenAcceleratedScrollingEnabled(bool compositorDrivenAcceleratedScrollingEnabled); \
    bool compositorTouchHitTesting() const { return m_compositorTouchHitTesting; } \
    void setCompositorTouchHitTesting(bool compositorTouchHitTesting); \
    bool containerCullingEnabled() const { return m_containerCullingEnabled; } \
    void setContainerCullingEnabled(bool containerCullingEnabled); \
    bool cookieEnabled() const { return m_cookieEnabled; } \
    void setCookieEnabled(bool cookieEnabled); \
    int defaultFixedFontSize() const { return m_defaultFixedFontSize; } \
    void setDefaultFixedFontSize(int defaultFixedFontSize); \
    int defaultFontSize() const { return m_defaultFontSize; } \
    void setDefaultFontSize(int defaultFontSize); \
    const String& defaultTextEncodingName() const { return m_defaultTextEncodingName; } \
    void setDefaultTextEncodingName(const String& defaultTextEncodingName); \
    const String& defaultVideoPosterURL() const { return m_defaultVideoPosterURL; } \
    void setDefaultVideoPosterURL(const String& defaultVideoPosterURL); \
    bool deferredFiltersEnabled() const { return m_deferredFiltersEnabled; } \
    void setDeferredFiltersEnabled(bool deferredFiltersEnabled); \
    double deviceScaleAdjustment() const { return m_deviceScaleAdjustment; } \
    void setDeviceScaleAdjustment(double deviceScaleAdjustment); \
    bool deviceSupportsMouse() const { return m_deviceSupportsMouse; } \
    void setDeviceSupportsMouse(bool deviceSupportsMouse); \
    bool deviceSupportsTouch() const { return m_deviceSupportsTouch; } \
    void setDeviceSupportsTouch(bool deviceSupportsTouch); \
    bool disallowFullscreenForNonMediaElements() const { return m_disallowFullscreenForNonMediaElements; } \
    void setDisallowFullscreenForNonMediaElements(bool disallowFullscreenForNonMediaElements); \
    bool dnsPrefetchingEnabled() const { return m_dnsPrefetchingEnabled; } \
    void setDNSPrefetchingEnabled(bool dnsPrefetchingEnabled); \
    bool downloadableBinaryFontsEnabled() const { return m_downloadableBinaryFontsEnabled; } \
    void setDownloadableBinaryFontsEnabled(bool downloadableBinaryFontsEnabled); \
    EditingBehaviorType editingBehaviorType() const { return m_editingBehaviorType; } \
    void setEditingBehaviorType(EditingBehaviorType editingBehaviorType); \
    bool forceZeroLayoutHeight() const { return m_forceZeroLayoutHeight; } \
    void setForceZeroLayoutHeight(bool forceZeroLayoutHeight); \
    bool fullscreenSupported() const { return m_fullscreenSupported; } \
    void setFullscreenSupported(bool fullscreenSupported); \
    bool hyperlinkAuditingEnabled() const { return m_hyperlinkAuditingEnabled; } \
    void setHyperlinkAuditingEnabled(bool hyperlinkAuditingEnabled); \
    bool ignoreMainFrameOverflowHiddenQuirk() const { return m_ignoreMainFrameOverflowHiddenQuirk; } \
    void setIgnoreMainFrameOverflowHiddenQuirk(bool ignoreMainFrameOverflowHiddenQuirk); \
    bool imagesEnabled() const { return m_imagesEnabled; } \
    void setImagesEnabled(bool imagesEnabled); \
    bool javaEnabled() const { return m_javaEnabled; } \
    void setJavaEnabled(bool javaEnabled); \
    bool javaScriptCanAccessClipboard() const { return m_javaScriptCanAccessClipboard; } \
    void setJavaScriptCanAccessClipboard(bool javaScriptCanAccessClipboard); \
    bool javaScriptCanOpenWindowsAutomatically() const { return m_javaScriptCanOpenWindowsAutomatically; } \
    void setJavaScriptCanOpenWindowsAutomatically(bool javaScriptCanOpenWindowsAutomatically); \
    bool layerSquashingEnabled() const { return m_layerSquashingEnabled; } \
    void setLayerSquashingEnabled(bool layerSquashingEnabled); \
    bool loadWithOverviewMode() const { return m_loadWithOverviewMode; } \
    void setLoadWithOverviewMode(bool loadWithOverviewMode); \
    bool loadsImagesAutomatically() const { return m_loadsImagesAutomatically; } \
    void setLoadsImagesAutomatically(bool loadsImagesAutomatically); \
    bool localStorageEnabled() const { return m_localStorageEnabled; } \
    void setLocalStorageEnabled(bool localStorageEnabled); \
    bool mainFrameClipsContent() const { return m_mainFrameClipsContent; } \
    void setMainFrameClipsContent(bool mainFrameClipsContent); \
    int maxBeaconTransmission() const { return m_maxBeaconTransmission; } \
    void setMaxBeaconTransmission(int maxBeaconTransmission); \
    int maxTouchPoints() const { return m_maxTouchPoints; } \
    void setMaxTouchPoints(int maxTouchPoints); \
    bool mediaControlsOverlayPlayButtonEnabled() const { return m_mediaControlsOverlayPlayButtonEnabled; } \
    void setMediaControlsOverlayPlayButtonEnabled(bool mediaControlsOverlayPlayButtonEnabled); \
    bool mediaPlaybackRequiresUserGesture() const { return m_mediaPlaybackRequiresUserGesture; } \
    void setMediaPlaybackRequiresUserGesture(bool mediaPlaybackRequiresUserGesture); \
    const String& mediaTypeOverride() const { return m_mediaTypeOverride; } \
    void setMediaTypeOverride(const String& mediaTypeOverride); \
    int minimumAccelerated2dCanvasSize() const { return m_minimumAccelerated2dCanvasSize; } \
    void setMinimumAccelerated2dCanvasSize(int minimumAccelerated2dCanvasSize); \
    int minimumFontSize() const { return m_minimumFontSize; } \
    void setMinimumFontSize(int minimumFontSize); \
    int minimumLogicalFontSize() const { return m_minimumLogicalFontSize; } \
    void setMinimumLogicalFontSize(int minimumLogicalFontSize); \
    bool navigateOnDragDrop() const { return m_navigateOnDragDrop; } \
    void setNavigateOnDragDrop(bool navigateOnDragDrop); \
    bool needsSiteSpecificQuirks() const { return m_needsSiteSpecificQuirks; } \
    void setNeedsSiteSpecificQuirks(bool needsSiteSpecificQuirks); \
    bool offlineWebApplicationCacheEnabled() const { return m_offlineWebApplicationCacheEnabled; } \
    void setOfflineWebApplicationCacheEnabled(bool offlineWebApplicationCacheEnabled); \
    bool pageCacheSupportsPlugins() const { return m_pageCacheSupportsPlugins; } \
    void setPageCacheSupportsPlugins(bool pageCacheSupportsPlugins); \
    double passwordEchoDurationInSeconds() const { return m_passwordEchoDurationInSeconds; } \
    void setPasswordEchoDurationInSeconds(double passwordEchoDurationInSeconds); \
    bool passwordEchoEnabled() const { return m_passwordEchoEnabled; } \
    void setPasswordEchoEnabled(bool passwordEchoEnabled); \
    int pinchOverlayScrollbarThickness() const { return m_pinchOverlayScrollbarThickness; } \
    void setPinchOverlayScrollbarThickness(int pinchOverlayScrollbarThickness); \
    bool pinchVirtualViewportEnabled() const { return m_pinchVirtualViewportEnabled; } \
    void setPinchVirtualViewportEnabled(bool pinchVirtualViewportEnabled); \
    bool pluginsEnabled() const { return m_pluginsEnabled; } \
    void setPluginsEnabled(bool pluginsEnabled); \
    bool regionBasedColumnsEnabled() const { return m_regionBasedColumnsEnabled; } \
    void setRegionBasedColumnsEnabled(bool regionBasedColumnsEnabled); \
    bool reportScreenSizeInPhysicalPixelsQuirk() const { return m_reportScreenSizeInPhysicalPixelsQuirk; } \
    void setReportScreenSizeInPhysicalPixelsQuirk(bool reportScreenSizeInPhysicalPixelsQuirk); \
    bool scriptEnabled() const { return m_scriptEnabled; } \
    void setScriptEnabled(bool scriptEnabled); \
    bool scrollAnimatorEnabled() const { return m_scrollAnimatorEnabled; } \
    void setScrollAnimatorEnabled(bool scrollAnimatorEnabled); \
    bool selectTrailingWhitespaceEnabled() const { return m_selectTrailingWhitespaceEnabled; } \
    void setSelectTrailingWhitespaceEnabled(bool selectTrailingWhitespaceEnabled); \
    bool selectionIncludesAltImageText() const { return m_selectionIncludesAltImageText; } \
    void setSelectionIncludesAltImageText(bool selectionIncludesAltImageText); \
    bool shouldClearDocumentBackground() const { return m_shouldClearDocumentBackground; } \
    void setShouldClearDocumentBackground(bool shouldClearDocumentBackground); \
    bool shouldPrintBackgrounds() const { return m_shouldPrintBackgrounds; } \
    void setShouldPrintBackgrounds(bool shouldPrintBackgrounds); \
    bool shouldRespectImageOrientation() const { return m_shouldRespectImageOrientation; } \
    void setShouldRespectImageOrientation(bool shouldRespectImageOrientation); \
    bool shrinksStandaloneImagesToFit() const { return m_shrinksStandaloneImagesToFit; } \
    void setShrinksStandaloneImagesToFit(bool shrinksStandaloneImagesToFit); \
    bool smartInsertDeleteEnabled() const { return m_smartInsertDeleteEnabled; } \
    void setSmartInsertDeleteEnabled(bool smartInsertDeleteEnabled); \
    bool spatialNavigationEnabled() const { return m_spatialNavigationEnabled; } \
    void setSpatialNavigationEnabled(bool spatialNavigationEnabled); \
    bool supportsMultipleWindows() const { return m_supportsMultipleWindows; } \
    void setSupportsMultipleWindows(bool supportsMultipleWindows); \
    bool syncXHRInDocumentsEnabled() const { return m_syncXHRInDocumentsEnabled; } \
    void setSyncXHRInDocumentsEnabled(bool syncXHRInDocumentsEnabled); \
    bool textAreasAreResizable() const { return m_textAreasAreResizable; } \
    void setTextAreasAreResizable(bool textAreasAreResizable); \
    bool touchAdjustmentEnabled() const { return m_touchAdjustmentEnabled; } \
    void setTouchAdjustmentEnabled(bool touchAdjustmentEnabled); \
    bool touchDragDropEnabled() const { return m_touchDragDropEnabled; } \
    void setTouchDragDropEnabled(bool touchDragDropEnabled); \
    bool touchEditingEnabled() const { return m_touchEditingEnabled; } \
    void setTouchEditingEnabled(bool touchEditingEnabled); \
    bool treatIPAddressAsDomain() const { return m_treatIPAddressAsDomain; } \
    void setTreatIPAddressAsDomain(bool treatIPAddressAsDomain); \
    bool unifiedTextCheckerEnabled() const { return m_unifiedTextCheckerEnabled; } \
    void setUnifiedTextCheckerEnabled(bool unifiedTextCheckerEnabled); \
    bool unsafePluginPastingEnabled() const { return m_unsafePluginPastingEnabled; } \
    void setUnsafePluginPastingEnabled(bool unsafePluginPastingEnabled); \
    bool useLegacyBackgroundSizeShorthandBehavior() const { return m_useLegacyBackgroundSizeShorthandBehavior; } \
    void setUseLegacyBackgroundSizeShorthandBehavior(bool useLegacyBackgroundSizeShorthandBehavior); \
    bool useSolidColorScrollbars() const { return m_useSolidColorScrollbars; } \
    void setUseSolidColorScrollbars(bool useSolidColorScrollbars); \
    bool useWideViewport() const { return m_useWideViewport; } \
    void setUseWideViewport(bool useWideViewport); \
    bool usesEncodingDetector() const { return m_usesEncodingDetector; } \
    void setUsesEncodingDetector(bool usesEncodingDetector); \
    int validationMessageTimerMagnification() const { return m_validationMessageTimerMagnification; } \
    void setValidationMessageTimerMagnification(int validationMessageTimerMagnification); \
    bool viewportEnabled() const { return m_viewportEnabled; } \
    void setViewportEnabled(bool viewportEnabled); \
    bool viewportMetaEnabled() const { return m_viewportMetaEnabled; } \
    void setViewportMetaEnabled(bool viewportMetaEnabled); \
    bool viewportMetaMergeContentQuirk() const { return m_viewportMetaMergeContentQuirk; } \
    void setViewportMetaMergeContentQuirk(bool viewportMetaMergeContentQuirk); \
    bool viewportMetaZeroValuesQuirk() const { return m_viewportMetaZeroValuesQuirk; } \
    void setViewportMetaZeroValuesQuirk(bool viewportMetaZeroValuesQuirk); \
    bool webAudioEnabled() const { return m_webAudioEnabled; } \
    void setWebAudioEnabled(bool webAudioEnabled); \
    bool webGLEnabled() const { return m_webGLEnabled; } \
    void setWebGLEnabled(bool webGLEnabled); \
    bool webGLErrorsToConsoleEnabled() const { return m_webGLErrorsToConsoleEnabled; } \
    void setWebGLErrorsToConsoleEnabled(bool webGLErrorsToConsoleEnabled); \
    bool webSecurityEnabled() const { return m_webSecurityEnabled; } \
    void setWebSecurityEnabled(bool webSecurityEnabled); \
    bool wideViewportQuirkEnabled() const { return m_wideViewportQuirkEnabled; } \
    void setWideViewportQuirkEnabled(bool wideViewportQuirkEnabled); \
    bool xssAuditorEnabled() const { return m_xssAuditorEnabled; } \
    void setXSSAuditorEnabled(bool xssAuditorEnabled); \
// End of SETTINGS_GETTERS_AND_SETTERS.

#define SETTINGS_MEMBER_VARIABLES \
    int m_accelerated2dCanvasMSAASampleCount; \
    double m_accessibilityFontScaleFactor; \
    int m_defaultFixedFontSize; \
    int m_defaultFontSize; \
    String m_defaultTextEncodingName; \
    String m_defaultVideoPosterURL; \
    double m_deviceScaleAdjustment; \
    EditingBehaviorType m_editingBehaviorType; \
    int m_maxBeaconTransmission; \
    int m_maxTouchPoints; \
    String m_mediaTypeOverride; \
    int m_minimumAccelerated2dCanvasSize; \
    int m_minimumFontSize; \
    int m_minimumLogicalFontSize; \
    double m_passwordEchoDurationInSeconds; \
    int m_pinchOverlayScrollbarThickness; \
    int m_validationMessageTimerMagnification; \
    bool m_DOMPasteAllowed : 1; \
    bool m_accelerated2dCanvasEnabled : 1; \
    bool m_acceleratedCompositingEnabled : 1; \
    bool m_acceleratedCompositingForCanvasEnabled : 1; \
    bool m_acceleratedCompositingForFiltersEnabled : 1; \
    bool m_acceleratedCompositingForFixedPositionEnabled : 1; \
    bool m_acceleratedCompositingForFixedRootBackgroundEnabled : 1; \
    bool m_acceleratedCompositingForOverflowScrollEnabled : 1; \
    bool m_acceleratedCompositingForVideoEnabled : 1; \
    bool m_allowConnectingInsecureWebSocket : 1; \
    bool m_allowCustomScrollbarInMainFrame : 1; \
    bool m_allowDisplayOfInsecureContent : 1; \
    bool m_allowFileAccessFromFileURLs : 1; \
    bool m_allowRunningOfInsecureContent : 1; \
    bool m_allowScriptsToCloseWindows : 1; \
    bool m_allowUniversalAccessFromFileURLs : 1; \
    bool m_antialiased2dCanvasEnabled : 1; \
    bool m_asynchronousSpellCheckingEnabled : 1; \
    bool m_caretBrowsingEnabled : 1; \
    bool m_compositedScrollingForFramesEnabled : 1; \
    bool m_compositorDrivenAcceleratedScrollingEnabled : 1; \
    bool m_compositorTouchHitTesting : 1; \
    bool m_containerCullingEnabled : 1; \
    bool m_cookieEnabled : 1; \
    bool m_deferredFiltersEnabled : 1; \
    bool m_deviceSupportsMouse : 1; \
    bool m_deviceSupportsTouch : 1; \
    bool m_disallowFullscreenForNonMediaElements : 1; \
    bool m_dnsPrefetchingEnabled : 1; \
    bool m_downloadableBinaryFontsEnabled : 1; \
    bool m_forceZeroLayoutHeight : 1; \
    bool m_fullscreenSupported : 1; \
    bool m_hyperlinkAuditingEnabled : 1; \
    bool m_ignoreMainFrameOverflowHiddenQuirk : 1; \
    bool m_imagesEnabled : 1; \
    bool m_javaEnabled : 1; \
    bool m_javaScriptCanAccessClipboard : 1; \
    bool m_javaScriptCanOpenWindowsAutomatically : 1; \
    bool m_layerSquashingEnabled : 1; \
    bool m_loadWithOverviewMode : 1; \
    bool m_loadsImagesAutomatically : 1; \
    bool m_localStorageEnabled : 1; \
    bool m_mainFrameClipsContent : 1; \
    bool m_mediaControlsOverlayPlayButtonEnabled : 1; \
    bool m_mediaPlaybackRequiresUserGesture : 1; \
    bool m_navigateOnDragDrop : 1; \
    bool m_needsSiteSpecificQuirks : 1; \
    bool m_offlineWebApplicationCacheEnabled : 1; \
    bool m_pageCacheSupportsPlugins : 1; \
    bool m_passwordEchoEnabled : 1; \
    bool m_pinchVirtualViewportEnabled : 1; \
    bool m_pluginsEnabled : 1; \
    bool m_regionBasedColumnsEnabled : 1; \
    bool m_reportScreenSizeInPhysicalPixelsQuirk : 1; \
    bool m_scriptEnabled : 1; \
    bool m_scrollAnimatorEnabled : 1; \
    bool m_selectTrailingWhitespaceEnabled : 1; \
    bool m_selectionIncludesAltImageText : 1; \
    bool m_shouldClearDocumentBackground : 1; \
    bool m_shouldPrintBackgrounds : 1; \
    bool m_shouldRespectImageOrientation : 1; \
    bool m_shrinksStandaloneImagesToFit : 1; \
    bool m_smartInsertDeleteEnabled : 1; \
    bool m_spatialNavigationEnabled : 1; \
    bool m_supportsMultipleWindows : 1; \
    bool m_syncXHRInDocumentsEnabled : 1; \
    bool m_textAreasAreResizable : 1; \
    bool m_touchAdjustmentEnabled : 1; \
    bool m_touchDragDropEnabled : 1; \
    bool m_touchEditingEnabled : 1; \
    bool m_treatIPAddressAsDomain : 1; \
    bool m_unifiedTextCheckerEnabled : 1; \
    bool m_unsafePluginPastingEnabled : 1; \
    bool m_useLegacyBackgroundSizeShorthandBehavior : 1; \
    bool m_useSolidColorScrollbars : 1; \
    bool m_useWideViewport : 1; \
    bool m_usesEncodingDetector : 1; \
    bool m_viewportEnabled : 1; \
    bool m_viewportMetaEnabled : 1; \
    bool m_viewportMetaMergeContentQuirk : 1; \
    bool m_viewportMetaZeroValuesQuirk : 1; \
    bool m_webAudioEnabled : 1; \
    bool m_webGLEnabled : 1; \
    bool m_webGLErrorsToConsoleEnabled : 1; \
    bool m_webSecurityEnabled : 1; \
    bool m_wideViewportQuirkEnabled : 1; \
    bool m_xssAuditorEnabled : 1; \
// End of SETTINGS_MEMBER_VARIABLES.

#define SETTINGS_INITIALIZER_LIST \
    , m_accelerated2dCanvasMSAASampleCount(0) \
    , m_accessibilityFontScaleFactor(1.0) \
    , m_defaultFixedFontSize(0) \
    , m_defaultFontSize(0) \
    , m_deviceScaleAdjustment(1.0) \
    , m_editingBehaviorType(editingBehaviorTypeForPlatform()) \
    , m_maxBeaconTransmission(16384) \
    , m_maxTouchPoints(0) \
    , m_mediaTypeOverride("screen") \
    , m_minimumAccelerated2dCanvasSize(257*256) \
    , m_minimumFontSize(0) \
    , m_minimumLogicalFontSize(0) \
    , m_passwordEchoDurationInSeconds(1) \
    , m_pinchOverlayScrollbarThickness(0) \
    , m_validationMessageTimerMagnification(50) \
    , m_DOMPasteAllowed(false) \
    , m_accelerated2dCanvasEnabled(false) \
    , m_acceleratedCompositingEnabled(true) \
    , m_acceleratedCompositingForCanvasEnabled(true) \
    , m_acceleratedCompositingForFiltersEnabled(false) \
    , m_acceleratedCompositingForFixedPositionEnabled(false) \
    , m_acceleratedCompositingForFixedRootBackgroundEnabled(false) \
    , m_acceleratedCompositingForOverflowScrollEnabled(false) \
    , m_acceleratedCompositingForVideoEnabled(true) \
    , m_allowConnectingInsecureWebSocket(false) \
    , m_allowCustomScrollbarInMainFrame(true) \
    , m_allowDisplayOfInsecureContent(true) \
    , m_allowFileAccessFromFileURLs(true) \
    , m_allowRunningOfInsecureContent(true) \
    , m_allowScriptsToCloseWindows(false) \
    , m_allowUniversalAccessFromFileURLs(true) \
    , m_antialiased2dCanvasEnabled(true) \
    , m_asynchronousSpellCheckingEnabled(false) \
    , m_caretBrowsingEnabled(false) \
    , m_compositedScrollingForFramesEnabled(false) \
    , m_compositorDrivenAcceleratedScrollingEnabled(false) \
    , m_compositorTouchHitTesting(true) \
    , m_containerCullingEnabled(false) \
    , m_cookieEnabled(true) \
    , m_deferredFiltersEnabled(true) \
    , m_deviceSupportsMouse(true) \
    , m_deviceSupportsTouch(false) \
    , m_disallowFullscreenForNonMediaElements(false) \
    , m_dnsPrefetchingEnabled(false) \
    , m_downloadableBinaryFontsEnabled(true) \
    , m_forceZeroLayoutHeight(false) \
    , m_fullscreenSupported(true) \
    , m_hyperlinkAuditingEnabled(false) \
    , m_ignoreMainFrameOverflowHiddenQuirk(false) \
    , m_imagesEnabled(true) \
    , m_javaEnabled(false) \
    , m_javaScriptCanAccessClipboard(false) \
    , m_javaScriptCanOpenWindowsAutomatically(false) \
    , m_layerSquashingEnabled(false) \
    , m_loadWithOverviewMode(true) \
    , m_loadsImagesAutomatically(false) \
    , m_localStorageEnabled(false) \
    , m_mainFrameClipsContent(true) \
    , m_mediaControlsOverlayPlayButtonEnabled(false) \
    , m_mediaPlaybackRequiresUserGesture(false) \
    , m_navigateOnDragDrop(true) \
    , m_needsSiteSpecificQuirks(false) \
    , m_offlineWebApplicationCacheEnabled(false) \
    , m_pageCacheSupportsPlugins(false) \
    , m_passwordEchoEnabled(false) \
    , m_pinchVirtualViewportEnabled(false) \
    , m_pluginsEnabled(false) \
    , m_regionBasedColumnsEnabled(false) \
    , m_reportScreenSizeInPhysicalPixelsQuirk(false) \
    , m_scriptEnabled(false) \
    , m_scrollAnimatorEnabled(true) \
    , m_selectTrailingWhitespaceEnabled(defaultSelectTrailingWhitespaceEnabled) \
    , m_selectionIncludesAltImageText(false) \
    , m_shouldClearDocumentBackground(true) \
    , m_shouldPrintBackgrounds(false) \
    , m_shouldRespectImageOrientation(false) \
    , m_shrinksStandaloneImagesToFit(true) \
    , m_smartInsertDeleteEnabled(defaultSmartInsertDeleteEnabled) \
    , m_spatialNavigationEnabled(false) \
    , m_supportsMultipleWindows(true) \
    , m_syncXHRInDocumentsEnabled(true) \
    , m_textAreasAreResizable(false) \
    , m_touchAdjustmentEnabled(true) \
    , m_touchDragDropEnabled(false) \
    , m_touchEditingEnabled(false) \
    , m_treatIPAddressAsDomain(false) \
    , m_unifiedTextCheckerEnabled(defaultUnifiedTextCheckerEnabled) \
    , m_unsafePluginPastingEnabled(true) \
    , m_useLegacyBackgroundSizeShorthandBehavior(false) \
    , m_useSolidColorScrollbars(false) \
    , m_useWideViewport(true) \
    , m_usesEncodingDetector(false) \
    , m_viewportEnabled(false) \
    , m_viewportMetaEnabled(false) \
    , m_viewportMetaMergeContentQuirk(false) \
    , m_viewportMetaZeroValuesQuirk(false) \
    , m_webAudioEnabled(false) \
    , m_webGLEnabled(false) \
    , m_webGLErrorsToConsoleEnabled(true) \
    , m_webSecurityEnabled(true) \
    , m_wideViewportQuirkEnabled(false) \
    , m_xssAuditorEnabled(false) \
// End of SETTINGS_INITIALIZER_LIST.

#define SETTINGS_SETTER_BODIES \
void Settings::setDOMPasteAllowed(bool DOMPasteAllowed) \
{ \
    if (m_DOMPasteAllowed == DOMPasteAllowed) \
        return; \
    m_DOMPasteAllowed = DOMPasteAllowed; \
} \
void Settings::setAccelerated2dCanvasEnabled(bool accelerated2dCanvasEnabled) \
{ \
    if (m_accelerated2dCanvasEnabled == accelerated2dCanvasEnabled) \
        return; \
    m_accelerated2dCanvasEnabled = accelerated2dCanvasEnabled; \
} \
void Settings::setAccelerated2dCanvasMSAASampleCount(int accelerated2dCanvasMSAASampleCount) \
{ \
    if (m_accelerated2dCanvasMSAASampleCount == accelerated2dCanvasMSAASampleCount) \
        return; \
    m_accelerated2dCanvasMSAASampleCount = accelerated2dCanvasMSAASampleCount; \
} \
void Settings::setAcceleratedCompositingEnabled(bool acceleratedCompositingEnabled) \
{ \
    if (m_acceleratedCompositingEnabled == acceleratedCompositingEnabled) \
        return; \
    m_acceleratedCompositingEnabled = acceleratedCompositingEnabled; \
    invalidate(SettingsDelegate::AcceleratedCompositingChange); \
} \
void Settings::setAcceleratedCompositingForCanvasEnabled(bool acceleratedCompositingForCanvasEnabled) \
{ \
    if (m_acceleratedCompositingForCanvasEnabled == acceleratedCompositingForCanvasEnabled) \
        return; \
    m_acceleratedCompositingForCanvasEnabled = acceleratedCompositingForCanvasEnabled; \
    invalidate(SettingsDelegate::AcceleratedCompositingChange); \
} \
void Settings::setAcceleratedCompositingForFiltersEnabled(bool acceleratedCompositingForFiltersEnabled) \
{ \
    if (m_acceleratedCompositingForFiltersEnabled == acceleratedCompositingForFiltersEnabled) \
        return; \
    m_acceleratedCompositingForFiltersEnabled = acceleratedCompositingForFiltersEnabled; \
    invalidate(SettingsDelegate::AcceleratedCompositingChange); \
} \
void Settings::setAcceleratedCompositingForFixedPositionEnabled(bool acceleratedCompositingForFixedPositionEnabled) \
{ \
    if (m_acceleratedCompositingForFixedPositionEnabled == acceleratedCompositingForFixedPositionEnabled) \
        return; \
    m_acceleratedCompositingForFixedPositionEnabled = acceleratedCompositingForFixedPositionEnabled; \
    invalidate(SettingsDelegate::AcceleratedCompositingChange); \
} \
void Settings::setAcceleratedCompositingForFixedRootBackgroundEnabled(bool acceleratedCompositingForFixedRootBackgroundEnabled) \
{ \
    if (m_acceleratedCompositingForFixedRootBackgroundEnabled == acceleratedCompositingForFixedRootBackgroundEnabled) \
        return; \
    m_acceleratedCompositingForFixedRootBackgroundEnabled = acceleratedCompositingForFixedRootBackgroundEnabled; \
} \
void Settings::setAcceleratedCompositingForOverflowScrollEnabled(bool acceleratedCompositingForOverflowScrollEnabled) \
{ \
    if (m_acceleratedCompositingForOverflowScrollEnabled == acceleratedCompositingForOverflowScrollEnabled) \
        return; \
    m_acceleratedCompositingForOverflowScrollEnabled = acceleratedCompositingForOverflowScrollEnabled; \
    invalidate(SettingsDelegate::AcceleratedCompositingChange); \
} \
void Settings::setAcceleratedCompositingForVideoEnabled(bool acceleratedCompositingForVideoEnabled) \
{ \
    if (m_acceleratedCompositingForVideoEnabled == acceleratedCompositingForVideoEnabled) \
        return; \
    m_acceleratedCompositingForVideoEnabled = acceleratedCompositingForVideoEnabled; \
    invalidate(SettingsDelegate::AcceleratedCompositingChange); \
} \
void Settings::setAccessibilityFontScaleFactor(double accessibilityFontScaleFactor) \
{ \
    if (m_accessibilityFontScaleFactor == accessibilityFontScaleFactor) \
        return; \
    m_accessibilityFontScaleFactor = accessibilityFontScaleFactor; \
    invalidate(SettingsDelegate::TextAutosizingChange); \
} \
void Settings::setAllowConnectingInsecureWebSocket(bool allowConnectingInsecureWebSocket) \
{ \
    if (m_allowConnectingInsecureWebSocket == allowConnectingInsecureWebSocket) \
        return; \
    m_allowConnectingInsecureWebSocket = allowConnectingInsecureWebSocket; \
} \
void Settings::setAllowCustomScrollbarInMainFrame(bool allowCustomScrollbarInMainFrame) \
{ \
    if (m_allowCustomScrollbarInMainFrame == allowCustomScrollbarInMainFrame) \
        return; \
    m_allowCustomScrollbarInMainFrame = allowCustomScrollbarInMainFrame; \
} \
void Settings::setAllowDisplayOfInsecureContent(bool allowDisplayOfInsecureContent) \
{ \
    if (m_allowDisplayOfInsecureContent == allowDisplayOfInsecureContent) \
        return; \
    m_allowDisplayOfInsecureContent = allowDisplayOfInsecureContent; \
} \
void Settings::setAllowFileAccessFromFileURLs(bool allowFileAccessFromFileURLs) \
{ \
    if (m_allowFileAccessFromFileURLs == allowFileAccessFromFileURLs) \
        return; \
    m_allowFileAccessFromFileURLs = allowFileAccessFromFileURLs; \
} \
void Settings::setAllowRunningOfInsecureContent(bool allowRunningOfInsecureContent) \
{ \
    if (m_allowRunningOfInsecureContent == allowRunningOfInsecureContent) \
        return; \
    m_allowRunningOfInsecureContent = allowRunningOfInsecureContent; \
} \
void Settings::setAllowScriptsToCloseWindows(bool allowScriptsToCloseWindows) \
{ \
    if (m_allowScriptsToCloseWindows == allowScriptsToCloseWindows) \
        return; \
    m_allowScriptsToCloseWindows = allowScriptsToCloseWindows; \
} \
void Settings::setAllowUniversalAccessFromFileURLs(bool allowUniversalAccessFromFileURLs) \
{ \
    if (m_allowUniversalAccessFromFileURLs == allowUniversalAccessFromFileURLs) \
        return; \
    m_allowUniversalAccessFromFileURLs = allowUniversalAccessFromFileURLs; \
} \
void Settings::setAntialiased2dCanvasEnabled(bool antialiased2dCanvasEnabled) \
{ \
    if (m_antialiased2dCanvasEnabled == antialiased2dCanvasEnabled) \
        return; \
    m_antialiased2dCanvasEnabled = antialiased2dCanvasEnabled; \
} \
void Settings::setAsynchronousSpellCheckingEnabled(bool asynchronousSpellCheckingEnabled) \
{ \
    if (m_asynchronousSpellCheckingEnabled == asynchronousSpellCheckingEnabled) \
        return; \
    m_asynchronousSpellCheckingEnabled = asynchronousSpellCheckingEnabled; \
} \
void Settings::setCaretBrowsingEnabled(bool caretBrowsingEnabled) \
{ \
    if (m_caretBrowsingEnabled == caretBrowsingEnabled) \
        return; \
    m_caretBrowsingEnabled = caretBrowsingEnabled; \
} \
void Settings::setCompositedScrollingForFramesEnabled(bool compositedScrollingForFramesEnabled) \
{ \
    if (m_compositedScrollingForFramesEnabled == compositedScrollingForFramesEnabled) \
        return; \
    m_compositedScrollingForFramesEnabled = compositedScrollingForFramesEnabled; \
    invalidate(SettingsDelegate::AcceleratedCompositingChange); \
} \
void Settings::setCompositorDrivenAcceleratedScrollingEnabled(bool compositorDrivenAcceleratedScrollingEnabled) \
{ \
    if (m_compositorDrivenAcceleratedScrollingEnabled == compositorDrivenAcceleratedScrollingEnabled) \
        return; \
    m_compositorDrivenAcceleratedScrollingEnabled = compositorDrivenAcceleratedScrollingEnabled; \
    invalidate(SettingsDelegate::AcceleratedCompositingChange); \
} \
void Settings::setCompositorTouchHitTesting(bool compositorTouchHitTesting) \
{ \
    if (m_compositorTouchHitTesting == compositorTouchHitTesting) \
        return; \
    m_compositorTouchHitTesting = compositorTouchHitTesting; \
} \
void Settings::setContainerCullingEnabled(bool containerCullingEnabled) \
{ \
    if (m_containerCullingEnabled == containerCullingEnabled) \
        return; \
    m_containerCullingEnabled = containerCullingEnabled; \
} \
void Settings::setCookieEnabled(bool cookieEnabled) \
{ \
    if (m_cookieEnabled == cookieEnabled) \
        return; \
    m_cookieEnabled = cookieEnabled; \
} \
void Settings::setDefaultFixedFontSize(int defaultFixedFontSize) \
{ \
    if (m_defaultFixedFontSize == defaultFixedFontSize) \
        return; \
    m_defaultFixedFontSize = defaultFixedFontSize; \
    invalidate(SettingsDelegate::StyleChange); \
} \
void Settings::setDefaultFontSize(int defaultFontSize) \
{ \
    if (m_defaultFontSize == defaultFontSize) \
        return; \
    m_defaultFontSize = defaultFontSize; \
    invalidate(SettingsDelegate::StyleChange); \
} \
void Settings::setDefaultTextEncodingName(const String& defaultTextEncodingName) \
{ \
    if (m_defaultTextEncodingName == defaultTextEncodingName) \
        return; \
    m_defaultTextEncodingName = defaultTextEncodingName; \
} \
void Settings::setDefaultVideoPosterURL(const String& defaultVideoPosterURL) \
{ \
    if (m_defaultVideoPosterURL == defaultVideoPosterURL) \
        return; \
    m_defaultVideoPosterURL = defaultVideoPosterURL; \
} \
void Settings::setDeferredFiltersEnabled(bool deferredFiltersEnabled) \
{ \
    if (m_deferredFiltersEnabled == deferredFiltersEnabled) \
        return; \
    m_deferredFiltersEnabled = deferredFiltersEnabled; \
} \
void Settings::setDeviceScaleAdjustment(double deviceScaleAdjustment) \
{ \
    if (m_deviceScaleAdjustment == deviceScaleAdjustment) \
        return; \
    m_deviceScaleAdjustment = deviceScaleAdjustment; \
    invalidate(SettingsDelegate::TextAutosizingChange); \
} \
void Settings::setDeviceSupportsMouse(bool deviceSupportsMouse) \
{ \
    if (m_deviceSupportsMouse == deviceSupportsMouse) \
        return; \
    m_deviceSupportsMouse = deviceSupportsMouse; \
} \
void Settings::setDeviceSupportsTouch(bool deviceSupportsTouch) \
{ \
    if (m_deviceSupportsTouch == deviceSupportsTouch) \
        return; \
    m_deviceSupportsTouch = deviceSupportsTouch; \
} \
void Settings::setDisallowFullscreenForNonMediaElements(bool disallowFullscreenForNonMediaElements) \
{ \
    if (m_disallowFullscreenForNonMediaElements == disallowFullscreenForNonMediaElements) \
        return; \
    m_disallowFullscreenForNonMediaElements = disallowFullscreenForNonMediaElements; \
} \
void Settings::setDNSPrefetchingEnabled(bool dnsPrefetchingEnabled) \
{ \
    if (m_dnsPrefetchingEnabled == dnsPrefetchingEnabled) \
        return; \
    m_dnsPrefetchingEnabled = dnsPrefetchingEnabled; \
    invalidate(SettingsDelegate::DNSPrefetchingChange); \
} \
void Settings::setDownloadableBinaryFontsEnabled(bool downloadableBinaryFontsEnabled) \
{ \
    if (m_downloadableBinaryFontsEnabled == downloadableBinaryFontsEnabled) \
        return; \
    m_downloadableBinaryFontsEnabled = downloadableBinaryFontsEnabled; \
} \
void Settings::setEditingBehaviorType(EditingBehaviorType editingBehaviorType) \
{ \
    if (m_editingBehaviorType == editingBehaviorType) \
        return; \
    m_editingBehaviorType = editingBehaviorType; \
} \
void Settings::setForceZeroLayoutHeight(bool forceZeroLayoutHeight) \
{ \
    if (m_forceZeroLayoutHeight == forceZeroLayoutHeight) \
        return; \
    m_forceZeroLayoutHeight = forceZeroLayoutHeight; \
    invalidate(SettingsDelegate::ViewportDescriptionChange); \
} \
void Settings::setFullscreenSupported(bool fullscreenSupported) \
{ \
    if (m_fullscreenSupported == fullscreenSupported) \
        return; \
    m_fullscreenSupported = fullscreenSupported; \
} \
void Settings::setHyperlinkAuditingEnabled(bool hyperlinkAuditingEnabled) \
{ \
    if (m_hyperlinkAuditingEnabled == hyperlinkAuditingEnabled) \
        return; \
    m_hyperlinkAuditingEnabled = hyperlinkAuditingEnabled; \
} \
void Settings::setIgnoreMainFrameOverflowHiddenQuirk(bool ignoreMainFrameOverflowHiddenQuirk) \
{ \
    if (m_ignoreMainFrameOverflowHiddenQuirk == ignoreMainFrameOverflowHiddenQuirk) \
        return; \
    m_ignoreMainFrameOverflowHiddenQuirk = ignoreMainFrameOverflowHiddenQuirk; \
} \
void Settings::setImagesEnabled(bool imagesEnabled) \
{ \
    if (m_imagesEnabled == imagesEnabled) \
        return; \
    m_imagesEnabled = imagesEnabled; \
    invalidate(SettingsDelegate::ImageLoadingChange); \
} \
void Settings::setJavaEnabled(bool javaEnabled) \
{ \
    if (m_javaEnabled == javaEnabled) \
        return; \
    m_javaEnabled = javaEnabled; \
} \
void Settings::setJavaScriptCanAccessClipboard(bool javaScriptCanAccessClipboard) \
{ \
    if (m_javaScriptCanAccessClipboard == javaScriptCanAccessClipboard) \
        return; \
    m_javaScriptCanAccessClipboard = javaScriptCanAccessClipboard; \
} \
void Settings::setJavaScriptCanOpenWindowsAutomatically(bool javaScriptCanOpenWindowsAutomatically) \
{ \
    if (m_javaScriptCanOpenWindowsAutomatically == javaScriptCanOpenWindowsAutomatically) \
        return; \
    m_javaScriptCanOpenWindowsAutomatically = javaScriptCanOpenWindowsAutomatically; \
} \
void Settings::setLayerSquashingEnabled(bool layerSquashingEnabled) \
{ \
    if (m_layerSquashingEnabled == layerSquashingEnabled) \
        return; \
    m_layerSquashingEnabled = layerSquashingEnabled; \
} \
void Settings::setLoadWithOverviewMode(bool loadWithOverviewMode) \
{ \
    if (m_loadWithOverviewMode == loadWithOverviewMode) \
        return; \
    m_loadWithOverviewMode = loadWithOverviewMode; \
    invalidate(SettingsDelegate::ViewportDescriptionChange); \
} \
void Settings::setLoadsImagesAutomatically(bool loadsImagesAutomatically) \
{ \
    if (m_loadsImagesAutomatically == loadsImagesAutomatically) \
        return; \
    m_loadsImagesAutomatically = loadsImagesAutomatically; \
    invalidate(SettingsDelegate::ImageLoadingChange); \
} \
void Settings::setLocalStorageEnabled(bool localStorageEnabled) \
{ \
    if (m_localStorageEnabled == localStorageEnabled) \
        return; \
    m_localStorageEnabled = localStorageEnabled; \
} \
void Settings::setMainFrameClipsContent(bool mainFrameClipsContent) \
{ \
    if (m_mainFrameClipsContent == mainFrameClipsContent) \
        return; \
    m_mainFrameClipsContent = mainFrameClipsContent; \
} \
void Settings::setMaxBeaconTransmission(int maxBeaconTransmission) \
{ \
    if (m_maxBeaconTransmission == maxBeaconTransmission) \
        return; \
    m_maxBeaconTransmission = maxBeaconTransmission; \
} \
void Settings::setMaxTouchPoints(int maxTouchPoints) \
{ \
    if (m_maxTouchPoints == maxTouchPoints) \
        return; \
    m_maxTouchPoints = maxTouchPoints; \
} \
void Settings::setMediaControlsOverlayPlayButtonEnabled(bool mediaControlsOverlayPlayButtonEnabled) \
{ \
    if (m_mediaControlsOverlayPlayButtonEnabled == mediaControlsOverlayPlayButtonEnabled) \
        return; \
    m_mediaControlsOverlayPlayButtonEnabled = mediaControlsOverlayPlayButtonEnabled; \
} \
void Settings::setMediaPlaybackRequiresUserGesture(bool mediaPlaybackRequiresUserGesture) \
{ \
    if (m_mediaPlaybackRequiresUserGesture == mediaPlaybackRequiresUserGesture) \
        return; \
    m_mediaPlaybackRequiresUserGesture = mediaPlaybackRequiresUserGesture; \
} \
void Settings::setMediaTypeOverride(const String& mediaTypeOverride) \
{ \
    if (m_mediaTypeOverride == mediaTypeOverride) \
        return; \
    m_mediaTypeOverride = mediaTypeOverride; \
    invalidate(SettingsDelegate::MediaTypeChange); \
} \
void Settings::setMinimumAccelerated2dCanvasSize(int minimumAccelerated2dCanvasSize) \
{ \
    if (m_minimumAccelerated2dCanvasSize == minimumAccelerated2dCanvasSize) \
        return; \
    m_minimumAccelerated2dCanvasSize = minimumAccelerated2dCanvasSize; \
} \
void Settings::setMinimumFontSize(int minimumFontSize) \
{ \
    if (m_minimumFontSize == minimumFontSize) \
        return; \
    m_minimumFontSize = minimumFontSize; \
    invalidate(SettingsDelegate::StyleChange); \
} \
void Settings::setMinimumLogicalFontSize(int minimumLogicalFontSize) \
{ \
    if (m_minimumLogicalFontSize == minimumLogicalFontSize) \
        return; \
    m_minimumLogicalFontSize = minimumLogicalFontSize; \
    invalidate(SettingsDelegate::StyleChange); \
} \
void Settings::setNavigateOnDragDrop(bool navigateOnDragDrop) \
{ \
    if (m_navigateOnDragDrop == navigateOnDragDrop) \
        return; \
    m_navigateOnDragDrop = navigateOnDragDrop; \
} \
void Settings::setNeedsSiteSpecificQuirks(bool needsSiteSpecificQuirks) \
{ \
    if (m_needsSiteSpecificQuirks == needsSiteSpecificQuirks) \
        return; \
    m_needsSiteSpecificQuirks = needsSiteSpecificQuirks; \
} \
void Settings::setOfflineWebApplicationCacheEnabled(bool offlineWebApplicationCacheEnabled) \
{ \
    if (m_offlineWebApplicationCacheEnabled == offlineWebApplicationCacheEnabled) \
        return; \
    m_offlineWebApplicationCacheEnabled = offlineWebApplicationCacheEnabled; \
} \
void Settings::setPageCacheSupportsPlugins(bool pageCacheSupportsPlugins) \
{ \
    if (m_pageCacheSupportsPlugins == pageCacheSupportsPlugins) \
        return; \
    m_pageCacheSupportsPlugins = pageCacheSupportsPlugins; \
} \
void Settings::setPasswordEchoDurationInSeconds(double passwordEchoDurationInSeconds) \
{ \
    if (m_passwordEchoDurationInSeconds == passwordEchoDurationInSeconds) \
        return; \
    m_passwordEchoDurationInSeconds = passwordEchoDurationInSeconds; \
} \
void Settings::setPasswordEchoEnabled(bool passwordEchoEnabled) \
{ \
    if (m_passwordEchoEnabled == passwordEchoEnabled) \
        return; \
    m_passwordEchoEnabled = passwordEchoEnabled; \
} \
void Settings::setPinchOverlayScrollbarThickness(int pinchOverlayScrollbarThickness) \
{ \
    if (m_pinchOverlayScrollbarThickness == pinchOverlayScrollbarThickness) \
        return; \
    m_pinchOverlayScrollbarThickness = pinchOverlayScrollbarThickness; \
} \
void Settings::setPinchVirtualViewportEnabled(bool pinchVirtualViewportEnabled) \
{ \
    if (m_pinchVirtualViewportEnabled == pinchVirtualViewportEnabled) \
        return; \
    m_pinchVirtualViewportEnabled = pinchVirtualViewportEnabled; \
} \
void Settings::setPluginsEnabled(bool pluginsEnabled) \
{ \
    if (m_pluginsEnabled == pluginsEnabled) \
        return; \
    m_pluginsEnabled = pluginsEnabled; \
} \
void Settings::setRegionBasedColumnsEnabled(bool regionBasedColumnsEnabled) \
{ \
    if (m_regionBasedColumnsEnabled == regionBasedColumnsEnabled) \
        return; \
    m_regionBasedColumnsEnabled = regionBasedColumnsEnabled; \
} \
void Settings::setReportScreenSizeInPhysicalPixelsQuirk(bool reportScreenSizeInPhysicalPixelsQuirk) \
{ \
    if (m_reportScreenSizeInPhysicalPixelsQuirk == reportScreenSizeInPhysicalPixelsQuirk) \
        return; \
    m_reportScreenSizeInPhysicalPixelsQuirk = reportScreenSizeInPhysicalPixelsQuirk; \
} \
void Settings::setScriptEnabled(bool scriptEnabled) \
{ \
    if (m_scriptEnabled == scriptEnabled) \
        return; \
    m_scriptEnabled = scriptEnabled; \
    invalidate(SettingsDelegate::ScriptEnableChange); \
} \
void Settings::setScrollAnimatorEnabled(bool scrollAnimatorEnabled) \
{ \
    if (m_scrollAnimatorEnabled == scrollAnimatorEnabled) \
        return; \
    m_scrollAnimatorEnabled = scrollAnimatorEnabled; \
} \
void Settings::setSelectTrailingWhitespaceEnabled(bool selectTrailingWhitespaceEnabled) \
{ \
    if (m_selectTrailingWhitespaceEnabled == selectTrailingWhitespaceEnabled) \
        return; \
    m_selectTrailingWhitespaceEnabled = selectTrailingWhitespaceEnabled; \
} \
void Settings::setSelectionIncludesAltImageText(bool selectionIncludesAltImageText) \
{ \
    if (m_selectionIncludesAltImageText == selectionIncludesAltImageText) \
        return; \
    m_selectionIncludesAltImageText = selectionIncludesAltImageText; \
} \
void Settings::setShouldClearDocumentBackground(bool shouldClearDocumentBackground) \
{ \
    if (m_shouldClearDocumentBackground == shouldClearDocumentBackground) \
        return; \
    m_shouldClearDocumentBackground = shouldClearDocumentBackground; \
} \
void Settings::setShouldPrintBackgrounds(bool shouldPrintBackgrounds) \
{ \
    if (m_shouldPrintBackgrounds == shouldPrintBackgrounds) \
        return; \
    m_shouldPrintBackgrounds = shouldPrintBackgrounds; \
} \
void Settings::setShouldRespectImageOrientation(bool shouldRespectImageOrientation) \
{ \
    if (m_shouldRespectImageOrientation == shouldRespectImageOrientation) \
        return; \
    m_shouldRespectImageOrientation = shouldRespectImageOrientation; \
} \
void Settings::setShrinksStandaloneImagesToFit(bool shrinksStandaloneImagesToFit) \
{ \
    if (m_shrinksStandaloneImagesToFit == shrinksStandaloneImagesToFit) \
        return; \
    m_shrinksStandaloneImagesToFit = shrinksStandaloneImagesToFit; \
} \
void Settings::setSmartInsertDeleteEnabled(bool smartInsertDeleteEnabled) \
{ \
    if (m_smartInsertDeleteEnabled == smartInsertDeleteEnabled) \
        return; \
    m_smartInsertDeleteEnabled = smartInsertDeleteEnabled; \
} \
void Settings::setSpatialNavigationEnabled(bool spatialNavigationEnabled) \
{ \
    if (m_spatialNavigationEnabled == spatialNavigationEnabled) \
        return; \
    m_spatialNavigationEnabled = spatialNavigationEnabled; \
} \
void Settings::setSupportsMultipleWindows(bool supportsMultipleWindows) \
{ \
    if (m_supportsMultipleWindows == supportsMultipleWindows) \
        return; \
    m_supportsMultipleWindows = supportsMultipleWindows; \
} \
void Settings::setSyncXHRInDocumentsEnabled(bool syncXHRInDocumentsEnabled) \
{ \
    if (m_syncXHRInDocumentsEnabled == syncXHRInDocumentsEnabled) \
        return; \
    m_syncXHRInDocumentsEnabled = syncXHRInDocumentsEnabled; \
} \
void Settings::setTextAreasAreResizable(bool textAreasAreResizable) \
{ \
    if (m_textAreasAreResizable == textAreasAreResizable) \
        return; \
    m_textAreasAreResizable = textAreasAreResizable; \
    invalidate(SettingsDelegate::StyleChange); \
} \
void Settings::setTouchAdjustmentEnabled(bool touchAdjustmentEnabled) \
{ \
    if (m_touchAdjustmentEnabled == touchAdjustmentEnabled) \
        return; \
    m_touchAdjustmentEnabled = touchAdjustmentEnabled; \
} \
void Settings::setTouchDragDropEnabled(bool touchDragDropEnabled) \
{ \
    if (m_touchDragDropEnabled == touchDragDropEnabled) \
        return; \
    m_touchDragDropEnabled = touchDragDropEnabled; \
} \
void Settings::setTouchEditingEnabled(bool touchEditingEnabled) \
{ \
    if (m_touchEditingEnabled == touchEditingEnabled) \
        return; \
    m_touchEditingEnabled = touchEditingEnabled; \
} \
void Settings::setTreatIPAddressAsDomain(bool treatIPAddressAsDomain) \
{ \
    if (m_treatIPAddressAsDomain == treatIPAddressAsDomain) \
        return; \
    m_treatIPAddressAsDomain = treatIPAddressAsDomain; \
} \
void Settings::setUnifiedTextCheckerEnabled(bool unifiedTextCheckerEnabled) \
{ \
    if (m_unifiedTextCheckerEnabled == unifiedTextCheckerEnabled) \
        return; \
    m_unifiedTextCheckerEnabled = unifiedTextCheckerEnabled; \
} \
void Settings::setUnsafePluginPastingEnabled(bool unsafePluginPastingEnabled) \
{ \
    if (m_unsafePluginPastingEnabled == unsafePluginPastingEnabled) \
        return; \
    m_unsafePluginPastingEnabled = unsafePluginPastingEnabled; \
} \
void Settings::setUseLegacyBackgroundSizeShorthandBehavior(bool useLegacyBackgroundSizeShorthandBehavior) \
{ \
    if (m_useLegacyBackgroundSizeShorthandBehavior == useLegacyBackgroundSizeShorthandBehavior) \
        return; \
    m_useLegacyBackgroundSizeShorthandBehavior = useLegacyBackgroundSizeShorthandBehavior; \
} \
void Settings::setUseSolidColorScrollbars(bool useSolidColorScrollbars) \
{ \
    if (m_useSolidColorScrollbars == useSolidColorScrollbars) \
        return; \
    m_useSolidColorScrollbars = useSolidColorScrollbars; \
} \
void Settings::setUseWideViewport(bool useWideViewport) \
{ \
    if (m_useWideViewport == useWideViewport) \
        return; \
    m_useWideViewport = useWideViewport; \
    invalidate(SettingsDelegate::ViewportDescriptionChange); \
} \
void Settings::setUsesEncodingDetector(bool usesEncodingDetector) \
{ \
    if (m_usesEncodingDetector == usesEncodingDetector) \
        return; \
    m_usesEncodingDetector = usesEncodingDetector; \
} \
void Settings::setValidationMessageTimerMagnification(int validationMessageTimerMagnification) \
{ \
    if (m_validationMessageTimerMagnification == validationMessageTimerMagnification) \
        return; \
    m_validationMessageTimerMagnification = validationMessageTimerMagnification; \
} \
void Settings::setViewportEnabled(bool viewportEnabled) \
{ \
    if (m_viewportEnabled == viewportEnabled) \
        return; \
    m_viewportEnabled = viewportEnabled; \
    invalidate(SettingsDelegate::ViewportDescriptionChange); \
} \
void Settings::setViewportMetaEnabled(bool viewportMetaEnabled) \
{ \
    if (m_viewportMetaEnabled == viewportMetaEnabled) \
        return; \
    m_viewportMetaEnabled = viewportMetaEnabled; \
} \
void Settings::setViewportMetaMergeContentQuirk(bool viewportMetaMergeContentQuirk) \
{ \
    if (m_viewportMetaMergeContentQuirk == viewportMetaMergeContentQuirk) \
        return; \
    m_viewportMetaMergeContentQuirk = viewportMetaMergeContentQuirk; \
} \
void Settings::setViewportMetaZeroValuesQuirk(bool viewportMetaZeroValuesQuirk) \
{ \
    if (m_viewportMetaZeroValuesQuirk == viewportMetaZeroValuesQuirk) \
        return; \
    m_viewportMetaZeroValuesQuirk = viewportMetaZeroValuesQuirk; \
} \
void Settings::setWebAudioEnabled(bool webAudioEnabled) \
{ \
    if (m_webAudioEnabled == webAudioEnabled) \
        return; \
    m_webAudioEnabled = webAudioEnabled; \
} \
void Settings::setWebGLEnabled(bool webGLEnabled) \
{ \
    if (m_webGLEnabled == webGLEnabled) \
        return; \
    m_webGLEnabled = webGLEnabled; \
} \
void Settings::setWebGLErrorsToConsoleEnabled(bool webGLErrorsToConsoleEnabled) \
{ \
    if (m_webGLErrorsToConsoleEnabled == webGLErrorsToConsoleEnabled) \
        return; \
    m_webGLErrorsToConsoleEnabled = webGLErrorsToConsoleEnabled; \
} \
void Settings::setWebSecurityEnabled(bool webSecurityEnabled) \
{ \
    if (m_webSecurityEnabled == webSecurityEnabled) \
        return; \
    m_webSecurityEnabled = webSecurityEnabled; \
} \
void Settings::setWideViewportQuirkEnabled(bool wideViewportQuirkEnabled) \
{ \
    if (m_wideViewportQuirkEnabled == wideViewportQuirkEnabled) \
        return; \
    m_wideViewportQuirkEnabled = wideViewportQuirkEnabled; \
} \
void Settings::setXSSAuditorEnabled(bool xssAuditorEnabled) \
{ \
    if (m_xssAuditorEnabled == xssAuditorEnabled) \
        return; \
    m_xssAuditorEnabled = xssAuditorEnabled; \
} \
// End of SETTINGS_SETTER_BODIES.

#endif // SettingsMacros_h
