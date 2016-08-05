#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Google.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/google/mms/utility/SqliteWrapper.h"
#include "elastos/droid/app/CActivityManagerMemoryInfo.h"
#include "elastos/droid/widget/CToastHelper.h"
#include "elastos/droid/R.h"

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::CActivityManagerMemoryInfo;
using Elastos::Droid::App::IActivityManagerMemoryInfo;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

//=====================================================================
//                            SqliteWrapper
//=====================================================================
const String SqliteWrapper::TAG("SqliteWrapper");
const String SqliteWrapper::SQLITE_EXCEPTION_DETAIL_MESSAGE("unable to open database file");

ECode SqliteWrapper::CheckSQLiteException(
    /* [in] */ IContext* context)
    ///* [in] */ ISQLiteException* e)
{
    // if (IsLowMemory(e)) {
        AutoPtr<IToastHelper> thlp;
        CToastHelper::AcquireSingleton((IToastHelper**)&thlp);
        AutoPtr<IToast> t;
        thlp->MakeText(context, R::string::low_memory,
                IToast::LENGTH_SHORT, (IToast**)&t);
        t->Show();
    // }
    // else {
    //     // throw e;
    // }
    return NOERROR;
}

AutoPtr<ICursor> SqliteWrapper::Query(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder)
{
    // try {
        AutoPtr<ICursor> c;
        resolver->Query(uri, projection, selection, selectionArgs, sortOrder, (ICursor**)&c);
        return c;
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Catch a SQLiteException when query: ", e);
    //     checkSQLiteException(context, e);
    //     return null;
    // }
}

Boolean SqliteWrapper::Requery(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor)
{
    // try {
        Boolean res = FALSE;
        cursor->Requery(&res);
        return res;
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Catch a SQLiteException when requery: ", e);
    //     checkSQLiteException(context, e);
    //     return false;
    // }
}

Int32 SqliteWrapper::Update(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    // try {
        Int32 res = 0;
        resolver->Update(uri, values, where, selectionArgs, &res);
        return res;
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Catch a SQLiteException when update: ", e);
    //     checkSQLiteException(context, e);
    //     return -1;
    // }
}

Int32 SqliteWrapper::Delete(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    // try {
        Int32 res = 0;
        resolver->Delete(uri, where, selectionArgs, &res);
        return res;
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Catch a SQLiteException when delete: ", e);
    //     checkSQLiteException(context, e);
    //     return -1;
    // }
}

AutoPtr<IUri> SqliteWrapper::Insert(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values)
{
    // try {
        AutoPtr<IUri> res;
        resolver->Insert(uri, values, (IUri**)&res);
        return res;
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Catch a SQLiteException when insert: ", e);
    //     checkSQLiteException(context, e);
    //     return null;
    // }
}

SqliteWrapper::SqliteWrapper()
{
    // Forbidden being instantiated.
}

Boolean SqliteWrapper::IsLowMemory(
    /* [in] */ IContext* context)
{
    if (NULL == context) {
        return FALSE;
    }

    AutoPtr<IInterface> serv;
    context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&serv);
    AutoPtr<IActivityManager> am = IActivityManager::Probe(serv);
    AutoPtr<IActivityManagerMemoryInfo> outInfo;
    CActivityManagerMemoryInfo::New((IActivityManagerMemoryInfo**)&outInfo);
    assert(0 && "TODO");
    // am->GetMemoryInfo(outInfo);

    Boolean bLowMemory = FALSE;
    outInfo->GetLowMemory(&bLowMemory);
    return bLowMemory;
}

Boolean SqliteWrapper::IsLowMemory()
    ///* [in] */ ISQLiteException* e)
{
    assert(0 && "TODO");
    // return e->GetMessage().Equals(SQLITE_EXCEPTION_DETAIL_MESSAGE);
    return FALSE;
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
