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

#include "MimeUtils.h"
#include "CFile.h"
#include "CFileHelper.h"
#include "CFileInputStream.h"
#include "CBufferedReader.h"
#include "CInputStreamReader.h"
#include "StringBuilder.h"
#include "CSystem.h"
#include "Character.h"

using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::IO::EIID_ICloseable;
using Elastos::IO::IFileHelper;
using Elastos::IO::CFileHelper;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CBufferedReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::IReader;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::Core::Character;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;

namespace Libcore {
namespace Net {

HashMap<String, String> MimeUtils::mMimeTypeToExtensionMap;
HashMap<String, String> MimeUtils::mExtensionToMimeTypeMap;
Boolean MimeUtils::sInited = FALSE;

ECode MimeUtils::HasMineType(
    /* [in] */ const String& mimeType,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result);
    *result = HasMineType(mimeType);
    return NOERROR;
}

Boolean MimeUtils::HasMineType(
    /* [in] */ const String& mimeType)
{
    if (mimeType.IsNullOrEmpty()) {
        return FALSE;
    }

    HashMap<String, String>::Iterator it = mMimeTypeToExtensionMap.Find(mimeType);
    return (it != mMimeTypeToExtensionMap.End());
}

ECode MimeUtils::GuessMimeTypeFromExtension(
    /* [in] */ const String& extension,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = GuessMimeTypeFromExtension(extension);
    return NOERROR;
}

String MimeUtils::GuessMimeTypeFromExtension(
    /* [in] */ const String& extension)
{
    if (extension.IsNullOrEmpty()) {
        return String(NULL);
    }

    return mExtensionToMimeTypeMap[extension];
}

ECode MimeUtils::HasExtension(
    /* [in] */ const String& extension,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = HasExtension(extension);
    return NOERROR;
}

Boolean MimeUtils::HasExtension(
    /* [in] */ const String& extension)
{
    if (extension.IsNullOrEmpty()) {
        return FALSE;
    }

    HashMap<String, String>::Iterator it = mExtensionToMimeTypeMap.Find(extension);
    return (it == mExtensionToMimeTypeMap.End());
}

ECode MimeUtils::GuessExtensionFromMimeType(
    /* [in] */ const String& mimeType,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = GuessExtensionFromMimeType(mimeType);
    return NOERROR;
}

String MimeUtils::GuessExtensionFromMimeType(
    /* [in] */ const String& mimeType)
{
    if (mimeType.IsNullOrEmpty()) {
        return String(NULL);
    }

    return mMimeTypeToExtensionMap[mimeType];
}

void MimeUtils::Add(
    /* [in] */ const String& mimeType,
    /* [in] */ const String& extension)
{
    // If we have an existing x -> y mapping, we do not want to
    // override it with another mapping x -> y2.
    // If a mime type maps to several extensions
    // the first extension added is considered the most popular
    // so we do not want to overwrite it later.
    HashMap<String, String>::Iterator find = mMimeTypeToExtensionMap.Find(mimeType);
    if(find == mMimeTypeToExtensionMap.End()) {
        mMimeTypeToExtensionMap[mimeType] = extension;
    }

    find = mExtensionToMimeTypeMap.Find(extension);
    if(find == mExtensionToMimeTypeMap.End()) {
        mExtensionToMimeTypeMap[extension] = mimeType;
    }
}

void MimeUtils::Init()
{
    if (sInited) return;
    sInited = TRUE;

    Add(String("application/andrew-inset"), String("ez"));
    Add(String("application/dsptype"), String("tsp"));
    Add(String("application/hta"), String("hta"));
    Add(String("application/mac-binhex40"), String("hqx"));
    Add(String("application/mathematica"), String("nb"));
    Add(String("application/msaccess"), String("mdb"));
    Add(String("application/oda"), String("oda"));
    Add(String("application/ogg"), String("ogg"));
    Add(String("application/ogg"), String("oga"));
    Add(String("application/pdf"), String("pdf"));
    Add(String("application/pgp-keys"), String("key"));
    Add(String("application/pgp-signature"), String("pgp"));
    Add(String("application/pics-rules"), String("prf"));
    Add(String("application/pkix-cert"), String("cer"));
    Add(String("application/rar"), String("rar"));
    Add(String("application/rdf+xml"), String("rdf"));
    Add(String("application/rss+xml"), String("rss"));
    Add(String("application/zip"), String("zip"));
    Add(String("application/vnd.android.package-archive"), String("apk"));
    Add(String("application/vnd.cinderella"), String("cdy"));
    Add(String("application/vnd.ms-pki.stl"), String("stl"));
    Add(String("application/vnd.oasis.opendocument.database"), String("odb"));
    Add(String("application/vnd.oasis.opendocument.formula"), String("odf"));
    Add(String("application/vnd.oasis.opendocument.graphics"), String("odg"));
    Add(String("application/vnd.oasis.opendocument.graphics-template"), String("otg"));
    Add(String("application/vnd.oasis.opendocument.image"), String("odi"));
    Add(String("application/vnd.oasis.opendocument.spreadsheet"), String("ods"));
    Add(String("application/vnd.oasis.opendocument.spreadsheet-template"), String("ots"));
    Add(String("application/vnd.oasis.opendocument.text"), String("odt"));
    Add(String("application/vnd.oasis.opendocument.text-master"), String("odm"));
    Add(String("application/vnd.oasis.opendocument.text-template"), String("ott"));
    Add(String("application/vnd.oasis.opendocument.text-web"), String("oth"));
    Add(String("application/vnd.google-earth.kml+xml"), String("kml"));
    Add(String("application/vnd.google-earth.kmz"), String("kmz"));
    Add(String("application/msword"), String("doc"));
    Add(String("application/msword"), String("dot"));
    Add(String("application/vnd.openxmlformats-officedocument.wordprocessingml.document"), String("docx"));
    Add(String("application/vnd.openxmlformats-officedocument.wordprocessingml.template"), String("dotx"));
    Add(String("application/vnd.ms-excel"), String("xls"));
    Add(String("application/vnd.ms-excel"), String("xlt"));
    Add(String("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"), String("xlsx"));
    Add(String("application/vnd.openxmlformats-officedocument.spreadsheetml.template"), String("xltx"));
    Add(String("application/vnd.ms-powerpoint"), String("ppt"));
    Add(String("application/vnd.ms-powerpoint"), String("pot"));
    Add(String("application/vnd.ms-powerpoint"), String("pps"));
    Add(String("application/vnd.openxmlformats-officedocument.presentationml.presentation"), String("pptx"));
    Add(String("application/vnd.openxmlformats-officedocument.presentationml.template"), String("potx"));
    Add(String("application/vnd.openxmlformats-officedocument.presentationml.slideshow"), String("ppsx"));
    Add(String("application/vnd.rim.cod"), String("cod"));
    Add(String("application/vnd.smaf"), String("mmf"));
    Add(String("application/vnd.stardivision.calc"), String("sdc"));
    Add(String("application/vnd.stardivision.draw"), String("sda"));
    Add(String("application/vnd.stardivision.impress"), String("sdd"));
    Add(String("application/vnd.stardivision.impress"), String("sdp"));
    Add(String("application/vnd.stardivision.math"), String("smf"));
    Add(String("application/vnd.stardivision.writer"), String("sdw"));
    Add(String("application/vnd.stardivision.writer"), String("vor"));
    Add(String("application/vnd.stardivision.writer-global"), String("sgl"));
    Add(String("application/vnd.sun.xml.calc"), String("sxc"));
    Add(String("application/vnd.sun.xml.calc.template"), String("stc"));
    Add(String("application/vnd.sun.xml.draw"), String("sxd"));
    Add(String("application/vnd.sun.xml.draw.template"), String("std"));
    Add(String("application/vnd.sun.xml.impress"), String("sxi"));
    Add(String("application/vnd.sun.xml.impress.template"), String("sti"));
    Add(String("application/vnd.sun.xml.math"), String("sxm"));
    Add(String("application/vnd.sun.xml.writer"), String("sxw"));
    Add(String("application/vnd.sun.xml.writer.global"), String("sxg"));
    Add(String("application/vnd.sun.xml.writer.template"), String("stw"));
    Add(String("application/vnd.visio"), String("vsd"));
    Add(String("application/x-abiword"), String("abw"));
    Add(String("application/x-apple-diskimage"), String("dmg"));
    Add(String("application/x-bcpio"), String("bcpio"));
    Add(String("application/x-bittorrent"), String("torrent"));
    Add(String("application/x-cdf"), String("cdf"));
    Add(String("application/x-cdlink"), String("vcd"));
    Add(String("application/x-chess-pgn"), String("pgn"));
    Add(String("application/x-cpio"), String("cpio"));
    Add(String("application/x-debian-package"), String("deb"));
    Add(String("application/x-debian-package"), String("udeb"));
    Add(String("application/x-director"), String("dcr"));
    Add(String("application/x-director"), String("dir"));
    Add(String("application/x-director"), String("dxr"));
    Add(String("application/x-dms"), String("dms"));
    Add(String("application/x-doom"), String("wad"));
    Add(String("application/x-dvi"), String("dvi"));
    Add(String("application/x-font"), String("pfa"));
    Add(String("application/x-font"), String("pfb"));
    Add(String("application/x-font"), String("gsf"));
    Add(String("application/x-font"), String("pcf"));
    Add(String("application/x-font"), String("pcf.Z"));
    Add(String("application/x-freemind"), String("mm"));
    // application/futuresplash isn't IANA, so application/x-futuresplash should come first.
    Add(String("application/x-futuresplash"), String("spl"));
    Add(String("application/futuresplash"), String("spl"));
    Add(String("application/x-gnumeric"), String("gnumeric"));
    Add(String("application/x-go-sgf"), String("sgf"));
    Add(String("application/x-graphing-calculator"), String("gcf"));
    Add(String("application/x-gtar"), String("tgz"));
    Add(String("application/x-gtar"), String("gtar"));
    Add(String("application/x-gtar"), String("taz"));
    Add(String("application/x-hdf"), String("hdf"));
    Add(String("application/x-ica"), String("ica"));
    Add(String("application/x-internet-signup"), String("ins"));
    Add(String("application/x-internet-signup"), String("isp"));
    Add(String("application/x-iphone"), String("iii"));
    Add(String("application/x-iso9660-image"), String("iso"));
    Add(String("application/x-jmol"), String("jmz"));
    Add(String("application/x-kchart"), String("chrt"));
    Add(String("application/x-killustrator"), String("kil"));
    Add(String("application/x-koan"), String("skp"));
    Add(String("application/x-koan"), String("skd"));
    Add(String("application/x-koan"), String("skt"));
    Add(String("application/x-koan"), String("skm"));
    Add(String("application/x-kpresenter"), String("kpr"));
    Add(String("application/x-kpresenter"), String("kpt"));
    Add(String("application/x-kspread"), String("ksp"));
    Add(String("application/x-kword"), String("kwd"));
    Add(String("application/x-kword"), String("kwt"));
    Add(String("application/x-latex"), String("latex"));
    Add(String("application/x-lha"), String("lha"));
    Add(String("application/x-lzh"), String("lzh"));
    Add(String("application/x-lzx"), String("lzx"));
    Add(String("application/x-maker"), String("frm"));
    Add(String("application/x-maker"), String("maker"));
    Add(String("application/x-maker"), String("frame"));
    Add(String("application/x-maker"), String("fb"));
    Add(String("application/x-maker"), String("book"));
    Add(String("application/x-maker"), String("fbdoc"));
    Add(String("application/x-mif"), String("mif"));
    Add(String("application/x-ms-wmd"), String("wmd"));
    Add(String("application/x-ms-wmz"), String("wmz"));
    Add(String("application/x-msi"), String("msi"));
    Add(String("application/x-ns-proxy-autoconfig"), String("pac"));
    Add(String("application/x-nwc"), String("nwc"));
    Add(String("application/x-object"), String("o"));
    Add(String("application/x-oz-application"), String("oza"));
    Add(String("application/x-pem-file"), String("pem"));
    Add(String("application/x-pkcs12"), String("p12"));
    Add(String("application/x-pkcs12"), String("pfx"));
    Add(String("application/x-pkcs7-certreqresp"), String("p7r"));
    Add(String("application/x-pkcs7-crl"), String("crl"));
    Add(String("application/x-quicktimeplayer"), String("qtl"));
    Add(String("application/x-shar"), String("shar"));
    Add(String("application/x-shockwave-flash"), String("swf"));
    Add(String("application/x-stuffit"), String("sit"));
    Add(String("application/x-sv4cpio"), String("sv4cpio"));
    Add(String("application/x-sv4crc"), String("sv4crc"));
    Add(String("application/x-tar"), String("tar"));
    Add(String("application/x-texinfo"), String("texinfo"));
    Add(String("application/x-texinfo"), String("texi"));
    Add(String("application/x-troff"), String("t"));
    Add(String("application/x-troff"), String("roff"));
    Add(String("application/x-troff-man"), String("man"));
    Add(String("application/x-ustar"), String("ustar"));
    Add(String("application/x-wais-source"), String("src"));
    Add(String("application/x-wingz"), String("wz"));
    Add(String("application/x-webarchive"), String("webarchive"));
    Add(String("application/x-webarchive-xml"), String("webarchivexml"));
    Add(String("application/x-x509-ca-cert"), String("crt"));
    Add(String("application/x-x509-user-cert"), String("crt"));
    Add(String("application/x-x509-server-cert"), String("crt"));
    Add(String("application/x-xcf"), String("xcf"));
    Add(String("application/x-xfig"), String("fig"));
    Add(String("application/xhtml+xml"), String("xhtml"));
    Add(String("audio/3gpp"), String("3gpp"));
    Add(String("audio/aac"), String("aac"));
    Add(String("audio/aac-adts"), String("aac"));
    Add(String("audio/amr"), String("amr"));
    Add(String("audio/amr-wb"), String("awb"));
    Add(String("audio/basic"), String("snd"));
    Add(String("audio/flac"), String("flac"));
    Add(String("application/x-flac"), String("flac"));
    Add(String("audio/imelody"), String("imy"));
    Add(String("audio/midi"), String("mid"));
    Add(String("audio/midi"), String("midi"));
    Add(String("audio/midi"), String("ota"));
    Add(String("audio/midi"), String("kar"));
    Add(String("audio/midi"), String("rtttl"));
    Add(String("audio/midi"), String("xmf"));
    Add(String("audio/mobile-xmf"), String("mxmf"));
    // add ".mp3" first so it will be the default for guessExtensionFromMimeType
    Add(String("audio/mpeg"), String("mp3"));
    Add(String("audio/mpeg"), String("mpga"));
    Add(String("audio/mpeg"), String("mpega"));
    Add(String("audio/mpeg"), String("mp2"));
    Add(String("audio/mpeg"), String("m4a"));
    Add(String("audio/mpegurl"), String("m3u"));
    Add(String("audio/prs.sid"), String("sid"));
    Add(String("audio/x-aiff"), String("aif"));
    Add(String("audio/x-aiff"), String("aiff"));
    Add(String("audio/x-aiff"), String("aifc"));
    Add(String("audio/x-gsm"), String("gsm"));
    Add(String("audio/x-matroska"), String("mka"));
    Add(String("audio/x-mpegurl"), String("m3u"));
    Add(String("audio/x-ms-wma"), String("wma"));
    Add(String("audio/x-ms-wax"), String("wax"));
    Add(String("audio/x-pn-realaudio"), String("ra"));
    Add(String("audio/x-pn-realaudio"), String("rm"));
    Add(String("audio/x-pn-realaudio"), String("ram"));
    Add(String("audio/x-realaudio"), String("ra"));
    Add(String("audio/x-scpls"), String("pls"));
    Add(String("audio/x-sd2"), String("sd2"));
    Add(String("audio/x-wav"), String("wav"));
    // image/bmp isn't IANA, so image/x-ms-bmp should come first.
    Add(String("image/x-ms-bmp"), String("bmp"));
    Add(String("image/bmp"), String("bmp"));
    Add(String("image/gif"), String("gif"));
    // image/ico isn't IANA, so image/x-icon should come first.
    Add(String("image/x-icon"), String("ico"));
    Add(String("image/ico"), String("cur"));
    Add(String("image/ico"), String("ico"));
    Add(String("image/ief"), String("ief"));
    Add(String("image/jpeg"), String("jpeg"));
    Add(String("image/jpeg"), String("jpg"));
    Add(String("image/jpeg"), String("jpe"));
    Add(String("image/pcx"), String("pcx"));
    Add(String("image/png"), String("png"));
    Add(String("image/svg+xml"), String("svg"));
    Add(String("image/svg+xml"), String("svgz"));
    Add(String("image/tiff"), String("tiff"));
    Add(String("image/tiff"), String("tif"));
    Add(String("image/vnd.djvu"), String("djvu"));
    Add(String("image/vnd.djvu"), String("djv"));
    Add(String("image/vnd.wap.wbmp"), String("wbmp"));
    Add(String("image/webp"), String("webp"));
    Add(String("image/x-cmu-raster"), String("ras"));
    Add(String("image/x-coreldraw"), String("cdr"));
    Add(String("image/x-coreldrawpattern"), String("pat"));
    Add(String("image/x-coreldrawtemplate"), String("cdt"));
    Add(String("image/x-corelphotopaint"), String("cpt"));
    Add(String("image/x-jg"), String("art"));
    Add(String("image/x-jng"), String("jng"));
    Add(String("image/x-photoshop"), String("psd"));
    Add(String("image/x-portable-anymap"), String("pnm"));
    Add(String("image/x-portable-bitmap"), String("pbm"));
    Add(String("image/x-portable-graymap"), String("pgm"));
    Add(String("image/x-portable-pixmap"), String("ppm"));
    Add(String("image/x-rgb"), String("rgb"));
    Add(String("image/x-xbitmap"), String("xbm"));
    Add(String("image/x-xpixmap"), String("xpm"));
    Add(String("image/x-xwindowdump"), String("xwd"));
    Add(String("model/iges"), String("igs"));
    Add(String("model/iges"), String("iges"));
    Add(String("model/mesh"), String("msh"));
    Add(String("model/mesh"), String("mesh"));
    Add(String("model/mesh"), String("silo"));
    Add(String("text/calendar"), String("ics"));
    Add(String("text/calendar"), String("icz"));
    Add(String("text/comma-separated-values"), String("csv"));
    Add(String("text/css"), String("css"));
    Add(String("text/html"), String("htm"));
    Add(String("text/html"), String("html"));
    Add(String("text/h323"), String("323"));
    Add(String("text/iuls"), String("uls"));
    Add(String("text/mathml"), String("mml"));
    // Add ".txt" first so it will be the default for guessExtensionFromMimeType
    Add(String("text/plain"), String("txt"));
    Add(String("text/plain"), String("asc"));
    Add(String("text/plain"), String("text"));
    Add(String("text/plain"), String("diff"));
    Add(String("text/plain"), String("po"));     // reserve "pot" for vnd.ms-powerpoint
    Add(String("text/richtext"), String("rtx"));
    Add(String("text/rtf"), String("rtf"));
    Add(String("text/text"), String("phps"));
    Add(String("text/tab-separated-values"), String("tsv"));
    Add(String("text/xml"), String("xml"));
    Add(String("text/x-bibtex"), String("bib"));
    Add(String("text/x-boo"), String("boo"));
    Add(String("text/x-c++hdr"), String("hpp"));
    Add(String("text/x-c++hdr"), String("h++"));
    Add(String("text/x-c++hdr"), String("hxx"));
    Add(String("text/x-c++hdr"), String("hh"));
    Add(String("text/x-c++src"), String("cpp"));
    Add(String("text/x-c++src"), String("c++"));
    Add(String("text/x-c++src"), String("cc"));
    Add(String("text/x-c++src"), String("cxx"));
    Add(String("text/x-chdr"), String("h"));
    Add(String("text/x-component"), String("htc"));
    Add(String("text/x-csh"), String("csh"));
    Add(String("text/x-csrc"), String("c"));
    Add(String("text/x-dsrc"), String("d"));
    Add(String("text/x-haskell"), String("hs"));
    Add(String("text/x-java"), String("java"));
    Add(String("text/x-literate-haskell"), String("lhs"));
    Add(String("text/x-moc"), String("moc"));
    Add(String("text/x-pascal"), String("p"));
    Add(String("text/x-pascal"), String("pas"));
    Add(String("text/x-pcs-gcd"), String("gcd"));
    Add(String("text/x-setext"), String("etx"));
    Add(String("text/x-tcl"), String("tcl"));
    Add(String("text/x-tex"), String("tex"));
    Add(String("text/x-tex"), String("ltx"));
    Add(String("text/x-tex"), String("sty"));
    Add(String("text/x-tex"), String("cls"));
    Add(String("text/x-vcalendar"), String("vcs"));
    Add(String("text/x-vcard"), String("vcf"));
    Add(String("video/3gpp"), String("3gpp"));
    Add(String("video/3gpp"), String("3gp"));
    Add(String("video/3gpp2"), String("3gpp2"));
    Add(String("video/3gpp2"), String("3g2"));
    Add(String("video/avi"), String("avi"));
    Add(String("video/dl"), String("dl"));
    Add(String("video/dv"), String("dif"));
    Add(String("video/dv"), String("dv"));
    Add(String("video/fli"), String("fli"));
    Add(String("video/m4v"), String("m4v"));
    Add(String("video/mp2ts"), String("ts"));
    Add(String("video/mpeg"), String("mpeg"));
    Add(String("video/mpeg"), String("mpg"));
    Add(String("video/mpeg"), String("mpe"));
    Add(String("video/mp4"), String("mp4"));
    Add(String("video/mpeg"), String("VOB"));
    Add(String("video/quicktime"), String("qt"));
    Add(String("video/quicktime"), String("mov"));
    Add(String("video/vnd.mpegurl"), String("mxu"));
    Add(String("video/webm"), String("webm"));
    Add(String("video/x-la-asf"), String("lsf"));
    Add(String("video/x-la-asf"), String("lsx"));
    Add(String("video/x-matroska"), String("mkv"));
    Add(String("video/x-mng"), String("mng"));
    Add(String("video/x-ms-asf"), String("asf"));
    Add(String("video/x-ms-asf"), String("asx"));
    Add(String("video/x-ms-wm"), String("wm"));
    Add(String("video/x-ms-wmv"), String("wmv"));
    Add(String("video/x-ms-wmx"), String("wmx"));
    Add(String("video/x-ms-wvx"), String("wvx"));
    Add(String("video/x-sgi-movie"), String("movie"));
    Add(String("video/x-webex"), String("wrf"));
    Add(String("x-epoc/x-sisx-app"), String("sisx"));
    Add(String("application/octet-stream"), String("ttf"));

    ApplyOverrides();
}

ECode MimeUtils::ApplyOverrides()
{
    AutoPtr<IInputStream> stream;
    ECode ec = GetContentTypesPropertiesStream((IInputStream**)&stream);
    if (FAILED(ec)) {
        AutoPtr<ICloseable> ic = (ICloseable*)stream->Probe(EIID_ICloseable);
        ic->Close();
        return ec;
    }

    if (stream == NULL) {
        return NOERROR;
    }

    HashMap<String, String> map;
    ec = Load(stream, map);
    if (FAILED(ec)) {
        AutoPtr<ICloseable> ic = (ICloseable*)stream->Probe(EIID_ICloseable);
        ic->Close();
        return ec;
    }

    HashMap<String, String>::Iterator it = map.Begin();
    for( ; it != map.End(); it++) {
        Add(it->mFirst, it->mSecond);
    }
    return NOERROR;
}

ECode MimeUtils::Load(
     /* [in] */ IInputStream* stream,
     /* [in] */ HashMap<String, String>& map)
{
    if (stream == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    const Int32 NONE = 0, SLASH = 1, UNICODE = 2, CONTINUE = 3,
                KEY_DONE = 4, IGNORE = 5;

    AutoPtr<IReader> sr;
    CInputStreamReader::New(stream, (IReader**)&sr);
    AutoPtr<IBufferedReader> br;
    CBufferedReader::New(sr, (IBufferedReader**)&br);

    Int32 mode = NONE, unicode = 0, count = 0;
    Byte nextChar;
    AutoPtr<ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(40);
    Int32 offset = 0, keyLength = -1, intVal;
    Boolean firstChar = TRUE;

    IReader* reader = IReader::Probe(br);
    assert(reader != NULL);
    while (TRUE) {
        reader->Read(&intVal);
        if (intVal == -1) {
            break;
        }
        nextChar = (Byte)intVal;

        if (offset == buf->GetLength()) {
            AutoPtr<ArrayOf<Byte> > newBuf = ArrayOf<Byte>::Alloc(buf->GetLength() * 2);
            newBuf->Copy(buf, 0, offset);
            buf = newBuf;
        }
        if (mode == UNICODE) {
            Int32 digit = Character::ToDigit(intVal, 16);
            if (digit >= 0) {
                unicode = (unicode << 4) + digit;
                if (++count < 4) {
                    continue;
                }
            } else if (count <= 4) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            mode = NONE;
            (*buf)[offset++] = (Byte) unicode;
            if (nextChar != '\n') {
                continue;
            }
        }
        if (mode == SLASH) {
            mode = NONE;
            switch (nextChar) {
            case '\r':
                mode = CONTINUE; // Look for a following \n
                continue;
            case '\n':
                mode = IGNORE; // Ignore whitespace on the next line
                continue;
            case 'b':
                nextChar = '\b';
                break;
            case 'f':
                nextChar = '\f';
                break;
            case 'n':
                nextChar = '\n';
                break;
            case 'r':
                nextChar = '\r';
                break;
            case 't':
                nextChar = '\t';
                break;
            case 'u':
                mode = UNICODE;
                unicode = count = 0;
                continue;
            }
        } else {
            switch (nextChar) {
            case '#':
            case '!':
                if (firstChar) {
                    while (TRUE) {
                        reader->Read(&intVal);
                        if (intVal == -1) {
                            break;
                        }
                        nextChar = (Byte) intVal;
                        if (nextChar == '\r' || nextChar == '\n') {
                            break;
                        }
                    }
                    continue;
                }
                break;
            case '\n':
                if (mode == CONTINUE) { // Part of a \r\n sequence
                    mode = IGNORE; // Ignore whitespace on the next line
                    continue;
                }
                // fall into the next case
            case '\r':
                mode = NONE;
                firstChar = TRUE;
                if (offset > 0 || (offset == 0 && keyLength == 0)) {
                    if (keyLength == -1) {
                        keyLength = offset;
                    }
                    String temp = String((const char*)buf->GetPayload(), offset);
                    map[temp.Substring(0, keyLength)] = temp.Substring(keyLength);
                }
                keyLength = -1;
                offset = 0;
                continue;
            case '\\':
                if (mode == KEY_DONE) {
                    keyLength = offset;
                }
                mode = SLASH;
                continue;
            case ':':
            case '=':
                if (keyLength == -1) { // if parsing the key
                    mode = NONE;
                    keyLength = offset;
                    continue;
                }
                break;
            }
            if (Character::IsWhitespace(nextChar)) {
                if (mode == CONTINUE) {
                    mode = IGNORE;
                }
                // if key length == 0 or value length == 0
                if (offset == 0 || offset == keyLength || mode == IGNORE) {
                    continue;
                }
                if (keyLength == -1) { // if parsing the key
                    mode = KEY_DONE;
                    continue;
                }
            }
            if (mode == IGNORE || mode == CONTINUE) {
                mode = NONE;
            }
        }
        firstChar = FALSE;
        if (mode == KEY_DONE) {
            keyLength = offset;
            mode = NONE;
        }
        (*buf)[offset++] = nextChar;
    }
    if (mode == UNICODE && count <= 4) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (keyLength == -1 && offset > 0) {
        keyLength = offset;
    }
    if (keyLength >= 0) {
        String temp = String((const char*)buf->GetPayload(), offset);
        String key = temp.Substring(0, keyLength);
        String value = temp.Substring(keyLength);
        if (mode == SLASH) {
            value += "\u0000";
        }
        map[key] = value;
    }
    return NOERROR;
}

ECode MimeUtils::GetContentTypesPropertiesStream(
    /* [out] */ IInputStream** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    String userTable;
    system->GetEnv(String("content.types.user.table"), &userTable);
    if (!userTable.IsNull()) {
        AutoPtr<IFile> f;
        Boolean exist;
        // CFile::New(userTable, (IFile**)&f);
        if (f->Exists(&exist), exist) {
            FAIL_RETURN(CFileInputStream::New(f, result));
            return NOERROR;
        }
    }

    AutoPtr<IFile> f;
    String separator;
    AutoPtr<IFileHelper> fHelper;
    CFileHelper::AcquireSingleton((IFileHelper**)&fHelper);
    fHelper->GetSeparator(&separator);
    String env;
    system->GetEnv(String("java.home"), &env);
    StringBuilder sb(128);
    sb += env;
    sb += "lib";
    sb += separator;
    sb += "content-types.properties";
    CFile::New(sb.ToString(), (IFile**)&f);
    Boolean exist;
    if(f->Exists(&exist), exist) {
        FAIL_RETURN(CFileInputStream::New(f, result));
        return NOERROR;
    }

    return NOERROR;
}

} // namespace Net
} // namespace Libcore
