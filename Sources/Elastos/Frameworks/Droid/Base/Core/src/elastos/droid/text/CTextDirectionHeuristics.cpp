#include "elastos/droid/text/CTextDirectionHeuristics.h"
#include "elastos/droid/text/TextDirectionHeuristics.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CTextDirectionHeuristics, Singleton, ITextDirectionHeuristics)

CAR_SINGLETON_IMPL(CTextDirectionHeuristics)

ECode CTextDirectionHeuristics::GetLTR(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::LTR;
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetRTL(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::RTL;
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetFIRSTSTRONG_LTR(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::FIRSTSTRONG_LTR;
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetFIRSTSTRONG_RTL(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::FIRSTSTRONG_RTL;
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetANYRTL_LTR(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::ANYRTL_LTR;
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetLOCALE(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::LOCALE;
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
