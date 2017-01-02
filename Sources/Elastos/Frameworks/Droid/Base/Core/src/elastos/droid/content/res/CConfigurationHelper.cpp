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

#include "elastos/droid/content/res/CConfigurationHelper.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CConfigurationHelper, Singleton, IConfigurationHelper)

CAR_SINGLETON_IMPL(CConfigurationHelper)

ECode CConfigurationHelper::ResetScreenLayout(
    /* [in] */ Int32 curLayout,
    /* [out] */ Int32* layout)
{
    VALIDATE_NOT_NULL(layout);
    *layout = CConfiguration::ResetScreenLayout(curLayout);
    return NOERROR;
}

ECode CConfigurationHelper::ReduceScreenLayout(
    /* [in] */ Int32 curLayout,
    /* [in] */ Int32 longSizeDp,
    /* [in] */ Int32 shortSizeDp,
    /* [out] */ Int32* layout)
{
    VALIDATE_NOT_NULL(layout);
    *layout = CConfiguration::ReduceScreenLayout(curLayout, longSizeDp, shortSizeDp);
    return NOERROR;
}

ECode CConfigurationHelper::NeedNewResources(
    /* [in] */ Int32 configChanges,
    /* [in] */ Int32 interestingChanges,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CConfiguration::NeedNewResources(configChanges, interestingChanges);
    return NOERROR;
}

ECode CConfigurationHelper::GetEmpty(
    /* [out] */ IConfiguration** empty)
{
    VALIDATE_NOT_NULL(empty);
    *empty = CConfiguration::EMPTY;
    REFCOUNT_ADD(*empty);
    return NOERROR;
}

ECode CConfigurationHelper::LocaleToResourceQualifier(
    /* [in] */ ILocale* locale,
    /* [out] */ String* resourceQualifier)
{
    VALIDATE_NOT_NULL(resourceQualifier);
    *resourceQualifier = CConfiguration::LocaleToResourceQualifier(locale);
    return NOERROR;
}

ECode CConfigurationHelper::ResourceQualifierString(
    /* [in] */ IConfiguration* config,
    /* [out] */ String* resourceQualifier)
{
    VALIDATE_NOT_NULL(resourceQualifier);
    *resourceQualifier = CConfiguration::ResourceQualifierString(config);
    return NOERROR;
}

ECode CConfigurationHelper::GenerateDelta(
    /* [in] */ IConfiguration* base,
    /* [in] */ IConfiguration* change,
    /* [out] */ IConfiguration** configuration)
{
    VALIDATE_NOT_NULL(configuration);
    AutoPtr<IConfiguration> ac = CConfiguration::GenerateDelta(base, change);
    *configuration = ac;
    REFCOUNT_ADD(*configuration);
    return NOERROR;
}

ECode CConfigurationHelper::ReadXmlAttrs(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IConfiguration* configOut)
{
    return CConfiguration::ReadXmlAttrs(parser, configOut);
}

ECode CConfigurationHelper::WriteXmlAttrs(
    /* [in] */ IXmlSerializer* xml,
    /* [in] */ IConfiguration* config)
{
    return CConfiguration::WriteXmlAttrs(xml, config);
}

}
}
}
}

