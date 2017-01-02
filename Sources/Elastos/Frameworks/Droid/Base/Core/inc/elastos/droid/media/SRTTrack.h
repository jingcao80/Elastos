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

#ifndef _ELASTOS_DROID_MEDIA_SRTTrack_H__
#define _ELASTOS_DROID_MEDIA_SRTTrack_H__

#include "elastos/droid/media/WebVttTrack.h"

namespace Elastos{
namespace Droid {
namespace Media {

class SRTTrack
	: public WebVttTrack
    , public ISRTTrack
{
public:
	SRTTrack();

	virtual ~SRTTrack();

    CAR_INTERFACE_DECL()

	CARAPI constructor(
		/* [in] */ IWebVttRenderingWidget* renderingWidget,
		/* [in] */ IMediaFormat* format);

	CARAPI constructor(
		/* [in] */ IHandler* eventHandler,
		/* [in] */ IMediaFormat* format);

	CARAPI OnData(
		/* [in] */ ArrayOf<Byte>* data,
		/* [in] */ Boolean eos,
		/* [in] */ Int64 runID);

	CARAPI UpdateView(
		/* [in] */ IVector* activeCues);

protected:
	CARAPI OnData(
		/* [in] */ ISubtitleData* data);

private:
	static CARAPI_(Int64) ParseMs(
		/* [in] */ const String& in);

private:
  	static const Int32 MEDIA_TIMED_TEXT;
    static const Int32 KEY_STRUCT_TEXT;
    static const Int32 KEY_START_TIME;
    static const Int32 KEY_LOCAL_SETTING;

    static const String TAG;
    AutoPtr<IHandler> mEventHandler;
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_SRTTrack_H__