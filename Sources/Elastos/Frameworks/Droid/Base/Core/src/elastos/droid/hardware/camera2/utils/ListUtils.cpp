
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/camera2/utils/ListUtils.h"
#include <elastos/core/StringBuffer.h>

using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

ECode ListUtils::ListContains(
    /* [in] */ IList* list,
    /* [in] */ IInterface* needle,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    if (list == NULL) {
        *value = FALSE;
        return NOERROR;
    }
    else {
        return list->Contains(needle, value);
    }
    return NOERROR;
}

ECode ListUtils::ListElementsEqualTo(
    /* [in] */ IList* list,
    /* [in] */ IInterface* single,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    if (list == NULL) {
        *value = FALSE;
        return NOERROR;
    }

    Boolean result;
    Int32 size;
    list->GetSize(&size);
    *value = (size == 1 && (list->Contains(single, &result), result));
    return NOERROR;
}

ECode ListUtils::ListToString(
    /* [in] */ IList* list,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    if (list == NULL) {
        *str = NULL;
    }

    StringBuffer sb;
    sb += '[';

    Int32 size;
    list->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        String tmp;
        IObject::Probe(obj)->ToString(&tmp);
        sb += tmp;

        if (i != size - 1) {
            sb += ',';
        }
    }
    sb += ']';

    return sb.ToString(str);
}

ECode ListUtils::ListSelectFirstFrom(
    /* [in] */ IList* list,
    /* [in] */ ArrayOf<IInterface*>* choices,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    if (list == NULL) {
        *outface = NULL;
        return NOERROR;
    }

    for (Int32 i = 0; i < choices->GetLength(); i++) {
        AutoPtr<IInterface> choice = (*choices)[i];
        Boolean result;
        list->Contains(choice, &result);
        if (result) {
            *outface = choice;
            REFCOUNT_ADD(*outface);
            return NOERROR;
        }
    }

    *outface = NULL;
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
