
#include "elastos/droid/server/location/LocationBlacklist.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const String LocationBlacklist::TAG("LocationBlacklist");
const Boolean LocationBlacklist::D = FALSE;//ILocationManagerService::D;
const String LocationBlacklist::BLACKLIST_CONFIG_NAME("locationPackagePrefixBlacklist");
const String LocationBlacklist::WHITELIST_CONFIG_NAME("locationPackagePrefixWhitelist");

LocationBlacklist::LocationBlacklist(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler)
    : mCurrentUserId(IUserHandle::USER_OWNER)
{
    ContentObserver::constructor(handler);
    mContext = context;
}

ECode LocationBlacklist::Init()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> uri;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetUriFor(BLACKLIST_CONFIG_NAME, (IUri**)&uri);
    cr->RegisterContentObserver(uri, FALSE, this, IUserHandle::USER_ALL);
    ReloadBlacklist();
    return NOERROR;
}

void LocationBlacklist::ReloadBlacklistLocked()
{
    mWhitelist = GetStringArrayLocked(WHITELIST_CONFIG_NAME);
    if (D) Slogger::D(TAG, "whitelist: %s", Arrays::ToString(mWhitelist).string());
    mBlacklist = GetStringArrayLocked(BLACKLIST_CONFIG_NAME);
    if (D) Slogger::D(TAG, "blacklist: %s", Arrays::ToString(mBlacklist).string());

}
void LocationBlacklist::ReloadBlacklist()
{
    synchronized(this) {
        ReloadBlacklistLocked();
    }
}
ECode LocationBlacklist::IsBlacklisted(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    synchronized(this) {
        Int32 length = mBlacklist->GetLength();
        for (Int32 i = 0; i < length; i++) {
            if (packageName.StartWith((*mBlacklist)[i])) {
                if (InWhitelist(packageName)) {
                    continue;
                }
                else {
                    if (D) {
                        Logger::D(TAG, "dropping location (blacklisted): %s matches %s",
                            packageName.string(), (*mBlacklist)[i].string());
                    }
                    *result = TRUE;
                    return NOERROR;
                }
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}
Boolean LocationBlacklist::InWhitelist(
    /* [in] */ const String& pkg)
{
    synchronized(this) {
        Int32 length = mWhitelist->GetLength();
        for (Int32 i = 0; i < length; i++) {
            if (pkg.StartWith((*mWhitelist)[i])) return TRUE;
        }
    }
    return FALSE;
}
ECode LocationBlacklist::OnChange(
    /* [in] */ Boolean selfChange)
{
    ReloadBlacklist();
    return NOERROR;
}

ECode LocationBlacklist::SwitchUser(
    /* [in] */ Int32 userId)
{
    synchronized(this) {
        mCurrentUserId = userId;
        ReloadBlacklistLocked();
    }
    return NOERROR;
}

AutoPtr<ArrayOf<String> > LocationBlacklist::GetStringArrayLocked(
    /* [in] */ const String& key)
{
    String flatString;
    synchronized(this) {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        settingsSecure->GetStringForUser(cr, key, mCurrentUserId, &flatString);
    }
    if (flatString.IsNull()) {
        return ArrayOf<String>::Alloc(0);
    }
    AutoPtr<ArrayOf<String> > splitStrings;
    StringUtils::Split(flatString, String(","), (ArrayOf<String>**)&splitStrings);
    if (splitStrings == NULL) return ArrayOf<String>::Alloc(0);

    AutoPtr<IArrayList> result;
    Int32 length = splitStrings->GetLength();
    for (Int32 i = 0; i < length; i++)
    {
        String pkg = (*splitStrings)[i];
        pkg = pkg.Trim();
        if (pkg.IsEmpty()) {
            continue;
        }
        AutoPtr<ICharSequence> obj;
        CString::New(pkg, (ICharSequence**)&obj);
        result->Add(obj.Get());
    }
    Int32 size;
    result->GetSize(&size);
    AutoPtr<ArrayOf<IInterface* > > inArray = ArrayOf<IInterface* >::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        result->Get(i, (IInterface**)&obj);
        inArray->Set(i, obj.Get());
    }
    AutoPtr<ArrayOf<IInterface* > > outArray;
    result->ToArray(inArray.Get(), (ArrayOf<IInterface* >**)&outArray);

    AutoPtr<ArrayOf<String> > stringarray = ArrayOf<String>::Alloc(size);
    for(Int32 i = 0; i < size; i++) {
        AutoPtr<ICharSequence> cs = ICharSequence::Probe((*outArray)[i]);
        String str;
        cs->ToString(&str);
        (*stringarray)[i] = str;
    }
    return stringarray.Get();
}

void LocationBlacklist::Dump(
    /* [in] */ IPrintWriter* pw)
{
    StringBuilder sb("mWhitelist=");
    sb += Arrays::ToString(mWhitelist.Get());
    sb += " mBlacklist=";
    sb += Arrays::ToString(mBlacklist.Get());
    pw->Println(sb.ToString());
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos