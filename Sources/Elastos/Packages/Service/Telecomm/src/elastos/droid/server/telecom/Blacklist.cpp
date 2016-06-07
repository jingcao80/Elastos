
#include "elastos/droid/server/telecom/Blacklist.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Provider.h>
#include <elastos/core/Object.h>
#include <elastos/droid/net/Uri.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Provider::CTelephonyBlacklist;
using Elastos::Droid::Provider::ITelephonyBlacklist;
using Elastos::Core::IInteger32;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IObjectInputStream;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {
//=============================================================================
// Blacklist::PhoneNumber
//=============================================================================
// CAR_INTERFACE_IMPL(Blacklist::PhoneNumber, Object, IExternalizable)
const Int64 Blacklist::PhoneNumber::serialVersionUID = 32847013274L;

ECode Blacklist::PhoneNumber::constructor()
{
    return NOERROR;
}

ECode Blacklist::PhoneNumber::WriteExternal(
    /* [in] */ IObjectOutput* out)
{
    return NOERROR;
}

ECode Blacklist::PhoneNumber::ReadExternal(
    /* [in] */ IObjectInput* in)
{
    AutoPtr<IInterface> obj;
    in->ReadObject((IInterface**)&obj);
    IObject::Probe(obj)->ToString(&mPhone);
    return NOERROR;
}

ECode Blacklist::PhoneNumber::HashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPhone != NULL ? mPhone.GetHashCode() : 0;
    return NOERROR;
}

//=============================================================================
// Blacklist
//=============================================================================
const String Blacklist::BLFILE("blacklist.dat");
const Int32 Blacklist::BLFILE_VER = 1;

ECode Blacklist::MigrateOldDataIfPresent(
    /* [in] */ IContext* context)
{
    AutoPtr<IObjectInputStream> ois;
    AutoPtr<IHashSet> data;
    // try {
    ECode ec;
    do {
        AutoPtr<IFileInputStream> fis;
        ec = context->OpenFileInput(BLFILE, (IFileInputStream**)&fis);
        if (FAILED(ec)) break;
        assert(0 && "TODO: CObjectInputStream");
        // ec = CObjectInputStream::New(fis, (IObjectInputStream**)&ois);
        if (FAILED(ec)) break;
        AutoPtr<IInterface> o;
        ec = IObjectInput::Probe(ois)->ReadObject((IInterface**)&o);
        if (FAILED(ec)) break;
        if (o != NULL && IInteger32::Probe(o) != NULL) {
            // check the version
            AutoPtr<IInteger32> version = IInteger32::Probe(o);
            Int32 versionValue;
            version->GetValue(&versionValue);
            if (versionValue == BLFILE_VER) {
                AutoPtr<IInterface> numbers;
                ec = IObjectInput::Probe(ois)->ReadObject((IInterface**)&numbers);
                if (FAILED(ec)) break;
                if (IHashSet::Probe(numbers) != NULL) {
                    data = IHashSet::Probe(numbers);
                }
            }
        }
    } while(FALSE);
    // } catch (IOException e) {
    if ((ECode) E_IO_EXCEPTION == ec) {
        // Do nothing
        ec = NOERROR;
    }
    // } catch (ClassNotFoundException e) {
    if ((ECode)E_CLASS_NOT_FOUND_EXCEPTION == ec) {
        // Do nothing
        ec = NOERROR;
    }
    // } finally {
    if (ois != NULL) {
        // try {
        ICloseable::Probe(ois)->Close();
        // } catch (IOException e) {
            // Do nothing
        // }
        Boolean bNoUse;
        context->DeleteFile(BLFILE, &bNoUse);
    }
    if (FAILED(ec)) return ec;
    // }
    if (data != NULL) {
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<IContentValues> cv;
        CContentValues::New((IContentValues**)&cv);
        cv->Put(ITelephonyBlacklist::PHONE_MODE, 1);
        AutoPtr<IIterator> it;
        data->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<PhoneNumber> number = (PhoneNumber*) IObject::Probe(obj);
            AutoPtr<ITelephonyBlacklist> helper;
            CTelephonyBlacklist::AcquireSingleton((ITelephonyBlacklist**)&helper);
            AutoPtr<IUri> contentFilterBynumberUri;
            helper->GetCONTENT_FILTER_BYNUMBER_URI((IUri**)&contentFilterBynumberUri);
            AutoPtr<IUri> uri;
            Uri::WithAppendedPath(contentFilterBynumberUri, number->mPhone, (IUri**)&uri);
            cv->Put(ITelephonyBlacklist::NUMBER, number->mPhone);
            Int32 iNotUsed;
            cr->Update(uri, cv, String(NULL), NULL, &iNotUsed);
        }
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
