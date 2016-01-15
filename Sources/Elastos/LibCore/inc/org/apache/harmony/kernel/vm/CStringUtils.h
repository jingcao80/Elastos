
#ifndef __ORG_APACHE_HARMONY_KERNEL_VM_CSTRINGUTILS_H__
#define __ORG_APACHE_HARMONY_KERNEL_VM_CSTRINGUTILS_H__

#include "_Org_Apache_Harmony_Kernel_Vm_CStringUtils.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Kernel {
namespace Vm {

CarClass(CStringUtils)
{
public:
    CARAPI CombineStrings(
        /* [in] */ ArrayOf<PInterface>* list,
        /* [out] */ String* cmbStr);

private:
    // TODO: Add your private member variables here.
};

} // namespace Vm
} // namespace Kernel
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_KERNEL_VM_CSTRINGUTILS_H__
