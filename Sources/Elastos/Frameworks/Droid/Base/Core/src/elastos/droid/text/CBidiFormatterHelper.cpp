#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/CBidiFormatterHelper.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CBidiFormatterHelper, Singleton, IBidiFormatterHelper)

CAR_SINGLETON_IMPL(CBidiFormatterHelper)


ECode CBidiFormatterHelper::GetInstance(
    /* [out] */ IBidiFormatter** instance)
{
    return NOERROR;
}

ECode CBidiFormatterHelper::GetInstance(
    /* [in] */ Boolean rtlContext,
    /* [out] */ IBidiFormatter** instance)
{
    return NOERROR;
}

ECode CBidiFormatterHelper::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ IBidiFormatter** instance)
{
    return NOERROR;
}


}//namespace Text
}//namespace Droid
}//namespace Elastos