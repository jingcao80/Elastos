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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPREQ_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPREQ_H__

#include "_Org_Apache_Harmony_Security_X509_Tsp_CTimeStampReq.h"
#include "org/apache/harmony/security/x509/tsp/TimeStampReq.h"

using Org::Apache::Harmony::Security::X509::Tsp::TimeStampReq;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CarClass(CTimeStampReq)
    , public TimeStampReq
{
public:
    CAR_OBJECT_DECL()
};

} //namespace Tsp
} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTIMESTAMPREQ_H__
