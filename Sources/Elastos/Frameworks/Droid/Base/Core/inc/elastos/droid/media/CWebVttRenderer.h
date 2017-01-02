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

#ifndef __ELASTOS_DROID_MEDIA_CWEBVTTRENDERER_H__
#define __ELASTOS_DROID_MEDIA_CWEBVTTRENDERER_H__

#include "_Elastos_Droid_Media_CWebVttRenderer.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CWebVttRenderer)
    , public Object
    , public ISubtitleControllerRenderer
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CWebVttRenderer();

    virtual ~CWebVttRenderer();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI Supports(
        /* [in] */ IMediaFormat* format,
        /* [out] */ Boolean* result);

    CARAPI CreateTrack(
        /* [in] */ IMediaFormat* format,
        /* [out] */ ISubtitleTrack** result);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IWebVttRenderingWidget> mRenderingWidget;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CWEBVTTRENDERER_H__