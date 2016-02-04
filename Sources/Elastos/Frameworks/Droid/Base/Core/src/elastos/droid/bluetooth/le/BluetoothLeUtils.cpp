#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Bluetooth.h"
#include "elastos/droid/bluetooth/le/BluetoothLeUtils.h"
#include "elastos/core/StringBuilder.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

//=====================================================================
//                           BluetoothLeUtils
//=====================================================================
String BluetoothLeUtils::ToString(
    /* [in] */ IMap* map)
{
    if (map == NULL) {
        return String("null");
    }
    Boolean isEmpty = TRUE;
    if (map->IsEmpty(&isEmpty), isEmpty) {
        return String("{}");
    }
    AutoPtr<StringBuilder> buffer = new StringBuilder();
    buffer->Append('{');
    //Iterator<Map.Entry<T, byte[]>> it = map.entrySet().iterator();
    AutoPtr<ISet> entrySet;
    map->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    AutoPtr<IMapEntry> entry;

    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> mapEntry;
        it->GetNext((IInterface**)&mapEntry);
        entry = IMapEntry::Probe(mapEntry);
        AutoPtr<IInterface> ko, vo;
        entry->GetKey((IInterface**)&ko);//ko string, vo is Pair
        entry->GetValue((IInterface**)&vo);
        //buffer.append(key).append("=").append(Arrays.toString(map.get(key)));
        //TODO update according to the line above.
        buffer->Append("key=value");
        if (it->HasNext(&hasNext), hasNext) {
            buffer->Append(", ");
        }
    }
    buffer->Append('}');
    return buffer->ToString();
}

Boolean BluetoothLeUtils::Equals(
    /* [in] */ IMap* map,
    /* [in] */ IMap* otherMap)
{
    Boolean result;
    map->Equals(TO_IINTERFACE(otherMap), &result);
    return result;
//    if (map == otherMap) {
//        return TRUE;
//    }
//    if (map == NULL|| otherMap == NULL) {
//        return FALSE;
//    }
//    Int32 size1, size2;
//    map->GetSize(&size1);
//    otherMap->GetSize(&size2);
//    if (size1 != size2) {
//        return FALSE;
//    }
//    //Set<T> keys = map.keySet();
//    AutoPtr<ISet> keyset1, keyset2;
//    map->GetKeySet((ISet**)&keyset1);
//    otherMap->GetKeySet((ISet**)&keyset2);
//    Boolean equals;
//    keyset1->Equals(TO_IINTERFACE(keyset2), &equals);
//    if (!equals) {
//        return FALSE;
//    }
//    for (T key : keys) {
//        if (!Objects.deepEquals(map.get(key), otherMap.get(key))) {
//            return false;
//        }
//    }
//    return TRUE;
}

String BluetoothLeUtils::ToString(
    /* [in] */ ISparseArray* array)
{
    if (array == NULL) {
        return String("null");
    }
    Int32 size;
    if ((array->GetSize(&size), size) == 0) {
        return String("{}");
    }
    AutoPtr<StringBuilder> buffer = new StringBuilder();
    buffer->Append('{');
    for (Int32 i = 0; i < size; ++i) {
        //buffer.append(array.keyAt(i)).append("=").append(Arrays.toString(array.valueAt(i)));
        buffer->Append("key=value");//TODO update according the above line
    }
    buffer->Append('}');
    return buffer->ToString();
}

Boolean BluetoothLeUtils::Equals(
    /* [in] */ ISparseArray* array,
    /* [in] */ ISparseArray* otherArray)
{
    if (array == otherArray) {
        return TRUE;
    }
    if (array == NULL|| otherArray == NULL) {
        return FALSE;
    }
    Int32 size1, size2;
    array->GetSize(&size1);
    otherArray->GetSize(&size2);
    if (size1 != size2) {
        return FALSE;
    }

    // Keys are guaranteed in ascending order when indices are in ascending order.
    for (Int32 i = 0; i < size1; ++i) {
        Int32 key1, key2;
        array->KeyAt(i, &key1);
        otherArray->KeyAt(i, &key2);
        if (key1 != key2)
            return FALSE;

        //if (array.keyAt(i) != otherArray.keyAt(i) ||
        //        !Arrays.equals(array.valueAt(i), otherArray.valueAt(i))) {
        //    return false;
        //}
        AutoPtr<IInterface> value1, value2;
        array->ValueAt(i, (IInterface**)&value1);
        otherArray->ValueAt(i, (IInterface**)&value2);
        //TODO more check
        //because we store IInterface*, so the pointer value need equal
        if (value1 != value2)
            return FALSE;
    }
    return TRUE;
}

ECode BluetoothLeUtils::CheckAdapterStateOn(
    /* [in] */ IBluetoothAdapter* adapter)
{
    Int32 state;
    if (adapter == NULL || (adapter->GetState(&state), state) != IBluetoothAdapter::STATE_ON) {
        //throw new IllegalStateException("BT Adapter is not turned ON");
        Logger::E("BluetoothLeUtils::CheckAdapterStateOn", "BT Adapter is not turned ON");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
