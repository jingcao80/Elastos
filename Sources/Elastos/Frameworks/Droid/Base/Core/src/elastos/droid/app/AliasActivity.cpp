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

#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/app/AliasActivity.h"
#include "elastos/droid/content/Intent.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Intent;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(AliasActivity, Activity, IAliasActivity)

AliasActivity::AliasActivity()
{}

AliasActivity::~AliasActivity()
{}

ECode AliasActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    AutoPtr<IXmlResourceParser> parser;
    AutoPtr<IIntent> intent;
    // try {
    AutoPtr<IComponentName> cn;
    GetComponentName((IComponentName**)&cn);
    AutoPtr<IPackageManager> mgr;
    AutoPtr<IActivityInfo> ai;
    ECode ec = mgr->GetActivityInfo(cn, IPackageManager::GET_META_DATA, (IActivityInfo**)&ai);
    FAIL_GOTO(ec, _EXIT_);

    IPackageItemInfo::Probe(ai)->LoadXmlMetaData(mgr, IAliasActivity::ALIAS_META_DATA, (IXmlResourceParser**)&parser);
    FAIL_GOTO(ec, _EXIT_);

    if (parser == NULL) {
        Logger::E("AliasActivity", "Alias requires a meta-data field %s", IAliasActivity::ALIAS_META_DATA.string());
        return E_RUNTIME_EXCEPTION;
    }
    ec = ParseAlias(IXmlPullParser::Probe(parser), (IIntent**)&intent);
    FAIL_GOTO(ec, _EXIT_);

    if (intent == NULL) {
        if (parser != NULL) ICloseable::Probe(parser)->Close();
        Logger::E("AliasActivity", "No <intent> tag found in alias description");
        return E_RUNTIME_EXCEPTION;
    }

    StartActivity(intent);
    FAIL_GOTO(ec, _EXIT_);

    Finish();

_EXIT_:
    if (parser != NULL) ICloseable::Probe(parser)->Close();
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION
        || ec == (ECode)E_XML_PULL_PARSER_EXCEPTION
        || ec == (ECode)E_IO_EXCEPTION
    ) {
        Logger::E("AliasActivity", "Error parsing alias %08x", ec);
        return E_RUNTIME_EXCEPTION;
    }

    return ec;
}

ECode AliasActivity::ParseAlias(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(parser);

    AutoPtr<IIntent> intent;

    Int32 type;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
        && type != IXmlPullParser::START_TAG) {
    }

    String nodeName;
    parser->GetName(&nodeName);
    if (!nodeName.Equals("alias")) {
        Logger::E("AliasActivity", "Alias meta-data must start with <alias> tag; found %s", nodeName.string());
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Int32 outerDepth, depth;
    parser->GetDepth(&outerDepth);
    while ((parser->Next(&type)) != IXmlPullParser::END_DOCUMENT
           && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        parser->GetName(&nodeName);
        if (nodeName.Equals("intent")) {
            AutoPtr<IIntent> gotIntent;
            Intent::ParseIntent(resources, parser, attrs, (IIntent**)&gotIntent);
            if (intent == NULL) intent = gotIntent;
        }
        else {
            XmlUtils::SkipCurrentTag(parser);
        }
    }

    *result = intent;
    REFCOUNT_ADD(*result)
    return NOERROR;
}




} // namespace App
} // namespace Droid
} // namespace Elastos
