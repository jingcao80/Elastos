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

#include "elastos/droid/media/tv/CTvStreamConfigBuilder.h"
#include "elastos/droid/media/tv/CTvStreamConfig.h"

using Elastos::Core::CInteger32;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_INTERFACE_IMPL(CTvStreamConfigBuilder, Object, ITvStreamConfigBuilder)

CAR_OBJECT_IMPL(CTvStreamConfigBuilder)

CTvStreamConfigBuilder::CTvStreamConfigBuilder()
{
}

CTvStreamConfigBuilder::~CTvStreamConfigBuilder()
{
}

ECode CTvStreamConfigBuilder::constructor()
{
    return NOERROR;
}

ECode CTvStreamConfigBuilder::StreamId(
    /* [in] */ Int32 streamId)
{
    return CInteger32::New(streamId, (IInteger32**)&mStreamId);
}

ECode CTvStreamConfigBuilder::Type(
    /* [in] */ Int32 type)
{
    return CInteger32::New(type, (IInteger32**)&mType);
}

ECode CTvStreamConfigBuilder::MaxWidth(
    /* [in] */ Int32 maxWidth)
{
    return CInteger32::New(maxWidth, (IInteger32**)&mMaxWidth);
}

ECode CTvStreamConfigBuilder::MaxHeight(
    /* [in] */ Int32 maxHeight)
{
    return CInteger32::New(maxHeight, (IInteger32**)&mMaxHeight);
}

ECode CTvStreamConfigBuilder::Generation(
    /* [in] */ Int32 generation)
{
    return CInteger32::New(generation, (IInteger32**)&mGeneration);
}

ECode CTvStreamConfigBuilder::Build(
    /* [out] */ ITvStreamConfig** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (mStreamId == NULL || mType == NULL || mMaxWidth == NULL
            || mMaxHeight == NULL || mGeneration == NULL) {
        // throw new UnsupportedOperationException();
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    AutoPtr<CTvStreamConfig> config;
    CTvStreamConfig::NewByFriend((CTvStreamConfig**)&config);
    mStreamId->GetValue(&config->mStreamId);
    mType->GetValue(&config->mType);
    mMaxWidth->GetValue(&config->mMaxWidth);
    mMaxHeight->GetValue(&config->mMaxHeight);
    mGeneration->GetValue(&config->mGeneration);
    *result = ITvStreamConfig::Probe(config);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
