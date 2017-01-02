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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/media/CWebVttRenderer.h"
#include "elastos/droid/media/CWebVttRenderingWidget.h"
#include "elastos/droid/media/CWebVttTrack.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_OBJECT_IMPL(CWebVttRenderer);

CAR_INTERFACE_IMPL(CWebVttRenderer, Object, ISubtitleControllerRenderer)

CWebVttRenderer::CWebVttRenderer()
{}

CWebVttRenderer::~CWebVttRenderer()
{}

ECode CWebVttRenderer::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode CWebVttRenderer::Supports(
    /* [in] */ IMediaFormat* format,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean flag = FALSE;
    format->ContainsKey(IMediaFormat::KEY_MIME, &flag);
    if (flag) {
        String str;
        format->GetString(IMediaFormat::KEY_MIME, &str);
        *result = str.Equals("text/vtt");
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CWebVttRenderer::CreateTrack(
    /* [in] */ IMediaFormat* format,
    /* [out] */ ISubtitleTrack** result)
{
    VALIDATE_NOT_NULL(result);
    if (mRenderingWidget == NULL) {
        CWebVttRenderingWidget::New(mContext, (IWebVttRenderingWidget**)&mRenderingWidget);
    }

    AutoPtr<ISubtitleTrack> ist;
    CWebVttTrack::New(mRenderingWidget, format, (ISubtitleTrack**)&ist);
    *result = ist;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos