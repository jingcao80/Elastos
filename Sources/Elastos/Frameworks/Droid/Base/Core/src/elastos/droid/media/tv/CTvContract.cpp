#include "elastos/droid/media/tv/CTvContract.h"
#include "elastos/droid/media/tv/TvContract.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_SINGLETON_IMPL(CTvContract)
CAR_INTERFACE_IMPL(CTvContract, Singleton, ITvContract);

ECode CTvContract::BuildInputId(
    /* [in] */ IComponentName* name,
    /* [out] */ String* result)
{
    return TvContract::BuildInputId(name, result);
}

ECode CTvContract::BuildChannelUri(
    /* [in] */ Int64 channelId,
    /* [out] */ IUri** result)
{
    return TvContract::BuildChannelUri(channelId, result);
}

ECode CTvContract::BuildChannelUriForPassthroughInput(
    /* [in] */ const String& inputId,
    /* [out] */ IUri** result)
{
    return TvContract::BuildChannelUriForPassthroughInput(inputId, result);
}

ECode CTvContract::BuildChannelLogoUri(
    /* [in] */ Int64 channelId,
    /* [out] */ IUri** result)
{
    return TvContract::BuildChannelLogoUri(channelId, result);
}

ECode CTvContract::BuildChannelLogoUri(
    /* [in] */ IUri* channelUri,
    /* [out] */ IUri** result)
{
    return TvContract::BuildChannelLogoUri(channelUri, result);
}

ECode CTvContract::BuildChannelsUriForInput(
    /* [in] */ const String& inputId,
    /* [out] */ IUri** result)
{
    return TvContract::BuildChannelsUriForInput(inputId, result);
}

ECode CTvContract::BuildChannelsUriForInput(
    /* [in] */ const String& inputId,
    /* [in] */ Boolean browsableOnly,
    /* [out] */ IUri** result)
{
    return TvContract::BuildChannelsUriForInput(inputId, browsableOnly, result);
}

ECode CTvContract::BuildChannelsUriForInput(
    /* [in] */ const String& inputId,
    /* [in] */ const String& genre,
    /* [in] */ Boolean browsableOnly,
    /* [out] */ IUri** result)
{
    return TvContract::BuildChannelsUriForInput(inputId, genre, browsableOnly, result);
}

ECode CTvContract::BuildProgramUri(
    /* [in] */ Int64 programId,
    /* [out] */ IUri** result)
{
    return TvContract::BuildProgramUri(programId, result);
}

ECode CTvContract::BuildProgramsUriForChannel(
    /* [in] */ Int64 channelId,
    /* [out] */ IUri** result)
{
    return TvContract::BuildProgramsUriForChannel(channelId, result);
}

ECode CTvContract::BuildProgramsUriForChannel(
    /* [in] */ IUri* channelUri,
    /* [out] */ IUri** result)
{
    return TvContract::BuildProgramsUriForChannel(channelUri, result);
}

ECode CTvContract::BuildProgramsUriForChannel(
    /* [in] */ Int64 channelId,
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 endTime,
    /* [out] */ IUri** result)
{
    return TvContract::BuildProgramsUriForChannel(channelId, startTime, endTime, result);
}

ECode CTvContract::BuildProgramsUriForChannel(
    /* [in] */ IUri* channelUri,
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 endTime,
    /* [out] */ IUri** result)
{
    return TvContract::BuildProgramsUriForChannel(channelUri, startTime, endTime, result);
}

ECode CTvContract::BuildWatchedProgramUri(
    /* [in] */ Int64 watchedProgramId,
    /* [out] */ IUri** result)
{
    return TvContract::BuildWatchedProgramUri(watchedProgramId, result);
}

ECode CTvContract::IsChannelUri(
    /* [in] */ IUri* uri,
    /* [out] */ Boolean* result)
{
    return TvContract::IsChannelUri(uri, result);
}

ECode CTvContract::IsChannelUriForTunerInput(
    /* [in] */ IUri* uri,
    /* [out] */ Boolean* result)
{
    return TvContract::IsChannelUriForTunerInput(uri, result);
}

ECode CTvContract::IsChannelUriForPassthroughInput(
    /* [in] */ IUri* uri,
    /* [out] */ Boolean* result)
{
    return TvContract::IsChannelUriForPassthroughInput(uri, result);
}

ECode CTvContract::IsProgramUri(
    /* [in] */ IUri* uri,
    /* [out] */ Boolean* result)
{
    return TvContract::IsProgramUri(uri, result);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
