#include "elastos/droid/server/firewall/CategoryFilter.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

FilterFactory::FilterFactory()
{}

ECode FilterFactory::constructor(
    /* in */ const String& tag)
{
    if (tag.IsNull()) {
        //throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    mTag = tag;

    return NOERROR;
}

String FilterFactory::GetTagName()
{
    return mTag;
}

} // Firewall
} // Server
} // Droid
} // Elastos
