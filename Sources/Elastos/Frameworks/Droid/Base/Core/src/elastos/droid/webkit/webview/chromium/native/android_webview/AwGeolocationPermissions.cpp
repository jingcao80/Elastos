
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwGeolocationPermissions.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/net/GURLUtils.h"

using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Net::GURLUtils;

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IMap;
using Elastos::Utility::IIterator;


namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

//===============================================================
//        AwGeolocationPermissions::GetAllowedRunnable
//===============================================================
CAR_INTERFACE_IMPL(AwGeolocationPermissions::GetAllowedRunnable, Object, IRunnable);

AwGeolocationPermissions::GetAllowedRunnable::GetAllowedRunnable(
    /* [in] */ AwGeolocationPermissions* owner,
    /* [in] */ IValueCallback* callback,
    /* [in] */ Boolean finalAllowed)
    : mOwner(owner)
    , mCallback(callback)
    , mFinalAllowed(finalAllowed)
{
}

ECode AwGeolocationPermissions::GetAllowedRunnable::Run()
{
    AutoPtr<IBoolean> boolVal;
    CBoolean::New(mFinalAllowed, (IBoolean**)&boolVal);
    return mCallback->OnReceiveValue(TO_IINTERFACE(boolVal));
}

//===============================================================
//        AwGeolocationPermissions::GetOriginsRunnable
//===============================================================
CAR_INTERFACE_IMPL(AwGeolocationPermissions::GetOriginsRunnable, Object, IRunnable);

AwGeolocationPermissions::GetOriginsRunnable::GetOriginsRunnable(
    /* [in] */ AwGeolocationPermissions* owner,
    /* [in] */ IValueCallback* callback)
    : mOwner(owner)
    , mCallback(callback)
    , mOrigins(NULL)
{
}

ECode AwGeolocationPermissions::GetOriginsRunnable::Run()
{
    mOrigins = NULL;
    CHashSet::New((ISet**)&mOrigins);

    AutoPtr<IMap> allSharedPre;
    mOwner->mSharedPreferences->GetAll((IMap**)&allSharedPre);
    AutoPtr<ISet> keySet;
    allSharedPre->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> keyIter;
    keySet->GetIterator((IIterator**)&keyIter);
    Boolean isflag = FALSE;
    while ((keyIter->HasNext(&isflag), isflag)) {
        AutoPtr<IInterface> outface;
        keyIter->GetNext((IInterface**)&outface);
        String name;
        AutoPtr<ICharSequence> csq = ICharSequence::Probe(outface);
        csq->ToString(&name);
        if (name.StartWith(PREF_PREFIX)) {
            String item = name.Substring(PREF_PREFIX.GetLength());
            AutoPtr<ICharSequence> csi;
            CString::New(item, (ICharSequence**)&csi);
            mOrigins->Add(csi);
        }
    }
    return mCallback->OnReceiveValue(TO_IINTERFACE(mOrigins));
}

//===============================================================
//                    AwGeolocationPermissions
//===============================================================

const String AwGeolocationPermissions::PREF_PREFIX("AwGeolocationPermissions%");

AwGeolocationPermissions::AwGeolocationPermissions(
    /* [in] */ ISharedPreferences* sharedPreferences)
    : mSharedPreferences(sharedPreferences)
{
}

/**
 * Set one origin to be allowed.
 */
void AwGeolocationPermissions::Allow(
    /* [in] */ const String& origin)
{
    String key = GetOriginKey(origin);
    if (!key.IsNullOrEmpty()) {
        AutoPtr<ISharedPreferencesEditor> edit;
        mSharedPreferences->Edit((ISharedPreferencesEditor**)&edit);
        edit->PutBoolean(key, TRUE);
        edit->Apply();
    }
}

/**
 * Set one origin to be denied.
 */
void AwGeolocationPermissions::Deny(
    /* [in] */ const String& origin)
{
    String key = GetOriginKey(origin);
    if (!key.IsNullOrEmpty()) {
        AutoPtr<ISharedPreferencesEditor> edit;
        mSharedPreferences->Edit((ISharedPreferencesEditor**)&edit);
        edit->PutBoolean(key, FALSE);
        edit->Apply();
    }
}

/**
 * Clear the stored permission for a particular origin.
 */
void AwGeolocationPermissions::Clear(
    /* [in] */ const String& origin)
{
    String key = GetOriginKey(origin);
    if (!key.IsNullOrEmpty()) {
        AutoPtr<ISharedPreferencesEditor> edit;
        mSharedPreferences->Edit((ISharedPreferencesEditor**)&edit);
        edit->Remove(key);
        edit->Apply();
    }
}

/**
 * Clear stored permissions for all origins.
 */
void AwGeolocationPermissions::ClearAll()
{
    AutoPtr<ISharedPreferencesEditor> editor;
    //for (String name : mSharedPreferences.getAll().keySet()) {
    AutoPtr<IMap> allSharedPre;
    mSharedPreferences->GetAll((IMap**)&allSharedPre);
    AutoPtr<ISet> keySet;
    allSharedPre->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> keyIter;
    keySet->GetIterator((IIterator**)&keyIter);
    Boolean isflag = FALSE;
    while ((keyIter->HasNext(&isflag), isflag)) {
        AutoPtr<IInterface> outface;
        keyIter->GetNext((IInterface**)&outface);
        String name;
        AutoPtr<ICharSequence> csq = ICharSequence::Probe(outface);
        csq->ToString(&name);
        if (name.StartWith(PREF_PREFIX)) {
            if (editor == NULL) {
                mSharedPreferences->Edit((ISharedPreferencesEditor**)&editor);
            }
            editor->Remove(name);
        }
    }
    //}
    if (editor != NULL) {
        editor->Apply();
    }
}

/**
 * Synchronous method to get if an origin is set to be allowed.
 */
Boolean AwGeolocationPermissions::IsOriginAllowed(
    /* [in] */ const String& origin)
{
    Boolean result = FALSE;
    mSharedPreferences->GetBoolean(GetOriginKey(origin), FALSE, &result);
    return result;
}

/**
 * Returns true if the origin is either set to allowed or denied.
 */
Boolean AwGeolocationPermissions::HasOrigin(
    /* [in] */ const String& origin)
{
    Boolean result = FALSE;
    mSharedPreferences->Contains(GetOriginKey(origin), &result);
    return result;
}

/**
 * Asynchronous method to get if an origin set to be allowed.
 */
void AwGeolocationPermissions::GetAllowed(
    /* [in] */ const String& origin,
    /* [in] */ IValueCallback* callback)
{
    Boolean finalAllowed = IsOriginAllowed(origin);
    AutoPtr<IRunnable> runnable = new GetAllowedRunnable(this, callback, finalAllowed);
    ThreadUtils::PostOnUiThread(runnable);
}

/**
 * Async method to get the domains currently allowed or denied.
 */
void AwGeolocationPermissions::GetOrigins(
    /* [in] */ IValueCallback* callback)
{
    AutoPtr<IRunnable> runnable = new GetOriginsRunnable(this, callback);
    ThreadUtils::PostOnUiThread(runnable);
}

/**
 * Get the domain of an URL using the GURL library.
 */
String AwGeolocationPermissions::GetOriginKey(
    /* [in] */ const String& url)
{
    String origin = GURLUtils::GetOrigin(url);
    if (origin.IsNullOrEmpty()) {
        return String(NULL);
    }

    return PREF_PREFIX + origin;
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
