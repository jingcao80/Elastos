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