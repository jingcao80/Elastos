#ifndef __ELASTOS_DROID_MEDIA_TV_TVINPUTINFOTVINPUTSETTINGS_H__
#define __ELASTOS_DROID_MEDIA_TV_TVINPUTINFOTVINPUTSETTINGS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * Utility class for putting and getting settings for TV input.
 *
 * @hide
 */
// @SystemApi
class TvInputInfoTvInputSettings
{
public:
    /**
     * Returns a set of TV input IDs which are marked as hidden by user in the settings.
     *
     * @param context The application context
     * @param userId The user ID for the stored hidden input set
     * @hide
     */
    // @SystemApi
    static CARAPI GetHiddenTvInputIds(
        /* [in] */ IContext * context,
        /* [in] */ Int32 userId,
        /* [out] */ ISet ** result);

    /**
     * Returns a map of TV input ID/custom label pairs set by the user in the settings.
     *
     * @param context The application context
     * @param userId The user ID for the stored hidden input map
     * @hide
     */
    // @SystemApi
    static CARAPI GetCustomLabels(
        /* [in] */ IContext * context,
        /* [in] */ Int32 userId,
        /* [out] */ IMap ** result);

    /**
     * Stores a set of TV input IDs which are marked as hidden by user. This is expected to
     * be called from the settings app.
     *
     * @param context The application context
     * @param hiddenInputIds A set including all the hidden TV input IDs
     * @param userId The user ID for the stored hidden input set
     * @hide
     */
    // @SystemApi
    static CARAPI PutHiddenTvInputs(
        /* [in] */ IContext * context,
        /* [in] */ ISet * hiddenInputIds,
        /* [in] */ Int32 userId);

    /**
     * Stores a map of TV input ID/custom label set by user. This is expected to be
     * called from the settings app.
     *
     * @param context The application context.
     * @param customLabels A map of TV input ID/custom label pairs
     * @param userId The user ID for the stored hidden input map
     * @hide
     */
    // @SystemApi
    static CARAPI PutCustomLabels(
        /* [in] */ IContext * context,
        /* [in] */ IMap * customLabels,
        /* [in] */ Int32 userId);

    static CARAPI_(Boolean) IsHidden(
        /* [in] */ IContext * context,
        /* [in] */ const String& inputId,
        /* [in] */ Int32 userId);

    static CARAPI_(String) GetCustomLabel(
        /* [in] */ IContext * context,
        /* [in] */ const String& inputId,
        /* [in] */ Int32 userId);

    static CARAPI EnsureValidField(
        /* [in] */ const String& value);

private:
    static String TV_INPUT_SEPARATOR;
    static String CUSTOM_NAME_SEPARATOR;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_TVINPUTINFOTVINPUTSETTINGS_H__
