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

#include "Elastos.Droid.Media.h"
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/media/tv/TvContract.h"
#include "elastos/droid/media/tv/TvContractChannels.h"
#include "elastos/droid/media/tv/TvContractPrograms.h"
#include "elastos/droid/media/tv/TvContractProgramsGenres.h"
#include "elastos/droid/media/tv/TvContractWatchedPrograms.h"
#include "elastos/droid/net/CUriBuilder.h"
#include "elastos/droid/net/CUriHelper.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Net::CUriBuilder;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

String TvContract::PATH_CHANNEL("channel");
String TvContract::PATH_PROGRAM("program");
String TvContract::PATH_PASSTHROUGH("passthrough");

ECode TvContract::BuildInputId(
    /* [in] */ IComponentName* name,
    /* [out] */ String* result)
{
    return name->FlattenToShortString(result);
}

ECode TvContract::BuildChannelUri(
    /* [in] */ Int64 channelId,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContentUris> uris;
    CContentUris::AcquireSingleton((IContentUris**)&uris);
    AutoPtr<IUri> uri;
    TvContractChannels::GetCONTENT_URI((IUri**)&uri);
    return uris->WithAppendedId(uri, channelId, result);
}

ECode TvContract::BuildChannelUriForPassthroughInput(
    /* [in] */ const String& inputId,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IUriBuilder> uriBuilder;
    CUriBuilder::New((IUriBuilder**)&uriBuilder);
    uriBuilder->Scheme(IContentResolver::SCHEME_CONTENT);
    uriBuilder->Authority(ITvContract::AUTHORITY);
    uriBuilder->AppendPath(PATH_PASSTHROUGH);
    uriBuilder->AppendPath(inputId);
    return uriBuilder->Build(result);
}

ECode TvContract::BuildChannelLogoUri(
    /* [in] */ Int64 channelId,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IUri> uri;
    BuildChannelUri(channelId, (IUri**)&uri);
    return BuildChannelLogoUri(uri, result);
}

ECode TvContract::BuildChannelLogoUri(
    /* [in] */ IUri* channelUri,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    Boolean b;
    IsChannelUriForTunerInput(channelUri, &b);
    if (!b) {
        // throw new IllegalArgumentException("Not a channel: " + channelUri);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    return helper->WithAppendedPath(channelUri, ITvContractChannelsLogo::CONTENT_DIRECTORY, result);
}

ECode TvContract::BuildChannelsUriForInput(
    /* [in] */ const String& inputId,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    return BuildChannelsUriForInput(inputId, FALSE, result);
}

ECode TvContract::BuildChannelsUriForInput(
    /* [in] */ const String& inputId,
    /* [in] */ Boolean browsableOnly,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IUri> uri;
    TvContractChannels::GetCONTENT_URI((IUri**)&uri);
    AutoPtr<IUriBuilder> builder;
    uri->BuildUpon((IUriBuilder**)&builder);
    if (!inputId.IsNull()) {
        builder->AppendQueryParameter(ITvContract::PARAM_INPUT, inputId);
    }
    builder->AppendQueryParameter(ITvContract::PARAM_BROWSABLE_ONLY, StringUtils::ToString(browsableOnly));
    return builder->Build(result);
}

ECode TvContract::BuildChannelsUriForInput(
    /* [in] */ const String& inputId,
    /* [in] */ const String& genre,
    /* [in] */ Boolean browsableOnly,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (genre.IsNull()) {
        return BuildChannelsUriForInput(inputId, browsableOnly, result);
    }
    Boolean b;
    TvContractProgramsGenres::IsCanonical(genre, &b);
    if (!b) {
        // throw new IllegalArgumentException("Not a canonical genre: '" + genre + "'");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IUri> uri;
    BuildChannelsUriForInput(inputId, browsableOnly, (IUri**)&uri);
    AutoPtr<IUriBuilder> uriBuilder;
    uri->BuildUpon((IUriBuilder**)&uriBuilder);
    uriBuilder->AppendQueryParameter(ITvContract::PARAM_CANONICAL_GENRE, genre);
    return uriBuilder->Build(result);
}

ECode TvContract::BuildProgramUri(
    /* [in] */ Int64 programId,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContentUris> uris;
    CContentUris::AcquireSingleton((IContentUris**)&uris);
    AutoPtr<IUri> uri;
    TvContractPrograms::GetCONTENT_URI((IUri**)&uri);
    return uris->WithAppendedId(uri, programId, result);
}

ECode TvContract::BuildProgramsUriForChannel(
    /* [in] */ Int64 channelId,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoPtr<IUri> uri;
    TvContractPrograms::GetCONTENT_URI((IUri**)&uri);
    AutoPtr<IUriBuilder> uriBuilder;
    uri->BuildUpon((IUriBuilder**)&uriBuilder);
    uriBuilder->AppendQueryParameter(ITvContract::PARAM_CHANNEL, StringUtils::ToString(channelId));
    return uriBuilder->Build(result);
}

ECode TvContract::BuildProgramsUriForChannel(
    /* [in] */ IUri* channelUri,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    Boolean b;
    IsChannelUriForTunerInput(channelUri, &b);
    if(!b) {
        // throw new IllegalArgumentException("Not a channel: " + channelUri);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IContentUris> uris;
    CContentUris::AcquireSingleton((IContentUris**)&uris);
    Int64 id;
    uris->ParseId(channelUri, &id);
    return BuildProgramsUriForChannel(id, result);
}

ECode TvContract::BuildProgramsUriForChannel(
    /* [in] */ Int64 channelId,
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 endTime,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoPtr<IUri> uri;
    BuildProgramsUriForChannel(channelId, (IUri**)&uri);
    AutoPtr<IUriBuilder> uriBuilder;
    uri->BuildUpon((IUriBuilder**)&uriBuilder);
    uriBuilder->AppendQueryParameter(ITvContract::PARAM_START_TIME, StringUtils::ToString(startTime));
    uriBuilder->AppendQueryParameter(ITvContract::PARAM_END_TIME, StringUtils::ToString(startTime));
    return uriBuilder->Build(result);
}

ECode TvContract::BuildProgramsUriForChannel(
    /* [in] */ IUri* channelUri,
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 endTime,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b;
    IsChannelUriForTunerInput(channelUri, &b);
    if(!b) {
        // throw new IllegalArgumentException("Not a channel: " + channelUri);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IContentUris> uris;
    CContentUris::AcquireSingleton((IContentUris**)&uris);
    Int64 id;
    uris->ParseId(channelUri, &id);
    return BuildProgramsUriForChannel(id, startTime, endTime, result);
}

ECode TvContract::BuildWatchedProgramUri(
    /* [in] */ Int64 watchedProgramId,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContentUris> uris;
    CContentUris::AcquireSingleton((IContentUris**)&uris);
    AutoPtr<IUri> uri;
    TvContractWatchedPrograms::GetCONTENT_URI((IUri**)&uri);
    return uris->WithAppendedId(uri, watchedProgramId, result);
}

ECode TvContract::IsChannelUri(
    /* [in] */ IUri* uri,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b1, b2;
    IsChannelUriForTunerInput(uri, &b1);
    IsChannelUriForPassthroughInput(uri, &b2);
    *result = b1 || b2;
    return NOERROR;
}

ECode TvContract::IsChannelUriForTunerInput(
    /* [in] */ IUri* uri,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsTvUri(uri) && IsTwoSegmentUriStartingWith(uri, PATH_CHANNEL);
    return NOERROR;
}

ECode TvContract::IsChannelUriForPassthroughInput(
    /* [in] */ IUri* uri,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsTvUri(uri) && IsTwoSegmentUriStartingWith(uri, PATH_PASSTHROUGH);
    return NOERROR;
}

ECode TvContract::IsProgramUri(
    /* [in] */ IUri* uri,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsTvUri(uri) && IsTwoSegmentUriStartingWith(uri, PATH_PROGRAM);
    return NOERROR;
}

Boolean TvContract::IsTvUri(
    /* [in] */ IUri* uri)
{
    if (uri == NULL) {
        return FALSE;
    }
    String scheme;
    uri->GetScheme(&scheme);
    String auth;
    uri->GetAuthority(&auth);
    return IContentResolver::SCHEME_CONTENT.Equals(scheme)
            && ITvContract::AUTHORITY.Equals(auth);
}

Boolean TvContract::IsTwoSegmentUriStartingWith(
    /* [in] */ IUri* uri,
    /* [in] */ const String& pathSegment)
{
    AutoPtr<IList> pathSegments;
    uri->GetPathSegments((IList**)&pathSegments);
    AutoPtr<IInterface> obj;
    pathSegments->Get(0, (IInterface**)&obj);
    AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj);
    String str;
    cs->ToString(&str);
    Int32 size;
    pathSegments->GetSize(&size);
    return size == 2 && pathSegment.Equals(str);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
