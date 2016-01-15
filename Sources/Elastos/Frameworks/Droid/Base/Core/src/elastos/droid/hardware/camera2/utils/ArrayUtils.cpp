
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/camera2/utils/ArrayUtils.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuffer;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Objects;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

const String ArrayUtils::TAG("ArrayUtils");
const Boolean ArrayUtils::VERBOSE = FALSE; // TODO: = Log.isLoggable(TAG, Log.VERBOSE);

ECode ArrayUtils::GetArrayIndex(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ IInterface* needle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    if (array == NULL) {
        *value = -1;
        return NOERROR;
    }

    Int32 index = 0;
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IInterface> elem = (*array)[i];
        if (Objects::Equals(elem.Get(), needle)) {
            *value = index;
            return NOERROR;
        }
        index++;
    }

    *value = -1;
    return NOERROR;
}

ECode ArrayUtils::GetArrayIndex(
    /* [in] */ ArrayOf<Int32>* array,
    /* [in] */ Int32 needle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    if (array == NULL) {
        *value = -1;
        return NOERROR;
    }
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        if ((*array)[i] == needle) {
            *value = i;
            return NOERROR;
        }
    }
    *value = -1;
    return NOERROR;
}

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
    Int32 size;
    list->GetSize(&size);
    CArrayList::New(size, (IList**)&convertedList);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        String str;
        AutoPtr<ICharSequence> csq = ICharSequence::Probe(obj);
        csq->ToString(&str);

        AutoPtr<ArrayOf<IInterface*> > faceArray = ArrayOf<IInterface*>::Alloc(convertFrom->GetLength());
        for (Int32 j = 0; j < convertFrom->GetLength(); j++) {
            AutoPtr<ICharSequence> obj;
            CString::New((*convertFrom)[j], (ICharSequence**)&obj);
            faceArray->Set(j, obj);
        }
        Int32 strIndex;
        GetArrayIndex(faceArray, csq, &strIndex);
        faceArray = NULL;

        // Guard against unexpected values
        if (strIndex < 0) {
            Slogger::W(TAG, "Ignoring invalid value %s", str.string());
            continue;
        }

        // Ignore values we can't map into (intentional)
        if (strIndex < convertTo->GetLength()) {
            AutoPtr<IInteger32> valueObj;
            CInteger32::New((*convertTo)[strIndex], (IInteger32**)&valueObj);
            convertedList->Add(TO_IINTERFACE(valueObj));
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
        *outarr = NULL;
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

ECode ArrayUtils::Contains(
    /* [in] */ ArrayOf<Int32>* array,
    /* [in] */ Int32 elem,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 value;
    GetArrayIndex(array, elem, &value);
    *result = (value != -1);
    return NOERROR;
}

ECode ArrayUtils::Contains(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ IInterface* elem,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 value;
    GetArrayIndex(array, elem, &value);
    *result = (value != -1);
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
