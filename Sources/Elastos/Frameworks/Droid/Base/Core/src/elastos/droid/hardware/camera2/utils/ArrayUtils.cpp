
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/camera2/utils/ArrayUtils.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuffer;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Objects;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

const String ArrayUtils::TAG("ArrayUtils");
const Boolean ArrayUtils::VERBOSE = FALSE; // TODO: = Log.isLoggable(TAG, Log.VERBOSE);

ECode ArrayUtils::ConvertStringListToIntArray(
    /* [in] */ IList* list,
    /* [in] */ ArrayOf<String>* convertFrom,
    /* [in] */ ArrayOf<Int32>* convertTo,
    /* [out, callee] */ ArrayOf<Int32>** outarr)
{
    VALIDATE_NOT_NULL(outarr);
    *outarr = NULL;

    if (list == NULL) {
        *outarr = NULL;
        return NOERROR;
    }

    AutoPtr<IList> convertedList;
    ConvertStringListToIntList(list, convertFrom, convertTo, (IList**)&convertedList);

    Int32 length;
    convertedList->GetSize(&length);
    AutoPtr<ArrayOf<Int32> > returnArray = ArrayOf<Int32>::Alloc(length);
    for (Int32 i = 0; i < returnArray->GetLength(); ++i) {
        AutoPtr<IInterface> obj;
        convertedList->Get(i, (IInterface**)&obj);
        Int32 value;
        IInteger32::Probe(obj)->GetValue(&value);
        (*returnArray)[i] = value;
    }

    *outarr = returnArray;
    REFCOUNT_ADD(*outarr);
    return NOERROR;
}

ECode ArrayUtils::ConvertStringArrayToIntArray(
    /* [in] */ ArrayOf<String>* list,
    /* [in] */ ArrayOf<String>* convertFrom,
    /* [in] */ ArrayOf<Int32>* convertTo,
    /* [out, callee] */ ArrayOf<Int32>** outarr)
{
    VALIDATE_NOT_NULL(outarr);
    *outarr = NULL;

    if (list == NULL) {
        return NOERROR;
    }

    AutoPtr<IList> convertedList;
    ConvertStringArrayToIntList(list, convertFrom, convertTo, (IList**)&convertedList);

    Int32 length;
    convertedList->GetSize(&length);
    AutoPtr<ArrayOf<Int32> > returnArray = ArrayOf<Int32>::Alloc(length);
    for (Int32 i = 0; i < returnArray->GetLength(); ++i) {
        AutoPtr<IInterface> obj;
        convertedList->Get(i, (IInterface**)&obj);
        Int32 value;
        IInteger32::Probe(obj)->GetValue(&value);
        (*returnArray)[i] = value;
    }

    *outarr = returnArray;
    REFCOUNT_ADD(*outarr);
    return NOERROR;
}

ECode ArrayUtils::ConvertStringListToIntList(
    /* [in] */ IList* list,
    /* [in] */ ArrayOf<String>* convertFrom,
    /* [in] */ ArrayOf<Int32>* convertTo,
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);

    if (list == NULL) {
        *outlist = NULL;
        return NOERROR;
    }

    AutoPtr<IList> convertedList;
    Int32 size, strIndex;
    list->GetSize(&size);
    CArrayList::New(size, (IList**)&convertedList);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        String str = Object::ToString(obj);
        GetArrayIndex(convertFrom, str, &strIndex);

        // Guard against unexpected values
        if (strIndex < 0) {
            Logger::W(TAG, "%s: Ignoring invalid value %s", __FUNCTION__, str.string());
            continue;
        }

        // Ignore values we can't map into (intentional)
        if (strIndex < convertTo->GetLength()) {
            AutoPtr<IInteger32> valueObj = CoreUtils::Convert((*convertTo)[strIndex]);
            convertedList->Add(valueObj);
        }
    }

    *outlist = convertedList;
    REFCOUNT_ADD(*outlist);
    return NOERROR;
}

ECode ArrayUtils::ConvertStringArrayToIntList(
    /* [in] */ ArrayOf<String>* list,
    /* [in] */ ArrayOf<String>* convertFrom,
    /* [in] */ ArrayOf<Int32>* convertTo,
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);

    if (list == NULL) {
        *outlist = NULL;
        return NOERROR;
    }

    Int32 size = list->GetLength();
    Int32 strIndex;
    AutoPtr<IList> convertedList;
    CArrayList::New(size, (IList**)&convertedList);
    for (Int32 i = 0; i < size; ++i) {
        String str = (*list)[i];
        GetArrayIndex(convertFrom, str, &strIndex);

        // Guard against unexpected values
        if (strIndex < 0) {
            Logger::W(TAG, "%s: Ignoring invalid value %s", __FUNCTION__, str.string());
            continue;
        }

        // Ignore values we can't map into (intentional)
        if (strIndex < convertTo->GetLength()) {
            AutoPtr<IInteger32> valueObj = CoreUtils::Convert((*convertTo)[strIndex]);
            convertedList->Add(valueObj);
        }
    }

    *outlist = convertedList;
    REFCOUNT_ADD(*outlist);
    return NOERROR;
}

ECode ArrayUtils::ToIntArray(
    /* [in] */ IList* list,
    /* [out, callee] */ ArrayOf<Int32>** outarr)
{
    VALIDATE_NOT_NULL(outarr);
    *outarr = NULL;

    if (list == NULL) {
        return NOERROR;
    }

    Int32 size;
    list->GetSize(&size);
    AutoPtr<ArrayOf<Int32> > arr = ArrayOf<Int32>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        Int32 elem;
        IInteger32::Probe(obj)->GetValue(&elem);
        (*arr)[i] = elem;
    }

    *outarr = arr;
    REFCOUNT_ADD(*outarr);
    return NOERROR;
}

ECode ArrayUtils::ArrayElementsEqualTo(
    /* [in] */ ArrayOf<String>* array,
    /* [in] */ const String& single,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;
    if (array && array->GetLength() == 1) {
        *value = (*array)[0].Equals(single);
    }
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
