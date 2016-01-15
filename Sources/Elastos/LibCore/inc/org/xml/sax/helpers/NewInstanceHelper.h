
#ifndef __ORG_XML_SAX_HELPERS_NEWINSTANCEHELPER_H__
#define __ORG_XML_SAX_HELPERS_NEWINSTANCEHELPER_H__

#include "Object.h"

using Elastos::Core::IClassLoader;
using namespace Elastos;

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

class NewInstanceHelper {

public:
    /**
     * Creates a new instance of the specified class name
     *
     * Package private so this code is not exposed at the API level.
     */
    static AutoPtr<IInterface> NewInstance (
        /* [in] */ IClassLoader* classLoader,
        /* [in] */ const String& className);

    /**
     * Figure out which ClassLoader to use.  For JDK 1.2 and later use
     * the context ClassLoader.
     */
    static AutoPtr<IClassLoader> GetClassLoader ();

private:
    NewInstanceHelper();
    NewInstanceHelper(const NewInstanceHelper&);
};

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_HELPERS_NEWINSTANCEHELPER_H__
