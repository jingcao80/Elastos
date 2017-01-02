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

#include "Preferences.h"
#include "XMLParser.h"

namespace Elastos {
namespace Utility {
namespace Prefs {

const Int32 Preferences::MAX_KEY_LENGTH = 80;
const Int32 Preferences::MAX_NAME_LENGTH = 80;
const Int32 Preferences::MAX_VALUE_LENGTH = 8192;

static AutoPtr<IPreferencesFactory> InitFactory()
{
    // // Try the system property first...
    // AutoPtr<IPreferencesFactory> result = ServiceLoader::LoadFromSystemProperty(PreferencesFactory.class);
    // if (result != NULL) {
    //     return result;
    // }
    // // Then use ServiceLoader for META-INF/services/...
    // for (PreferencesFactory impl : ServiceLoader.load(PreferencesFactory.class)) {
    //     return impl;
    // }
    // // Finally return a default...
    // return new FilePreferencesFactoryImpl();
    return NULL;
}

AutoPtr<IPreferencesFactory> Preferences::mFactory = InitFactory();

CAR_INTERFACE_IMPL(Preferences, Object, IPreferences)

AutoPtr<IPreferencesFactory> Preferences::SetPreferencesFactory(
    /* [in] */ IPreferencesFactory* pf)
{
    AutoPtr<IPreferencesFactory> previous = mFactory;
    mFactory = pf;
    return previous;
}

AutoPtr<IPreferencesFactory> Preferences::FindPreferencesFactory()
{
    return InitFactory();
}

Preferences::Preferences()
{
}

Preferences::~Preferences()
{
}

ECode Preferences::ImportPreferences(
    /* [in] */ IInputStream* istream)
{
    if (istream == NULL){
        // throw new MalformedURLException("Inputstream cannot be null");
        return E_MALFORMED_URL_EXCEPTION;
    }

    return XMLParser::ImportPrefs(istream);
}

ECode Preferences::SystemNodeForPackage(
    /* [in] */ IInterface* c,
    /* [out] */ IPreferences** outpre)
{
    VALIDATE_NOT_NULL(outpre)
    AutoPtr<IPreferences> pref;
    mFactory->SystemRoot((IPreferences**)&pref);
    String path = GetNodeName(c);
    AutoPtr<IPreferences> result;
    pref->GetNode(path, (IPreferences**)&result);
    *outpre = result;
    REFCOUNT_ADD(*outpre);
    return NOERROR;
}

ECode Preferences::SystemRoot(
    /* [out] */ IPreferences** outpre)
{
   return mFactory->SystemRoot(outpre);
}

ECode Preferences::UserNodeForPackage(
    /* [in] */ IInterface* c,
    /* [out] */ IPreferences** outpre)
{
    VALIDATE_NOT_NULL(outpre)
    AutoPtr<IPreferences> pref;
    mFactory->UserRoot((IPreferences**)&pref);
    String path = GetNodeName(c);
    AutoPtr<IPreferences> result;
    pref->GetNode(path, (IPreferences**)&result);
    *outpre = result;
    REFCOUNT_ADD(*outpre);
    return NOERROR;
}

String Preferences::GetNodeName(
    /* [in] */ IInterface* c)
{
    // Package p = c.getPackage();
    // if (p == null){
    //     return "/<unnamed>";
    // }
    // return "/"+p.getName().replace('.', '/');
    //TODO
    assert(0);
    return String(NULL);
}

ECode Preferences::UserRoot(
    /* [out] */ IPreferences** outpre)
{
    return mFactory->UserRoot(outpre);
}

} // namespace Prefs
} // namespace Utility
} // namespace Elastos
