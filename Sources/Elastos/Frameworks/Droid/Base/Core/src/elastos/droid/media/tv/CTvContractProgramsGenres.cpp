#include "elastos/droid/media/tv/CTvContractProgramsGenres.h"
#include "elastos/droid/media/tv/TvContractProgramsGenres.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_SINGLETON_IMPL(CTvContractProgramsGenres)
CAR_INTERFACE_IMPL(CTvContractProgramsGenres, Singleton, ITvContractProgramsGenres);

ECode CTvContractProgramsGenres::Encode(
    /* [in] */ ArrayOf<String>* genres,
    /* [out] */ String* result)
{
    return TvContractProgramsGenres::Encode(genres, result);
}

ECode CTvContractProgramsGenres::Decode(
    /* [in] */ const String& genres,
    /* [out, callee] */ ArrayOf<String>** result)
{
    return TvContractProgramsGenres::Decode(genres, result);
}

ECode CTvContractProgramsGenres::IsCanonical(
    /* [in] */ const String& genre,
    /* [out] */ Boolean* result)
{
    return TvContractProgramsGenres::IsCanonical(genre, result);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
