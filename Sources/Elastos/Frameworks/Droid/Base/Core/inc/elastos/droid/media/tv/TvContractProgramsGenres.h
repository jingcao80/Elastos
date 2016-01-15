#ifndef __ELASTOS_DROID_MEDIA_TV_TVCONTRACTPROGRAMSGENRES_H__
#define __ELASTOS_DROID_MEDIA_TV_TVCONTRACTPROGRAMSGENRES_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::IArraySet;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/** Canonical genres for TV programs. */
class TvContractProgramsGenres
{
public:
    /**
     * Encodes canonical genre strings to a text that can be put into the database.
     *
     * @param genres Canonical genre strings. Use the strings defined in this class.
     * @return an encoded genre string that can be inserted into the
     *         {@link #COLUMN_CANONICAL_GENRE} column.
     */
    static CARAPI Encode(
        /* [in] */ ArrayOf<String> * genres,
        /* [out] */ String * result);

    /**
     * Decodes the canonical genre strings from the text stored in the database.
     *
     * @param genres The encoded genre string retrieved from the
     *            {@link #COLUMN_CANONICAL_GENRE} column.
     * @return canonical genre strings.
     */
    static CARAPI Decode(
        /* [in] */ const String& genres,
        /* [out, callee] */ ArrayOf<String> ** result);

    /**
     * Check whether a given genre is canonical or not.
     *
     * @param genre The name of genre to be checked.
     * @return {@code true} if the genre is canonical, otherwise {@code false}.
     * @hide
     */
    static CARAPI IsCanonical(
        /* [in] */ const String& genre,
        /* [out] */ Boolean * result);

private:
    TvContractProgramsGenres();

private:
    static AutoPtr<IArraySet> CANONICAL_GENRES;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_TVCONTRACTPROGRAMSGENRES_H__
