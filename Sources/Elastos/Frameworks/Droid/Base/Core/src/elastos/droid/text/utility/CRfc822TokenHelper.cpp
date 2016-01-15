
#include "elastos/droid/text/utility/CRfc822TokenHelper.h"
#include "elastos/droid/text/utility/Rfc822Token.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

CAR_INTERFACE_IMPL(CRfc822TokenHelper, Singleton, IRfc822TokenHelper)

CAR_SINGLETON_IMPL(CRfc822TokenHelper)

ECode CRfc822TokenHelper::QuoteNameIfNecessary(
    /* [in] */ const String& name,
    /* [out] */ String* ret)
{
    return Rfc822Token::QuoteNameIfNecessary(name, ret);
}

ECode CRfc822TokenHelper::QuoteName(
    /* [in] */ const String& name,
    /* [out] */ String* ret)
{
    return Rfc822Token::QuoteName(name, ret);
}

ECode CRfc822TokenHelper::QuoteComment(
    /* [in] */ const String& comment,
    /* [out] */ String* ret)
{
    return Rfc822Token::QuoteComment(comment, ret);
}

} //namespace Utility
} //namespace Text
} //namespace Droid
} //namespace Elastos

