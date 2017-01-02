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

#include "elastos/droid/providers/userdictionary/CDictionaryBackupAgent.h"
#include "R.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/R.h>
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CUriMatcher;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::Sqlite::CSQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
// using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace UserDictionary {

CAR_OBJECT_IMPL(CDictionaryBackupAgent)

CDictionaryBackupAgent::CDictionaryBackupAgent()
{}

CDictionaryBackupAgent::~CDictionaryBackupAgent()
{}

ECode CDictionaryBackupAgent::constructor()
{
    return ContentProvider::constructor();
}

} // namespace UserDictionary
} // namespace Providers
} // namespace Droid
} // namespace Elastos