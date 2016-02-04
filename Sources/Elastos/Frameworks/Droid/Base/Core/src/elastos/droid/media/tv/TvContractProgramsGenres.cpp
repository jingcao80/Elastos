
#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/tv/TvContractProgramsGenres.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/utility/CArraySet.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

static AutoPtr<IArraySet> Init()
{
    AutoPtr<ISet> set;
    CArraySet::New((ISet**)&set);

    AutoPtr<ICharSequence> cs;
    CString::New(ITvContractProgramsGenres::FAMILY_KIDS, (ICharSequence**)&cs);
    set->Add(cs);
    cs = NULL;
    CString::New(ITvContractProgramsGenres::SPORTS, (ICharSequence**)&cs);
    set->Add(cs);
    cs = NULL;
    CString::New(ITvContractProgramsGenres::SHOPPING, (ICharSequence**)&cs);
    set->Add(cs);
    cs = NULL;
    CString::New(ITvContractProgramsGenres::MOVIES, (ICharSequence**)&cs);
    set->Add(cs);
    cs = NULL;
    CString::New(ITvContractProgramsGenres::COMEDY, (ICharSequence**)&cs);
    set->Add(cs);
    cs = NULL;
    CString::New(ITvContractProgramsGenres::TRAVEL, (ICharSequence**)&cs);
    set->Add(cs);
    cs = NULL;
    CString::New(ITvContractProgramsGenres::DRAMA, (ICharSequence**)&cs);
    set->Add(cs);
    cs = NULL;
    CString::New(ITvContractProgramsGenres::EDUCATION, (ICharSequence**)&cs);
    set->Add(cs);
    cs = NULL;
    CString::New(ITvContractProgramsGenres::ANIMAL_WILDLIFE, (ICharSequence**)&cs);
    set->Add(cs);
    cs = NULL;
    CString::New(ITvContractProgramsGenres::NEWS, (ICharSequence**)&cs);
    set->Add(cs);
    cs = NULL;
    CString::New(ITvContractProgramsGenres::GAMING, (ICharSequence**)&cs);
    set->Add(cs);

    return IArraySet::Probe(set);
}

INIT_PROI_4 AutoPtr<IArraySet> TvContractProgramsGenres::CANONICAL_GENRES = Init();

TvContractProgramsGenres::TvContractProgramsGenres()
{
}

ECode TvContractProgramsGenres::Encode(
    /* [in] */ ArrayOf<String>* genres,
    /* [out] */ String* result)
{
    StringBuilder sb;
    String separator("");
    for (Int32 i = 0; i < genres->GetLength(); i++) {
        String genre = (*genres)[i];
        sb.Append(separator);
        sb.Append(genre);
        separator = ",";
    }
    return sb.ToString(result);
}

ECode TvContractProgramsGenres::Decode(
    /* [in] */ const String& genres,
    /* [out, callee] */ ArrayOf<String> ** result)
{
    VALIDATE_NOT_NULL(result)
    return StringUtils::Split(genres, String("\\s*,\\s*"), result);
}

ECode TvContractProgramsGenres::IsCanonical(
    /* [in] */ const String& genre,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> cs;
    CString::New(genre, (ICharSequence**)&cs);
    return ISet::Probe(CANONICAL_GENRES)->Contains(cs, result);
}


} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
