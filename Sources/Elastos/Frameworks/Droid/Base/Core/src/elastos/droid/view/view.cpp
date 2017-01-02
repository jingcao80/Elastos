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

#include "elastos/droid/view/CViewBaseSavedState.h"
#include "elastos/droid/view/CAbsSavedState.h"
#include "elastos/droid/view/CView.h"
#include "elastos/droid/view/CTextureView.h"
#include "elastos/droid/view/CMenuInflater.h"
#include "elastos/droid/view/CContextThemeWrapper.h"
#include "elastos/droid/view/CMotionEvent.h"
#include "elastos/droid/view/CAccessibilityInteractionController.h"
#include "elastos/droid/view/CDispatcherState.h"
#include "elastos/droid/view/CDisplay.h"
#include "elastos/droid/view/CDisplayInfo.h"
#include "elastos/droid/view/CDragEvent.h"
#include "elastos/droid/view/CGestureDetector.h"
#include "elastos/droid/view/CGestureDetectorSimpleOnGestureListener.h"
#include "elastos/droid/view/CGraphicBuffer.h"
#include "elastos/droid/view/CInputChannel.h"
#include "elastos/droid/view/CInputDevice.h"
#include "elastos/droid/view/CKeyCharacterMap.h"
#include "elastos/droid/view/CKeyData.h"
#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/view/CPhysicalDisplayInfo.h"
#include "elastos/droid/view/CPointerCoords.h"
#include "elastos/droid/view/CPointerProperties.h"
#include "elastos/droid/view/CRenderNodeAnimator.h"
#include "elastos/droid/view/CScaleGestureDetector.h"
#include "elastos/droid/view/CScaleGestureDetectorSimpleOnScaleGestureListener.h"
#include "elastos/droid/view/CSurface.h"
#include "elastos/droid/view/CSurfaceControl.h"
#include "elastos/droid/view/CSurfaceView.h"
#include "elastos/droid/view/CTouchDelegate.h"
#include "elastos/droid/view/CViewConfiguration.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/view/CViewGroupMarginLayoutParams.h"
#include "elastos/droid/view/CViewOverlay.h"
#include "elastos/droid/view/CViewPropertyAnimator.h"
#include "elastos/droid/view/CViewStub.h"
#include "elastos/droid/view/CViewTreeObserver.h"


namespace Elastos {
namespace Droid {
namespace View {


CAR_OBJECT_IMPL(CViewBaseSavedState)
CAR_OBJECT_IMPL(CAbsSavedState)
CAR_OBJECT_IMPL(CTextureView)
CAR_OBJECT_IMPL(CMenuInflater)

CAR_OBJECT_IMPL(CAccessibilityInteractionController)
CAR_OBJECT_IMPL(CContextThemeWrapper)
CAR_OBJECT_IMPL(CDispatcherState)
CAR_OBJECT_IMPL(CDisplay)
CAR_OBJECT_IMPL(CDisplayInfo)
CAR_OBJECT_IMPL(CDragEvent)
CAR_OBJECT_IMPL(CGestureDetector)
CAR_OBJECT_IMPL(CGestureDetectorSimpleOnGestureListener)
CAR_OBJECT_IMPL(CGraphicBuffer)
CAR_OBJECT_IMPL(CInputChannel)
CAR_OBJECT_IMPL(CInputDevice)
CAR_OBJECT_IMPL(CKeyCharacterMap)
CAR_OBJECT_IMPL(CKeyData)
CAR_OBJECT_IMPL(CKeyEvent)
CAR_OBJECT_IMPL(CMotionEvent)
CAR_OBJECT_IMPL(CPhysicalDisplayInfo)
CAR_OBJECT_IMPL(CPointerCoords)
CAR_OBJECT_IMPL(CPointerProperties)
CAR_OBJECT_IMPL(CRenderNodeAnimator)
CAR_OBJECT_IMPL(CScaleGestureDetector)
CAR_OBJECT_IMPL(CScaleGestureDetectorSimpleOnScaleGestureListener)
CAR_OBJECT_IMPL(CSurface)
CAR_OBJECT_IMPL(CSurfaceControl)
CAR_OBJECT_IMPL(CSurfaceView)
CAR_OBJECT_IMPL(CTouchDelegate)
CAR_OBJECT_IMPL(CView)
CAR_OBJECT_IMPL(CViewConfiguration)
CAR_OBJECT_IMPL(CViewGroupLayoutParams)
CAR_OBJECT_IMPL(CViewGroupMarginLayoutParams)
CAR_OBJECT_IMPL(CViewOverlay)
CAR_OBJECT_IMPL(CViewPropertyAnimator)
CAR_OBJECT_IMPL(CViewStub)
CAR_OBJECT_IMPL(CViewTreeObserver)


} // namespace View
} // namespace Droid
} // namespace Elastos
