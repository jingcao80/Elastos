#include "elastos/droid/media/SRTRenderer.h"
#include "elastos/droid/media/CSRTTrack.h"
#include "elastos/droid/media/CWebVttRenderingWidget.h"

namespace Elastos{
namespace Droid {
namespace Media {

SRTRenderer::SRTRenderer()
{}

SRTRenderer::~SRTRenderer()
{}

CAR_INTERFACE_IMPL(SRTRenderer, Object, ISubtitleControllerRenderer)

ECode SRTRenderer::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode SRTRenderer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* eventHandler)
{
     mContext = context;
     mRender = (eventHandler == NULL);
     mEventHandler = eventHandler;
     return NOERROR;
}

ECode SRTRenderer::Supports(
    /* [in] */ IMediaFormat* format,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean flag = FALSE;
    format->ContainsKey(IMediaFormat::KEY_MIME, &flag);

    if (flag) {
        String str;
        format->GetString(IMediaFormat::KEY_MIME, &str);
        if (!str.Equals(IMediaPlayer::MEDIA_MIMETYPE_TEXT_SUBRIP)) {
            *result = FALSE;
             return NOERROR;
        }

        Int32 ret;
        format->GetInt32(IMediaFormat::KEY_IS_TIMED_TEXT, 0, &ret);

        *result = mRender == (format->GetInt32(IMediaFormat::KEY_IS_TIMED_TEXT, 0, &ret), ret == 0);
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode SRTRenderer::CreateTrack(
    /* [in] */ IMediaFormat* format,
    /* [out] */ ISubtitleTrack** result)
{
    VALIDATE_NOT_NULL(result);
    if (mRender && mRenderingWidget == NULL) {
        CWebVttRenderingWidget::New(mContext, (IWebVttRenderingWidget**)&mRenderingWidget);
    }

    AutoPtr<ISRTTrack> stk;
    if (mRender) {
        CSRTTrack::New(mRenderingWidget, format, (ISRTTrack**)&stk);
    } else {
        CSRTTrack::New(mEventHandler, format, (ISRTTrack**)&stk);
    }

    *result = ISubtitleTrack::Probe(stk);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Elastos
} // namespace Droid
} // namespace Media