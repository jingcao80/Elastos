
#include "CQNameHelper.h"
#include "CQName.h"

namespace Elastosx {
namespace Xml {
namespace Namespace {

CAR_INTERFACE_IMPL(CQNameHelper, Singleton, IQNameHelper)

CAR_SINGLETON_IMPL(CQNameHelper)


ECode CQNameHelper::ValueOf(
    /* [in] */ const String& qNameAsString,
    /* [out] */ Elastosx::Xml::Namespace::IQName ** ppName)
{
    // TODO: Add your code here
    return CQName::ValueOf(qNameAsString, ppName);
}

}
}
}

