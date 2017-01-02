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

#ifndef __ELASTOS_DROID_MEDIA_TV_CTvStreamConfigBUILDER_H__
#define __ELASTOS_DROID_MEDIA_TV_CTvStreamConfigBUILDER_H__

#include "_Elastos_Droid_Media_Tv_CTvStreamConfigBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * A helper class for creating a TvStreamConfig object.
 */
CarClass(CTvStreamConfigBuilder)
    , public Object
    , public ITvStreamConfigBuilder
{
public:
    CTvStreamConfigBuilder();

    virtual ~CTvStreamConfigBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI StreamId(
        /* [in] */ Int32 streamId);

    CARAPI Type(
        /* [in] */ Int32 type);

    CARAPI MaxWidth(
        /* [in] */ Int32 maxWidth);

    CARAPI MaxHeight(
        /* [in] */ Int32 maxHeight);

    CARAPI Generation(
        /* [in] */ Int32 generation);

    CARAPI Build(
        /* [out] */ ITvStreamConfig** result);

public:
    AutoPtr<IInteger32> mStreamId;
    AutoPtr<IInteger32> mType;
    AutoPtr<IInteger32> mMaxWidth;
    AutoPtr<IInteger32> mMaxHeight;
    AutoPtr<IInteger32> mGeneration;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTvStreamConfigBUILDER_H__
