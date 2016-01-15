
#include "CCodingErrorActionHelper.h"
#include "CCodingErrorAction.h"

namespace Elastos {
namespace IO {
namespace Charset {

CAR_INTERFACE_IMPL(CCodingErrorActionHelper, Singleton, ICodingErrorActionHelper)

CAR_SINGLETON_IMPL(CCodingErrorActionHelper)

ECode CCodingErrorActionHelper::GetIGNORE(
    /* [out] */ ICodingErrorAction** ignore)
{
    return CCodingErrorAction::GetIGNORE(ignore);
}

ECode CCodingErrorActionHelper::GetREPLACE(
    /* [out] */ ICodingErrorAction** replace)
{
    return CCodingErrorAction::GetREPLACE(replace);
}

ECode CCodingErrorActionHelper::GetREPORT(
    /* [out] */ ICodingErrorAction** report)
{
    return CCodingErrorAction::GetREPORT(report);
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
