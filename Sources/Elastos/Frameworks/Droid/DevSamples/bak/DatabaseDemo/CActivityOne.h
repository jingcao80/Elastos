
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"
#include "BooksDB.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace DatabaseDemo {

class CActivityOne : public Activity
{
public:
    class MyListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        AutoPtr<CActivityOne> mHost;
        Int32 mRef;
    };

    CARAPI SetUpViews();

    CARAPI Add();

    CARAPI Find();

    CARAPI Delete();

    CARAPI Updata();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

protected:
    /*const static Int32 MENU_ADD = Menu.FIRST;
    const static Int32 MENU_DELETE = Menu.FIRST + 1;
    const static Int32 MENU_UPDATE = Menu.FIRST + 2;*/
private:
    BooksDB* mBooksDB;
    AutoPtr<ICursor> mCursor;
    /*EditText BookName;
    EditText BookAuthor;
    ListView BooksList;*/

    Int32 BOOK_ID;
};

} // namespace DatabaseDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
