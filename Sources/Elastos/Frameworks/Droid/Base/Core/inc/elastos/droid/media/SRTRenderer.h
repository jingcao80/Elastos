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

#ifndef _ELASTOS_DROID_MEDIA_SRTRENDERER_H_
#define _ELASTOS_DROID_MEDIA_SRTRENDERER_H_

#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/media/SubtitleController.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos{
namespace Droid {
namespace Media {

class SRTRenderer
	: public Object
	, public ISubtitleControllerRenderer
{
public:
	SRTRenderer();

	virtual ~SRTRenderer();

	CAR_INTERFACE_DECL()

	CARAPI constructor(
		/* [in] */ IContext* context);

	CARAPI constructor(
		/* [in] */ IContext* context,
		/* [in] */ IHandler* eventHandler);

	CARAPI Supports(
		/* [in] */ IMediaFormat* format,
		/* [out] */ Boolean* result);

	CARAPI CreateTrack(
		/* [in] */ IMediaFormat* format,
		/* [out] */ ISubtitleTrack** result);

private:
	AutoPtr<IContext> mContext;
	Boolean mRender;
	AutoPtr<IHandler> mEventHandler;
	AutoPtr<IWebVttRenderingWidget> mRenderingWidget;
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_SRTRENDERER_H_