
#include "elastos/droid/contacts/common/list/ProfileAndContactsLoader.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::CMatrixCursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Provider::IContactsContractProfile;
using Elastos::Droid::Provider::CContactsContractProfile;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

//=================================================================
// ProfileAndContactsLoader::MyMergeCursor
//=================================================================
ECode ProfileAndContactsLoader::MyMergeCursor::constructor(
    /* [in] */ ArrayOf<ICursor*>* cursors)
{
    return MergeCursor::constructor(cursors);
}

ECode ProfileAndContactsLoader::MyMergeCursor::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    // Need to get the extras from the contacts cursor.
    return mContactsCursor == NULL ? CBundle::New(extras) : mContactsCursor->GetExtras(extras);
}


//=================================================================
// ProfileAndContactsLoader
//=================================================================
ProfileAndContactsLoader::ProfileAndContactsLoader()
    : mLoadProfile(FALSE)
{}

ECode ProfileAndContactsLoader::constructor(
    /* [in] */ IContext* context)
{
    return CursorLoader::constructor(context);
}

void ProfileAndContactsLoader::SetLoadProfile(
    /* [in] */ Boolean flag)
{
    mLoadProfile = flag;
}

ECode ProfileAndContactsLoader::SetProjection(
    /* [in] */ ArrayOf<String>* projection)
{
    CursorLoader::SetProjection(projection);
    mProjection = projection;
    return NOERROR;
}

ECode ProfileAndContactsLoader::LoadInBackground(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // First load the profile, if enabled.
    Elastos::Utility::Etl::List<AutoPtr<ICursor> > cursors;
    if (mLoadProfile) {
        cursors.PushBack(ICursor::Probe(LoadProfile()));
    }
    // ContactsCursor.loadInBackground() can return null; MergeCursor
    // correctly handles null cursors.
    AutoPtr<ICursor> cursor;
    // try {
    AutoPtr<IInterface> temp;
    CursorLoader::LoadInBackground((IInterface**)&temp);
    cursor = ICursor::Probe(temp);
    // } catch (NullPointerException e) {
    //     // Ignore NPEs thrown by providers
    // }
    AutoPtr<ICursor> contactsCursor = cursor;
    cursors.PushBack(contactsCursor);
    AutoPtr<ArrayOf<ICursor*> > cursorsArray = ArrayOf<ICursor*>::Alloc(cursors.GetSize());
    Elastos::Utility::Etl::List<AutoPtr<ICursor> >::Iterator it = cursors.Begin();
    for (Int32 i = 0; it != cursors.End(); ++it, ++i) {
        cursorsArray->Set(i, *it);
    }
    AutoPtr<MyMergeCursor> mergeCursor = new MyMergeCursor(contactsCursor);
    mergeCursor->constructor(cursorsArray);
    *result = TO_IINTERFACE(mergeCursor);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IMatrixCursor> ProfileAndContactsLoader::LoadProfile()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IContactsContractProfile> profile;
    CContactsContractProfile::AcquireSingleton((IContactsContractProfile**)&profile);
    AutoPtr<IUri> uri;
    profile->GetCONTENT_URI((IUri**)&uri);
    AutoPtr<ICursor> cursor;
    cr->Query(uri, mProjection, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    if (cursor == NULL) {
        return NULL;
    }
    // try {
    AutoPtr<IMatrixCursor> matrix;
    CMatrixCursor::New(mProjection, (IMatrixCursor**)&matrix);
    AutoPtr<ArrayOf<IInterface*> > row = ArrayOf<IInterface*>::Alloc(mProjection->GetLength());
    Boolean result;
    while (cursor->MoveToNext(&result), result) {
        for (Int32 i = 0; i < row->GetLength(); i++) {
            String str;
            cursor->GetString(i, &str);
            AutoPtr<ICharSequence> cs;
            CString::New(str, (ICharSequence**)&cs);
            row->Set(i, cs);
        }
        matrix->AddRow(*row);
    }
    ICloseable::Probe(cursor)->Close();
    return matrix;
    // } finally {
    //     cursor.close();
    // }
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
