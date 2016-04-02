#include "elastos/droid/text/TextDirectionHeuristics.h"
#include "elastos/droid/text/CTextDirectionHeuristics.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CTextDirectionHeuristics, Singleton, ITextDirectionHeuristics)

CAR_SINGLETON_IMPL(CTextDirectionHeuristics)

ECode CTextDirectionHeuristics::GetLTR(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::GetLTR();
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetRTL(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::GetRTL();
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetFIRSTSTRONG_LTR(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::GetFIRSTSTRONG_LTR();
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetFIRSTSTRONG_RTL(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::GetFIRSTSTRONG_RTL();
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetANYRTL_LTR(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::GetANYRTL_LTR();
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

ECode CTextDirectionHeuristics::GetLOCALE(
    /* [out] */ ITextDirectionHeuristic** tdh)
{
    VALIDATE_NOT_NULL(tdh)
    AutoPtr<ITextDirectionHeuristic> result = TextDirectionHeuristics::GetLOCALE();
    *tdh = result;
    REFCOUNT_ADD(*tdh)
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
