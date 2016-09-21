// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#ifdef SKIP_STATIC_CONSTRUCTORS_ON_GCC
#define MEDIA_FEATURE_NAMES_HIDE_GLOBALS 1
#endif

#include "MediaFeatureNames.h"

#include "wtf/StaticConstructors.h"

namespace WebCore {
namespace MediaFeatureNames {

using namespace WTF;

DEFINE_GLOBAL(AtomicString, devicePixelRatioMediaFeature)
DEFINE_GLOBAL(AtomicString, maxDevicePixelRatioMediaFeature)
DEFINE_GLOBAL(AtomicString, minDevicePixelRatioMediaFeature)
DEFINE_GLOBAL(AtomicString, transform3dMediaFeature)
DEFINE_GLOBAL(AtomicString, aspectRatioMediaFeature)
DEFINE_GLOBAL(AtomicString, colorMediaFeature)
DEFINE_GLOBAL(AtomicString, colorIndexMediaFeature)
DEFINE_GLOBAL(AtomicString, deviceAspectRatioMediaFeature)
DEFINE_GLOBAL(AtomicString, deviceHeightMediaFeature)
DEFINE_GLOBAL(AtomicString, deviceWidthMediaFeature)
DEFINE_GLOBAL(AtomicString, gridMediaFeature)
DEFINE_GLOBAL(AtomicString, heightMediaFeature)
DEFINE_GLOBAL(AtomicString, hoverMediaFeature)
DEFINE_GLOBAL(AtomicString, maxAspectRatioMediaFeature)
DEFINE_GLOBAL(AtomicString, maxColorMediaFeature)
DEFINE_GLOBAL(AtomicString, maxColorIndexMediaFeature)
DEFINE_GLOBAL(AtomicString, maxDeviceAspectRatioMediaFeature)
DEFINE_GLOBAL(AtomicString, maxDeviceHeightMediaFeature)
DEFINE_GLOBAL(AtomicString, maxDeviceWidthMediaFeature)
DEFINE_GLOBAL(AtomicString, maxHeightMediaFeature)
DEFINE_GLOBAL(AtomicString, maxMonochromeMediaFeature)
DEFINE_GLOBAL(AtomicString, maxResolutionMediaFeature)
DEFINE_GLOBAL(AtomicString, maxWidthMediaFeature)
DEFINE_GLOBAL(AtomicString, minAspectRatioMediaFeature)
DEFINE_GLOBAL(AtomicString, minColorMediaFeature)
DEFINE_GLOBAL(AtomicString, minColorIndexMediaFeature)
DEFINE_GLOBAL(AtomicString, minDeviceAspectRatioMediaFeature)
DEFINE_GLOBAL(AtomicString, minDeviceHeightMediaFeature)
DEFINE_GLOBAL(AtomicString, minDeviceWidthMediaFeature)
DEFINE_GLOBAL(AtomicString, minHeightMediaFeature)
DEFINE_GLOBAL(AtomicString, minMonochromeMediaFeature)
DEFINE_GLOBAL(AtomicString, minResolutionMediaFeature)
DEFINE_GLOBAL(AtomicString, minWidthMediaFeature)
DEFINE_GLOBAL(AtomicString, monochromeMediaFeature)
DEFINE_GLOBAL(AtomicString, orientationMediaFeature)
DEFINE_GLOBAL(AtomicString, pointerMediaFeature)
DEFINE_GLOBAL(AtomicString, resolutionMediaFeature)
DEFINE_GLOBAL(AtomicString, scanMediaFeature)
DEFINE_GLOBAL(AtomicString, widthMediaFeature)

void init()
{
    StringImpl* devicePixelRatioMediaFeatureImpl = StringImpl::createStatic("-webkit-device-pixel-ratio", 26, 12532418);
    StringImpl* maxDevicePixelRatioMediaFeatureImpl = StringImpl::createStatic("-webkit-max-device-pixel-ratio", 30, 15699528);
    StringImpl* minDevicePixelRatioMediaFeatureImpl = StringImpl::createStatic("-webkit-min-device-pixel-ratio", 30, 6230028);
    StringImpl* transform3dMediaFeatureImpl = StringImpl::createStatic("-webkit-transform-3d", 20, 11719803);
    StringImpl* aspectRatioMediaFeatureImpl = StringImpl::createStatic("aspect-ratio", 12, 10453312);
    StringImpl* colorMediaFeatureImpl = StringImpl::createStatic("color", 5, 2734929);
    StringImpl* colorIndexMediaFeatureImpl = StringImpl::createStatic("color-index", 11, 13972807);
    StringImpl* deviceAspectRatioMediaFeatureImpl = StringImpl::createStatic("device-aspect-ratio", 19, 5022071);
    StringImpl* deviceHeightMediaFeatureImpl = StringImpl::createStatic("device-height", 13, 9832216);
    StringImpl* deviceWidthMediaFeatureImpl = StringImpl::createStatic("device-width", 12, 4551487);
    StringImpl* gridMediaFeatureImpl = StringImpl::createStatic("grid", 4, 1786690);
    StringImpl* heightMediaFeatureImpl = StringImpl::createStatic("height", 6, 6697833);
    StringImpl* hoverMediaFeatureImpl = StringImpl::createStatic("hover", 5, 3481422);
    StringImpl* maxAspectRatioMediaFeatureImpl = StringImpl::createStatic("max-aspect-ratio", 16, 12094319);
    StringImpl* maxColorMediaFeatureImpl = StringImpl::createStatic("max-color", 9, 629821);
    StringImpl* maxColorIndexMediaFeatureImpl = StringImpl::createStatic("max-color-index", 15, 7430117);
    StringImpl* maxDeviceAspectRatioMediaFeatureImpl = StringImpl::createStatic("max-device-aspect-ratio", 23, 12942084);
    StringImpl* maxDeviceHeightMediaFeatureImpl = StringImpl::createStatic("max-device-height", 17, 1041693);
    StringImpl* maxDeviceWidthMediaFeatureImpl = StringImpl::createStatic("max-device-width", 16, 5334797);
    StringImpl* maxHeightMediaFeatureImpl = StringImpl::createStatic("max-height", 10, 12768354);
    StringImpl* maxMonochromeMediaFeatureImpl = StringImpl::createStatic("max-monochrome", 14, 15262871);
    StringImpl* maxResolutionMediaFeatureImpl = StringImpl::createStatic("max-resolution", 14, 956048);
    StringImpl* maxWidthMediaFeatureImpl = StringImpl::createStatic("max-width", 9, 6572179);
    StringImpl* minAspectRatioMediaFeatureImpl = StringImpl::createStatic("min-aspect-ratio", 16, 16386843);
    StringImpl* minColorMediaFeatureImpl = StringImpl::createStatic("min-color", 9, 12833068);
    StringImpl* minColorIndexMediaFeatureImpl = StringImpl::createStatic("min-color-index", 15, 9407342);
    StringImpl* minDeviceAspectRatioMediaFeatureImpl = StringImpl::createStatic("min-device-aspect-ratio", 23, 5241967);
    StringImpl* minDeviceHeightMediaFeatureImpl = StringImpl::createStatic("min-device-height", 17, 9440595);
    StringImpl* minDeviceWidthMediaFeatureImpl = StringImpl::createStatic("min-device-width", 16, 8878672);
    StringImpl* minHeightMediaFeatureImpl = StringImpl::createStatic("min-height", 10, 12467950);
    StringImpl* minMonochromeMediaFeatureImpl = StringImpl::createStatic("min-monochrome", 14, 5118153);
    StringImpl* minResolutionMediaFeatureImpl = StringImpl::createStatic("min-resolution", 14, 3458349);
    StringImpl* minWidthMediaFeatureImpl = StringImpl::createStatic("min-width", 9, 1786673);
    StringImpl* monochromeMediaFeatureImpl = StringImpl::createStatic("monochrome", 10, 1202583);
    StringImpl* orientationMediaFeatureImpl = StringImpl::createStatic("orientation", 11, 10339552);
    StringImpl* pointerMediaFeatureImpl = StringImpl::createStatic("pointer", 7, 8738242);
    StringImpl* resolutionMediaFeatureImpl = StringImpl::createStatic("resolution", 10, 1320318);
    StringImpl* scanMediaFeatureImpl = StringImpl::createStatic("scan", 4, 2480616);
    StringImpl* widthMediaFeatureImpl = StringImpl::createStatic("width", 5, 12902275);

    new ((void*)&devicePixelRatioMediaFeature) AtomicString(devicePixelRatioMediaFeatureImpl);
    new ((void*)&maxDevicePixelRatioMediaFeature) AtomicString(maxDevicePixelRatioMediaFeatureImpl);
    new ((void*)&minDevicePixelRatioMediaFeature) AtomicString(minDevicePixelRatioMediaFeatureImpl);
    new ((void*)&transform3dMediaFeature) AtomicString(transform3dMediaFeatureImpl);
    new ((void*)&aspectRatioMediaFeature) AtomicString(aspectRatioMediaFeatureImpl);
    new ((void*)&colorMediaFeature) AtomicString(colorMediaFeatureImpl);
    new ((void*)&colorIndexMediaFeature) AtomicString(colorIndexMediaFeatureImpl);
    new ((void*)&deviceAspectRatioMediaFeature) AtomicString(deviceAspectRatioMediaFeatureImpl);
    new ((void*)&deviceHeightMediaFeature) AtomicString(deviceHeightMediaFeatureImpl);
    new ((void*)&deviceWidthMediaFeature) AtomicString(deviceWidthMediaFeatureImpl);
    new ((void*)&gridMediaFeature) AtomicString(gridMediaFeatureImpl);
    new ((void*)&heightMediaFeature) AtomicString(heightMediaFeatureImpl);
    new ((void*)&hoverMediaFeature) AtomicString(hoverMediaFeatureImpl);
    new ((void*)&maxAspectRatioMediaFeature) AtomicString(maxAspectRatioMediaFeatureImpl);
    new ((void*)&maxColorMediaFeature) AtomicString(maxColorMediaFeatureImpl);
    new ((void*)&maxColorIndexMediaFeature) AtomicString(maxColorIndexMediaFeatureImpl);
    new ((void*)&maxDeviceAspectRatioMediaFeature) AtomicString(maxDeviceAspectRatioMediaFeatureImpl);
    new ((void*)&maxDeviceHeightMediaFeature) AtomicString(maxDeviceHeightMediaFeatureImpl);
    new ((void*)&maxDeviceWidthMediaFeature) AtomicString(maxDeviceWidthMediaFeatureImpl);
    new ((void*)&maxHeightMediaFeature) AtomicString(maxHeightMediaFeatureImpl);
    new ((void*)&maxMonochromeMediaFeature) AtomicString(maxMonochromeMediaFeatureImpl);
    new ((void*)&maxResolutionMediaFeature) AtomicString(maxResolutionMediaFeatureImpl);
    new ((void*)&maxWidthMediaFeature) AtomicString(maxWidthMediaFeatureImpl);
    new ((void*)&minAspectRatioMediaFeature) AtomicString(minAspectRatioMediaFeatureImpl);
    new ((void*)&minColorMediaFeature) AtomicString(minColorMediaFeatureImpl);
    new ((void*)&minColorIndexMediaFeature) AtomicString(minColorIndexMediaFeatureImpl);
    new ((void*)&minDeviceAspectRatioMediaFeature) AtomicString(minDeviceAspectRatioMediaFeatureImpl);
    new ((void*)&minDeviceHeightMediaFeature) AtomicString(minDeviceHeightMediaFeatureImpl);
    new ((void*)&minDeviceWidthMediaFeature) AtomicString(minDeviceWidthMediaFeatureImpl);
    new ((void*)&minHeightMediaFeature) AtomicString(minHeightMediaFeatureImpl);
    new ((void*)&minMonochromeMediaFeature) AtomicString(minMonochromeMediaFeatureImpl);
    new ((void*)&minResolutionMediaFeature) AtomicString(minResolutionMediaFeatureImpl);
    new ((void*)&minWidthMediaFeature) AtomicString(minWidthMediaFeatureImpl);
    new ((void*)&monochromeMediaFeature) AtomicString(monochromeMediaFeatureImpl);
    new ((void*)&orientationMediaFeature) AtomicString(orientationMediaFeatureImpl);
    new ((void*)&pointerMediaFeature) AtomicString(pointerMediaFeatureImpl);
    new ((void*)&resolutionMediaFeature) AtomicString(resolutionMediaFeatureImpl);
    new ((void*)&scanMediaFeature) AtomicString(scanMediaFeatureImpl);
    new ((void*)&widthMediaFeature) AtomicString(widthMediaFeatureImpl);
}

} // MediaFeatureNames
} // WebCore
