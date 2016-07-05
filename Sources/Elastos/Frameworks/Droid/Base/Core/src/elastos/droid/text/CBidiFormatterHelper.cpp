#include "elastos/droid/text/CBidiFormatterHelper.h"
#include "elastos/droid/text/CBidiFormatterBuilder.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CBidiFormatterHelper, Singleton, IBidiFormatterHelper)

CAR_SINGLETON_IMPL(CBidiFormatterHelper)

ECode CBidiFormatterHelper::GetInstance(
    /* [out] */ IBidiFormatter** instance)
{
    VALIDATE_NOT_NULL(instance)

    AutoPtr<IBidiFormatterBuilder> builder;
    CBidiFormatterBuilder::New((IBidiFormatterBuilder**)&builder);
    return builder->Build(instance);
}

ECode CBidiFormatterHelper::GetInstance(
    /* [in] */ Boolean rtlContext,
    /* [out] */ IBidiFormatter** instance)
{
    VALIDATE_NOT_NULL(instance)

    AutoPtr<IBidiFormatterBuilder> builder;
    CBidiFormatterBuilder::New(rtlContext, (IBidiFormatterBuilder**)&builder);
    return builder->Build(instance);
}

ECode CBidiFormatterHelper::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ IBidiFormatter** instance)
{
    VALIDATE_NOT_NULL(instance)

    AutoPtr<IBidiFormatterBuilder> builder;
    CBidiFormatterBuilder::New(locale, (IBidiFormatterBuilder**)&builder);
    return builder->Build(instance);
}

}//namespace Text
}//namespace Droid
}//namespace Elastos