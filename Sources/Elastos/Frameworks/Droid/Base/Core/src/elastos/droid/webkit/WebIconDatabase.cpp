
#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/webkit/WebIconDatabase.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

AutoPtr<IWebIconDatabase> WebIconDatabase::GetInstance()
{
    // XXX: Must be created in the UI thread.
    AutoPtr<IWebIconDatabase> temp;
    WebViewFactory::GetProvider()->GetWebIconDatabase((IWebIconDatabase**)&temp);
    return temp;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
