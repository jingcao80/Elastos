
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