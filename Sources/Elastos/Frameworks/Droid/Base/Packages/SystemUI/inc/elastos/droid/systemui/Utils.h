
#ifndef  __ELASTOS_DROID_SYSTEMUI_UTILS_H__
#define  __ELASTOS_DROID_SYSTEMUI_UTILS_H__

#include <elastos/droid/ext/frameworkext.h>

using Elastos::Core::IClassLoader;

_ETL_NAMESPACE_BEGIN
template<> struct Hash<Elastos::InterfaceID>
{
    size_t operator()(const Elastos::InterfaceID& x) const { return size_t(&x); }
};
_ETL_NAMESPACE_END

namespace Elastos {
namespace Droid {
namespace SystemUI {

class Utils
{
public:
    static AutoPtr<IClassInfo> GetClassInfo(
        /* [in] */ const String& className);

private:
    static const String sModulePath;
    static AutoPtr<IClassLoader> sClassLoader;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_UTILS_H__
