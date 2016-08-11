
#include "elastos/droid/contacts/common/extensions/ExtensionsFactory.h"
#include <elastos/utility/logging/Logger.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::IO::IInputStream;
using Elastos::Utility::CProperties;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Extensions {

const String ExtensionsFactory::TAG("ExtensionsFactory");
const String ExtensionsFactory::EXTENSIONS_PROPERTIES("contacts_extensions.properties");
const String ExtensionsFactory::EXTENDED_PHONE_DIRECTORIES_KEY("extendedPhoneDirectories");
AutoPtr<IProperties> ExtensionsFactory::sProperties;
AutoPtr<IExtendedPhoneDirectoriesManager> ExtensionsFactory::sExtendedPhoneDirectoriesManager;

void ExtensionsFactory::Init(
    /* [in] */ IContext* context)
{
    if (sProperties != null) {
        return;
    }
    // try {
    AutoPtr<IAssetManager> asset;
    context->getAssets((IAssetManager**)&asset);
    AutoPtr<IInputStream> fileStream;
    if (FAILED(asset->Open(EXTENSIONS_PROPERTIES, (IInputStream**)&fileStream))) {
        Logger::E(TAG, "No custom extensions.");
        return;
    }
    CProperties::New((IProperties**)&sProperties);
    sProperties->Load(fileStream);
    fileStream->Close();

    String className;
    sProperties->GetProperty(EXTENDED_PHONE_DIRECTORIES_KEY, &className);
    if (!className.IsNull()) {
        AutoPtr<IInterface> instance = CreateInstance(className)
        sExtendedPhoneDirectoriesManager = IExtendedPhoneDirectoriesManager::Probe(instance);
    }
    else {
        Logger::D(TAG, "%s not found in properties file.", EXTENDED_PHONE_DIRECTORIES_KEY.string());
    }

    // } catch (FileNotFoundException e) {
    //     // No custom extensions. Ignore.
    //     Log.d(TAG, "No custom extensions.");
    // } catch (IOException e) {
    //     Log.d(TAG, e.toString());
    // }
}

AutoPtr<IExtendedPhoneDirectoriesManager> ExtensionsFactory::GetExtendedPhoneDirectoriesManager()
{
    return sExtendedPhoneDirectoriesManager;
}

AutoPtr<IInterface> ExtensionsFactory::CreateInstance(
    /* [in] */ const String& className)
{
    // try {
    //     Class<?> c = Class.forName(className);
    //     //noinspection unchecked
    //     return (T) c.newInstance();
    // } catch (ClassNotFoundException e) {
    //     Log.e(TAG, className + ": unable to create instance.", e);
    // } catch (IllegalAccessException e) {
    //     Log.e(TAG, className + ": unable to create instance.", e);
    // } catch (InstantiationException e) {
    //     Log.e(TAG, className + ": unable to create instance.", e);
    // }
    return NULL;
}

} // namespace Extensions
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
