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

#include "elastos/droid/media/CAudioPort.h"
#include "elastos/droid/media/CAudioPortConfig.h"
#include "elastos/droid/media/CFocusRequester.h"
#include "elastos/droid/media/CMediaFocusControl.h"
#include "elastos/droid/media/CMediaRouterClientState.h"
#include "elastos/droid/media/CMediaRouterClientStateRouteInfo.h"
#include "elastos/droid/media/CMediaRouterStaticAudioRoutesObserver.h"
#include "elastos/droid/media/CMediaRouterStaticClient.h"
#include "elastos/droid/media/CMediaSyncEvent.h"
#include "elastos/droid/media/CPlayerRecord.h"
#include "elastos/droid/media/CPlayerRecordRccPlaybackState.h"
#include "elastos/droid/media/CPlayerRecordRemotePlaybackState.h"
#include "elastos/droid/media/CRemoteDisplayInfo.h"
#include "elastos/droid/media/CRemoteDisplayState.h"
#include "elastos/droid/media/CSRTRenderer.h"
#include "elastos/droid/media/CSRTTrack.h"
#include "elastos/droid/media/CSubtitleController.h"
#include "elastos/droid/media/CSubtitleData.h"
#include "elastos/droid/media/CTextTrackCue.h"
#include "elastos/droid/media/CTextTrackRegion.h"
#include "elastos/droid/media/CUnstyledTextExtractor.h"
#include "elastos/droid/media/CWebVttParser.h"
#include "elastos/droid/media/CWebVttRendererTextTrackCue.h"
#include "elastos/droid/media/CWebVttRendererTokenizer.h"
#include "elastos/droid/media/CWebVttRendererTokenizerDataTokenizer.h"
#include "elastos/droid/media/CWebVttRendererTokenizerTagTokenizer.h"
#include "elastos/droid/media/CWebVttRenderingWidget.h"
#include "elastos/droid/media/CWebVttTrack.h"
#include "elastos/droid/media/CTtmlRenderer.h"
#include "elastos/droid/media/CMediaRouterRouteInfo.h"
#include "elastos/droid/media/CMediaRouterRouteGroup.h"
#include "elastos/droid/media/CRouteCategory.h"
#include "elastos/droid/media/CMediaRouterSimpleCallback.h"
#include "elastos/droid/media/CMediaRouterUserRouteInfo.h"
#include "elastos/droid/media/CRemoteController.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_OBJECT_IMPL(CAudioPort)
CAR_OBJECT_IMPL(CAudioPortConfig)
CAR_OBJECT_IMPL(CFocusRequester)
CAR_OBJECT_IMPL(CMediaFocusControl)
CAR_OBJECT_IMPL(CSubtitleController)
CAR_OBJECT_IMPL(CSubtitleData)
CAR_OBJECT_IMPL(CWebVttRendererTokenizer)
CAR_OBJECT_IMPL(CWebVttRendererTokenizerDataTokenizer)
CAR_OBJECT_IMPL(CWebVttRendererTokenizerTagTokenizer)
CAR_OBJECT_IMPL(CWebVttRendererTextTrackCue)
CAR_OBJECT_IMPL(CWebVttParser)
CAR_OBJECT_IMPL(CTextTrackRegion)
CAR_OBJECT_IMPL(CWebVttRenderingWidget)
CAR_OBJECT_IMPL(CTextTrackCue)
CAR_OBJECT_IMPL(CUnstyledTextExtractor)
CAR_OBJECT_IMPL(CMediaSyncEvent)
CAR_OBJECT_IMPL(CSRTRenderer)
CAR_OBJECT_IMPL(CSRTTrack)
CAR_OBJECT_IMPL(CWebVttTrack)
CAR_OBJECT_IMPL(CMediaRouterClientState)
CAR_OBJECT_IMPL(CMediaRouterClientStateRouteInfo)
CAR_OBJECT_IMPL(CRemoteDisplayState)
CAR_OBJECT_IMPL(CRemoteDisplayInfo)
CAR_OBJECT_IMPL(CTtmlRenderer)
CAR_OBJECT_IMPL(CMediaRouterRouteInfo)
CAR_OBJECT_IMPL(CMediaRouterUserRouteInfo)
CAR_OBJECT_IMPL(CMediaRouterRouteGroup)
CAR_OBJECT_IMPL(CMediaRouterStaticAudioRoutesObserver)
CAR_OBJECT_IMPL(CMediaRouterStaticClient)
CAR_OBJECT_IMPL(CRouteCategory)
CAR_OBJECT_IMPL(CMediaRouterSimpleCallback)
CAR_OBJECT_IMPL(CPlayerRecordRccPlaybackState)
CAR_OBJECT_IMPL(CPlayerRecordRemotePlaybackState)
CAR_OBJECT_IMPL(CPlayerRecord)
CAR_OBJECT_IMPL(CRemoteController)

} // namespace Media
} // namepsace Droid
} // namespace Elastos
