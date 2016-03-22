
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/CContentProviderOperation.h"
#include "elastos/droid/content/CContentProviderResult.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/content/CContentProviderOperationBuilder.h"
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Core::CInteger64;
using Elastos::Core::IInteger32;
using Elastos::IO::ICloseable;
using Elastos::Utility::ISet;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {

const String CContentProviderOperation::TAG("CContentProviderOperation");


CAR_INTERFACE_IMPL_2(CContentProviderOperation, Object, IContentProviderOperation, IParcelable)

CAR_OBJECT_IMPL(CContentProviderOperation)

CContentProviderOperation::CContentProviderOperation()
    : mType(0)
    , mExpectedCount(0)
    , mYieldAllowed(FALSE)
{}

CContentProviderOperation::~CContentProviderOperation()
{
}

ECode CContentProviderOperation::constructor()
{
    return NOERROR;
}

ECode CContentProviderOperation::constructor(
    /* [in] */ IContentProviderOperationBuilder* operationBuilder)
{
    VALIDATE_NOT_NULL(operationBuilder)

    FAIL_RETURN(operationBuilder->GetType(&mType))
    FAIL_RETURN(operationBuilder->GetUri((IUri**)&mUri))
    FAIL_RETURN(operationBuilder->GetValues((IContentValues**)&mValues))
    FAIL_RETURN(operationBuilder->GetSelection(&mSelection))
    FAIL_RETURN(operationBuilder->GetSelectionArgs((ArrayOf<String>**)&mSelectionArgs))
    FAIL_RETURN(operationBuilder->GetExpectedCount(&mExpectedCount))
    FAIL_RETURN(operationBuilder->GetSelectionArgsBackReferences((IHashMap**)&mSelectionArgsBackReferences))
    FAIL_RETURN(operationBuilder->GetValuesBackReferences((IContentValues**)&mValuesBackReferences))
    FAIL_RETURN(operationBuilder->GetYieldAllowed(&mYieldAllowed))
    return NOERROR;
}

ECode CContentProviderOperation::constructor(
    /* [in] */ IContentProviderOperation* cpo,
    /* [in] */ Boolean removeUserIdFromUri)
{
    VALIDATE_NOT_NULL(cpo)
    CContentProviderOperation* other = (CContentProviderOperation*)cpo;

    mType = other->mType;
    if (removeUserIdFromUri) {
        ContentProvider::GetUriWithoutUserId(other->mUri, (IUri**)&mUri);
    }
    else {
        mUri = other->mUri;
    }
    mValues = other->mValues;
    mSelection = other->mSelection;
    mSelectionArgs = other->mSelectionArgs;
    mExpectedCount = other->mExpectedCount;
    mSelectionArgsBackReferences = other->mSelectionArgsBackReferences;
    mValuesBackReferences = other->mValuesBackReferences;
    mYieldAllowed = other->mYieldAllowed;
    return NOERROR;
}

ECode CContentProviderOperation::GetWithoutUserIdInUri(
    /* [out] */ IContentProviderOperation** op)
{
    VALIDATE_NOT_NULL(op)
    *op = NULL;
    if (ContentProvider::UriHasUserId(mUri)) {
        AutoPtr<CContentProviderOperation> cpo;
        CContentProviderOperation::NewByFriend((CContentProviderOperation**)&cpo);
        cpo->constructor(THIS_PROBE(IContentProviderOperation), TRUE);
        *op = (IContentProviderOperation*)cpo.Get();
        REFCOUNT_ADD(*op);
        return NOERROR;
    }

    *op = THIS_PROBE(IContentProviderOperation);
    REFCOUNT_ADD(*op)
    return NOERROR;
}


ECode CContentProviderOperation::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source)
    source->ReadInt32(&mType);

    Int32 value = 0;

    Uri::ReadFromParcel(source, (IUri**)&mUri);

    if (source->ReadInt32(&value), value != 0) {
        AutoPtr<IInterface> contentValues;
        source->ReadInterfacePtr((Handle32*)(IInterface**)&contentValues);
        mValues = IContentValues::Probe(contentValues);
    }

    if (source->ReadInt32(&value), value != 0) {
        source->ReadString(&mSelection);
    }

    if (source->ReadInt32(&value), value != 0) {
        source->ReadArrayOfString((ArrayOf<String>**)&mSelectionArgs);
    }

    if (source->ReadInt32(&value), value != 0) {
        source->ReadInt32(&mExpectedCount);
    }

    if (source->ReadInt32(&value), value != 0) {
        AutoPtr<IContentValues> values;
        CContentValues::New((IContentValues**)&values);
        IParcelable::Probe(values)->ReadFromParcel(source);
        mValuesBackReferences = values;
    }

    if (source->ReadInt32(&value), value != 0) {
        CHashMap::New((IHashMap**)&mSelectionArgsBackReferences);
        Int32 count;
        source->ReadInt32(&count);
        Int32 key = 0;
        Int32 value = 0;
        for (Int32 i = 0; i < count; i++) {
            source->ReadInt32(&key);
            source->ReadInt32(&value);
            AutoPtr<IInteger32> ko = CoreUtils::Convert(key);
            AutoPtr<IInteger32> vo = CoreUtils::Convert(value);
            mSelectionArgsBackReferences->Put(TO_IINTERFACE(ko), TO_IINTERFACE(vo));
        }
    }

    mYieldAllowed = (source->ReadInt32(&value), value) != 0;
    return NOERROR;
}

ECode CContentProviderOperation::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest)
    dest->WriteInt32(mType);

    Uri::WriteToParcel(dest, mUri);

    if (NULL != mValues) {
        dest->WriteInt32(1);
        dest->WriteInterfacePtr(mValues);
    }
    else {
        dest->WriteInt32(0);
    }

    if (!mSelection.IsNull()) {
        dest->WriteInt32(1);
        dest->WriteString(mSelection);
    }
    else {
        dest->WriteInt32(0);
    }

    if (NULL != mSelectionArgs) {
        dest->WriteInt32(1);
        dest->WriteArrayOfString(mSelectionArgs);
    }
    else {
        dest->WriteInt32(0);
    }

    if (mExpectedCount != 0) {
        dest->WriteInt32(1);
        dest->WriteInt32(mExpectedCount);
    }
    else {
        dest->WriteInt32(0);
    }

    if (NULL != mValuesBackReferences) {
        dest->WriteInt32(1);
        AutoPtr<IParcelable> valueRefParcelable = (IParcelable*) mValuesBackReferences->Probe(EIID_IParcelable);
        FAIL_RETURN(valueRefParcelable->WriteToParcel(dest))
    }
    else {
        dest->WriteInt32(0);
    }

    if (NULL != mSelectionArgsBackReferences) {
        dest->WriteInt32(1);
        Int32 size;
        mSelectionArgsBackReferences->GetSize(&size);
        dest->WriteInt32(size);

        AutoPtr<ISet> entrySet;
        mSelectionArgsBackReferences->GetEntrySet((ISet**)&entrySet);
        AutoPtr<IIterator> it;
        entrySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        IMapEntry* entry;
        Int32 k, v;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> pair;
            it->GetNext((IInterface**)&pair);
            entry = IMapEntry::Probe(pair);
            AutoPtr<IInterface> ko, vo;
            entry->GetKey((IInterface**)&ko);
            entry->GetValue((IInterface**)&vo);
            IInteger32::Probe(ko)->GetValue(&k);
            IInteger32::Probe(vo)->GetValue(&v);
            dest->WriteInt32(k);
            dest->WriteInt32(v);
        }
    }
    else {
        dest->WriteInt32(0);
    }

    dest->WriteInt32(mYieldAllowed ? 1 : 0);
    return NOERROR;
}

AutoPtr<IContentProviderOperationBuilder> CContentProviderOperation::NewInsert(
    /* [in] */ IUri* uri)
{
    AutoPtr<CContentProviderOperationBuilder> builder;
    CContentProviderOperationBuilder::NewByFriend((CContentProviderOperationBuilder**)&builder);
    builder->constructor(IContentProviderOperation::TYPE_INSERT, uri);
    return (IContentProviderOperationBuilder*)builder.Get();
}

AutoPtr<IContentProviderOperationBuilder> CContentProviderOperation::NewUpdate(
    /* [in] */ IUri* uri)
{
    AutoPtr<CContentProviderOperationBuilder> builder;
    CContentProviderOperationBuilder::NewByFriend((CContentProviderOperationBuilder**)&builder);
    builder->constructor(IContentProviderOperation::TYPE_UPDATE, uri);
    return (IContentProviderOperationBuilder*)builder.Get();
}

AutoPtr<IContentProviderOperationBuilder> CContentProviderOperation::NewDelete(
    /* [in] */ IUri* uri)
{
    AutoPtr<CContentProviderOperationBuilder> builder;
    CContentProviderOperationBuilder::NewByFriend((CContentProviderOperationBuilder**)&builder);
    builder->constructor(IContentProviderOperation::TYPE_DELETE, uri);
    return (IContentProviderOperationBuilder*)builder.Get();
}

AutoPtr<IContentProviderOperationBuilder> CContentProviderOperation::NewAssertQuery(
    /* [in] */ IUri* uri)
{
    AutoPtr<CContentProviderOperationBuilder> builder;
    CContentProviderOperationBuilder::NewByFriend((CContentProviderOperationBuilder**)&builder);
    builder->constructor(IContentProviderOperation::TYPE_ASSERT, uri);
    return (IContentProviderOperationBuilder*)builder.Get();
}

ECode CContentProviderOperation::GetUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = mUri;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode CContentProviderOperation::IsYieldAllowed(
    /* [out] */ Boolean* isAllowed)
{
    VALIDATE_NOT_NULL(isAllowed)
    *isAllowed = mYieldAllowed;
    return NOERROR;
}

ECode CContentProviderOperation::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    return NOERROR;
}

ECode CContentProviderOperation::IsDeleteOperation(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mType == TYPE_DELETE;
    return NOERROR;
}

ECode CContentProviderOperation::IsWriteOperation(
    /* [out] */ Boolean* isWrite)
{
    VALIDATE_NOT_NULL(isWrite)
    *isWrite = mType == IContentProviderOperation::TYPE_DELETE
        || mType == IContentProviderOperation::TYPE_INSERT
        || mType == IContentProviderOperation::TYPE_UPDATE;
    return NOERROR;
}

ECode CContentProviderOperation::IsReadOperation(
    /* [out] */ Boolean* isRead)
{
    VALIDATE_NOT_NULL(isRead)
    *isRead = mType == IContentProviderOperation::TYPE_ASSERT;
    return NOERROR;
}

ECode CContentProviderOperation::Apply(
    /* [in] */ IContentProvider* provider,
    /* [in] */ ArrayOf<IContentProviderResult*>* backRefs,
    /* [in] */ Int32 numBackRefs,
    /* [out] */ IContentProviderResult** providerResult)
{
    VALIDATE_NOT_NULL(providerResult)
    *providerResult = NULL;
    VALIDATE_NOT_NULL(provider)

    ECode ec = NOERROR;
    AutoPtr<IContentValues> values;
    FAIL_RETURN(ResolveValueBackReferences(backRefs, numBackRefs, (IContentValues**)&values))
    AutoPtr<ArrayOf<String> > selectionArgs;
    FAIL_RETURN(ResolveSelectionArgsBackReferences(backRefs, numBackRefs, (ArrayOf<String>**)&selectionArgs))

    if (mType == IContentProviderOperation::TYPE_INSERT) {
        AutoPtr<IUri> newUri;
        FAIL_RETURN(provider->Insert(mUri, values, (IUri**)&newUri))
        if (NULL == newUri) {
            //throw new OperationApplicationException("insert failed");
            return E_OPERATION_APPLICATION_EXCEPTION;
        }
        FAIL_RETURN(CContentProviderResult::New(newUri, providerResult))
        return NOERROR;
    }

    Int32 numRows = 0;

    if (mType == IContentProviderOperation::TYPE_DELETE) {
        FAIL_RETURN(provider->Delete(mUri, mSelection, selectionArgs, &numRows))
    }
    else if (mType == IContentProviderOperation::TYPE_UPDATE) {
        FAIL_RETURN(provider->Update(mUri, values, mSelection, selectionArgs, &numRows))
    }
    else if (mType == IContentProviderOperation::TYPE_ASSERT) {
        // Assert that all rows match expected values
        AutoPtr<ArrayOf<String> > projection;
        if (NULL != values) {
            // Build projection map from expected values
            AutoPtr<ISet> valueSet;
            FAIL_RETURN(values->GetValueSet((ISet**)&valueSet))
            Int32 size;
            valueSet->GetSize(&size);
            projection = ArrayOf<String>::Alloc(size);
            AutoPtr<IIterator> it;
            valueSet->GetIterator((IIterator**)&it);
            Boolean hasNext;
            IMapEntry* entry;
            Int32 i = 0;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                entry = IMapEntry::Probe(obj);
                AutoPtr<IInterface> ko;
                entry->GetKey((IInterface**)&ko);
                projection->Set(i++, Object::ToString(ko));
            }
        }

        AutoPtr<ICursor> cursor;
        String sortOrder;
        ECode ecode = NOERROR;
        String cursorValue, expectedValue;
        Boolean hasNext = FALSE;
        FAIL_RETURN(provider->Query(mUri, projection, mSelection, selectionArgs, sortOrder, (ICursor**)&cursor))
        ecode = cursor->GetCount(&numRows);
        FAIL_GOTO(ecode, _EXIT_)

        if (projection != NULL) {
            while ((cursor->MoveToNext(&hasNext), hasNext)) {
                for (Int32 i = 0; i < projection->GetLength(); ++i) {
                    ecode = cursor->GetString(i, &cursorValue);
                    FAIL_GOTO(ecode, _EXIT_)

                    ecode = values->GetAsString((*projection)[i], &expectedValue);
                    FAIL_GOTO(ecode, _EXIT_)

                    AutoPtr<ICharSequence> charSeq = CoreUtils::Convert(cursorValue);
                    AutoPtr<ICharSequence> charSeq2 = CoreUtils::Convert(expectedValue);

                    if (!TextUtils::Equals(charSeq, charSeq2)) {
                        // Throw exception when expected values don't match
                        String str;
                        ToString(&str);
                        Logger::E(TAG, str);

                        ecode = E_OPERATION_APPLICATION_EXCEPTION;
                        FAIL_GOTO(ecode, _EXIT_)
                        //throw new OperationApplicationException("Found value " + cursorValue
                        //        + " when expected " + expectedValue + " for column "
                        //        + projection[i]);
                    }
                }
            }
        }

_EXIT_:
        FAIL_RETURN(ICloseable::Probe(cursor)->Close())
        FAIL_RETURN(ecode)
    }
    else {
        String str;
        FAIL_RETURN(ToString(&str))
        Logger::E(TAG, str);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (mExpectedCount != 0 && mExpectedCount != numRows) {
        String str;
        FAIL_RETURN(ToString(&str))
        Logger::E(TAG, str);
        // throw new OperationApplicationException("wrong number of rows: " + numRows);
        return E_OPERATION_APPLICATION_EXCEPTION;
    }

    FAIL_RETURN(CContentProviderResult::New(numRows, providerResult))
    return ec;
}

ECode CContentProviderOperation::ResolveValueBackReferences(
    /* [in] */ ArrayOf<IContentProviderResult*>* backRefs,
    /* [in] */ Int32 numBackRefs,
    /* [out] */ IContentValues** contentValues)
{
    VALIDATE_NOT_NULL(contentValues)
    *contentValues = NULL;
    VALIDATE_NOT_NULL(backRefs)

    if (NULL == mValuesBackReferences) {
        *contentValues = mValues;
        REFCOUNT_ADD(*contentValues);
        return NOERROR;
    }

    if (NULL == mValues) {
        FAIL_RETURN(CContentValues::New(contentValues))
    } else {
        FAIL_RETURN(CContentValues::New(mValues, contentValues))
    }

    AutoPtr<ISet> valueSet;
    FAIL_RETURN(mValues->GetValueSet((ISet**)&valueSet))

    if (NULL != valueSet) {
        AutoPtr<IIterator> it;
        valueSet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        IMapEntry* entry;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            entry = IMapEntry::Probe(obj);
            AutoPtr<IInterface> ko;
            entry->GetKey((IInterface**)&ko);
            String key = Object::ToString(ko);

            Int32 backRefIndex;
            FAIL_RETURN(mValuesBackReferences->GetAsInt32(key, &backRefIndex))
            if (0 == backRefIndex) {
                String str;
                FAIL_RETURN(ToString(&str))
                Logger::E(TAG, str);
                // throw new IllegalArgumentException("values backref " + key + " is not an integer");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            Int64 backRefValue;
            FAIL_RETURN(BackRefToValue(backRefs, numBackRefs, backRefIndex, &backRefValue))
            FAIL_RETURN((*contentValues)->Put(key, backRefValue))
        }
    }

    return NOERROR;
}

ECode CContentProviderOperation::ResolveSelectionArgsBackReferences(
    /* [in] */ ArrayOf<IContentProviderResult*>* backRefs,
    /* [in] */ Int32 numBackRefs,
    /* [out, callee] */ ArrayOf<String>** stringArray)
{
    VALIDATE_NOT_NULL(stringArray)
    *stringArray = NULL;
    VALIDATE_NOT_NULL(backRefs)

    if (NULL == mSelectionArgsBackReferences) {
        *stringArray = mSelectionArgs;
        REFCOUNT_ADD(*stringArray);
        return NOERROR;
    }

    AutoPtr<ArrayOf<String> > newArgs = ArrayOf<String>::Alloc(mSelectionArgs->GetLength());
    newArgs->Copy(mSelectionArgs);

    AutoPtr<ISet> entrySet;
    FAIL_RETURN(mSelectionArgsBackReferences->GetEntrySet((ISet**)&entrySet))
    if (entrySet != NULL) {
        AutoPtr<IIterator> it;
        entrySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        IMapEntry* entry;
        Int32 selectionArgIndex, backRefIndex;
        Int64 backRefValue = 0;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            entry = IMapEntry::Probe(obj);
            AutoPtr<IInterface> ko, vo;
            entry->GetKey((IInterface**)&ko);
            entry->GetValue((IInterface**)&vo);
            IInteger32::Probe(ko)->GetValue(&selectionArgIndex);
            IInteger32::Probe(vo)->GetValue(&backRefIndex);
            FAIL_RETURN(BackRefToValue(backRefs, numBackRefs, backRefIndex, &backRefValue))
            (*newArgs)[selectionArgIndex] = StringUtils::ToString(backRefValue);
        }
    }

    *stringArray = newArgs;
    REFCOUNT_ADD(*stringArray);
    return NOERROR;
}

ECode CContentProviderOperation::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String str1("mType: ");
    String strType;
    String strUri("null");
    String strExpectedCount;
    String strYieldAllowed;
    String strValues("null");
    String strValuesBackReferences("null");
    String strSelectionArgsBackReferences("null");
    strType = StringUtils::ToString(mType);

    if (NULL != mUri) {
        strUri = Object::ToString(mUri);
    }

    strExpectedCount = StringUtils::ToString(mExpectedCount);
    strYieldAllowed = mYieldAllowed ? String("true") : String("false");

    if (NULL != mValues) {
        strValues = Object::ToString(mValues);
    }

    if (NULL != mValuesBackReferences) {
        strValuesBackReferences = Object::ToString(mValuesBackReferences);
    }

    if (NULL != mSelectionArgsBackReferences) {
        Boolean isEmpty;
        mSelectionArgsBackReferences->IsEmpty(&isEmpty);
        if (isEmpty) {
            strSelectionArgsBackReferences = String("{}");
        }
        else {
            String tmpStr("{");
            String key;
            String value;

            AutoPtr<ISet> entrySet;
            FAIL_RETURN(mSelectionArgsBackReferences->GetEntrySet((ISet**)&entrySet))
            if (entrySet != NULL) {
                AutoPtr<IIterator> it;
                entrySet->GetIterator((IIterator**)&it);
                Boolean hasNext;
                IMapEntry* entry;
                Int32 selectionArgIndex, backRefIndex;
                Int32 i = 0;
                while (it->HasNext(&hasNext), hasNext) {
                    if (i++ != 0) {
                        tmpStr.Append(", ");
                    }
                    AutoPtr<IInterface> obj;
                    it->GetNext((IInterface**)&obj);
                    entry = IMapEntry::Probe(obj);
                    AutoPtr<IInterface> ko, vo;
                    entry->GetKey((IInterface**)&ko);
                    entry->GetValue((IInterface**)&vo);
                    IInteger32::Probe(ko)->GetValue(&selectionArgIndex);
                    IInteger32::Probe(vo)->GetValue(&backRefIndex);
                    key = StringUtils::ToString(selectionArgIndex);
                    value = StringUtils::ToString(backRefIndex);
                    tmpStr.Append(key);
                    tmpStr.Append("=");
                    tmpStr.Append(value);
                }
            }

            strSelectionArgsBackReferences = tmpStr;
            strSelectionArgsBackReferences.Append("}");
        }
    }

    str1.Append(strType);
    str1.Append(", mUri: ");
    str1.Append(strUri);
    str1.Append(", mSelection: ");

    if (mSelection.IsNull()) {
        str1.Append("null");
    }
    else {
        str1.Append(mSelection);
    }

    str1.Append(", mExpectedCount: ");
    str1.Append(strExpectedCount);
    str1.Append(", mYieldAllowed: ");
    str1.Append(strYieldAllowed);
    str1.Append(", mValues: ");
    str1.Append(strValues);
    str1.Append(", mValuesBackReferences: ");
    str1.Append(strValuesBackReferences);
    str1.Append(", mSelectionArgsBackReferences: ");
    str1.Append(strSelectionArgsBackReferences);
    *result = str1;
    return NOERROR;
}

ECode CContentProviderOperation::BackRefToValue(
    /* [in] */ ArrayOf<IContentProviderResult*>* backRefs,
    /* [in] */ const Int32 numBackRefs,
    /* [in] */ const Int32 backRefIndex,
    /* [out] */ Int64* backRefValue)
{
    VALIDATE_NOT_NULL(backRefValue)
    *backRefValue = 0;
    VALIDATE_NOT_NULL(backRefs)

    if (backRefIndex >= numBackRefs) {
        String str;
        FAIL_RETURN(ToString(&str))
        Logger::E(TAG, str);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    AutoPtr<IContentProviderResult> backRef = (*backRefs)[backRefIndex];
    Int64 tmpBackRefValue = 0;
    AutoPtr<IUri> uri;
    FAIL_RETURN(backRef->GetUri((IUri**)&uri));

    if (NULL != uri) {
        AutoPtr<IContentUris> contentUris;
        FAIL_RETURN(CContentUris::AcquireSingleton((IContentUris**)&contentUris))
        FAIL_RETURN(contentUris->ParseId(uri, &tmpBackRefValue))
    }
    else {
        FAIL_RETURN(backRef->GetCount((Int32*)&tmpBackRefValue))
    }

    *backRefValue = tmpBackRefValue;
    return NOERROR;
}

}
}
}

