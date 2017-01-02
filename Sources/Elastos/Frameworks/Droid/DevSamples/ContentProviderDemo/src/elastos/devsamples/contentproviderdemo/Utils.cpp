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

#include "Utils.h"

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;

namespace Elastos {
namespace DevSamples {
namespace ContentProviderDemo {

const String Utils::DB_NAME("usersdb");
const String Utils::TABLE_NAME("user");
const Int32 Utils::VERSION = 1;

const String Utils::TAG_ID("_id");
const String Utils::USERNAME("username");
const String Utils::SEX("sex");
const String Utils::EMAIL("email");

const String Utils::AUTOHORITY("elastos.kesalin.users");
const Int32 Utils::ITEM;
const Int32 Utils::ITEM_ID;

const String Utils::CONTENT_TYPE("vnd.android.cursor.dir/vnd.kesalin.users");
const String Utils::CONTENT_ITEM_TYPE("vnd.android.cursor.item/vnd.kesalin.users");

static AutoPtr<IUri> InitCONTENT_URI()
{
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(String("content://elastos.kesalin.users/user"), (IUri**)&uri);
    assert(uri != NULL);
    return uri;
}

const AutoPtr<IUri> Utils::CONTENT_URI = InitCONTENT_URI();

} // namespace ContentProviderDemo
} // namespace DevSamples
} // namespace Elastos