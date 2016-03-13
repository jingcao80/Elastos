
#ifndef  __ELASTOS_DROID_SYSTEMUI_UTILS_H__
#define  __ELASTOS_DROID_SYSTEMUI_UTILS_H__

#include <elastos/droid/ext/frameworkext.h>

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
        /* [in] */ const String& className,
        /* [in] */ Int32 flag = 0);

public:
    static const Int32 ELASTOS_DROID_CORE_ECO_FALG;

private:
    static const String sModulePath;
    static AutoPtr<IModuleInfo> sModuleInfo;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_UTILS_H__
