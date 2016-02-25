#ifndef __ELASTOS_DROID_MEDIA_TV_CTVCONTRACTPROGRAMSGENRES_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVCONTRACTPROGRAMSGENRES_H__

#include "_Elastos_Droid_Media_Tv_CTvContractProgramsGenres.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::IArraySet;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/** Canonical genres for TV programs. */
CarClass(CTvContractProgramsGenres)
    , public Singleton
    , public ITvContractProgramsGenres
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL()

    /**
     * Encodes canonical genre strings to a text that can be put into the database.
     *
     * @param genres Canonical genre strings. Use the strings defined in this class.
     * @return an encoded genre string that can be inserted into the
     *         {@link #COLUMN_CANONICAL_GENRE} column.
     */
    CARAPI Encode(
        /* [in] */ ArrayOf<String>* genres,
        /* [out] */ String* result);

    /**
     * Decodes the canonical genre strings from the text stored in the database.
     *
     * @param genres The encoded genre string retrieved from the
     *            {@link #COLUMN_CANONICAL_GENRE} column.
     * @return canonical genre strings.
     */
    CARAPI Decode(
        /* [in] */ const String& genres,
        /* [out, callee] */ ArrayOf<String>** result);

    /**
     * Check whether a given genre is canonical or not.
     *
     * @param genre The name of genre to be checked.
     * @return {@code true} if the genre is canonical, otherwise {@code false}.
     * @hide
     */
    CARAPI IsCanonical(
        /* [in] */ const String& genre,
        /* [out] */ Boolean* result);
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVCONTRACTPROGRAMSGENRES_H__
