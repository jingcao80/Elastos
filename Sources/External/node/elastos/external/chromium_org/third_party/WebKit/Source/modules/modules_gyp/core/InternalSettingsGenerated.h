// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InternalSettingsGenerated_h
#define InternalSettingsGenerated_h

#include "platform/heap/Handle.h"
#include "platform/RefCountedSupplement.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace WebCore {

class Page;

class InternalSettingsGenerated : public RefCountedWillBeGarbageCollectedFinalized<InternalSettingsGenerated> {
public:
    explicit InternalSettingsGenerated(Page*);
    virtual ~InternalSettingsGenerated();
    void resetToConsistentState();
    void setDOMPasteAllowed(bool DOMPasteAllowed);
    void setAccelerated2dCanvasEnabled(bool accelerated2dCanvasEnabled);
    void setAccelerated2dCanvasMSAASampleCount(int accelerated2dCanvasMSAASampleCount);
    void setAcceleratedCompositingEnabled(bool acceleratedCompositingEnabled);
    void setAcceleratedCompositingForCanvasEnabled(bool acceleratedCompositingForCanvasEnabled);
    void setAcceleratedCompositingForFiltersEnabled(bool acceleratedCompositingForFiltersEnabled);
    void setAcceleratedCompositingForFixedPositionEnabled(bool acceleratedCompositingForFixedPositionEnabled);
    void setAcceleratedCompositingForFixedRootBackgroundEnabled(bool acceleratedCompositingForFixedRootBackgroundEnabled);
    void setAcceleratedCompositingForOverflowScrollEnabled(bool acceleratedCompositingForOverflowScrollEnabled);
    void setAcceleratedCompositingForVideoEnabled(bool acceleratedCompositingForVideoEnabled);
    void setAccessibilityFontScaleFactor(double accessibilityFontScaleFactor);
    void setAllowConnectingInsecureWebSocket(bool allowConnectingInsecureWebSocket);
    void setAllowCustomScrollbarInMainFrame(bool allowCustomScrollbarInMainFrame);
    void setAllowDisplayOfInsecureContent(bool allowDisplayOfInsecureContent);
    void setAllowFileAccessFromFileURLs(bool allowFileAccessFromFileURLs);
    void setAllowRunningOfInsecureContent(bool allowRunningOfInsecureContent);
    void setAllowScriptsToCloseWindows(bool allowScriptsToCloseWindows);
    void setAllowUniversalAccessFromFileURLs(bool allowUniversalAccessFromFileURLs);
    void setAntialiased2dCanvasEnabled(bool antialiased2dCanvasEnabled);
    void setAsynchronousSpellCheckingEnabled(bool asynchronousSpellCheckingEnabled);
    void setCaretBrowsingEnabled(bool caretBrowsingEnabled);
    void setCompositedScrollingForFramesEnabled(bool compositedScrollingForFramesEnabled);
    void setCompositorDrivenAcceleratedScrollingEnabled(bool compositorDrivenAcceleratedScrollingEnabled);
    void setCompositorTouchHitTesting(bool compositorTouchHitTesting);
    void setContainerCullingEnabled(bool containerCullingEnabled);
    void setCookieEnabled(bool cookieEnabled);
    void setDefaultFixedFontSize(int defaultFixedFontSize);
    void setDefaultFontSize(int defaultFontSize);
    void setDefaultTextEncodingName(const String& defaultTextEncodingName);
    void setDefaultVideoPosterURL(const String& defaultVideoPosterURL);
    void setDeferredFiltersEnabled(bool deferredFiltersEnabled);
    void setDeviceScaleAdjustment(double deviceScaleAdjustment);
    void setDeviceSupportsMouse(bool deviceSupportsMouse);
    void setDeviceSupportsTouch(bool deviceSupportsTouch);
    void setDisallowFullscreenForNonMediaElements(bool disallowFullscreenForNonMediaElements);
    void setDNSPrefetchingEnabled(bool dnsPrefetchingEnabled);
    void setDownloadableBinaryFontsEnabled(bool downloadableBinaryFontsEnabled);
    void setForceZeroLayoutHeight(bool forceZeroLayoutHeight);
    void setFullscreenSupported(bool fullscreenSupported);
    void setHyperlinkAuditingEnabled(bool hyperlinkAuditingEnabled);
    void setIgnoreMainFrameOverflowHiddenQuirk(bool ignoreMainFrameOverflowHiddenQuirk);
    void setImagesEnabled(bool imagesEnabled);
    void setJavaEnabled(bool javaEnabled);
    void setJavaScriptCanAccessClipboard(bool javaScriptCanAccessClipboard);
    void setJavaScriptCanOpenWindowsAutomatically(bool javaScriptCanOpenWindowsAutomatically);
    void setLayerSquashingEnabled(bool layerSquashingEnabled);
    void setLoadWithOverviewMode(bool loadWithOverviewMode);
    void setLoadsImagesAutomatically(bool loadsImagesAutomatically);
    void setLocalStorageEnabled(bool localStorageEnabled);
    void setMainFrameClipsContent(bool mainFrameClipsContent);
    void setMaxBeaconTransmission(int maxBeaconTransmission);
    void setMaxTouchPoints(int maxTouchPoints);
    void setMediaControlsOverlayPlayButtonEnabled(bool mediaControlsOverlayPlayButtonEnabled);
    void setMediaPlaybackRequiresUserGesture(bool mediaPlaybackRequiresUserGesture);
    void setMediaTypeOverride(const String& mediaTypeOverride);
    void setMinimumAccelerated2dCanvasSize(int minimumAccelerated2dCanvasSize);
    void setMinimumFontSize(int minimumFontSize);
    void setMinimumLogicalFontSize(int minimumLogicalFontSize);
    void setNavigateOnDragDrop(bool navigateOnDragDrop);
    void setNeedsSiteSpecificQuirks(bool needsSiteSpecificQuirks);
    void setOfflineWebApplicationCacheEnabled(bool offlineWebApplicationCacheEnabled);
    void setPageCacheSupportsPlugins(bool pageCacheSupportsPlugins);
    void setPasswordEchoDurationInSeconds(double passwordEchoDurationInSeconds);
    void setPasswordEchoEnabled(bool passwordEchoEnabled);
    void setPinchOverlayScrollbarThickness(int pinchOverlayScrollbarThickness);
    void setPinchVirtualViewportEnabled(bool pinchVirtualViewportEnabled);
    void setPluginsEnabled(bool pluginsEnabled);
    void setRegionBasedColumnsEnabled(bool regionBasedColumnsEnabled);
    void setReportScreenSizeInPhysicalPixelsQuirk(bool reportScreenSizeInPhysicalPixelsQuirk);
    void setScriptEnabled(bool scriptEnabled);
    void setScrollAnimatorEnabled(bool scrollAnimatorEnabled);
    void setSelectTrailingWhitespaceEnabled(bool selectTrailingWhitespaceEnabled);
    void setSelectionIncludesAltImageText(bool selectionIncludesAltImageText);
    void setShouldClearDocumentBackground(bool shouldClearDocumentBackground);
    void setShouldPrintBackgrounds(bool shouldPrintBackgrounds);
    void setShouldRespectImageOrientation(bool shouldRespectImageOrientation);
    void setShrinksStandaloneImagesToFit(bool shrinksStandaloneImagesToFit);
    void setSmartInsertDeleteEnabled(bool smartInsertDeleteEnabled);
    void setSpatialNavigationEnabled(bool spatialNavigationEnabled);
    void setSupportsMultipleWindows(bool supportsMultipleWindows);
    void setSyncXHRInDocumentsEnabled(bool syncXHRInDocumentsEnabled);
    void setTextAreasAreResizable(bool textAreasAreResizable);
    void setTouchAdjustmentEnabled(bool touchAdjustmentEnabled);
    void setTouchDragDropEnabled(bool touchDragDropEnabled);
    void setTouchEditingEnabled(bool touchEditingEnabled);
    void setTreatIPAddressAsDomain(bool treatIPAddressAsDomain);
    void setUnifiedTextCheckerEnabled(bool unifiedTextCheckerEnabled);
    void setUnsafePluginPastingEnabled(bool unsafePluginPastingEnabled);
    void setUseLegacyBackgroundSizeShorthandBehavior(bool useLegacyBackgroundSizeShorthandBehavior);
    void setUseSolidColorScrollbars(bool useSolidColorScrollbars);
    void setUseWideViewport(bool useWideViewport);
    void setUsesEncodingDetector(bool usesEncodingDetector);
    void setValidationMessageTimerMagnification(int validationMessageTimerMagnification);
    void setViewportEnabled(bool viewportEnabled);
    void setViewportMetaEnabled(bool viewportMetaEnabled);
    void setViewportMetaMergeContentQuirk(bool viewportMetaMergeContentQuirk);
    void setViewportMetaZeroValuesQuirk(bool viewportMetaZeroValuesQuirk);
    void setWebAudioEnabled(bool webAudioEnabled);
    void setWebGLEnabled(bool webGLEnabled);
    void setWebGLErrorsToConsoleEnabled(bool webGLErrorsToConsoleEnabled);
    void setWebSecurityEnabled(bool webSecurityEnabled);
    void setWideViewportQuirkEnabled(bool wideViewportQuirkEnabled);
    void setXSSAuditorEnabled(bool xssAuditorEnabled);

    virtual void trace(Visitor*) { }

private:
    Page* m_page;

    bool m_DOMPasteAllowed;
    bool m_accelerated2dCanvasEnabled;
    int m_accelerated2dCanvasMSAASampleCount;
    bool m_acceleratedCompositingEnabled;
    bool m_acceleratedCompositingForCanvasEnabled;
    bool m_acceleratedCompositingForFiltersEnabled;
    bool m_acceleratedCompositingForFixedPositionEnabled;
    bool m_acceleratedCompositingForFixedRootBackgroundEnabled;
    bool m_acceleratedCompositingForOverflowScrollEnabled;
    bool m_acceleratedCompositingForVideoEnabled;
    double m_accessibilityFontScaleFactor;
    bool m_allowConnectingInsecureWebSocket;
    bool m_allowCustomScrollbarInMainFrame;
    bool m_allowDisplayOfInsecureContent;
    bool m_allowFileAccessFromFileURLs;
    bool m_allowRunningOfInsecureContent;
    bool m_allowScriptsToCloseWindows;
    bool m_allowUniversalAccessFromFileURLs;
    bool m_antialiased2dCanvasEnabled;
    bool m_asynchronousSpellCheckingEnabled;
    bool m_caretBrowsingEnabled;
    bool m_compositedScrollingForFramesEnabled;
    bool m_compositorDrivenAcceleratedScrollingEnabled;
    bool m_compositorTouchHitTesting;
    bool m_containerCullingEnabled;
    bool m_cookieEnabled;
    int m_defaultFixedFontSize;
    int m_defaultFontSize;
    String m_defaultTextEncodingName;
    String m_defaultVideoPosterURL;
    bool m_deferredFiltersEnabled;
    double m_deviceScaleAdjustment;
    bool m_deviceSupportsMouse;
    bool m_deviceSupportsTouch;
    bool m_disallowFullscreenForNonMediaElements;
    bool m_dnsPrefetchingEnabled;
    bool m_downloadableBinaryFontsEnabled;
    bool m_forceZeroLayoutHeight;
    bool m_fullscreenSupported;
    bool m_hyperlinkAuditingEnabled;
    bool m_ignoreMainFrameOverflowHiddenQuirk;
    bool m_imagesEnabled;
    bool m_javaEnabled;
    bool m_javaScriptCanAccessClipboard;
    bool m_javaScriptCanOpenWindowsAutomatically;
    bool m_layerSquashingEnabled;
    bool m_loadWithOverviewMode;
    bool m_loadsImagesAutomatically;
    bool m_localStorageEnabled;
    bool m_mainFrameClipsContent;
    int m_maxBeaconTransmission;
    int m_maxTouchPoints;
    bool m_mediaControlsOverlayPlayButtonEnabled;
    bool m_mediaPlaybackRequiresUserGesture;
    String m_mediaTypeOverride;
    int m_minimumAccelerated2dCanvasSize;
    int m_minimumFontSize;
    int m_minimumLogicalFontSize;
    bool m_navigateOnDragDrop;
    bool m_needsSiteSpecificQuirks;
    bool m_offlineWebApplicationCacheEnabled;
    bool m_pageCacheSupportsPlugins;
    double m_passwordEchoDurationInSeconds;
    bool m_passwordEchoEnabled;
    int m_pinchOverlayScrollbarThickness;
    bool m_pinchVirtualViewportEnabled;
    bool m_pluginsEnabled;
    bool m_regionBasedColumnsEnabled;
    bool m_reportScreenSizeInPhysicalPixelsQuirk;
    bool m_scriptEnabled;
    bool m_scrollAnimatorEnabled;
    bool m_selectTrailingWhitespaceEnabled;
    bool m_selectionIncludesAltImageText;
    bool m_shouldClearDocumentBackground;
    bool m_shouldPrintBackgrounds;
    bool m_shouldRespectImageOrientation;
    bool m_shrinksStandaloneImagesToFit;
    bool m_smartInsertDeleteEnabled;
    bool m_spatialNavigationEnabled;
    bool m_supportsMultipleWindows;
    bool m_syncXHRInDocumentsEnabled;
    bool m_textAreasAreResizable;
    bool m_touchAdjustmentEnabled;
    bool m_touchDragDropEnabled;
    bool m_touchEditingEnabled;
    bool m_treatIPAddressAsDomain;
    bool m_unifiedTextCheckerEnabled;
    bool m_unsafePluginPastingEnabled;
    bool m_useLegacyBackgroundSizeShorthandBehavior;
    bool m_useSolidColorScrollbars;
    bool m_useWideViewport;
    bool m_usesEncodingDetector;
    int m_validationMessageTimerMagnification;
    bool m_viewportEnabled;
    bool m_viewportMetaEnabled;
    bool m_viewportMetaMergeContentQuirk;
    bool m_viewportMetaZeroValuesQuirk;
    bool m_webAudioEnabled;
    bool m_webGLEnabled;
    bool m_webGLErrorsToConsoleEnabled;
    bool m_webSecurityEnabled;
    bool m_wideViewportQuirkEnabled;
    bool m_xssAuditorEnabled;
};

} // namespace WebCore

#endif // InternalSettingsGenerated_h
