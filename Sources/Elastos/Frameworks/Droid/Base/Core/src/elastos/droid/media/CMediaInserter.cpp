
#include "elastos/droid/media/CMediaInserter.h"
#include "elastos/droid/content/CContentValues.h"

using Elastos::Droid::Content::CContentValues;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaInserter, Object, IMediaInserter)

CAR_OBJECT_IMPL(CMediaInserter)

CMediaInserter::CMediaInserter()
    : mBufferSizePerUri(0)
{
    CHashMap::New((IHashMap**)&mRowMap);
    CHashMap::New((IHashMap**)&mPriorityRowMap);
}

CMediaInserter::~CMediaInserter()
{
    mRowMap->Clear();
    mPriorityRowMap->Clear();
}

ECode CMediaInserter::constructor()
{
    return NOERROR;
}

ECode CMediaInserter::constructor(
    /* [in] */ IIContentProvider* provider,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 bufferSizePerUri)
{
    mProvider = provider;
    mPackageName = packageName;
    mBufferSizePerUri = bufferSizePerUri;
    return NOERROR;
}

ECode CMediaInserter::Insert(
    /* [in] */ IUri* tableUri,
    /* [in] */ IContentValues* values)
{
    return Insert(tableUri, values, FALSE);
}

ECode CMediaInserter::InsertwithPriority(
    /* [in] */ IUri* tableUri,
    /* [in] */ IContentValues* values)
{
    return Insert(tableUri, values, TRUE);
}

ECode CMediaInserter::Insert(
    /* [in] */ IUri* tableUri,
    /* [in] */ IContentValues* values,
    /* [in] */ Boolean priority)
{
    VALIDATE_NOT_NULL(tableUri);

    AutoPtr<IHashMap> rowmap = priority ? mPriorityRowMap : mRowMap;
    AutoPtr<IInterface> obj;
    rowmap->Get(tableUri, (IInterface**)&obj);
    AutoPtr<IArrayList> list;
    if (list == NULL) {
        CArrayList::New((IArrayList**)&list);
        rowmap->Put(tableUri, list);
    }

    AutoPtr<IContentValues> contentValues;
    CContentValues::New(values, (IContentValues**)&contentValues);
    list->Add(contentValues);
    Int32 size;
    list->GetSize(&size);
    if (size >= mBufferSizePerUri) {
        FlushAllPriority();
        Flush(tableUri, IList::Probe(list));
    }
    return NOERROR;
}

ECode CMediaInserter::FlushAll()
{
    FlushAllPriority();
    AutoPtr<ISet> set;
    mRowMap->GetKeySet((ISet**)&set);
    AutoPtr<ArrayOf<IInterface*> > array;
    set->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IUri> tableUri = IUri::Probe((*array)[i]);
        AutoPtr<IInterface> obj;
        mRowMap->Get(tableUri, (IInterface**)&obj);
        Flush(tableUri, IList::Probe(obj));
    }
    mRowMap->Clear();
    return NOERROR;
}

ECode CMediaInserter::FlushAllPriority() // throws RemoteException
{
    AutoPtr<ISet> set;
    mPriorityRowMap->GetKeySet((ISet**)&set);
    AutoPtr<ArrayOf<IInterface*> > array;
    set->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IUri> tableUri = IUri::Probe((*array)[i]);
        AutoPtr<IInterface> obj;
        mPriorityRowMap->Get(tableUri, (IInterface**)&obj);
        Flush(tableUri, IList::Probe(obj));
    }
    mPriorityRowMap->Clear();
    return NOERROR;
}

ECode CMediaInserter::Flush(
    /* [in] */ IUri* tableUri,
    /* [in] */ IList* list)
{
    if (list != NULL) {
        AutoPtr<ArrayOf<IInterface*> > array;
        list->ToArray((ArrayOf<IInterface*>**)&array);
        AutoPtr<ArrayOf<IContentValues*> > valuesArray =
                ArrayOf<IContentValues*>::Alloc(array->GetLength());
        for (Int32 i = 0; i < array->GetLength(); i++) {
            valuesArray->Set(i, IContentValues::Probe((*array)[i]));
        }

        Int32 result;
        mProvider->BulkInsert(mPackageName, tableUri, valuesArray, &result);
        list->Clear();
    }
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
