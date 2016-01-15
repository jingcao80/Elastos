
#include "NewInstanceHelper.h"

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

AutoPtr<IInterface> NewInstanceHelper::NewInstance(
    /* [in] */ IClassLoader* classLoader,
    /* [in] */ const String& className)
{
    assert(0 && "TODO");
    // Class driverClass;
    // if (classLoader == null) {
    //     driverClass = Class.forName(className);
    // } else {
    //     driverClass = classLoader.loadClass(className);
    // }
    // return driverClass.newInstance();
    return NULL;
}

AutoPtr<IClassLoader> NewInstanceHelper::GetClassLoader ()
{
    assert(0 && "TODO");

    // Method m = null;

    // try {
    //     m = Thread.class.getMethod("getContextClassLoader");
    // } catch (NoSuchMethodException e) {
    //     // Assume that we are running JDK 1.1, use the current ClassLoader
    //     return NewInstance.class.getClassLoader();
    // }

    // try {
    //     return (ClassLoader) m.invoke(Thread.currentThread());
    // } catch (IllegalAccessException e) {
    //     // assert(false)
    //     throw new UnknownError(e.getMessage());
    // } catch (InvocationTargetException e) {
    //     // assert(e.getTargetException() instanceof SecurityException)
    //     throw new UnknownError(e.getMessage());
    // }
    return NULL;
}

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org
