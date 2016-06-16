
#include "elastos/droid/phone/CPhoneSearchIndexablesProvider.h"

namespace Elastos {
namespace Droid {
namespace Phone {

const String CPhoneSearchIndexablesProvider::TAG("PhoneSearchIndexablesProvider");

static AutoPtr<ArrayOf<ISearchIndexableResource*> > initINDEXABLE_RES()
{
    AutoPtr<ArrayOf<ISearchIndexableResource*> > array = ArrayOf<ISearchIndexableResource*>::Alloc(1);

    AutoPtr<ISearchIndexableResource> resource;
    CSearchIndexableResource::New(1, R.xml.network_setting,
            MobileNetworkSettings.class.getName(),
            R.mipmap.ic_launcher_phone, (ISearchIndexableResource**)&resource);
    array->Set(0, resource);
    return array;
}

AutoPtr<ArrayOf<ISearchIndexableResource*> > CPhoneSearchIndexablesProvider::INDEXABLE_RES = initINDEXABLE_RES();

CAR_INTERFACE_IMPL(CPhoneSearchIndexablesProvider, SearchIndexablesProvider, IPhoneSearchIndexablesProvider)

CAR_OBJECT_IMPL(CPhoneSearchIndexablesProvider)

ECode CPhoneSearchIndexablesProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return true;
}

ECode CPhoneSearchIndexablesProvider::QueryXmlResources(
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** outcursor)
{
    VALIDATE_NOT_NULL(outcursor);

    AutoPtr<IMatrixCursor> cursor;
    CMatrixCursor::New(INDEXABLES_XML_RES_COLUMNS, (IMatrixCursor**)&cursor);
    Int32 count = INDEXABLE_RES->GetLength();
    for (Int32 n = 0; n < count; n++) {
        AutoPtr<ArrayOf<IInterface*> > ref = ArrayOf<IInterface*>::Alloc(7);
        ref->Set(COLUMN_INDEX_XML_RES_RANK, INDEXABLE_RES[n].rank);
        ref->Set(COLUMN_INDEX_XML_RES_RESID, INDEXABLE_RES[n].xmlResId);
        ref->Set(COLUMN_INDEX_XML_RES_CLASS_NAME, null);
        ref->Set(COLUMN_INDEX_XML_RES_ICON_RESID, INDEXABLE_RES[n].iconResId);
        AutoPtr<ICharSequence> cchar = CoreUtil::Convert(String("android.intent.action.MAIN"));
        ref->Set(COLUMN_INDEX_XML_RES_INTENT_ACTION, TO_IINTERFACE(cchar));
        AutoPtr<ICharSequence> cchar2 = CoreUtil::Convert(String("com.android.phone"));
        ref->Set(COLUMN_INDEX_XML_RES_INTENT_TARGET_PACKAGE, TO_IINTERFACE(cchar2));
        ref->Set(COLUMN_INDEX_XML_RES_INTENT_TARGET_CLASS, INDEXABLE_RES[n].className);
        cursor->AddRow(ref);
    }
    *outcursor = ICursor::Probe(cursor);
    REFCOUNT_ADD(*outcursor);
    return NOERROR;
}

ECode CPhoneSearchIndexablesProvider::QueryRawData(
   /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** outcursor)
{
    VALIDATE_NOT_NULL(outcursor);

    AutoPtr<IMatrixCursor> cursor;
    CMatrixCursor::New(INDEXABLES_RAW_COLUMNS, (IMatrixCursor**)&cursor);
    *outcursor = ICursor::Probe(cursor);
    REFCOUNT_ADD(*outcursor);
    return NOERROR;
}

ECode CPhoneSearchIndexablesProvider::QueryNonIndexableKeys(
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** outcursor)
{
    VALIDATE_NOT_NULL(outcursor);

    AutoPtr<IMatrixCursor> cursor;
    CMatrixCursor::New(NON_INDEXABLES_KEYS_COLUMNS, (IMatrixCursor**)&cursor);
    *outcursor = ICursor::Probe(cursor);
    REFCOUNT_ADD(*outcursor);
    return NOERROR;
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos