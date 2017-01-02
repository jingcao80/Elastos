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

#ifndef __ELASTOS_DROID_APP_CEVENTLOG_H__
#define __ELASTOS_DROID_APP_CEVENTLOG_H__

#include "_Elastos_Droid_App_CEventLog.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CEventLog)
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CEventLog();

    ~CEventLog();

    /**
     * Get the name associated with an event type tag code.
     * @param tag code to look up
     * @return the name of the tag, or null if no tag has that number
     */
     static CARAPI GetTagName(
         /* [in] */ Int32 tag,
         /* [out] */ String* name);

    /**
     * Get the event type tag code associated with an event name.
     * @param name of event to look up
     * @return the tag code, or -1 if no tag has that name
     */
    static CARAPI GetTagCode(
        /* [in] */ const String& name,
        /* [out] */ Int32* code);

public:
    const static String TAG = "EventLog";
    const static String TAGS_FILE = "/system/etc/event-log-tags";
    const static String COMMENT_PATTERN = "^\\s*(#.*)?$";
    const static String TAG_PATTERN = "^\\s*(\\d+)\\s+(\\w+)\\s*(\\(.*\\))?\\s*$";

    static HashMap<String, Int32> sTagCodes;
    static HashMap<Int32, String> sTagNames;

private:
    static CARAPI_(void) ReadTagsFile();
};

}
}
}

#endif // __ELASTOS_DROID_APP_CEVENTLOG_H__
