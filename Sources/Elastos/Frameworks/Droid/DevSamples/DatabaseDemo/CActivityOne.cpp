
#include "CActivityOne.h"
#include "R.h"
//#include "Uri.h"
#include <elautoptr.h>
#include <stdio.h>
#include <elastos/utility/logging/Slogger.h>
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::App::IIActivityManager;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace DatabaseDemo {

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host) :
    mHost(host),
    mRef(0)
{
}

PInterface CActivityOne::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return this;
    }

    return NULL;
}

UInt32 CActivityOne::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    return NOERROR;
}

ECode CActivityOne::SetUpViews()
{
    mBooksDB = new BooksDB(this);
    return NOERROR;
}

ECode CActivityOne::Add()
{
    mBooksDB->Insert(String("snow"), String("123"));
    mBooksDB->Insert(String("moon"), String("456"));
    return NOERROR;
}


ECode CActivityOne::Find()
{
    Int32 id, b_id;
    String name,author;
    AutoPtr<ICursor> cursor = mBooksDB->Select();
    Boolean success;

    while(cursor->MoveToNext(&success),success) {
        cursor->GetColumnIndex(String("book_id"), &id);
        cursor->GetInt32(id, &b_id);
        cursor->GetColumnIndex(String("book_name"), &id);
        cursor->GetString(id, &name);
        cursor->GetColumnIndex(String("book_author"), &id);
        cursor->GetString(id, &author);
        Slogger::D("DatabaseDemo", "==== File: %s, Line: %d ,id: %d ,name:%s ,author:%s ====\n", __FILE__, __LINE__,b_id,name.string(),author.string());
    }
    return NOERROR;
}

ECode CActivityOne::Delete()
{
    if (BOOK_ID == 0) {
        return NOERROR;
    }
    mBooksDB->Delete(BOOK_ID);

    return NOERROR;
}

ECode CActivityOne::Updata()
{
    if (BOOK_ID == 0) {
        return NOERROR;
    }
    mBooksDB->Update(BOOK_ID, String("snowmoon"), String("123456"));
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    BOOK_ID = 0;
    printf("==== File: %s, Line: %d ====$$$$$$$$$ Begine $$$$$$$$$$\n", __FILE__, __LINE__);
    SetUpViews();
    printf("==== File: %s, Line: %d ====$$$$$$$$$ Insert $$$$$$$$$$\n", __FILE__, __LINE__);
    Add();
    printf("==== File: %s, Line: %d ====$$$$$$$$$ Select * $$$$$$$$$$\n", __FILE__, __LINE__);
    Find();
    printf("==== File: %s, Line: %d ====$$$$$$$$$ Delete id = 1 record $$$$$$$$$$\n", __FILE__, __LINE__);
    BOOK_ID = 1;
    Delete();
    printf("==== File: %s, Line: %d ====$$$$$$$$$ Select * After Delete $$$$$$$$$$\n", __FILE__, __LINE__);
    Find();
    printf("==== File: %s, Line: %d ====$$$$$$$$$ Updata id = 2 record $$$$$$$$$$\n", __FILE__, __LINE__);
    BOOK_ID = 2;
    Updata();
    printf("==== File: %s, Line: %d ====$$$$$$$$$ Select * After Updata $$$$$$$$$$\n", __FILE__, __LINE__);
    Find();
    printf("==== File: %s, Line: %d ====$$$$$$$$$ End $$$$$$$$$$\n", __FILE__, __LINE__);

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

} // namespace DatabaseDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
