// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InternalRuntimeFlags_h
#define InternalRuntimeFlags_h

#include "platform/RuntimeEnabledFeatures.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/RefCounted.h"

namespace WebCore {

class InternalRuntimeFlags : public RefCountedWillBeGarbageCollected<InternalRuntimeFlags> {
public:
    static PassRefPtrWillBeRawPtr<InternalRuntimeFlags> create()
    {
        return adoptRefWillBeNoop(new InternalRuntimeFlags);
    }

    bool applicationCacheEnabled() { return RuntimeEnabledFeatures::applicationCacheEnabled(); }
    bool audioVideoTracksEnabled() { return RuntimeEnabledFeatures::audioVideoTracksEnabled(); }
    bool authorShadowDOMForAnyElementEnabled() { return RuntimeEnabledFeatures::authorShadowDOMForAnyElementEnabled(); }
    bool batteryStatusEnabled() { return RuntimeEnabledFeatures::batteryStatusEnabled(); }
    bool beaconEnabled() { return RuntimeEnabledFeatures::beaconEnabled(); }
    bool prefixedVideoFullscreenEnabled() { return RuntimeEnabledFeatures::prefixedVideoFullscreenEnabled(); }
    bool bleedingEdgeFastPathsEnabled() { return RuntimeEnabledFeatures::bleedingEdgeFastPathsEnabled(); }
    bool themeColorEnabled() { return RuntimeEnabledFeatures::themeColorEnabled(); }
    bool clientHintsDprEnabled() { return RuntimeEnabledFeatures::clientHintsDprEnabled(); }
    bool cryptoEnabled() { return RuntimeEnabledFeatures::cryptoEnabled(); }
    bool cssAnimationUnprefixedEnabled() { return RuntimeEnabledFeatures::cssAnimationUnprefixedEnabled(); }
    bool cssCompositingEnabled() { return RuntimeEnabledFeatures::cssCompositingEnabled(); }
    bool cssExclusionsEnabled() { return RuntimeEnabledFeatures::cssExclusionsEnabled(); }
    bool cssGridLayoutEnabled() { return RuntimeEnabledFeatures::cssGridLayoutEnabled(); }
    bool cssMaskSourceTypeEnabled() { return RuntimeEnabledFeatures::cssMaskSourceTypeEnabled(); }
    bool cssomSmoothScrollEnabled() { return RuntimeEnabledFeatures::cssomSmoothScrollEnabled(); }
    bool cssShapesEnabled() { return RuntimeEnabledFeatures::cssShapesEnabled(); }
    bool cssStickyPositionEnabled() { return RuntimeEnabledFeatures::cssStickyPositionEnabled(); }
    bool cssTouchActionEnabled() { return RuntimeEnabledFeatures::cssTouchActionEnabled(); }
    bool cssTouchActionDelayEnabled() { return RuntimeEnabledFeatures::cssTouchActionDelayEnabled(); }
    bool cssTransformsUnprefixedEnabled() { return RuntimeEnabledFeatures::cssTransformsUnprefixedEnabled(); }
    bool cssViewportEnabled() { return RuntimeEnabledFeatures::cssViewportEnabled(); }
    bool cssWillChangeEnabled() { return RuntimeEnabledFeatures::cssWillChangeEnabled(); }
    bool css3TextEnabled() { return RuntimeEnabledFeatures::css3TextEnabled(); }
    bool css3TextDecorationsEnabled() { return RuntimeEnabledFeatures::css3TextDecorationsEnabled(); }
    bool customSchemeHandlerEnabled() { return RuntimeEnabledFeatures::customSchemeHandlerEnabled(); }
    bool databaseEnabled() { return RuntimeEnabledFeatures::databaseEnabled(); }
    bool deviceLightEnabled() { return RuntimeEnabledFeatures::deviceLightEnabled(); }
    bool dialogElementEnabled() { return RuntimeEnabledFeatures::dialogElementEnabled(); }
    bool encodingAPIEnabled() { return RuntimeEnabledFeatures::encodingAPIEnabled(); }
    bool encryptedMediaEnabled() { return RuntimeEnabledFeatures::encryptedMediaEnabled(); }
    bool encryptedMediaAnyVersionEnabled() { return RuntimeEnabledFeatures::encryptedMediaAnyVersionEnabled(); }
    bool experimentalCanvasFeaturesEnabled() { return RuntimeEnabledFeatures::experimentalCanvasFeaturesEnabled(); }
    bool experimentalContentSecurityPolicyFeaturesEnabled() { return RuntimeEnabledFeatures::experimentalContentSecurityPolicyFeaturesEnabled(); }
    bool experimentalWebSocketEnabled() { return RuntimeEnabledFeatures::experimentalWebSocketEnabled(); }
    bool fastMobileScrollingEnabled() { return RuntimeEnabledFeatures::fastMobileScrollingEnabled(); }
    bool fileAPIBlobCloseEnabled() { return RuntimeEnabledFeatures::fileAPIBlobCloseEnabled(); }
    bool fileConstructorEnabled() { return RuntimeEnabledFeatures::fileConstructorEnabled(); }
    bool fileSystemEnabled() { return RuntimeEnabledFeatures::fileSystemEnabled(); }
    bool fontLoadEventsEnabled() { return RuntimeEnabledFeatures::fontLoadEventsEnabled(); }
    bool fastTextAutosizingEnabled() { return RuntimeEnabledFeatures::fastTextAutosizingEnabled(); }
    bool gamepadEnabled() { return RuntimeEnabledFeatures::gamepadEnabled(); }
    bool htmlImportsEnabled() { return RuntimeEnabledFeatures::htmlImportsEnabled(); }
    bool highResolutionTimeInWorkersEnabled() { return RuntimeEnabledFeatures::highResolutionTimeInWorkersEnabled(); }
    bool imeAPIEnabled() { return RuntimeEnabledFeatures::imeAPIEnabled(); }
    bool imageDataConstructorEnabled() { return RuntimeEnabledFeatures::imageDataConstructorEnabled(); }
    bool indexedDBExperimentalEnabled() { return RuntimeEnabledFeatures::indexedDBExperimentalEnabled(); }
    bool inputModeAttributeEnabled() { return RuntimeEnabledFeatures::inputModeAttributeEnabled(); }
    bool langAttributeAwareFormControlUIEnabled() { return RuntimeEnabledFeatures::langAttributeAwareFormControlUIEnabled(); }
    bool layerSquashingEnabled() { return RuntimeEnabledFeatures::layerSquashingEnabled(); }
    bool prefixedEncryptedMediaEnabled() { return RuntimeEnabledFeatures::prefixedEncryptedMediaEnabled(); }
    bool localStorageEnabled() { return RuntimeEnabledFeatures::localStorageEnabled(); }
    bool manifestEnabled() { return RuntimeEnabledFeatures::manifestEnabled(); }
    bool mediaEnabled() { return RuntimeEnabledFeatures::mediaEnabled(); }
    bool mediaControllerEnabled() { return RuntimeEnabledFeatures::mediaControllerEnabled(); }
    bool mediaQueryParserEnabled() { return RuntimeEnabledFeatures::mediaQueryParserEnabled(); }
    bool mediaSourceEnabled() { return RuntimeEnabledFeatures::mediaSourceEnabled(); }
    bool mediaSourceExperimentalEnabled() { return RuntimeEnabledFeatures::mediaSourceExperimentalEnabled(); }
    bool mediaStreamEnabled() { return RuntimeEnabledFeatures::mediaStreamEnabled(); }
    bool memoryInfoInWorkersEnabled() { return RuntimeEnabledFeatures::memoryInfoInWorkersEnabled(); }
    bool navigationTransitionsEnabled() { return RuntimeEnabledFeatures::navigationTransitionsEnabled(); }
    bool navigatorContentUtilsEnabled() { return RuntimeEnabledFeatures::navigatorContentUtilsEnabled(); }
    bool networkInformationEnabled() { return RuntimeEnabledFeatures::networkInformationEnabled(); }
    bool notificationsEnabled() { return RuntimeEnabledFeatures::notificationsEnabled(); }
    bool objectFitPositionEnabled() { return RuntimeEnabledFeatures::objectFitPositionEnabled(); }
    bool orientationEventEnabled() { return RuntimeEnabledFeatures::orientationEventEnabled(); }
    bool overlayFullscreenVideoEnabled() { return RuntimeEnabledFeatures::overlayFullscreenVideoEnabled(); }
    bool pagePopupEnabled() { return RuntimeEnabledFeatures::pagePopupEnabled(); }
    bool path2DEnabled() { return RuntimeEnabledFeatures::path2DEnabled(); }
    bool pathOpsSVGClippingEnabled() { return RuntimeEnabledFeatures::pathOpsSVGClippingEnabled(); }
    bool peerConnectionEnabled() { return RuntimeEnabledFeatures::peerConnectionEnabled(); }
    bool preciseMemoryInfoEnabled() { return RuntimeEnabledFeatures::preciseMemoryInfoEnabled(); }
    bool pushMessagingEnabled() { return RuntimeEnabledFeatures::pushMessagingEnabled(); }
    bool quotaEnabled() { return RuntimeEnabledFeatures::quotaEnabled(); }
    bool quotaPromiseEnabled() { return RuntimeEnabledFeatures::quotaPromiseEnabled(); }
    bool overlayScrollbarsEnabled() { return RuntimeEnabledFeatures::overlayScrollbarsEnabled(); }
    bool regionBasedColumnsEnabled() { return RuntimeEnabledFeatures::regionBasedColumnsEnabled(); }
    bool repaintAfterLayoutEnabled() { return RuntimeEnabledFeatures::repaintAfterLayoutEnabled(); }
    bool requestAutocompleteEnabled() { return RuntimeEnabledFeatures::requestAutocompleteEnabled(); }
    bool screenOrientationEnabled() { return RuntimeEnabledFeatures::screenOrientationEnabled(); }
    bool scriptedSpeechEnabled() { return RuntimeEnabledFeatures::scriptedSpeechEnabled(); }
    bool scrollTopLeftInteropEnabled() { return RuntimeEnabledFeatures::scrollTopLeftInteropEnabled(); }
    bool serviceWorkerEnabled() { return RuntimeEnabledFeatures::serviceWorkerEnabled(); }
    bool sessionStorageEnabled() { return RuntimeEnabledFeatures::sessionStorageEnabled(); }
    bool sharedWorkerEnabled() { return RuntimeEnabledFeatures::sharedWorkerEnabled(); }
    bool pictureSizesEnabled() { return RuntimeEnabledFeatures::pictureSizesEnabled(); }
    bool pictureEnabled() { return RuntimeEnabledFeatures::pictureEnabled(); }
    bool showModalDialogEnabled() { return RuntimeEnabledFeatures::showModalDialogEnabled(); }
    bool speechSynthesisEnabled() { return RuntimeEnabledFeatures::speechSynthesisEnabled(); }
    bool streamEnabled() { return RuntimeEnabledFeatures::streamEnabled(); }
    bool subpixelFontScalingEnabled() { return RuntimeEnabledFeatures::subpixelFontScalingEnabled(); }
    bool subresourceIntegrityEnabled() { return RuntimeEnabledFeatures::subresourceIntegrityEnabled(); }
    bool targetedStyleRecalcEnabled() { return RuntimeEnabledFeatures::targetedStyleRecalcEnabled(); }
    bool touchEnabled() { return RuntimeEnabledFeatures::touchEnabled(); }
    bool touchIconLoadingEnabled() { return RuntimeEnabledFeatures::touchIconLoadingEnabled(); }
    bool threadedParserDataReceiverEnabled() { return RuntimeEnabledFeatures::threadedParserDataReceiverEnabled(); }
    bool userSelectAllEnabled() { return RuntimeEnabledFeatures::userSelectAllEnabled(); }
    bool webAnimationsSVGEnabled() { return RuntimeEnabledFeatures::webAnimationsSVGEnabled(); }
    bool webAnimationsElementAnimateEnabled() { return RuntimeEnabledFeatures::webAnimationsElementAnimateEnabled(); }
    bool webAnimationsAPIEnabled() { return RuntimeEnabledFeatures::webAnimationsAPIEnabled(); }
#if ENABLE(WEB_AUDIO)
    bool webAudioEnabled() { return RuntimeEnabledFeatures::webAudioEnabled(); }
#endif // ENABLE(WEB_AUDIO)
    bool webGLDraftExtensionsEnabled() { return RuntimeEnabledFeatures::webGLDraftExtensionsEnabled(); }
    bool webGLImageChromiumEnabled() { return RuntimeEnabledFeatures::webGLImageChromiumEnabled(); }
    bool webMIDIEnabled() { return RuntimeEnabledFeatures::webMIDIEnabled(); }
    bool webVTTRegionsEnabled() { return RuntimeEnabledFeatures::webVTTRegionsEnabled(); }
    bool woff2Enabled() { return RuntimeEnabledFeatures::woff2Enabled(); }
    bool xsltEnabled() { return RuntimeEnabledFeatures::xsltEnabled(); }
    bool pseudoClassesInMatchingCriteriaInAuthorShadowTreesEnabled() { return RuntimeEnabledFeatures::pseudoClassesInMatchingCriteriaInAuthorShadowTreesEnabled(); }

    void trace(Visitor*) { }

private:
    InternalRuntimeFlags() { }
};

} // namespace WebCore

#endif // InternalRuntimeFlags_h
