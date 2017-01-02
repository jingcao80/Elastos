//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "PrivacyCursor.h"
#include "elastos/droid/os/CBundle.h"
#include <elastos/utility/logging/Logger.h>
//#include <Math/CBigInteger.h>
#include <stdlib.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Droid::Os::CBundle;
//using Elastos::Math::IBigInteger;
//using Elastos::Math::CBigInteger;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Privacy {
namespace Surrogate {

const String PrivacyCursor::TAG("PrivacyCursor");
const String PrivacyCursor::mask[16] = {String("0"), String("1"), String("2"), String("3"), String("4"), String("5"), String("6"), String("7"), String("8"), String("9"), String("a"), String("b"), String("c"), String("d"), String("e"), String("f")};

CAR_INTERFACE_IMPL(PrivacyCursor, ICursor)

PrivacyCursor::PrivacyCursor()
: blockedColumnAndroidID(0)
, googleCursor(FALSE)
{
}

PrivacyCursor::PrivacyCursor(
    /* [in] */ ICursor* realCursor,
    /* [in] */ ArrayOf<Int32>* allowedIds)
: blockedColumnAndroidID(0)
, googleCursor(FALSE)
{
    this->realCursor = (allowedIds == NULL || allowedIds->GetLength() == 0) ? NULL : realCursor;
    this->allowedIds = allowedIds;

    if (this->realCursor != NULL) {
        Int32 currentPos = -1;
        this->realCursor->GetPosition(&currentPos);
        this->allowedIdMapping = ArrayOf<Int32>::Alloc(allowedIds->GetLength());
        Int32 i = 0;
        Boolean moveResult = FALSE;
        this->realCursor->MoveToNext(&moveResult);
        Int32 position = -1;

        while (moveResult) {
            if (IsAllowed(this->realCursor)) {
                this->realCursor->GetPosition(&position);
                (*allowedIdMapping)[i] = position;
                i++;
            }

            this->realCursor->MoveToNext(&moveResult);
        }

        this->realCursor->MoveToPosition(currentPos, &moveResult);
    }
}

PrivacyCursor::PrivacyCursor(
    /* [in] */ ICursor* realCursor,
    /* [in] */ Int32 blockedColumnAndroidID)
{
    if (blockedColumnAndroidID == -11) {
        this->realCursor = NULL;
    }
    else {
        this->realCursor = realCursor;
    }

    this->blockedColumnAndroidID = blockedColumnAndroidID;
    this->googleCursor = TRUE;
    Logger::I(TAG, String("constructor is ready for google cursor! forbidden_position: ") + blockedColumnAndroidID);
}

ECode PrivacyCursor::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)

    if (realCursor != NULL && allowedIdMapping != NULL && !googleCursor) {
        Logger::D(TAG, String("getCount: ") + allowedIdMapping->GetLength());
        *count = allowedIdMapping->GetLength();
        return NOERROR;
    }
    else if (realCursor != NULL) {
        return realCursor->GetCount(count);
    }
    *count = 0;
    return NOERROR;
}

ECode PrivacyCursor::GetPosition(
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position)

    if (realCursor != NULL && allowedIdMapping != NULL && !googleCursor) {
        Int32 pos = -1;
        realCursor->GetPosition(&pos);
        Int32 result = GetMappedPos(pos);
        Logger::D(TAG, String("getPosition - mapped position: ") + StringUtils::Int32ToString(result) + String(" real position: ") + StringUtils::Int32ToString(pos));
        *position = result;
        return NOERROR;
    }
    else if (realCursor != NULL) {
        return realCursor->GetPosition(position);
    }

    *position = -1;
    return NOERROR;
}

ECode PrivacyCursor::Move(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)

    if (realCursor != NULL && allowedIdMapping != NULL && !googleCursor) {
        Logger::D(TAG, String("move - offset: ") + offset);
//        try {
        Int32 pos = -1;
        FAIL_RETURN(realCursor->GetPosition(&pos))
        Int32 realPos = (*allowedIdMapping)[GetMappedPos(pos) + offset];
        return realCursor->MoveToPosition(realPos, succeeded);
//        } catch (ArrayIndexOutOfBoundsException e) {
//            return false;
//        }
    } else if (realCursor != NULL) { //here we go with our google cursor
        realCursor->Move(offset, succeeded);
    }
    *succeeded = FALSE;
    return NOERROR;
}

ECode PrivacyCursor::MoveToPosition(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = FALSE;

    if (realCursor != NULL && allowedIdMapping != NULL && !googleCursor) {
//            try {
        return realCursor->MoveToPosition((*allowedIdMapping)[position], succeeded);
//            } catch (ArrayIndexOutOfBoundsException e) {
//                return false;
//            }
    } else if (realCursor != NULL) {
        return realCursor->MoveToPosition(position, succeeded);
    }

    *succeeded = FALSE;
    return NOERROR;
}

ECode PrivacyCursor::MoveToFirst(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)

    if (realCursor != NULL && allowedIdMapping != NULL && !googleCursor) {
        return realCursor->MoveToPosition((*allowedIdMapping)[0], succeeded);
    } else if (realCursor != NULL) {
        return realCursor->MoveToFirst(succeeded);
    }

    *succeeded = FALSE;
    return NOERROR;
}

ECode PrivacyCursor::MoveToLast(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)

    if (realCursor != NULL && allowedIdMapping != NULL && !googleCursor) {
        return realCursor->MoveToPosition((*allowedIdMapping)[allowedIdMapping->GetLength() - 1], succeeded);
    } else if (realCursor != NULL) {
        return realCursor->MoveToLast(succeeded);
    }

    *succeeded = FALSE;
    return NOERROR;
}

ECode PrivacyCursor::MoveToNext(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)

    if (realCursor != NULL && allowedIdMapping != NULL && !googleCursor) {
        Boolean moveResult = FALSE;
        realCursor->MoveToNext(&moveResult);
        if (!moveResult) {
            *succeeded = FALSE;
            return NOERROR;
        }

        Boolean result = TRUE;
        while (result == TRUE && !IsAllowed(realCursor)) {
            realCursor->MoveToNext(&result);
        }

        *succeeded = result;
        return NOERROR;
    }
    else if(realCursor != NULL) {
        return realCursor->MoveToNext(succeeded);
    }

    *succeeded = FALSE;
    return NOERROR;
}

ECode PrivacyCursor::MoveToPrevious(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)

    if (realCursor != NULL && allowedIdMapping != NULL && !googleCursor) {
        Boolean result = FALSE;
        realCursor->MoveToPrevious(&result);
        if (!result) {
            *succeeded = FALSE;
            return NOERROR;
        }

        result = TRUE;
        while (result == TRUE && !IsAllowed(realCursor)) {
            realCursor->MoveToPrevious(&result);
        }

        *succeeded = result;
        return NOERROR;
    }
    else if(realCursor != NULL) {
        return realCursor->MoveToPrevious(succeeded);
    }

    *succeeded = FALSE;
    return NOERROR;
}

ECode PrivacyCursor::IsFirst(
    /* [out] */ Boolean* isFirst)
{
    VALIDATE_NOT_NULL(isFirst)

    if (realCursor != NULL && allowedIdMapping != NULL && !googleCursor) {
        Int32 pos = -1;
        realCursor->GetPosition(&pos);

        if (pos == (*allowedIdMapping)[0]) {
            *isFirst = TRUE;
            return NOERROR;
        }
    }
    else if(realCursor != NULL) {
        return realCursor->IsFirst(isFirst);

    }
    *isFirst = FALSE;
    return NOERROR;
}

ECode PrivacyCursor::IsLast(
    /* [out] */ Boolean* isLast)
{
    VALIDATE_NOT_NULL(isLast)

    if (realCursor != NULL && allowedIdMapping != NULL && !googleCursor) {
        Int32 pos = -1;
        realCursor->GetPosition(&pos);

        if (pos == (*allowedIdMapping)[allowedIdMapping->GetLength() - 1]) {
            *isLast = TRUE;
            return NOERROR;
        }
    }
    else if (realCursor != NULL) {
        return realCursor->IsLast(isLast);
    }
    *isLast = FALSE;
    return NOERROR;
}

ECode PrivacyCursor::IsBeforeFirst(
    /* [out] */ Boolean* isBeforeFirst)
{
    VALIDATE_NOT_NULL(isBeforeFirst)

    if (realCursor != NULL) {
        return realCursor->IsBeforeFirst(isBeforeFirst);
    }

    *isBeforeFirst = FALSE;
    return NOERROR;
}

ECode PrivacyCursor::IsAfterLast(
    /* [out] */ Boolean* isAfterLast)
{
    VALIDATE_NOT_NULL(isAfterLast)

    if (realCursor != NULL) {
        return realCursor->IsAfterLast(isAfterLast);
    }

    *isAfterLast = FALSE;
    return NOERROR;
}

ECode PrivacyCursor::GetColumnIndex(
    /* [in] */ const String& columnName,
    /* [out] */ Int32* columnIndex)
{
    VALIDATE_NOT_NULL(columnIndex)

    if (realCursor != NULL) {
        return realCursor->GetColumnIndex(columnName, columnIndex);
    }

    *columnIndex = -1;
    return NOERROR;
}

ECode PrivacyCursor::GetColumnIndexOrThrow(
    /* [in] */ const String& columnName,
    /* [out] */ Int32* columnIndex)
{
    VALIDATE_NOT_NULL(columnIndex)

    if (realCursor != NULL) {
        return realCursor->GetColumnIndexOrThrow(columnName, columnIndex);
    }

    *columnIndex = -1;
    return NOERROR;
}

ECode PrivacyCursor::GetColumnName(
    /* [in] */ Int32 columnIndex,
    /* [out] */ String* columnName)
{
    VALIDATE_NOT_NULL(columnName)

    if (realCursor != NULL) {
        return realCursor->GetColumnName(columnIndex, columnName);
    }

    *columnName = String("");
    return NOERROR;
}

ECode PrivacyCursor::GetColumnNames(
    /* [out, callee] */ ArrayOf<String>** columnNames)
{
    if (realCursor != NULL) {
        return realCursor->GetColumnNames(columnNames);
    }

    *columnNames = ArrayOf<String>::Alloc(1);
    REFCOUNT_ADD(*columnNames);
    (**columnNames)[0] = String("");
    return NOERROR;
}

ECode PrivacyCursor::GetColumnCount(
    /* [out] */ Int32* columnCount)
{
    VALIDATE_NOT_NULL(columnCount)

    if (realCursor != NULL) {
        return realCursor->GetColumnCount(columnCount);
    }

    *columnCount = 0;
    return NOERROR;
}

ECode PrivacyCursor::GetBlob(
    /* [in] */ Int32 columnIndex,
    /* [out, callee] */ ArrayOf<Byte>** blob)
{
    VALIDATE_NOT_NULL(blob)

    if (realCursor != NULL) {
        return realCursor->GetBlob(columnIndex, blob);
    }

    *blob = ArrayOf<Byte>::Alloc(0);
    REFCOUNT_ADD(*blob);
    return NOERROR;
}

ECode PrivacyCursor::GetString(
    /* [in] */ Int32 columnIndex,
    /* [out] */ String* columnValue)
{
    VALIDATE_NOT_NULL(columnValue)

    Int32 pos = -1;
    realCursor->GetPosition(&pos);

    if (googleCursor && (columnIndex == 1) && (pos == blockedColumnAndroidID)) {
        Logger::I(TAG, String("google tries to get android_id with getString()"));
        String id = GetFakeID();

        if (!id.IsNull()) {
//            try{

//            TODO: Mike, CBigInteger is not available
            //AutoPtr<CBigInteger> value;
            //CBigInteger::New(id, 16, (IBigInteger**)&value);
            //String fakeValue;
            //value->ToString(&fakeValue);
            //Logger::I(TAG, String("returned : ") + fakeValue + "(String) as fake android id");
            //*columnValue = fakeValue;
            return NOERROR;
//            }
//            catch(Exception e){
//                e.printStackTrace();
//                Log.i(TAG,"returned \"\" as android_id");
//                return "";
//            }
        }
        else {
            Logger::I(TAG, String("returned \"\" as android_id"));
            *columnValue = String("");
            return NOERROR;
        }
    }
    if (realCursor != NULL) {
        return realCursor->GetString(columnIndex, columnValue);
    }
    *columnValue = String("");
    return NOERROR;
}

ECode PrivacyCursor::CopyStringToBuffer(
    /* [in] */ Int32 columnIndex,
    /* [in] */ ICharArrayBuffer* buffer)
{
    if (realCursor != NULL) {
        return realCursor->CopyStringToBuffer(columnIndex, buffer);
    }

    return NOERROR;
}

ECode PrivacyCursor::GetInt16(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int16* columnValue)
{
    VALIDATE_NOT_NULL(columnValue)

    if (realCursor != NULL) {
        return realCursor->GetInt16(columnIndex, columnValue);
    }

    *columnValue = 0;
    return NOERROR;
}

ECode PrivacyCursor::GetInt32(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int32* columnValue)
{
    VALIDATE_NOT_NULL(columnValue)

    if (realCursor != NULL) {
        return realCursor->GetInt32(columnIndex, columnValue);
    }

    *columnValue = 0;
    return NOERROR;
}

ECode PrivacyCursor::GetInt64(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int64* columnValue)
{
    VALIDATE_NOT_NULL(columnValue)

    if (realCursor != NULL) {
        return realCursor->GetInt64(columnIndex, columnValue);
    }

    *columnValue = 0;
    return NOERROR;
}

ECode PrivacyCursor::GetFloat(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Float* columnValue)
{
    VALIDATE_NOT_NULL(columnValue)

    if (realCursor != NULL) {
        return realCursor->GetFloat(columnIndex, columnValue);
    }

    *columnValue = 0.0f;
    return NOERROR;
}

ECode PrivacyCursor::GetDouble(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Double* columnValue)
{
    VALIDATE_NOT_NULL(columnValue)

    if (realCursor != NULL) {
        return realCursor->GetDouble(columnIndex, columnValue);
    }

    *columnValue = 0;
    return NOERROR;
}

ECode PrivacyCursor::GetType(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    if (realCursor != NULL) {
        return realCursor->GetType(columnIndex, index);
    }

    *index = 0; //ICursor::FIELD_TYPE_NULL;
    return NOERROR;
}

ECode PrivacyCursor::IsNull(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* isNull)
{
    VALIDATE_NOT_NULL(isNull)

    if (realCursor != NULL) {
        return realCursor->IsNull(columnIndex, isNull);
    }

    *isNull = FALSE;
    return NOERROR;
}

ECode PrivacyCursor::Deactivate()
{
    if (realCursor != NULL) {
        return realCursor->Deactivate();
    }

    return NOERROR;
}

ECode PrivacyCursor::Requery(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)

    if (realCursor != NULL) {
        return realCursor->Requery(succeeded);
    }

    *succeeded = FALSE;
    return NOERROR;
}

ECode PrivacyCursor::Close()
{
    if (realCursor != NULL) {
        return realCursor->Close();
    }

    return NOERROR;
}

ECode PrivacyCursor::IsClosed(
    /* [out] */ Boolean* closed)
{
    VALIDATE_NOT_NULL(closed)

    if (realCursor != NULL) {
        return realCursor->IsClosed(closed);
    }

    *closed = FALSE;
    return NOERROR;
}

ECode PrivacyCursor::RegisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    if (realCursor != NULL) {
        return realCursor->RegisterContentObserver(observer);
    }

    return NOERROR;
}

ECode PrivacyCursor::UnregisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    if (realCursor != NULL) {
        return realCursor->UnregisterContentObserver(observer);
    }

    return NOERROR;
}

ECode PrivacyCursor::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    if (realCursor != NULL) {
        return realCursor->RegisterDataSetObserver(observer);
    }

    return NOERROR;
}

ECode PrivacyCursor::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    if (realCursor != NULL) {
        return realCursor->UnregisterDataSetObserver(observer);
    }

    return NOERROR;
}

ECode PrivacyCursor::SetNotificationUri(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Elastos::Droid::Net::IUri* uri)
{
    if (realCursor != NULL) {
        return realCursor->SetNotificationUri(cr, uri);
    }

    return NOERROR;
}

ECode PrivacyCursor::GetWantsAllOnMoveCalls(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (realCursor != NULL) {
        return realCursor->GetWantsAllOnMoveCalls(result);
    }

    *result = FALSE;
    return NOERROR;
}

ECode PrivacyCursor::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras)

    if (realCursor != NULL) {
        return realCursor->GetExtras(extras);
    }

    CBundle::New(extras);
    return NOERROR;
}

ECode PrivacyCursor::Respond(
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)

    if (realCursor != NULL) {
        return realCursor->Respond(extras, result);
    }

    CBundle::New(result);
    return NOERROR;
}

String PrivacyCursor::GetFakeID()
{
//    try{
//    Random rand = new Random();
    String output("");
    for(Int32 i=0; i < 16; i++){
        //Int32 index = (int)(Math::Random() * 100) % 16;
        output += mask[random() % 16];
    }
    return output;
//} catch (Exception e){
//    Log.e(TAG,"something went wrong with creating fake ID");
//    return NULL;
//}
}

Int32 PrivacyCursor::GetContactId(
   /* [in] */ ICursor* c)
{
    Int32 colIndex = -1;
    c->GetColumnIndex(String("_id")/*IContactsContractContacts::_ID*/, &colIndex);
    Int32 result = -1;

    if (colIndex != -1) {
        c->GetInt32(colIndex, &result);
    }
//        Log.d(TAG, "getContactId - colIndex: " + colIndex + " id: " + result);
    return result;
}

Boolean PrivacyCursor::IsAllowed(
   /* [in] */ Int32 id)
{
    Boolean result = FALSE;
    if(allowedIds != NULL) {
        Int32 size = allowedIds->GetLength();
        for (Int32 i = 0; i < size; i++) {
            Int32 allowedId = (*allowedIds)[i];
            if (id == allowedId) {
                result = TRUE;
                break;
            }
        }
    }
//    Log.d(TAG, "isAllowed - id: " + id + " result: " + result);
    return result;
}

Boolean PrivacyCursor::IsAllowed(
   /* [in] */ ICursor* c)
{
    return IsAllowed(GetContactId(realCursor));
}

/**
* TODO: switch to HashMap etc to speed this up?
* @param realPos
* @return
*/
Int32 PrivacyCursor::GetMappedPos(
   /* [in] */ Int32 realPos)
{
    for (Int32 i = 0; i < allowedIdMapping->GetLength(); i++) {
        if ((*allowedIdMapping)[i] == realPos) {
            return i;
        }
    }

    return -1;
}

} // namespace Surrogate
} // namespace Privacy
} // namespace Droid
} // namespace Elastos
