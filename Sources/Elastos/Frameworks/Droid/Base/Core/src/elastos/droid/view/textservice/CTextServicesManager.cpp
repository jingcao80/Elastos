
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/textservice/CTextServicesManager.h"
#include "elastos/droid/view/textservice/CSpellCheckerSession.h"
#include "elastos/droid/view/textservice/CSpellCheckerSubtype.h"
#include "elastos/droid/os/CServiceManager.h"

#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Internal::TextService::IITextServicesSessionListener;
using Elastos::Droid::Internal::TextService::IISpellCheckerSessionListener;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

//========================================================================================
//              CTextServicesManager::
//========================================================================================
const String CTextServicesManager::TAG("CTextServicesManager");
const Boolean CTextServicesManager::DBG = FALSE;

AutoPtr<ITextServicesManager> CTextServicesManager::sInstance;
AutoPtr<IITextServicesManager> CTextServicesManager::sService;

Object CTextServicesManager::sLock;

CAR_INTERFACE_IMPL(CTextServicesManager, Object, ITextServicesManager)

CAR_OBJECT_IMPL(CTextServicesManager)

ECode CTextServicesManager::constructor()
{
    if (sService == NULL) {
        AutoPtr<IServiceManager> sm;
        CServiceManager::AcquireSingleton((IServiceManager**)&sm);
        AutoPtr<IInterface> s;
        sm->GetService(IContext::TEXT_SERVICES_MANAGER_SERVICE, (IInterface**)&s);
        AutoPtr<IBinder> b = IBinder::Probe(s);
        sService = IITextServicesManager::Probe(b);
    }
    return NOERROR;
}

AutoPtr<ITextServicesManager> CTextServicesManager::GetInstance()
{
    {
        AutoLock lock(sLock);
        if (sInstance != NULL) {
            return sInstance;
        }
        CTextServicesManager::New((ITextServicesManager**)&sInstance);
    }

    return sInstance;
}

String CTextServicesManager::ParseLanguageFromLocaleString(
    /* [in] */ const String& locale)
{
    Int32 idx = locale.IndexOf('_');
    if (idx < 0) {
        return locale;
    }
    else {
        return locale.Substring(0, idx);
    }
}

ECode CTextServicesManager::NewSpellCheckerSession(
    /* [in] */ IBundle* bundle,
    /* [in] */ ILocale* locale,
    /* [in] */ ISpellCheckerSessionListener* listener,
    /* [in] */ Boolean referToSpellCheckerLanguageSettings,
    /* [out] */ ISpellCheckerSession** session)
{
    VALIDATE_NOT_NULL(session);

    if (listener == NULL) {
        *session = NULL;
       return E_NULL_POINTER_EXCEPTION;
    }

    if (!referToSpellCheckerLanguageSettings && locale == NULL) {
        *session = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean enabled = FALSE;
    if (referToSpellCheckerLanguageSettings && !(IsSpellCheckerEnabled(&enabled), enabled)) {
        *session = NULL;
        return NOERROR;
    }

    AutoPtr<ISpellCheckerInfo> sci;
    ECode ec = sService->GetCurrentSpellChecker(String(NULL), (ISpellCheckerInfo**)&sci);
    if (FAILED(ec)) {
        *session = NULL;
        return NOERROR;
    }
    if (sci == NULL) {
        *session = NULL;
        return NOERROR;
    }

    AutoPtr<ISpellCheckerSubtype> subtypeInUse;
    if (referToSpellCheckerLanguageSettings) {
        GetCurrentSpellCheckerSubtype(TRUE, (ISpellCheckerSubtype**)&subtypeInUse);
        if (subtypeInUse == NULL) {
            *session = NULL;
            return NOERROR;
        }
        if (locale != NULL) {

            String subtypeLocale;
            subtypeInUse->GetLocale(&subtypeLocale);
            String subtypeLanguage = ParseLanguageFromLocaleString(subtypeLocale);
            String language;
            if (subtypeLanguage.GetLength() < 2 ||
                    !(locale->GetLanguage(&language), language).Equals(subtypeLanguage)) {
                *session = NULL;
                return NOERROR;
            }
        }
    }
    else {
        String localeStr;
        locale->ToString(&localeStr);

        Int32 count;
        sci->GetSubtypeCount(&count);

        for (Int32 i = 0; i < count; ++i) {
            AutoPtr<ISpellCheckerSubtype> subtype;
            sci->GetSubtypeAt(i, (ISpellCheckerSubtype**)&subtype);
            String tempSubtypeLocale;
            subtype->GetLocale(&tempSubtypeLocale);
            String tempSubtypeLanguage = ParseLanguageFromLocaleString(tempSubtypeLocale);

            String language;
            if (tempSubtypeLocale.Equals(localeStr)) {
                subtypeInUse = subtype;
                break;
            }
            else if (tempSubtypeLanguage.GetLength() >= 2 &&
                    (locale->GetLanguage(&language), language).Equals(tempSubtypeLanguage)) {
                subtypeInUse = subtype;
            }
        }
    }

    if (subtypeInUse == NULL) {
        *session = NULL;
        return NOERROR;
    }

    AutoPtr<ISpellCheckerSession> _session;
    CSpellCheckerSession::New(
            sci, sService, listener, subtypeInUse, (ISpellCheckerSession**)&_session);

    String id, localeString;
    AutoPtr<IITextServicesSessionListener> textListener;
    AutoPtr<IISpellCheckerSessionListener> spellListener;

    sci->GetId(&id);
    subtypeInUse->GetLocale(&localeString);
    ((CSpellCheckerSession*)_session.Get())->GetTextServicesSessionListener((IITextServicesSessionListener**)&textListener);
    ((CSpellCheckerSession*)_session.Get())->GetSpellCheckerSessionListener((IISpellCheckerSessionListener**)&spellListener);

    ec = sService->GetSpellCheckerService(id, localeString,
            textListener, spellListener, bundle);
    if (FAILED(ec)) {
        *session = NULL;
        return NOERROR;
    }

    *session = _session;
    REFCOUNT_ADD(*session);
    return NOERROR;
}

ECode CTextServicesManager::GetEnabledSpellCheckers(
    /* [out] */ ArrayOf<ISpellCheckerInfo*>** infoArray)
{
    VALIDATE_NOT_NULL(infoArray);

    return sService->GetEnabledSpellCheckers(infoArray);
}

ECode CTextServicesManager::GetCurrentSpellChecker(
    /* [out] */ ISpellCheckerInfo** info)
{
    VALIDATE_NOT_NULL(info);

    return sService->GetCurrentSpellChecker(String(NULL), info);
}

ECode CTextServicesManager::SetCurrentSpellChecker(
    /* [in] */ ISpellCheckerInfo* sci)
{
   if (sci == NULL) {
       // throw new NullPointerException("SpellCheckerInfo is null.");
       return E_NULL_POINTER_EXCEPTION;
   }

    String id;
    sci->GetId(&id);

    return sService->SetCurrentSpellChecker(String(NULL), id);
}

ECode CTextServicesManager::GetCurrentSpellCheckerSubtype(
    /* [in] */ Boolean allowImplicitlySelectedSubtype,
    /* [out] */ ISpellCheckerSubtype** subtype)
{
    VALIDATE_NOT_NULL(subtype);

    if (sService == NULL) {
        // TODO: This is a workaround. Needs to investigate why sService could be null
        // here.
        Logger::E(TAG, "sService is null.");
        *subtype = NULL;
        return NOERROR;
    }
    // Passing null as a locale until we support multiple enabled spell checker subtypes.
    return sService->GetCurrentSpellCheckerSubtype(String(NULL), allowImplicitlySelectedSubtype, subtype);
}

ECode CTextServicesManager::SetSpellCheckerSubtype(
    /* [in] */ ISpellCheckerSubtype* subtype)
{
    Int32 hashCode;
    if (subtype == NULL) {
        hashCode = 0;
    }
    else {
        ((CSpellCheckerSubtype*)subtype)->GetHashCode(&hashCode);
    }
    return sService->SetCurrentSpellCheckerSubtype(String(NULL), hashCode);
}

ECode CTextServicesManager::SetSpellCheckerEnabled(
    /* [in] */ Boolean enabled)
{
    return sService->SetSpellCheckerEnabled(enabled);
}

ECode CTextServicesManager::IsSpellCheckerEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);

    return sService->IsSpellCheckerEnabled(enabled);
}

}   //namespace TextService
}   //namespace View
}   //namespace Droid
}   //namespace Elastos
