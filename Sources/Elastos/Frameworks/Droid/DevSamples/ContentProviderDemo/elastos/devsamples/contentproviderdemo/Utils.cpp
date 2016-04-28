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