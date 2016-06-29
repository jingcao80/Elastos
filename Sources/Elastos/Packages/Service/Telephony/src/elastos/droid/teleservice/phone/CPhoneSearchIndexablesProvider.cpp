
#include "elastos/droid/teleservice/phone/CPhoneSearchIndexablesProvider.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::Database::IMatrixCursor;
using Elastos::Droid::Database::CMatrixCursor;
using Elastos::Droid::Provider::ISearchIndexablesContract;
using Elastos::Droid::Provider::CSearchIndexablesContract;
using Elastos::Droid::Provider::ISearchIndexableData;
using Elastos::Droid::Provider::CSearchIndexableResource;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

const String CPhoneSearchIndexablesProvider::TAG("PhoneSearchIndexablesProvider");

static AutoPtr<ArrayOf<ISearchIndexableResource*> > initINDEXABLE_RES()
{
    AutoPtr<ArrayOf<ISearchIndexableResource*> > array = ArrayOf<ISearchIndexableResource*>::Alloc(1);

    AutoPtr<ISearchIndexableResource> resource;
    CSearchIndexableResource::New(1, R::xml::network_setting,
        /*MobileNetworkSettings.class.getName()*/String("Elastos.Droid.TeleService.Phone.CMobileNetworkSettings"),
        Elastos::Droid::TeleService::R::mipmap::ic_launcher_phone, (ISearchIndexableResource**)&resource);
    array->Set(0, resource);
    return array;
}

AutoPtr<ArrayOf<ISearchIndexableResource*> > CPhoneSearchIndexablesProvider::INDEXABLE_RES = initINDEXABLE_RES();

CAR_OBJECT_IMPL(CPhoneSearchIndexablesProvider)

ECode CPhoneSearchIndexablesProvider::constructor()
{
    return SearchIndexablesProvider::constructor();
}

ECode CPhoneSearchIndexablesProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = TRUE;
    return NOERROR;
}

ECode CPhoneSearchIndexablesProvider::QueryXmlResources(
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** outcursor)
{
    VALIDATE_NOT_NULL(outcursor);

    AutoPtr<ISearchIndexablesContract> helper;
    CSearchIndexablesContract::AcquireSingleton((ISearchIndexablesContract**)&helper);
    AutoPtr<ArrayOf<String> > resColumns;
    helper->GetINDEXABLES_XML_RES_COLUMNS((ArrayOf<String>**)&resColumns);
    AutoPtr<IMatrixCursor> cursor;
    CMatrixCursor::New(resColumns, (IMatrixCursor**)&cursor);
    Int32 count = INDEXABLE_RES->GetLength();
    for (Int32 n = 0; n < count; n++) {
        AutoPtr<ArrayOf<IInterface*> > ref = ArrayOf<IInterface*>::Alloc(7);

        Int32 rank;
        ISearchIndexableData::Probe((*INDEXABLE_RES)[n])->GetRank(&rank);
        AutoPtr<IInteger32> value = CoreUtils::Convert(rank);
        ref->Set(ISearchIndexablesContract::COLUMN_INDEX_XML_RES_RANK, TO_IINTERFACE(value));

        Int32 xmlResId;
        (*INDEXABLE_RES)[n]->GetXmlResId(&xmlResId);
        AutoPtr<IInteger32> value2 = CoreUtils::Convert(xmlResId);
        ref->Set(ISearchIndexablesContract::COLUMN_INDEX_XML_RES_RESID, TO_IINTERFACE(value2));

        ref->Set(ISearchIndexablesContract::COLUMN_INDEX_XML_RES_CLASS_NAME, NULL);

        Int32 iconResId;
        ISearchIndexableData::Probe((*INDEXABLE_RES)[n])->GetIconResId(&iconResId);
        AutoPtr<IInteger32> value3 = CoreUtils::Convert(iconResId);
        ref->Set(ISearchIndexablesContract::COLUMN_INDEX_XML_RES_ICON_RESID, TO_IINTERFACE(value3));

        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(String("android.intent.action.MAIN"));
        ref->Set(ISearchIndexablesContract::COLUMN_INDEX_XML_RES_INTENT_ACTION, TO_IINTERFACE(cchar));
        AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(String("com.android.phone"));
        ref->Set(ISearchIndexablesContract::COLUMN_INDEX_XML_RES_INTENT_TARGET_PACKAGE, TO_IINTERFACE(cchar2));

        String className;
        ISearchIndexableData::Probe((*INDEXABLE_RES)[n])->GetClassName(&className);
        AutoPtr<ICharSequence> cchar3 = CoreUtils::Convert(className);
        ref->Set(ISearchIndexablesContract::COLUMN_INDEX_XML_RES_INTENT_TARGET_CLASS, TO_IINTERFACE(cchar3));
        cursor->AddRow(*ref.Get());
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

    AutoPtr<ISearchIndexablesContract> helper;
    CSearchIndexablesContract::AcquireSingleton((ISearchIndexablesContract**)&helper);
    AutoPtr<ArrayOf<String> > rawColumns;
    helper->GetINDEXABLES_RAW_COLUMNS((ArrayOf<String>**)&rawColumns);
    AutoPtr<IMatrixCursor> cursor;
    CMatrixCursor::New(rawColumns, (IMatrixCursor**)&cursor);
    *outcursor = ICursor::Probe(cursor);
    REFCOUNT_ADD(*outcursor);
    return NOERROR;
}

ECode CPhoneSearchIndexablesProvider::QueryNonIndexableKeys(
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** outcursor)
{
    VALIDATE_NOT_NULL(outcursor);

    AutoPtr<ISearchIndexablesContract> helper;
    CSearchIndexablesContract::AcquireSingleton((ISearchIndexablesContract**)&helper);
    AutoPtr<ArrayOf<String> > nonColumns;
    helper->GetNON_INDEXABLES_KEYS_COLUMNS((ArrayOf<String>**)&nonColumns);
    AutoPtr<IMatrixCursor> cursor;
    CMatrixCursor::New(nonColumns, (IMatrixCursor**)&cursor);
    *outcursor = ICursor::Probe(cursor);
    REFCOUNT_ADD(*outcursor);
    return NOERROR;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos