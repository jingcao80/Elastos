
#include "CJSONTokenerHelper.h"
#include "JSONTokener.h"

namespace Org {
namespace Json {

CAR_SINGLETON_IMPL(CJSONTokenerHelper);

CAR_INTERFACE_IMPL(CJSONTokenerHelper, Singleton, IJSONTokenerHelper)

ECode CJSONTokenerHelper::Dehexchar(
    /* [in] */ Char32 hex,
    /* [out] */ Int32* data)
{
    VALIDATE_NOT_NULL(data);
    *data = JSONTokener::Dehexchar(hex);
    return NOERROR;
}

} //namespace Json
} //namespace Org
