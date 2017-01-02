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

#include "elastos/droid/server/ethernet/EthernetConfigStore.h"
#include "elastos/droid/os/Environment.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Net::CIpConfiguration;
using Elastos::Droid::Net::DHCP_IpAssignment;
using Elastos::Droid::Net::NONE_ProxySettings;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::IO::IFile;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Ethernet {

const String EthernetConfigStore::TAG("EthernetConfigStore");

static String InitIpConfigFile()
{
    AutoPtr<IFile> dir = Environment::GetDataDirectory();
    String str = TO_STR(dir);
    str = str + "/misc/ethernet/ipconfig.txt";
    return str;
}
const String EthernetConfigStore::sIpConfigFile = InitIpConfigFile();

ECode EthernetConfigStore::constructor()
{
    return IpConfigStore::constructor();
}

AutoPtr<IIpConfiguration> EthernetConfigStore::ReadIpAndProxyConfigurations()
{
    AutoPtr<ISparseArray> networks;
    IpConfigStore::ReadIpAndProxyConfigurations(sIpConfigFile, (ISparseArray**)&networks);

    Int32 size;
    networks->GetSize(&size);
    if (size == 0) {
        Logger::W(TAG, "No Ethernet configuration found. Using default.");
        AutoPtr<IIpConfiguration> ipConfig;
        CIpConfiguration::New(DHCP_IpAssignment, NONE_ProxySettings, NULL, NULL, (IIpConfiguration**)&ipConfig);
        return ipConfig;
    }

    if (size > 1) {
        // Currently we only support a single Ethernet interface.
        Logger::W(TAG, "Multiple Ethernet configurations detected. Only reading first one.");
    }

    AutoPtr<IInterface> value;
    networks->ValueAt(0, (IInterface**)&value);
    return IIpConfiguration::Probe(value);
}

void EthernetConfigStore::WriteIpAndProxyConfigurations(
    /* [in] */ IIpConfiguration* config)
{
    AutoPtr<ISparseArray> networks;
    CSparseArray::New((ISparseArray**)&networks);
    networks->Put(0, config);
    IpConfigStore::WriteIpAndProxyConfigurations(sIpConfigFile, networks);
}

} // namespace Ethernet
} // namespace Server
} // namespace Droid
} // namespace Elastos
