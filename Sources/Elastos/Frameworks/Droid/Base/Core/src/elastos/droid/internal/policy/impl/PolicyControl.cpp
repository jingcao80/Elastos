#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Os.h>
#include "elastos/droid/app/CActivityManagerHelper.h"
#include "elastos/droid/internal/policy/impl/PolicyControl.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/utility/CArraySet.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::View::IView;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::IO::CPrintWriter;
using Elastos::IO::CStringWriter;
using Elastos::IO::IStringWriter;
using Elastos::IO::IWriter;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//=====================================================================
//                        PolicyControl::Filter
//=====================================================================
const String PolicyControl::Filter::ALL("*");
const String PolicyControl::Filter::APPS("apps");

ECode PolicyControl::Filter::Matches(
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (attrs == NULL)
    {
        *result = FALSE;
        return NOERROR;
    }
    Int32 type;
    attrs->GetType(&type);
    Boolean isApp = type >= IWindowManagerLayoutParams::FIRST_APPLICATION_WINDOW
            && type <= IWindowManagerLayoutParams::LAST_APPLICATION_WINDOW;
    AutoPtr<ICharSequence> apps;
    CString::New(APPS, (ICharSequence**)&apps);
    Boolean inBlacklist;
    ISet* blSet = ISet::Probe(mBlacklist);
    blSet->Contains(TO_IINTERFACE(apps), &inBlacklist);
    if (isApp && inBlacklist)
    {
        *result = FALSE;
        return NOERROR;
    }
    String pkName;
    attrs->GetPackageName(&pkName);
    if (OnBlacklist(pkName))
    {
        *result = FALSE;
        return NOERROR;
    }
    Boolean inWhitelist = FALSE;
    ISet* wlSet = ISet::Probe(mWhitelist);
    wlSet->Contains(TO_IINTERFACE(apps), &inWhitelist);
    if (isApp && inWhitelist)
    {
        *result = TRUE;
        return NOERROR;
    }
    *result = OnWhitelist(pkName);
    return NOERROR;
}

ECode PolicyControl::Filter::Matches(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = !OnBlacklist(packageName) && OnWhitelist(packageName);
    return NOERROR;
}

ECode PolicyControl::Filter::Dump(
    /* [in] */ IPrintWriter* pw)
{
    VALIDATE_NOT_NULL(pw);
    pw->Print(String("Filter["));
    Dump(String("whitelist"), mWhitelist, pw);
    pw->Print(',');
    Dump(String("blacklist"), mBlacklist, pw);
    pw->Print(']');
    return NOERROR;
}

String PolicyControl::Filter::ToString()
{
    AutoPtr<IStringWriter> sw;
    CStringWriter::New((IStringWriter**)&sw);
    AutoPtr<IPrintWriter> pw;
    IWriter* w = IWriter::Probe(sw);
    CPrintWriter::New(w, TRUE, (IPrintWriter**)&pw);
    Dump(pw);
    String res;
    sw->ToString(&res);
    return res;
}

AutoPtr<PolicyControl::Filter> PolicyControl::Filter::Parse(
    /* [in] */ const String& value)
{
    if (value.IsNullOrEmpty()) return NULL;
    AutoPtr<IArraySet> whitelist;// = new ArraySet<String>();
    CArraySet::New((IArraySet**)&whitelist);
    AutoPtr<IArraySet> blacklist;// = new ArraySet<String>();
    CArraySet::New((IArraySet**)&blacklist);
    AutoPtr<ArrayOf<String> > splitOut;
    StringUtils::Split(value, String(","), (ArrayOf<String>**)&splitOut);
    ISet* bSet = ISet::Probe(blacklist);
    ISet* wSet = ISet::Probe(whitelist);
    for (Int32 i = 0; i < splitOut->GetLength(); ++i)
    {
        String token = (*splitOut)[i].Trim();
        if (token.StartWith("-") && token.GetLength() > 1)
        {
            token = token.Substring(1);
            AutoPtr<ICharSequence> cs;
            CString::New(token, (ICharSequence**)&cs);
            bSet->Add(TO_IINTERFACE(cs));
        }
        else
        {
            AutoPtr<ICharSequence> cs;
            CString::New(token, (ICharSequence**)&cs);
            wSet->Add(TO_IINTERFACE(cs));
        }
    }
    return new Filter(whitelist, blacklist);
}

PolicyControl::Filter::Filter(
    /* [in] */ IArraySet* whitelist,
    /* [in] */ IArraySet* blacklist)
{
    mWhitelist = whitelist;
    mBlacklist = blacklist;
}

Boolean PolicyControl::Filter::OnBlacklist(
    /* [in] */ const String& packageName)
{
    AutoPtr<ICharSequence> pncs;
    CString::New(packageName, (ICharSequence**)&pncs);
    Boolean res1;
    ISet* blSet = ISet::Probe(mBlacklist);
    blSet->Contains(pncs, &res1);
    if (res1)
    {
        return TRUE;
    }
    else
    {
        AutoPtr<ICharSequence> allcs;
        CString::New(ALL, (ICharSequence**)&allcs);
        Boolean res2;
        blSet->Contains(allcs, &res2);
        return res2;
    }
}

Boolean PolicyControl::Filter::OnWhitelist(
    /* [in] */ const String& packageName)
{
    AutoPtr<ICharSequence> allcs;
    CString::New(ALL, (ICharSequence**)&allcs);
    Boolean res1;
    ISet* wlSet = ISet::Probe(mWhitelist);
    wlSet->Contains(allcs, &res1);
    if (res1)
    {
        return TRUE;
    }
    else
    {
        AutoPtr<ICharSequence> pncs;
        CString::New(packageName, (ICharSequence**)&pncs);
        Boolean res2;
        wlSet->Contains(pncs, &res2);
        return res2;
    }

}

void PolicyControl::Filter::Dump(
    /* [in] */ const String& name,
    /* [in] */ IArraySet* set,
    /* [in] */ IPrintWriter* pw)
{
    pw->Print(name);
    pw->Print(String("=("));
    Int32 n;
    ISet* s = ISet::Probe(set);
    s->GetSize(&n);
    for (Int32 i = 0; i < n; i++)
    {
        if (i > 0) pw->Print(',');
        AutoPtr<IInterface> iValue;
        set->GetValueAt(i, (IInterface**)&iValue);
        String value;
        ICharSequence* csq = ICharSequence::Probe(iValue);
        csq->ToString(&value);
        pw->Print(value);
    }
    pw->Print(')');
}

//=====================================================================
//                            PolicyControl
//=====================================================================

String PolicyControl::TAG("PolicyControl");
Boolean PolicyControl::DEBUG = FALSE;
const String PolicyControl::NAME_IMMERSIVE_FULL("immersive.full");
const String PolicyControl::NAME_IMMERSIVE_STATUS("immersive.status");
const String PolicyControl::NAME_IMMERSIVE_NAVIGATION("immersive.navigation");
const String PolicyControl::NAME_IMMERSIVE_PRECONFIRMATIONS("immersive.preconfirms");
String PolicyControl::sSettingValue;
AutoPtr<PolicyControl::Filter> PolicyControl::sImmersivePreconfirmationsFilter;
AutoPtr<PolicyControl::Filter> PolicyControl::sImmersiveStatusFilter;
AutoPtr<PolicyControl::Filter> PolicyControl::sImmersiveNavigationFilter;

ECode PolicyControl::GetSystemUiVisibility(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
    //attrs = attrs != null ? attrs : win.getAttrs();
    if (attrs == NULL)
    {
        win->GetAttrs(&attrs);
    }
    Int32 vis;// = win != null ? win.getSystemUiVisibility() : attrs.systemUiVisibility;
    if (win != NULL)
    {
        win->GetSystemUiVisibility(&vis);
    }
    else
    {
        attrs->GetSystemUiVisibility(&vis);
    }
    if (sImmersiveStatusFilter != NULL)
    {
        Boolean sfRes;
        sImmersiveStatusFilter->Matches(attrs, &sfRes);
        if (sfRes)
        {
            vis |= IView::SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | IView::SYSTEM_UI_FLAG_FULLSCREEN
                | IView::SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN;
            vis &= ~(IView::SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | IView::STATUS_BAR_TRANSLUCENT);
        }
    }
    if (sImmersiveNavigationFilter != NULL)
    {
        Boolean nfRes;
        sImmersiveNavigationFilter->Matches(attrs, &nfRes);
        if (nfRes)
        {
            vis |= IView::SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION;
            vis &= ~(IView::SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | IView::NAVIGATION_BAR_TRANSLUCENT);
        }
    }
    *res = vis;
    return NOERROR;
}

ECode PolicyControl::GetWindowFlags(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* _attrs,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IWindowManagerLayoutParams> attrs = _attrs;
    if (attrs == NULL) {
        win->GetAttrs((IWindowManagerLayoutParams**)&attrs);
    }
    Int32 flags;
    attrs->GetFlags(&flags);
    if (sImmersiveStatusFilter != NULL) {
        Boolean sfRes;
        sImmersiveStatusFilter->Matches(attrs, &sfRes);
        if (sfRes) {
            flags |= IWindowManagerLayoutParams::FLAG_FULLSCREEN;
            flags &= ~(IWindowManagerLayoutParams::FLAG_TRANSLUCENT_STATUS
                    | IWindowManagerLayoutParams::FLAG_FORCE_NOT_FULLSCREEN);
        }
    }
    if (sImmersiveNavigationFilter != NULL) {
        Boolean nfRes;
        sImmersiveNavigationFilter->Matches(attrs, &nfRes);
        if (nfRes) {
            flags &= ~IWindowManagerLayoutParams::FLAG_TRANSLUCENT_NAVIGATION;
        }
    }
    *res = flags;
    return NOERROR;
}

ECode PolicyControl::AdjustClearableFlags(
    /* [in] */ IWindowState* win,
    /* [in] */ Int32 clearableFlags,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IWindowManagerLayoutParams> attrs;
    if (win != NULL) {
        win->GetAttrs((IWindowManagerLayoutParams**)&attrs);
    }
    if (sImmersiveStatusFilter != NULL) {
        Boolean sfRes;
        sImmersiveStatusFilter->Matches(attrs, &sfRes);
        if (sfRes)
        {
            clearableFlags &= ~IView::SYSTEM_UI_FLAG_FULLSCREEN;
        }
    }
    *res = clearableFlags;
    return NOERROR;
}

ECode PolicyControl::DisableImmersiveConfirmation(
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    // ==================before translated======================
    // return (sImmersivePreconfirmationsFilter != null
    //         && sImmersivePreconfirmationsFilter.matches(pkg))
    //         || ActivityManager.isRunningInTestHarness();
    if (sImmersivePreconfirmationsFilter != NULL)
    {
        Boolean pcfRes;
        sImmersivePreconfirmationsFilter->Matches(pkg, &pcfRes);
        if (pcfRes)
        {
            *res = TRUE;
        }
    }
    else
    {
        AutoPtr<IActivityManagerHelper> amHelper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
        amHelper->IsRunningInTestHarness(res);
    }
    return NOERROR;
}

ECode PolicyControl::ReloadFromSetting(
    /* [in] */ IContext* context)
{
    VALIDATE_NOT_NULL(context);
    if (DEBUG) Slogger::D(TAG, "reloadFromSetting()");
    String value(NULL);
    //try {
    AutoPtr<IContentResolver> cResolver;
    context->GetContentResolver((IContentResolver**)&cResolver);
    Settings::Global::GetStringForUser(cResolver, ISettingsGlobal::POLICY_CONTROL, IUserHandle::USER_CURRENT, &value);
    if (!sSettingValue.IsNullOrEmpty() && sSettingValue.Equals(value)) return NOERROR;
    SetFilters(value);
    sSettingValue = value;
    //} catch (Throwable t) {
    //    Slog.w(TAG, "Error loading policy control, value=" + value, t);
    //}
    return NOERROR;
}

ECode PolicyControl::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* pw)
{
    VALIDATE_NOT_NULL(pw);
    Dump(String("sImmersiveStatusFilter"), sImmersiveStatusFilter, prefix, pw);
    Dump(String("sImmersiveNavigationFilter"), sImmersiveNavigationFilter, prefix, pw);
    Dump(String("sImmersivePreconfirmationsFilter"), sImmersivePreconfirmationsFilter, prefix, pw);
    return NOERROR;
}

void PolicyControl::Dump(
    /* [in] */ const String& name,
    /* [in] */ Filter* filter,
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* pw)
{
    pw->Print(prefix);
    pw->Print(String("PolicyControl."));
    pw->Print(name);
    pw->Print('=');
    if (filter == NULL)
    {
        pw->Println(String("null"));
    }
    else
    {
        filter->Dump(pw);
        pw->Println();
    }
}

void PolicyControl::SetFilters(
    /* [in] */ const String& value)
{
    if (DEBUG) Slogger::D(TAG, "setFilters: %s", value.string());
    sImmersiveStatusFilter = NULL;
    sImmersiveNavigationFilter = NULL;
    sImmersivePreconfirmationsFilter = NULL;
    if (!value.IsNullOrEmpty())
    {
        AutoPtr<ArrayOf<String> > splitOut;
        StringUtils::Split(value, String(":"), (ArrayOf<String>**)&splitOut);
        for (Int32 i = 0; i < splitOut->GetLength(); ++i)
        {
            String nvp= (*splitOut)[i];
            Int32 index = nvp.IndexOf('=');
            if (index == -1) continue;
            String n = nvp.Substring(0, index);
            String v = nvp.Substring(index + 1);
            if (n.Equals(NAME_IMMERSIVE_FULL))
            {
                AutoPtr<Filter> f = Filter::Parse(v);
                sImmersiveStatusFilter = sImmersiveNavigationFilter = f;
                if (sImmersivePreconfirmationsFilter == NULL)
                {
                    sImmersivePreconfirmationsFilter = f;
                }
            }
            else if (n.Equals(NAME_IMMERSIVE_STATUS))
            {
                AutoPtr<Filter> f = Filter::Parse(v);
                sImmersiveStatusFilter = f;
            }
            else if (n.Equals(NAME_IMMERSIVE_NAVIGATION))
            {
                AutoPtr<Filter> f = Filter::Parse(v);
                sImmersiveNavigationFilter = f;
                if (sImmersivePreconfirmationsFilter == NULL)
                {
                    sImmersivePreconfirmationsFilter = f;
                }
            }
            else if (n.Equals(NAME_IMMERSIVE_PRECONFIRMATIONS))
            {
                AutoPtr<Filter> f = Filter::Parse(v);
                sImmersivePreconfirmationsFilter = f;
            }
        }
    }
    if (DEBUG)
    {
        Slogger::D(TAG, "immersiveStatusFilter: %s",  sImmersiveStatusFilter->ToString().string());
        Slogger::D(TAG, "immersiveNavigationFilter: %s", sImmersiveNavigationFilter->ToString().string());
        Slogger::D(TAG, "immersivePreconfirmationsFilter: %s", sImmersivePreconfirmationsFilter->ToString().string());
    }
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos


