#ifndef __ELASTOS_DROID_MEDIA_TV_TvContract_H__
#define __ELASTOS_DROID_MEDIA_TV_TvContract_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * <p>
 * The contract between the TV provider and applications. Contains definitions for the supported
 * URIs and columns.
 * </p>
 * <h3>Overview</h3>
 * <p>
 * TvContract defines a basic database of TV content metadata such as channel and program
 * information. The information is stored in {@link Channels} and {@link Programs} tables.
 * </p>
 * <ul>
 *     <li>A row in the {@link Channels} table represents information about a TV channel. The data
 *         format can vary greatly from standard to standard or according to service provider, thus
 *         the columns here are mostly comprised of basic entities that are usually seen to users
 *         regardless of standard such as channel number and name.</li>
 *     <li>A row in the {@link Programs} table represents a set of data describing a TV program such
 *         as program title and start time.</li>
 * </ul>
 */
class TvContract
{
public:
    friend class TvContractChannels;
    friend class TvContractPrograms;

    /**
     * Builds an ID that uniquely identifies a TV input service.
     *
     * @param name The {@link ComponentName} of the TV input service to build ID for.
     * @return the ID for the given TV input service.
     */
    CARAPI BuildInputId(
        /* [in] */ IComponentName * name,
        /* [out] */ String * result);

    /**
     * Builds a URI that points to a specific channel.
     *
     * @param channelId The ID of the channel to point to.
     */
    CARAPI BuildChannelUri(
        /* [in] */ Int64 channelId,
        /* [out] */ IUri ** result);

    /**
     * Build a special channel URI intended to be used with pass-through inputs. (e.g. HDMI)
     *
     * @param inputId The ID of the pass-through input to build a channels URI for.
     * @see TvInputInfo#isPassthroughInput()
     */
    CARAPI BuildChannelUriForPassthroughInput(
        /* [in] */ const String& inputId,
        /* [out] */ IUri ** result);

    /**
     * Builds a URI that points to a channel logo. See {@link Channels.Logo}.
     *
     * @param channelId The ID of the channel whose logo is pointed to.
     */
    CARAPI BuildChannelLogoUri(
        /* [in] */ Int64 channelId,
        /* [out] */ IUri ** result);

    /**
     * Builds a URI that points to a channel logo. See {@link Channels.Logo}.
     *
     * @param channelUri The URI of the channel whose logo is pointed to.
     */
    CARAPI BuildChannelLogoUri(
        /* [in] */ IUri * channelUri,
        /* [out] */ IUri ** result);

    /**
     * Builds a URI that points to all channels from a given TV input.
     *
     * @param inputId The ID of the TV input to build a channels URI for. If {@code null}, builds a
     *            URI for all the TV inputs.
     */
    CARAPI BuildChannelsUriForInput(
        /* [in] */ const String& inputId,
        /* [out] */ IUri ** result);

    /**
     * Builds a URI that points to all or browsable-only channels from a given TV input.
     *
     * @param inputId The ID of the TV input to build a channels URI for. If {@code null}, builds a
     *            URI for all the TV inputs.
     * @param browsableOnly If set to {@code true} the URI points to only browsable channels. If set
     *            to {@code false} the URI points to all channels regardless of whether they are
     *            browsable or not.
     * @hide
     */
    // @SystemApi
    CARAPI BuildChannelsUriForInput(
        /* [in] */ const String& inputId,
        /* [in] */ Boolean browsableOnly,
        /* [out] */ IUri ** result);

    /**
     * Builds a URI that points to all or browsable-only channels which have programs with the given
     * genre from the given TV input.
     *
     * @param inputId The ID of the TV input to build a channels URI for. If {@code null}, builds a
     *            URI for all the TV inputs.
     * @param genre {@link Programs.Genres} to search. If {@code null}, builds a URI for all genres.
     * @param browsableOnly If set to {@code true} the URI points to only browsable channels. If set
     *            to {@code false} the URI points to all channels regardless of whether they are
     *            browsable or not.
     * @hide
     */
    // @SystemApi
    CARAPI BuildChannelsUriForInput(
        /* [in] */ const String& inputId,
        /* [in] */ const String& genre,
        /* [in] */ Boolean browsableOnly,
        /* [out] */ IUri ** result);

    /**
     * Builds a URI that points to a specific program.
     *
     * @param programId The ID of the program to point to.
     */
    CARAPI BuildProgramUri(
        /* [in] */ Int64 programId,
        /* [out] */ IUri ** result);

    /**
     * Builds a URI that points to all programs on a given channel.
     *
     * @param channelId The ID of the channel to return programs for.
     */
    CARAPI BuildProgramsUriForChannel(
        /* [in] */ Int64 channelId,
        /* [out] */ IUri ** result);

    /**
     * Builds a URI that points to all programs on a given channel.
     *
     * @param channelUri The URI of the channel to return programs for.
     */
    CARAPI BuildProgramsUriForChannel(
        /* [in] */ IUri * channelUri,
        /* [out] */ IUri ** result);

    /**
     * Builds a URI that points to programs on a specific channel whose schedules overlap with the
     * given time frame.
     *
     * @param channelId The ID of the channel to return programs for.
     * @param startTime The start time used to filter programs. The returned programs should have
     *            {@link Programs#COLUMN_END_TIME_UTC_MILLIS} that is greater than this time.
     * @param endTime The end time used to filter programs. The returned programs should have
     *            {@link Programs#COLUMN_START_TIME_UTC_MILLIS} that is less than this time.
     */
    CARAPI BuildProgramsUriForChannel(
        /* [in] */ Int64 channelId,
        /* [in] */ Int64 startTime,
        /* [in] */ Int64 endTime,
        /* [out] */ IUri ** result);

    /**
     * Builds a URI that points to programs on a specific channel whose schedules overlap with the
     * given time frame.
     *
     * @param channelUri The URI of the channel to return programs for.
     * @param startTime The start time used to filter programs. The returned programs should have
     *            {@link Programs#COLUMN_END_TIME_UTC_MILLIS} that is greater than this time.
     * @param endTime The end time used to filter programs. The returned programs should have
     *            {@link Programs#COLUMN_START_TIME_UTC_MILLIS} that is less than this time.
     */
    CARAPI BuildProgramsUriForChannel(
        /* [in] */ IUri * channelUri,
        /* [in] */ Int64 startTime,
        /* [in] */ Int64 endTime,
        /* [out] */ IUri ** result);

    /**
     * Builds a URI that points to a specific program the user watched.
     *
     * @param watchedProgramId The ID of the watched program to point to.
     * @hide
     */
    CARAPI BuildWatchedProgramUri(
        /* [in] */ Int64 watchedProgramId,
        /* [out] */ IUri ** result);

    /**
     * Returns true, if {@code uri} is a channel URI.
     * @hide
     */
    CARAPI IsChannelUri(
        /* [in] */ IUri * uri,
        /* [out] */ Boolean * result);

    /**
     * Returns true, if {@code uri} is a channel URI for a tuner input.
     * @hide
     */
    CARAPI IsChannelUriForTunerInput(
        /* [in] */ IUri * uri,
        /* [out] */ Boolean * result);

    /**
     * Returns true, if {@code uri} is a channel URI for a passthrough input.
     * @hide
     */
    // @SystemApi
    CARAPI IsChannelUriForPassthroughInput(
        /* [in] */ IUri * uri,
        /* [out] */ Boolean * result);

    /**
     * Returns true, if {@code uri} is a program URI.
     * @hide
     */
    CARAPI IsProgramUri(
        /* [in] */ IUri * uri,
        /* [out] */ Boolean * result);

private:
    static CARAPI_(Boolean) IsTvUri(
        /* [in] */ IUri * uri);

    static CARAPI_(Boolean) IsTwoSegmentUriStartingWith(
        /* [in] */ IUri * uri,
        /* [in] */ const String& pathSegment);

    TvContract();

private:
    static String PATH_CHANNEL;
    static String PATH_PROGRAM;
    static String PATH_PASSTHROUGH;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_TvContract_H__
