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