
#include <ext/frameworkdef.h>
#include "CActivityOne.h"
#include "R.h"
#include <elautoptr.h>

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace ContentProviderDemo {

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    PRINT_FILE_LINE;

    AutoPtr<IContentResolver> resolver;
    ECode ec = GetContentResolver((IContentResolver**)&resolver);
    PRINT_FILE_LINE_EX("ec: %d, resolver: %p", ec, resolver.Get());
    FAIL_RETURN(ec);

    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> uri;
    ec = uriHelper->Parse(String("content://CContentProviderOne"), (IUri**)&uri);
    PRINT_FILE_LINE_EX("ec: %d, uri: %p", ec, uri.Get());

    AutoPtr<ICursor> cursor;
    ec = resolver->Query(uri, NULL, String(""), NULL, String(""), (ICursor**)&cursor);
    PRINT_FILE_LINE_EX("ec: %d, cursor: %p", ec, cursor.Get());

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
    return NOERROR;
}

// ECode CActivityOne::OnClick(
//     /* [in] */ Handle32 serverHandle)
// {
//     AutoPtr<IContentResolver> resolver;
//     ECode ec = GetContentResolver((IContentResolver**)&resolver);
//     if (FAILED(ec)) {
//         return ec;
//     }
//
//     AutoPtr<IUri> uri;
//     AutoPtr<ICursor> cursor;
//     String type;
//     ec = CUri::Parse("content://CContentProviderOne", (IUri**)&uri);
//     if (FAILED(ec)) {
//         return ec;
//     }
//
//     ArrayOf_<String, 0> arr;
//     ec = resolver->Query((IUri*)uri, arr, "", arr, "", (ICursor**)&cursor);
//     if (FAILED(ec)) {
//         return ec;
//     }
//
//     return NOERROR;
// }

// ECode CActivityOne::OnClick2(
//     /* [in] */ Handle32 serverHandle)
// {
//     AutoPtr<IIntent> intent;
//     CIntent::New((IIntent**)&intent);
//     intent->SetAction(String("com.kortide.userevents.ALARM"));
//     return SendBroadcast(intent);
// }

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    AutoPtr<IUri> uri;
    String uriString;
    data->GetData((IUri**)&uri);
    uri->ToString(&uriString);
    return NOERROR;
}

} // namespace ContentProviderDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
