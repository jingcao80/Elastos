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

#include "Elastos.Droid.Database.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/search/CSettingsSearchIndexablesProvider.h"
#include "elastos/droid/settings/search/SearchIndexableResources.h"
#include <elastos/core/CoreUtils.h>
#include "../R.h"

using Elastos::Droid::Database::CMatrixCursor;
using Elastos::Droid::Database::IMatrixCursor;
using Elastos::Droid::Provider::ISearchIndexableData;
using Elastos::Droid::Settings::Search::SearchIndexableResources;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISearchIndexablesContract;
using Elastos::Droid::Provider::CSearchIndexablesContract;
using Elastos::Core::CoreUtils;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

const String CSettingsSearchIndexablesProvider::TAG("CSettingsSearchIndexablesProvider");

CAR_OBJECT_IMPL(CSettingsSearchIndexablesProvider)

CSettingsSearchIndexablesProvider::CSettingsSearchIndexablesProvider()
{}

CSettingsSearchIndexablesProvider::~CSettingsSearchIndexablesProvider()
{}

ECode CSettingsSearchIndexablesProvider::constructor()
{
    return SearchIndexablesProvider::constructor();
}

ECode CSettingsSearchIndexablesProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode CSettingsSearchIndexablesProvider::QueryXmlResources(
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISearchIndexablesContract> sic;
    CSearchIndexablesContract::AcquireSingleton((ISearchIndexablesContract**)&sic);
    AutoPtr< ArrayOf<String> > INDEXABLES_XML_RES_COLUMNS;
    sic->GetINDEXABLES_XML_RES_COLUMNS((ArrayOf<String>**)&INDEXABLES_XML_RES_COLUMNS);


    AutoPtr<IMatrixCursor> cursor;
    CMatrixCursor::New(INDEXABLES_XML_RES_COLUMNS, (IMatrixCursor**)&cursor);

    AutoPtr<ICollection> values = SearchIndexableResources::GetValues();
    AutoPtr<IIterator> iterator;
    values->GetIterator((IIterator**)&iterator);
    Boolean res;
    while (iterator->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        ISearchIndexableResource* val = ISearchIndexableResource::Probe(obj);

        Int32 xmlResId;
        val->GetXmlResId(&xmlResId);

        ISearchIndexableData* _val = ISearchIndexableData::Probe(val);
        Int32 rank, iconResId;
        _val->GetRank(&rank);
        _val->GetIconResId(&iconResId);
        String className;
        _val->GetClassName(&className);

        AutoPtr< ArrayOf<IInterface*> > ref = ArrayOf<IInterface*>::Alloc(7);
        ref->Set(ISearchIndexablesContract::COLUMN_INDEX_XML_RES_RANK, CoreUtils::Convert(rank));
        ref->Set(ISearchIndexablesContract::COLUMN_INDEX_XML_RES_RESID, CoreUtils::Convert(xmlResId));
        ref->Set(ISearchIndexablesContract::COLUMN_INDEX_XML_RES_CLASS_NAME, CoreUtils::Convert(className));
        ref->Set(ISearchIndexablesContract::COLUMN_INDEX_XML_RES_ICON_RESID, CoreUtils::Convert(iconResId));
        ref->Set(ISearchIndexablesContract::COLUMN_INDEX_XML_RES_INTENT_ACTION, NULL); // intent action
        ref->Set(ISearchIndexablesContract::COLUMN_INDEX_XML_RES_INTENT_TARGET_PACKAGE, NULL); // intent target package
        ref->Set(ISearchIndexablesContract::COLUMN_INDEX_XML_RES_INTENT_TARGET_CLASS, NULL); // intent target class

        cursor->AddRow(*ref);
    }
    *result = ICursor::Probe(cursor);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSettingsSearchIndexablesProvider::QueryRawData(
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISearchIndexablesContract> sic;
    CSearchIndexablesContract::AcquireSingleton((ISearchIndexablesContract**)&sic);
    AutoPtr< ArrayOf<String> > INDEXABLES_RAW_COLUMNS;
    sic->GetINDEXABLES_RAW_COLUMNS((ArrayOf<String>**)&INDEXABLES_RAW_COLUMNS);

    AutoPtr<IMatrixCursor> cursor;
    CMatrixCursor::New(INDEXABLES_RAW_COLUMNS, (IMatrixCursor**)&cursor);

    *result = ICursor::Probe(cursor);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSettingsSearchIndexablesProvider::QueryNonIndexableKeys(
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISearchIndexablesContract> sic;
    CSearchIndexablesContract::AcquireSingleton((ISearchIndexablesContract**)&sic);
    AutoPtr< ArrayOf<String> > NON_INDEXABLES_KEYS_COLUMNS;
    sic->GetNON_INDEXABLES_KEYS_COLUMNS((ArrayOf<String>**)&NON_INDEXABLES_KEYS_COLUMNS);

    AutoPtr<IMatrixCursor> cursor;
    CMatrixCursor::New(NON_INDEXABLES_KEYS_COLUMNS, (IMatrixCursor**)&cursor);

    *result = ICursor::Probe(cursor);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos
