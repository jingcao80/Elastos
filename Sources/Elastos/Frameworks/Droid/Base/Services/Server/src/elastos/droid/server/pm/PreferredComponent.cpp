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

#include "elastos/droid/server/pm/PreferredComponent.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

// {5169C3ED-4FBC-4306-A108-376EB189B823}
extern "C" const InterfaceID EIID_IPreferredComponentCallbacks =
        { 0x5169c3ed, 0x4fbc, 0x4306, { 0xa1, 0x8, 0x37, 0x6e, 0xb1, 0x89, 0xb8, 0x23 } };

const String PreferredComponent::TAG_SET("set");
const String PreferredComponent::ATTR_ALWAYS("always");
const String PreferredComponent::ATTR_MATCH("match");
const String PreferredComponent::ATTR_NAME("name");
const String PreferredComponent::ATTR_SET("set");

PreferredComponent::PreferredComponent(
    /* [in] */ ICallbacks* callbacks,
    /* [in] */ Int32 match,
    /* [in] */ ArrayOf<IComponentName*>* set,
    /* [in] */ IComponentName* component,
    /* [in] */ Boolean always)
{
    mCallbacks = callbacks;
    mMatch = match & IIntentFilter::MATCH_CATEGORY_MASK;
    mComponent = component;
    mAlways = always;
    component->FlattenToShortString(&mShortComponent);
    if (set != NULL) {
        const Int32 N = set->GetLength();
        AutoPtr<ArrayOf<String> > myPackages = ArrayOf<String>::Alloc(N);
        AutoPtr<ArrayOf<String> > myClasses = ArrayOf<String>::Alloc(N);
        AutoPtr<ArrayOf<String> > myComponents = ArrayOf<String>::Alloc(N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IComponentName> cn = (*set)[i];
            if (cn == NULL) {
                mSetPackages = NULL;
                mSetClasses = NULL;
                mSetComponents = NULL;
                return;
            }
            String packageName, className, shortStr;
            cn->GetPackageName(&packageName);
            cn->GetClassName(&className);
            cn->FlattenToShortString(&shortStr);
            (*myPackages)[i] = packageName;
            (*myClasses)[i] = className;
            cn->FlattenToShortString(&(*myComponents)[i]);
        }
        mSetPackages = myPackages;
        mSetClasses = myClasses;
        mSetComponents = myComponents;
    }
}

PreferredComponent::PreferredComponent(
    /* [in] */ ICallbacks* callbacks,
    /* [in] */ IXmlPullParser* parser)
{
    mCallbacks = callbacks;
    parser->GetAttributeValue(String(NULL), ATTR_NAME, &mShortComponent);
    AutoPtr<IComponentNameHelper> componentNameHelper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&componentNameHelper);
    componentNameHelper->UnflattenFromString(mShortComponent, (IComponentName**)&mComponent);
    if (mComponent == NULL) {
        mParseError = String("Bad activity name ") + mShortComponent;
    }
    String matchStr;
    parser->GetAttributeValue(String(NULL), ATTR_MATCH, &matchStr);
    mMatch = matchStr != NULL ? StringUtils::ParseInt32(matchStr, 16) : 0;
    String setCountStr;
    parser->GetAttributeValue(String(NULL), ATTR_SET, &setCountStr);
    Int32 setCount = setCountStr != NULL ? StringUtils::ParseInt32(setCountStr) : 0;
    String alwaysStr;
    parser->GetAttributeValue(String(NULL), ATTR_ALWAYS, &alwaysStr);
    mAlways = !alwaysStr.IsNull() ? StringUtils::ParseBoolean(alwaysStr) : TRUE;

    AutoPtr<ArrayOf<String> > myPackages = setCount > 0 ? ArrayOf<String>::Alloc(setCount) : NULL;
    AutoPtr<ArrayOf<String> > myClasses = setCount > 0 ? ArrayOf<String>::Alloc(setCount) : NULL;
    AutoPtr<ArrayOf<String> > myComponents = setCount > 0 ? ArrayOf<String>::Alloc(setCount) : NULL;

    Int32 setPos = 0;

    Int32 outerDepth, depth;
    parser->GetDepth(&outerDepth);
    Int32 type;
    Boolean res;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
        && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG
            || type == IXmlPullParser::TEXT) {
            continue;
        }

        String tagName;
        parser->GetName(&tagName);
        //Log.i(TAG, "Parse outerDepth=" + outerDepth + " depth="
        //        + parser.getDepth() + " tag=" + tagName);
        if (tagName.Equals(TAG_SET)) {
            String name;
            parser->GetAttributeValue(String(NULL), ATTR_NAME, &name);
            if (name.IsNull()) {
                if (mParseError.IsNull()) {
                    mParseError = String("No name in set tag in preferred activity ")
                        + mShortComponent;
                }
            }
            else if (setPos >= setCount) {
                if (mParseError.IsNull()) {
                    mParseError = String("Too many set tags in preferred activity ")
                        + mShortComponent;
                }
            }
            else {
                AutoPtr<IComponentName> cn;
                componentNameHelper->UnflattenFromString(name, (IComponentName**)&cn);
                if (cn == NULL) {
                    if (mParseError.IsNull()) {
                        mParseError = String("Bad set name ") + name + " in preferred activity "
                            + mShortComponent;
                    }
                }
                else {
                    String packageName, className;
                    cn->GetPackageName(&packageName);
                    cn->GetClassName(&className);
                    (*myPackages)[setPos] = packageName;
                    (*myClasses)[setPos] = className;
                    (*myComponents)[setPos] = name;
                    setPos++;
                }
            }
            XmlUtils::SkipCurrentTag(parser);
        }
        else if (!(mCallbacks->OnReadTag(tagName, parser, &res), res)) {
            String name;
            parser->GetName(&name);
            Slogger::W("PreferredComponent", String("Unknown element: ") + name);
            XmlUtils::SkipCurrentTag(parser);
        }
    }

    if (setPos != setCount) {
        if (mParseError.IsNull()) {
            mParseError.AppendFormat("Not enough set tags (expected %d"
                " but found %d ) in %s", setCount, setPos, mShortComponent.string());
        }
    }

    mSetPackages = myPackages;
    mSetClasses = myClasses;
    mSetComponents = myComponents;
}

String PreferredComponent::GetParseError()
{
    return mParseError;
}

ECode PreferredComponent::WriteToXml(
    /* [in] */ IXmlSerializer* serializer,
    /* [in] */ Boolean full)
{
    assert(serializer != NULL);
    Int32 NS = mSetClasses != NULL ? mSetClasses->GetLength() : 0;
    serializer->WriteAttribute(String(NULL), ATTR_NAME, mShortComponent);
    if (full) {
        if (mMatch != 0) {
            serializer->WriteAttribute(String(NULL), ATTR_MATCH, StringUtils::ToHexString(mMatch));
        }
        serializer->WriteAttribute(String(NULL), ATTR_ALWAYS, StringUtils::ToString(mAlways));
        serializer->WriteAttribute(String(NULL), ATTR_SET, StringUtils::ToString(NS));
        for (Int32 s = 0; s < NS; s++) {
            serializer->WriteStartTag(String(NULL), TAG_SET);
            serializer->WriteAttribute(String(NULL), ATTR_NAME, (*mSetComponents)[s]);
            serializer->WriteEndTag(String(NULL), TAG_SET);
        }
    }
    return NOERROR;
}

Boolean PreferredComponent::SameSet(
    /* [in] */ IList* query)
{
    if (mSetPackages == NULL) {
        return query == NULL;
    }
    if (query == NULL) {
        return FALSE;
    }
    Int32 NS = mSetPackages->GetLength();
    Int32 numMatch = 0;
    Boolean hasNext = FALSE;
    AutoPtr<IIterator> it;
    query->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> item;
        it->GetNext((IInterface**)&item);
        AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(item);
        AutoPtr<IActivityInfo> ai;
        ri->GetActivityInfo((IActivityInfo**)&ai);
        String packageName, name;
        AutoPtr<IPackageItemInfo> pii = IPackageItemInfo::Probe(ai);
        pii->GetPackageName(&packageName);
        pii->GetName(&name);
        Boolean good = FALSE;
        for (Int32 j = 0; j < NS; j++) {
            if ((*mSetPackages)[j].Equals(packageName) && (*mSetClasses)[j].Equals(name)) {
                numMatch++;
                good = TRUE;
                break;
            }
        }
        if (!good) return FALSE;
    }

    return numMatch == NS;
}

Boolean PreferredComponent::SameSet(
    /* [in] */ ArrayOf<IComponentName*>* comps)
{
    if (mSetPackages == NULL) return FALSE;
    Int32 NQ = comps->GetLength();
    Int32 NS = mSetPackages->GetLength();
    Int32 numMatch = 0;
    for (Int32 i = 0; i < NQ; i++) {
        AutoPtr<IComponentName> cn = (*comps)[i];
        String packageName, className;
        cn->GetPackageName(&packageName);
        cn->GetClassName(&className);
        Boolean good = FALSE;
        for (Int32 j = 0; j < NS; j++) {
            if ((*mSetPackages)[j].Equals(packageName)
                    && (*mSetClasses)[j].Equals(className)) {
                numMatch++;
                good = TRUE;
                break;
            }
        }
        if (!good) return FALSE;
    }
    return numMatch == NS;
}

void PreferredComponent::Dump(
    /* [in] */ IPrintWriter* out,
    /* [in] */ const String& prefix,
    /* [in] */ IInterface* ident)
{
    out->Print(prefix);
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int32 hashCode;
    sys->IdentityHashCode(ident, &hashCode);
    out->Print(StringUtils::ToHexString(hashCode));
    out->PrintChar(' ');
    out->Println(mShortComponent);
    out->Print(prefix);
    out->Print(String(" match=0x"));
    out->Print(StringUtils::ToHexString(mMatch));
    out->Print(String(" mAlways="));
    out->Println(mAlways);
    if (mSetComponents != NULL) {
        out->Print(prefix);
        out->Println(String("  Selected from:"));
        for (Int32 i = 0; i < mSetComponents->GetLength(); i++) {
            out->Print(prefix);
            out->Print(String("    "));
            out->Println((*mSetComponents)[i]);
        }
    }
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
