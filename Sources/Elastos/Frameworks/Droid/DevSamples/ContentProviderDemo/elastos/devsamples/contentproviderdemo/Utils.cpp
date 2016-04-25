#include "Utils.h"

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;

namespace Elastos {
namespace DevSamples {
namespace ContentProviderDemo {

const String Utils::DB_NAME("userdb");
const String Utils::TABLE_NAME("users");
const Int32 Utils::VERSION = 1;

const String Utils::TABLE_ID("tid");
const String Utils::USERNAME("username");
const String Utils::SEX("sex");
const String Utils::EMAIL("email");

const String Utils::AUTOHORITY("elastos.kesalin.contentprovider");
const Int32 Utils::ITEM;
const Int32 Utils::ITEM_ID;

const String Utils::CONTENT_TYPE("elastos.kesalin.dir");
const String Utils::CONTENT_ITEM_TYPE("elastos.kesalin.item");

static AutoPtr<IUri> InitCONTENT_URI()
{
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(String("content://elastos.kesalin.contentprovider/kesalin"), (IUri**)&uri);
    assert(uri != NULL);
    return uri;
}

const AutoPtr<IUri> Utils::CONTENT_URI = InitCONTENT_URI();

} // namespace ContentProviderDemo
} // namespace DevSamples
} // namespace Elastos