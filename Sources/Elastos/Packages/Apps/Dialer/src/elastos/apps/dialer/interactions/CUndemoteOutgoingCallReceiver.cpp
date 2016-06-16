
#include "interactions/CUndemoteOutgoingCallReceiver.h"

using Elastos::Droid::Provider::IContactsContractPhoneLookup;
using Elastos::Droid::Provider::CContactsContractPhoneLookup;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Interactions {

//=================================================================
// CUndemoteOutgoingCallReceiver::MyThread
//=================================================================
CUndemoteOutgoingCallReceiver::MyThread::MyThread(
    /* [in] */ CUndemoteOutgoingCallReceiver* host)
    : mHost(host)
{}

ECode CUndemoteOutgoingCallReceiver::Run()
{
    Int64 id = mHost->GetContactIdFromPhoneNumber(context, number);
    if (id != NO_CONTACT_FOUND) {
        mHost->undemoteContactWithId(context, id);
    }

    return NOERROR;
}

//=================================================================
// CUndemoteOutgoingCallReceiver
//=================================================================
const Int64 CUndemoteOutgoingCallReceiver::NO_CONTACT_FOUND = -1;

CAR_INTERFACE_IMPL(CUndemoteOutgoingCallReceiver, BroadcastReceiver, ICUndemoteOutgoingCallReceiver);

CAR_OBJECT_IMPL(CUndemoteOutgoingCallReceiver)

ECode CUndemoteOutgoingCallReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    if (intent != NULL &&
            intent->GetAction(&action), IIntent::ACTION_NEW_OUTGOING_CALL.Equals(action)) {
        String number;
        intent->GetStringExtra(IIntent::EXTRA_PHONE_NUMBER, &number);
        if (TextUtils::IsEmpty(number)) {
            return NOERROR;
        }

        AutoPtr<MyThread> thread = new MyThread(this);
        thread->Start();
    }

    return NOERROR;
}

void CUndemoteOutgoingCallReceiver::UndemoteContactWithId(
    /* [in] */ IContext* context,
    /* [in] */ Int64 id)
{
    // If the contact is not demoted, this will not do anything. Otherwise, it will
    // restore it to an unpinned position. If it was a frequently called contact, it will
    // show up once again show up on the favorites screen.
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    assert(0 && "TODO");
    // PinnedPositions.undemote(resolver, id);
}

Int64 CUndemoteOutgoingCallReceiver::GetContactIdFromPhoneNumber(
    /* [in] */ IContext* context,
    /* [in] */ const String& number)
{
    AutoPtr<IContactsContractPhoneLookup> lookup;
    CContactsContractPhoneLookup::AcquireSingleton((IContactsContractPhoneLookup**)&lookup);
    AutoPtr<IUri> filterUri;
    lookup->GetCONTENT_FILTER_URI((IUri**)&filterUri);

    String encodeStr;
    Uri::Encode(number, &encodeStr);
    AutoPtr<IUri> contactUri;
    Uri.withAppendedPath(filterUri, encodeStr, (IUri**)&contactUri);

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    args->Set(0, PhoneLookup._ID);
    resolver->Query(contactUri, args,
            String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    if (cursor == NULL) {
        return NO_CONTACT_FOUND;
    }
    // try {
    Boolean succeeded;
    if (cursor->MoveToFirst(&succeeded), succeeded) {
        Int64 id;
        cursor->GetInt64(0, &id);
        cursor->Close();
        return id;
    }
    else {
        cursor->Close();
        return NO_CONTACT_FOUND;
    }
    // } finally {
    //     cursor.close();
    // }
}

} // Interactions
} // Dialer
} // Apps
} // Elastos
