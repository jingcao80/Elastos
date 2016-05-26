
#include "CStringUtils.h"
#include <cmdef.h>
#include <elastos/StringBuilder.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Kernel {
namespace Vm {

CAR_SINGLETON_IMPL(CStringUtils)

CAR_INTERFACE_IMPL(CStringUtils, Singleton, IStringUtils)

ECode CStringUtils::CombineStrings(
    /* [in] */ ArrayOf<PInterface>* list,
    /* [out] */ String* cmbStr)
{
    VALIDATE_NOT_NULL(cmbStr)
    Int32 listLength = list->GetLength();

    switch (listLength) {
        case 0: {
            *cmbStr = "";
            return NOERROR;
        }
        case 1: {
            return ICharSequence::Probe((*list)[0])->ToString(cmbStr);
        }
    }

    Int32 strLength = 0;

    for (Int32 i = 0; i < listLength; i++) {
        String str;
        ICharSequence::Probe((*list)[i])->ToString(&str);
        strLength += str.GetLength();
    }

    StringBuilder sb(strLength);

    for (Int32 i = 0; i < listLength; i++) {
        sb.AppendObject((*list)[i]);
    }
    return sb.ToString(cmbStr);
}

} // namespace Vm
} // namespace Kernel
} // namespace Harmony
} // namespace Apache
} // namespace Org

