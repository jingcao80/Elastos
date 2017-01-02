//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/view/WindowManagerPolicyControl.h"
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/utility/CArraySet.h"

#include "Elastos.Droid.Provider.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::IO::IWriter;
using Elastos::IO::CPrintWriter;
using Elastos::IO::IStringWriter;
using Elastos::IO::CStringWriter;

namespace Elastos {
namespace Droid {
namespace View {

const String WindowManagerPolicyControl::_TAG("PolicyControl");
Boolean WindowManagerPolicyControl::DEBUG = FALSE;

const String WindowManagerPolicyControl::NAME_IMMERSIVE_FULL("immersive.full");
const String WindowManagerPolicyControl::NAME_IMMERSIVE_STATUS("immersive.status");
const String WindowManagerPolicyControl::NAME_IMMERSIVE_NAVIGATION("immersive.navigation");
const String WindowManagerPolicyControl::NAME_IMMERSIVE_PRECONFIRMATIONS("immersive.preconfirms");

Int32 WindowManagerPolicyControl::sDefaultImmersiveStyle;
String WindowManagerPolicyControl::sSettingValue;
AutoPtr<WindowManagerPolicyControl::Filter> WindowManagerPolicyControl::sImmersivePreconfirmationsFilter;
AutoPtr<WindowManagerPolicyControl::Filter> WindowManagerPolicyControl::sImmersiveStatusFilter;
AutoPtr<WindowManagerPolicyControl::Filter> WindowManagerPolicyControl::sImmersiveNavigationFilter;


Int32 WindowManagerPolicyControl::GetSystemUiVisibility(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    //attrs = attrs != null ? attrs : win.getAttrs();
    AutoPtr<IWindowManagerLayoutParams> winParams = attrs;
    if (winParams == NULL)
    {
        win->GetAttrs((IWindowManagerLayoutParams**)&winParams);
    }
    Int32 vis;// = win != null ? win.getSystemUiVisibility() : attrs.systemUiVisibility;
    if (win != NULL)
    {
        win->GetSystemUiVisibility(&vis);
    }
    else
    {
        winParams->GetSystemUiVisibility(&vis);
    }
    if (sImmersiveStatusFilter != NULL && sImmersiveStatusFilter->Matches(winParams)
        && (sDefaultImmersiveStyle == ImmersiveDefaultStyles::IMMERSIVE_FULL ||
                sDefaultImmersiveStyle == ImmersiveDefaultStyles::IMMERSIVE_STATUS))
    {
        vis |= IView::SYSTEM_UI_FLAG_IMMERSIVE_STICKY
            | IView::SYSTEM_UI_FLAG_FULLSCREEN
            | IView::SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN;
        vis &= ~(IView::SYSTEM_UI_FLAG_LAYOUT_STABLE
                | IView::STATUS_BAR_TRANSLUCENT);
    }
    if (sImmersiveNavigationFilter != NULL && sImmersiveNavigationFilter->Matches(winParams)
        && (sDefaultImmersiveStyle == ImmersiveDefaultStyles::IMMERSIVE_FULL ||
                sDefaultImmersiveStyle == ImmersiveDefaultStyles::IMMERSIVE_NAVIGATION))
    {
        vis |= IView::SYSTEM_UI_FLAG_IMMERSIVE_STICKY
            | IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            | IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION;
        vis &= ~(IView::SYSTEM_UI_FLAG_LAYOUT_STABLE
                | IView::NAVIGATION_BAR_TRANSLUCENT);
    }
    return vis;
}

Int32 WindowManagerPolicyControl::GetWindowFlags(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    //attrs = attrs != null ? attrs : win.getAttrs();
    AutoPtr<IWindowManagerLayoutParams> winParams = attrs;
    if (winParams == NULL) {
        win->GetAttrs((IWindowManagerLayoutParams**)&winParams);
    }
    Int32 flags;
    winParams->GetFlags(&flags);
    if (sImmersiveStatusFilter != NULL && sImmersiveStatusFilter->Matches(winParams)
        && (sDefaultImmersiveStyle == ImmersiveDefaultStyles::IMMERSIVE_FULL ||
        sDefaultImmersiveStyle == ImmersiveDefaultStyles::IMMERSIVE_STATUS))
    {
        flags |= IWindowManagerLayoutParams::FLAG_FULLSCREEN;
        flags &= ~(IWindowManagerLayoutParams::FLAG_TRANSLUCENT_STATUS
                | IWindowManagerLayoutParams::FLAG_FORCE_NOT_FULLSCREEN);
    }
    if (sImmersiveNavigationFilter != NULL && sImmersiveNavigationFilter->Matches(winParams)
        && (sDefaultImmersiveStyle == ImmersiveDefaultStyles::IMMERSIVE_FULL ||
            sDefaultImmersiveStyle == ImmersiveDefaultStyles::IMMERSIVE_NAVIGATION))
    {
        flags &= ~IWindowManagerLayoutParams::FLAG_TRANSLUCENT_NAVIGATION;
    }
    return flags;
}

Int32 WindowManagerPolicyControl::GetPrivateWindowFlags(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    AutoPtr<IWindowManagerLayoutParams> winParams = attrs;
    if (winParams == NULL) {
        win->GetAttrs((IWindowManagerLayoutParams**)&winParams);
    }
    Int32 privateFlags;
    winParams->GetPrivateFlags(&privateFlags);

    if (sImmersiveStatusFilter != NULL && sImmersiveNavigationFilter != NULL &&
            sImmersiveStatusFilter->IsEnabledForAll()
            && sImmersiveNavigationFilter->IsEnabledForAll()) {

        Int32 flags;
        winParams->GetFlags(&flags);
        if ((flags & IWindowManagerLayoutParams::FLAG_FULLSCREEN) == 0) {
            privateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_WAS_NOT_FULLSCREEN;
        }

        switch (sDefaultImmersiveStyle) {
            case ImmersiveDefaultStyles::IMMERSIVE_FULL:
                privateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_NAV_HIDE_FORCED;
                privateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_STATUS_HIDE_FORCED;
                return privateFlags;
            case ImmersiveDefaultStyles::IMMERSIVE_STATUS:
                privateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_STATUS_HIDE_FORCED;
                return privateFlags;
            case ImmersiveDefaultStyles::IMMERSIVE_NAVIGATION:
                privateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_NAV_HIDE_FORCED;
                return privateFlags;
            }
    }

    if (sImmersiveStatusFilter != NULL && sImmersiveStatusFilter->Matches(winParams)) {
        Int32 flags;
        winParams->GetFlags(&flags);
        if ((flags & IWindowManagerLayoutParams::FLAG_FULLSCREEN) == 0) {
            privateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_WAS_NOT_FULLSCREEN;
        }
        privateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_STATUS_HIDE_FORCED;
    }

    if (sImmersiveNavigationFilter != NULL && sImmersiveNavigationFilter->Matches(winParams)) {
        privateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_NAV_HIDE_FORCED;
    }

    return privateFlags;
}

Boolean WindowManagerPolicyControl::ImmersiveStatusFilterMatches(
    /* [in] */ const String& packageName)
{
    return sImmersiveStatusFilter != NULL && sImmersiveStatusFilter->Matches(packageName);
}

Boolean WindowManagerPolicyControl::ImmersiveNavigationFilterMatches(
    /* [in] */ const String& packageName)
{
    return sImmersiveNavigationFilter != NULL && sImmersiveNavigationFilter->Matches(packageName);
}

Int32 WindowManagerPolicyControl::AdjustClearableFlags(
    /* [in] */ IWindowState* win,
    /* [in] */ Int32 clearableFlags)
{
    AutoPtr<IWindowManagerLayoutParams> attrs;
    if (win != NULL)
        win->GetAttrs((IWindowManagerLayoutParams**)&attrs);
    Int32 clearable = clearableFlags;
    if (sImmersiveStatusFilter != NULL && sImmersiveStatusFilter->Matches(attrs)) {
        clearable &= ~IView::SYSTEM_UI_FLAG_FULLSCREEN;
    }
    return clearable;
}

Boolean WindowManagerPolicyControl::DisableImmersiveConfirmation(
    /* [in] */ const String& pkg)
{
    return (sImmersivePreconfirmationsFilter != NULL
            && sImmersivePreconfirmationsFilter->Matches(pkg))
            || CActivityManager::IsRunningInTestHarness();
}

void WindowManagerPolicyControl::ReloadFromSetting(
    /* [in] */ IContext* context)
{
    ReloadStyleFromSetting(context, ISettingsGlobal::POLICY_CONTROL_STYLE);
    ReloadFromSetting(context, ISettingsGlobal::POLICY_CONTROL);
}

void WindowManagerPolicyControl::ReloadFromSetting(
    /* [in] */ IContext* context,
    /* [in] */ const String& key)
{
    if (DEBUG) SLOGGERD(_TAG, "reloadFromSetting()");
    String value;
    // try {
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        if (FAILED(Settings::Global::GetStringForUser(cr,
                key, IUserHandle::USER_CURRENT, &value)))
        {
            SLOGGERW(_TAG, String("Error loading policy control, value=") + value)
        }
        SetFilters(value);
        sSettingValue = value;
    // } catch (Throwable t) {
    //     Slog.w(_TAG, "Error loading policy control, value=" + value, t);
    // }
}

void WindowManagerPolicyControl::ReloadStyleFromSetting(
    /* [in] */ IContext* context,
    /* [in] */ const String& key)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Settings::Global::GetInt32(cr,
            key, ImmersiveDefaultStyles::IMMERSIVE_FULL, &sDefaultImmersiveStyle);
    if (DEBUG)
        SLOGGERD(_TAG, "reloadStyleFromSetting %d", sDefaultImmersiveStyle);
}

void WindowManagerPolicyControl::SaveToSettings(
    /* [in] */ IContext* context)
{
    SaveToSettings(context, ISettingsGlobal::POLICY_CONTROL);
}

void WindowManagerPolicyControl::SaveToSettings(
    /* [in] */ IContext* context,
    /* [in] */ const String& key)
{
    StringBuilder value;
    Boolean needSemicolon = FALSE;
    if (sImmersiveStatusFilter != NULL) {
        WriteFilter(NAME_IMMERSIVE_STATUS, sImmersiveStatusFilter, value);
        needSemicolon = TRUE;
    }
    if (sImmersiveNavigationFilter != NULL) {
        if (needSemicolon) {
            value += ":";
        }
        WriteFilter(NAME_IMMERSIVE_NAVIGATION, sImmersiveNavigationFilter, value);
    }

    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Boolean res;
    Settings::Global::PutString(cr, key, value.ToString(), &res);
}

void WindowManagerPolicyControl::SaveStyleToSettings(
    /* [in] */ IContext* context,
    /* [in] */ Int32 value)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Boolean res;
    Settings::Global::PutInt32(cr, ISettingsGlobal::POLICY_CONTROL_STYLE, value, &res);
    sDefaultImmersiveStyle = value;
}

void WindowManagerPolicyControl::AddToStatusWhiteList(
    /* [in] */ const String& packageName)
{
    if (sImmersiveStatusFilter == NULL) {
        AutoPtr<IArraySet> whitelist, blacklist;
        CArraySet::New((IArraySet**)&whitelist);
        CArraySet::New((IArraySet**)&blacklist);
        sImmersiveStatusFilter = new Filter(whitelist, blacklist);
    }

    AutoPtr<ICharSequence> cs = CoreUtils::Convert(packageName);
    Boolean contains, res;
    // Contains of CArraySet use Hashcode for binary search
    // so just need to convert String to CharSequence
    if (ISet::Probe(sImmersiveStatusFilter->mWhitelist)->Contains(cs, &contains), !contains) {
        ISet::Probe(sImmersiveStatusFilter->mWhitelist)->Add(cs, &res);
    }
}

void WindowManagerPolicyControl::AddToNavigationWhiteList(
    /* [in] */ const String& packageName)
{
    if (sImmersiveNavigationFilter == NULL) {
        AutoPtr<IArraySet> whitelist, blacklist;
        CArraySet::New((IArraySet**)&whitelist);
        CArraySet::New((IArraySet**)&blacklist);
        sImmersiveNavigationFilter = new Filter(whitelist, blacklist);
    }

    AutoPtr<ICharSequence> cs = CoreUtils::Convert(packageName);
    Boolean contains, res;
    // Contains of CArraySet use Hashcode for binary search
    // so just need to convert String to CharSequence
    if (ISet::Probe(sImmersiveNavigationFilter->mWhitelist)->Contains(cs, &contains), !contains) {
        ISet::Probe(sImmersiveNavigationFilter->mWhitelist)->Add(cs, &res);
    }
}

void WindowManagerPolicyControl::RemoveFromWhiteLists(
    /* [in] */ const String& packageName)
{
    AutoPtr<ICharSequence> cs = CoreUtils::Convert(packageName);
    Boolean res;
    if (sImmersiveStatusFilter != NULL) {
        ISet::Probe(sImmersiveStatusFilter->mWhitelist)->Remove(cs, &res);
    }

    if (sImmersiveNavigationFilter != NULL) {
        ISet::Probe(sImmersiveNavigationFilter->mWhitelist)->Remove(cs, &res);
    }
}

AutoPtr<IArraySet> WindowManagerPolicyControl::GetWhiteLists()
{
    AutoPtr<IArraySet> result;
    CArraySet::New((IArraySet**)&result);

    if (sImmersiveStatusFilter != NULL) {
        result->AddAll(sImmersiveStatusFilter->mWhitelist);
    }
    if (sImmersiveNavigationFilter != NULL
            && sImmersiveNavigationFilter != sImmersiveStatusFilter) {
        result->AddAll(sImmersiveNavigationFilter->mWhitelist);
    }

    return result;
}

Boolean WindowManagerPolicyControl::IsImmersiveFiltersActive()
{
    return sImmersiveStatusFilter != NULL || sImmersiveNavigationFilter != NULL;
}

void WindowManagerPolicyControl::WriteFilter(
    /* [in] */ const String& name,
    /* [in] */ Filter* filter,
    /* [in] */ StringBuilder& stringBuilder)
{
    Boolean wEmpty, bEmpty;
    if ((ISet::Probe(filter->mWhitelist)->IsEmpty(&wEmpty), wEmpty) && (ISet::Probe(filter->mBlacklist)->IsEmpty(&bEmpty), bEmpty)) {
        return;
    }
    stringBuilder += name;
    stringBuilder += "=";

    Boolean needComma = FALSE;
    if (ISet::Probe(filter->mWhitelist)->IsEmpty(&wEmpty), !wEmpty) {
        WritePackages(filter->mWhitelist, stringBuilder, FALSE);
        needComma = TRUE;
    }
    if (ISet::Probe(filter->mBlacklist)->IsEmpty(&bEmpty), !bEmpty) {
        if (needComma) {
            stringBuilder += ",";
        }
        WritePackages(filter->mBlacklist, stringBuilder, TRUE);
    }
}

void WindowManagerPolicyControl::WritePackages(
    /* [in] */ IArraySet* set,
    /* [in] */ StringBuilder& stringBuilder,
    /* [in] */ Boolean isBlackList)
{
    AutoPtr<IIterator> iterator;
    ISet::Probe(set)->GetIterator((IIterator**)&iterator);
    Boolean hasNext;
    while (iterator->HasNext(&hasNext), hasNext) {
        if (isBlackList) {
            stringBuilder += "-";
        }
        AutoPtr<ICharSequence> name;
        IInterface* tmp;
        iterator->GetNext(&tmp);
        stringBuilder += name;
        if (iterator->HasNext(&hasNext), hasNext) {
            stringBuilder += ",";
        }
    }
}

void WindowManagerPolicyControl::SetFilters(
    /* [in] */ const String& value)
{
    if (DEBUG) SLOGGERD(_TAG, "setFilters: %s", value.string());
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
        String statusFilter, navigationFilter, preconfirmationsFilter;
        sImmersiveStatusFilter->ToString(&statusFilter);
        sImmersiveNavigationFilter->ToString(&navigationFilter);
        sImmersivePreconfirmationsFilter->ToString(&preconfirmationsFilter);
        SLOGGERD(_TAG, "immersiveStatusFilter: %s",  statusFilter.string());
        SLOGGERD(_TAG, "immersiveNavigationFilter: %s", navigationFilter.string());
        SLOGGERD(_TAG, "immersivePreconfirmationsFilter: %s", preconfirmationsFilter.string());
    }
}


const String WindowManagerPolicyControl::Filter::ALL("*");
const String WindowManagerPolicyControl::Filter::APPS("apps");

Boolean WindowManagerPolicyControl::Filter::Matches(
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    if (attrs == NULL)
    {
        return FALSE;
    }
    Int32 type;
    attrs->GetType(&type);
    Boolean isApp = type >= IWindowManagerLayoutParams::FIRST_APPLICATION_WINDOW
            && type <= IWindowManagerLayoutParams::LAST_APPLICATION_WINDOW;
    AutoPtr<ICharSequence> cs = CoreUtils::Convert(APPS);
    Boolean inBlacklist;
    ISet::Probe(mBlacklist)->Contains(cs, &inBlacklist);
    if (isApp && inBlacklist)
    {
        return FALSE;
    }
    String pkName;
    attrs->GetPackageName(&pkName);
    if (OnBlacklist(pkName))
    {
        return FALSE;
    }
    Boolean inWhitelist = FALSE;
    ISet::Probe(mWhitelist)->Contains(cs, &inWhitelist);
    if (isApp && inWhitelist)
    {
        return TRUE;
    }
    return OnWhitelist(pkName);
}

Boolean WindowManagerPolicyControl::Filter::Matches(
    /* [in] */ const String& packageName)
{
    return !OnBlacklist(packageName) && OnWhitelist(packageName);
}

Boolean WindowManagerPolicyControl::Filter::IsEnabledForAll()
{
    AutoPtr<ICharSequence> cs = CoreUtils::Convert(ALL);
    Boolean res;
    ISet::Probe(mWhitelist)->Contains(cs, &res);
    return res;
}

Boolean WindowManagerPolicyControl::Filter::OnBlacklist(
    /* [in] */ const String& packageName)
{
    AutoPtr<ICharSequence> cPackageName = CoreUtils::Convert(packageName);
    AutoPtr<ICharSequence> cALL = CoreUtils::Convert(ALL);

    Boolean containsPackageName, containsALL;
    ISet::Probe(mBlacklist)->Contains(cPackageName, &containsPackageName);
    ISet::Probe(mBlacklist)->Contains(cALL, &containsALL);
    return containsPackageName || containsALL;
}

Boolean WindowManagerPolicyControl::Filter::OnWhitelist(
    /* [in] */ const String& packageName)
{
    AutoPtr<ICharSequence> cPackageName = CoreUtils::Convert(packageName);
    AutoPtr<ICharSequence> cALL = CoreUtils::Convert(ALL);

    Boolean containsPackageName, containsALL;
    ISet::Probe(mWhitelist)->Contains(cPackageName, &containsPackageName);
    ISet::Probe(mWhitelist)->Contains(cALL, &containsALL);
    return containsPackageName || containsALL;
}

ECode WindowManagerPolicyControl::Filter::Dump(
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

void WindowManagerPolicyControl::Filter::Dump(
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

ECode WindowManagerPolicyControl::Filter::ToString(
    /* [out] */ String* str)
{
    AutoPtr<IStringWriter> sw;
    CStringWriter::New((IStringWriter**)&sw);
    AutoPtr<IPrintWriter> pw;
    IWriter* w = IWriter::Probe(sw);
    CPrintWriter::New(w, TRUE, (IPrintWriter**)&pw);
    Dump(pw);
    sw->ToString(str);
    return NOERROR;
}

AutoPtr<WindowManagerPolicyControl::Filter> WindowManagerPolicyControl::Filter::Parse(
    /* [in] */ const String&  value)
{
    if (value.IsNullOrEmpty()) return NULL;
    AutoPtr<IArraySet> whitelist;// = new ArraySet<String>();
    CArraySet::New((IArraySet**)&whitelist);
    AutoPtr<IArraySet> blacklist;// = new ArraySet<String>();
    CArraySet::New((IArraySet**)&blacklist);
    AutoPtr<ArrayOf<String> > splitOut;
    StringUtils::Split(value, String(","), (ArrayOf<String>**)&splitOut);
    for (Int32 i = 0; i < splitOut->GetLength(); ++i)
    {
        String token = (*splitOut)[i].Trim();
        if (token.StartWith("-") && token.GetLength() > 1)
        {
            token = token.Substring(1);
            AutoPtr<ICharSequence> cs;
            CString::New(token, (ICharSequence**)&cs);
            ISet::Probe(blacklist)->Add(cs);
        }
        else
        {
            AutoPtr<ICharSequence> cs;
            CString::New(token, (ICharSequence**)&cs);
            ISet::Probe(whitelist)->Add(cs);
        }
    }
    return new Filter(whitelist, blacklist);
}

WindowManagerPolicyControl::Filter::Filter(
    /* [in] */ IArraySet* whitelist,
    /* [in] */ IArraySet* blacklist)
{
    mWhitelist = whitelist;
    mBlacklist = blacklist;
}


} // namespace View
} // namespace Droid
} // namespace Elastos