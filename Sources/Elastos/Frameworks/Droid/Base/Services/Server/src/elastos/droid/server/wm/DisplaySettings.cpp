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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.Internal.h>
#include "elastos/droid/server/wm/DisplaySettings.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Slogger;
using Org::Xmlpull::V1::IXmlSerializer;
using Elastos::Droid::Internal::Utility::IFastXmlSerializer;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::Xml;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

const String DisplaySettings::TAG(CWindowManagerService::TAG);

DisplaySettings::DisplaySettings(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dataDir;
    env->GetDataDirectory((IFile**)&dataDir);
    AutoPtr<IFile> systemDir;
    CFile::New(dataDir, String("system"), (IFile**)&systemDir);
    AutoPtr<IFile> settingsDir;
    CFile::New(systemDir, String("display_settings.xml"), (IFile**)&settingsDir);
    CAtomicFile::New(settingsDir, (IAtomicFile**)&mFile);
}

void DisplaySettings::GetOverscanLocked(
    /* [in] */ const String& name,
    /* [in] */ IRect* outRect)
{
    AutoPtr<Entry> entry;
    HashMap<String, AutoPtr<Entry> >::Iterator it = mEntries.Find(name);
    if (it != mEntries.End()) {
        entry = it->mSecond;
    }
    if (entry != NULL) {
        outRect->SetLeft(entry->mOverscanLeft);
        outRect->SetTop(entry->mOverscanTop);
        outRect->SetRight(entry->mOverscanRight);
        outRect->SetBottom(entry->mOverscanBottom);
    }
    else {
        outRect->Set(0, 0, 0, 0);
    }
}

void DisplaySettings::SetOverscanLocked(
    /* [in] */ const String& name,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    if (left == 0 && top == 0 && right == 0 && bottom == 0) {
        // Right now all we are storing is overscan; if there is no overscan,
        // we have no need for the entry.
        mEntries.Erase(name);
        return;
    }
    AutoPtr<Entry> entry;
    HashMap<String, AutoPtr<Entry> >::Iterator it = mEntries.Find(name);
    if (it != mEntries.End()) {
        entry = it->mSecond;
    }
    if (entry == NULL) {
        entry = new Entry(name);
        mEntries[name] = entry;
    }
    entry->mOverscanLeft = left;
    entry->mOverscanTop = top;
    entry->mOverscanRight = right;
    entry->mOverscanBottom = bottom;
}

ECode DisplaySettings::ReadSettingsLocked()
{
    AutoPtr<IFileInputStream> stream;
    // try {
    ECode ec = mFile->OpenRead((IFileInputStream**)&stream);
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        Slogger::I(TAG, "No existing display settings %s; starting empty", TO_CSTR(mFile));
        return NOERROR;
    }
    // } catch (FileNotFoundException e) {
    //     Slog.i(TAG, "No existing display settings " + mFile.getBaseFile()
    //             + "; starting empty");
    //     return;
    // }
    Boolean success = FALSE;
    // try {
    AutoPtr<IXmlPullParser> parser;
    Xml::NewPullParser((IXmlPullParser**)&parser);
    parser->SetInput(IInputStream::Probe(stream), String(NULL));
    Int32 type;
    while ((parser->Next(&type), type != IXmlPullParser::START_TAG)
            && type != IXmlPullParser::END_DOCUMENT) {
        ;
    }

    if (type != IXmlPullParser::START_TAG) {
        Slogger::E(TAG, "no start tag found");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int32 outerDepth, depth;
    parser->GetDepth(&outerDepth);
    while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth > outerDepth))) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals("display")) {
            ReadDisplay(parser);
        }
        else {
            Slogger::W(TAG, "Unknown element under <display-settings>: %s", tagName.string());
            XmlUtils::SkipCurrentTag(parser);
        }
    }
    success = TRUE;
    // } catch (IllegalStateException e) {
    //     Slog.w(TAG, "Failed parsing " + e);
    // } catch (NullPointerException e) {
    //     Slog.w(TAG, "Failed parsing " + e);
    // } catch (NumberFormatException e) {
    //     Slog.w(TAG, "Failed parsing " + e);
    // } catch (XmlPullParserException e) {
    //     Slog.w(TAG, "Failed parsing " + e);
    // } catch (IOException e) {
    //     Slog.w(TAG, "Failed parsing " + e);
    // } catch (IndexOutOfBoundsException e) {
    //     Slog.w(TAG, "Failed parsing " + e);
    // } finally {
    //     if (!success) {
    //         mEntries.clear();
    //     }
    //     try {
    //         stream.close();
    //     } catch (IOException e) {
    //     }
    // }
    if (!success) {
        mEntries.Clear();
    }
    // try {
    ICloseable::Probe(stream)->Close();
    // } catch (IOException e) {
    // }
    return NOERROR;
}

Int32 DisplaySettings::GetIntAttribute(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& name)
{
    // try {
    String str;
    if (FAILED(parser->GetAttributeValue(String(NULL), name, &str))) {
        return 0;
    }
    return !str.IsNull() ? StringUtils::ParseInt32(str) : 0;
    // } catch (NumberFormatException e) {
    //     return 0;
    // }
}

ECode DisplaySettings::ReadDisplay(
    /* [in] */ IXmlPullParser* parser)
{
    String name;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("name"), &name))
    if (!name.IsNull()) {
        AutoPtr<Entry> entry = new Entry(name);
        entry->mOverscanLeft = GetIntAttribute(parser, String("overscanLeft"));
        entry->mOverscanTop = GetIntAttribute(parser, String("overscanTop"));
        entry->mOverscanRight = GetIntAttribute(parser, String("overscanRight"));
        entry->mOverscanBottom = GetIntAttribute(parser, String("overscanBottom"));
        mEntries[name] = entry;
    }
    return XmlUtils::SkipCurrentTag(parser);
}

void DisplaySettings::WriteSettingsLocked()
{
    AutoPtr<IFileOutputStream> stream;
    // try {
    if (FAILED(mFile->StartWrite((IFileOutputStream**)&stream))) {
        Slogger::E(TAG, "Failed to write display settings");
        return;
    }
    // } catch (IOException e) {
    //     Slog.w(TAG, "Failed to write display settings: " + e);
    //     return;
    // }

    // try {
    AutoPtr<IFastXmlSerializer> fxs;
    CFastXmlSerializer::New((IFastXmlSerializer**)&fxs);
    AutoPtr<IXmlSerializer> out = IXmlSerializer::Probe(fxs);
    out->SetOutput(IOutputStream::Probe(stream), String("utf-8"));
    out->StartDocument(String(NULL), TRUE);
    out->WriteStartTag(String(NULL), String("display-settings"));

    HashMap<String, AutoPtr<Entry> >::Iterator it = mEntries.Begin();
    for (; it != mEntries.End(); ++it) {
        AutoPtr<Entry> entry = it->mSecond;
        out->WriteStartTag(String(NULL), String("display"));
        out->WriteAttribute(String(NULL), String("name"), entry->mName);
        if (entry->mOverscanLeft != 0) {
            out->WriteAttribute(String(NULL), String("overscanLeft"), StringUtils::ToString(entry->mOverscanLeft));
        }
        if (entry->mOverscanTop != 0) {
            out->WriteAttribute(String(NULL), String("overscanTop"), StringUtils::ToString(entry->mOverscanTop));
        }
        if (entry->mOverscanRight != 0) {
            out->WriteAttribute(String(NULL), String("overscanRight"), StringUtils::ToString(entry->mOverscanRight));
        }
        if (entry->mOverscanBottom != 0) {
            out->WriteAttribute(String(NULL), String("overscanBottom"), StringUtils::ToString(entry->mOverscanBottom));
        }
        out->WriteEndTag(String(NULL), String("display"));
    }

    out->WriteEndTag(String(NULL), String("display-settings"));
    out->EndDocument();
    mFile->FinishWrite(stream);
    // } catch (IOException e) {
    //     Slog.w(TAG, "Failed to write display settings, restoring backup.", e);
    //     mFile.failWrite(stream);
    // }
}

} // Wm
} // Server
} // Droid
} // Elastos
