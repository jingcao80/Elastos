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

#include "elastos/droid/server/pm/DexFile.h"
#include "elastos/droid/server/pm/dex/DexFileVerifier.h"
#include "elastos/droid/server/pm/dex/ZipArchive.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.CoreLibrary.IO.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

using Elastos::Droid::Server::Pm::Dex::kArm;
using Elastos::Droid::Server::Pm::Dex::kNone;
using Elastos::Droid::Server::Pm::Dex::kArm;
using Elastos::Droid::Server::Pm::Dex::kArm64;
using Elastos::Droid::Server::Pm::Dex::kThumb2;
using Elastos::Droid::Server::Pm::Dex::kX86;
using Elastos::Droid::Server::Pm::Dex::kX86_64;
using Elastos::Droid::Server::Pm::Dex::kMips;
using Elastos::Droid::Server::Pm::Dex::kMips64;
using Elastos::Droid::Server::Pm::Dex::DexFileVerifier;
using Elastos::Droid::Server::Pm::Dex::MemMap;
using Elastos::Droid::Server::Pm::Dex::ZipArchive;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

static AutoPtr<ArrayOf<String> > InitBootClassPath()
{
    const char* boot_class_path_string = getenv("BOOTCLASSPATH");
    AutoPtr<ArrayOf<String> > boot_class_path;
    StringUtils::Split(String(boot_class_path_string), String(":"), (ArrayOf<String>**)&boot_class_path);
    return boot_class_path;
}

const Byte DexFile::UP_TO_DATE;
const Byte DexFile::PATCHOAT_NEEDED;
const Byte DexFile::DEXOPT_NEEDED;
static const String TAG("DexFile");
static const AutoPtr<ArrayOf<String> > sBootClassPath = InitBootClassPath();

// Spammy logging for DexFile::UP_TO_DATE
static const Boolean kVerboseLogging = FALSE;
// Logging of reason for returning DEXOPT_NEEDED or kPatchoatNeeded.
static const Boolean kReasonLogging = TRUE;

static const char* GetInstructionSetString(
    /* [in] */ const InstructionSet isa)
{
    switch (isa) {
        case kArm:
        case kThumb2:
            return "arm";
        case kArm64:
            return "arm64";
        case kX86:
            return "x86";
        case kX86_64:
            return "x86_64";
        case kMips:
            return "mips";
        case kNone:
            return "none";
        default:
            Slogger::D(TAG, "Unknown ISA %d", isa);
            return NULL;
    }
}

static InstructionSet GetInstructionSetFromString(
    /* [in] */ const char* isa_str)
{
    if (strcmp("arm", isa_str) == 0) {
        return kArm;
    }
    else if (strcmp("arm64", isa_str) == 0) {
        return kArm64;
    }
    else if (strcmp("x86", isa_str) == 0) {
        return kX86;
    }
    else if (strcmp("x86_64", isa_str) == 0) {
        return kX86_64;
    }
    else if (strcmp("mips", isa_str) == 0) {
        return kMips;
    }

    return kNone;
}

static void InsertIsaDirectory(
    /* [in] */ const InstructionSet isa,
    /* [in, out] */ String* filename)
{
    // in = /foo/bar/baz
    // out = /foo/bar/<isa>/baz
    Int32 pos = filename->LastIndexOf('/');
    assert(pos > 0);
    // CHECK_NE(pos, std::string::npos) << *filename << " " << isa;
    StringBuilder sb;
    sb += filename->Substring(0, pos);
    sb += "/";
    sb += GetInstructionSetString(isa);
    sb += filename->Substring(pos, filename->GetLength());
    *filename = sb.ToString();
}

static String DexFilenameToOdexFilename(
    /* [in] */ const String& location,
    /* [in] */ const InstructionSet isa)
{
    // location = /foo/bar/baz.jar
    // odex_location = /foo/bar/<isa>/baz.odex

    // CHECK_GE(location.size(), 4U) << location;  // must be at least .123
    assert(location.GetLength() >= 4);
    String odex_location(location);
    InsertIsaDirectory(isa, &odex_location);
    Int32 dot_index = odex_location.GetLength() - 3 - 1;  // 3=dex or zip or apk
    // CHECK_EQ('.', odex_location[dot_index]) << location;
    assert(odex_location.GetChar(dot_index) == '.');
    odex_location = odex_location.Substring(0, dot_index + 1);
    assert(odex_location.GetChar(odex_location.GetLength() - 1) == '.');
    // CHECK_EQ('.', odex_location[odex_location.size()-1]) << location << " " << odex_location;
    odex_location += "odex";
    return odex_location;
}

static const char* GetAndroidDataSafe(
    /* [out] */ String* error_msg)
{
    const char* android_data = getenv("ANDROID_DATA");
    AutoPtr<IFile> file;
    if (android_data == NULL) {
        CFile::New(String("/data"), (IFile**)&file);
        Boolean isExists;
        file->Exists(&isExists);
        if (isExists) {
            android_data = "/data";
        }
        else {
            *error_msg = "ANDROID_DATA not set and /data does not exist";
            return NULL;
        }
    }
    CFile::New(String(android_data), (IFile**)&file);
    Boolean isExists;
    file->Exists(&isExists);
    if (!isExists) {
        *error_msg = NULL;
        error_msg->AppendFormat("Failed to find ANDROID_DATA directory %s", android_data);
        return NULL;
    }
    return android_data;
}

static void GetDalvikCache(
    /* [in] */ const char* subdir,
    /* [in] */ const Boolean create_if_absent,
    /* [out] */ String* dalvik_cache,
    /* [out] */ Boolean* have_android_data,
    /* [out] */ Boolean* dalvik_cache_exists,
    /* [out] */ Boolean* is_global_cache)
{
    assert(subdir != NULL);
    String error_msg;
    const char* android_data = GetAndroidDataSafe(&error_msg);
    if (android_data == NULL) {
        *have_android_data = FALSE;
        *dalvik_cache_exists = FALSE;
        *is_global_cache = FALSE;
        return;
    }
    else {
        *have_android_data = TRUE;
    }
    String dalvik_cache_root;
    dalvik_cache_root.AppendFormat("%s/dalvik-cache/", android_data);
    *dalvik_cache = dalvik_cache_root + subdir;
    AutoPtr<IFile> dir;
    CFile::New(*dalvik_cache, (IFile**)&dir);
    dir->Exists(dalvik_cache_exists);
    *is_global_cache = strcmp(android_data, "/data") == 0;
    if (create_if_absent && !*dalvik_cache_exists && !*is_global_cache) {
        // Don't create the system's /data/dalvik-cache/... because it needs special permissions.
        // *dalvik_cache_exists = ((mkdir(dalvik_cache_root.c_str(), 0700) == 0 || errno == EEXIST) &&
        //                         (mkdir(dalvik_cache->c_str(), 0700) == 0 || errno == EEXIST));
        AutoPtr<IFile> root;
        CFile::New(dalvik_cache_root, (IFile**)&root);
        Boolean isExists;
        root->Exists(&isExists);
        if (!isExists) {
            Boolean res;
            root->Mkdir(&res);
        }
        dir->Mkdir(dalvik_cache_exists);
    }
}

static Boolean GetDalvikCacheFilename(
    /* [in] */ const char* location,
    /* [in] */ const char* cache_location,
    /* [in] */ String* filename,
    /* [in] */ String* error_msg)
{
    if (location[0] != '/') {
        *error_msg = NULL;
        error_msg->AppendFormat("Expected path in location to be absolute: %s", location);
        return FALSE;
    }
    String strlocation(location);
    String cache_file(&location[1]);  // skip leading slash
    if (!strlocation.EndWith(".dex") && !strlocation.EndWith(".art") && !strlocation.EndWith(".oat")) {
        cache_file.Append("/");
        cache_file.Append("classes.dex");
    }
    cache_file = cache_file.Replace('/', '@');
    *filename = NULL;
    filename->AppendFormat("%s/%s", cache_location, cache_file.string());
    return TRUE;
}

static Byte IsDexOptNeededForFile(
    /* [in] */ const String& oat_filename,
    /* [in] */ const char* filename,
    /* [in] */ InstructionSet target_instruction_set)
{
    AutoPtr<IFile> file;
    CFile::New(oat_filename, (IFile**)&file);
    Boolean isExists;
    file->Exists(&isExists);
    if (isExists) {
        return DexFile::UP_TO_DATE;
    }
    return DexFile::DEXOPT_NEEDED;

    String error_msg;
    AutoPtr<OatFile> oat_file = OatFile::Open(oat_filename, oat_filename, NULL, FALSE, &error_msg);
    if (oat_file == NULL) {
        if (kReasonLogging) {
            Slogger::D("DexFile", "DexFile_isDexOptNeeded failed to open oat file '%s' for file location '%s: %s",
                    oat_filename.string(), filename, error_msg.string());
        }
        return DexFile::DEXOPT_NEEDED;
    }
    // Boolean should_relocate_if_possible = Runtime::Current()->ShouldRelocate();
    uint32_t location_checksum = 0;
    AutoPtr<OatFile::OatDexFile> oat_dex_file = oat_file->GetOatDexFile(filename, NULL, kReasonLogging);
    if (oat_dex_file != NULL) {
        // If its not possible to read the classes.dex assume up-to-date as we won't be able to
        // compile it anyway.
        if (!DexFile::GetChecksum(filename, &location_checksum, &error_msg)) {
            if (kVerboseLogging) {
                Slogger::D("DexFile", "DexFile_isDexOptNeeded found precompiled stripped file: %s for %s: %s",
                        filename, oat_filename.string(), error_msg.string());
            }
            // if (ClassLinker::VerifyOatChecksums(oat_file, target_instruction_set, &error_msg)) {
            //     if (kVerboseLogging) {
            //         Slogger::D("DexFile", "DexFile_isDexOptNeeded file %s is up-to-date for %s",
            //                 oat_filename.string(), filename);
            //     }
            //     return DexFile::UP_TO_DATE;
            // }
            // else if (should_relocate_if_possible &&
            //         ClassLinker::VerifyOatImageChecksum(oat_file, target_instruction_set)) {
            //     if (kReasonLogging) {
            //         Slogger::D("DexFile", "DexFile_isDexOptNeeded file %s needs to be relocated for %s",
            //                 oat_filename.string(), filename);
            //     }
            //     return kPatchoatNeeded;
            // }
            // else {
                if (kReasonLogging) {
                    Slogger::D("DexFile", "DexFile_isDexOptNeeded file %s is out of date for %s",
                            oat_filename.string(), filename);
                }
                return DexFile::DEXOPT_NEEDED;
            // }
        // If we get here the file is out of date and we should use the system one to relocate.
        }
        else {
            if (DexFile::VerifyOatAndDexFileChecksums(oat_file, filename, location_checksum,
                    target_instruction_set, &error_msg)) {
                if (kVerboseLogging) {
                    Slogger::D("DexFile", "DexFile_isDexOptNeeded file %s is up-to-date for %s",
                            oat_filename.string(), filename);
                }
                return DexFile::UP_TO_DATE;
            }
            // else if (location_checksum == oat_dex_file->GetDexFileLocationChecksum()
            //             && should_relocate_if_possible
            //             && ClassLinker::VerifyOatImageChecksum(oat_file, target_instruction_set)) {
            //     if (kReasonLogging) {
            //         Slogger::D("DexFile", "DexFile_isDexOptNeeded file %s needs to be relocated for %s",
            //                 oat_filename.string(), filename);
            //     }
            //     return kPatchoatNeeded;
            // }
            else {
                if (kReasonLogging) {
                    Slogger::D("DexFile", "DexFile_isDexOptNeeded file %s is out of date for %s",
                            oat_filename.string(), filename);
                }
                return DexFile::DEXOPT_NEEDED;
            }
        }
    }
    else {
        if (kReasonLogging) {
            Slogger::D("DexFile", "DexFile_isDexOptNeeded file %s does not contain %s",
                    oat_filename.string(), filename);
        }
        return DexFile::DEXOPT_NEEDED;
    }
}

Byte DexFile::IsDexOptNeededInternal(
    /* [in] */ const String& filename,
    /* [in] */ const String& pkgname,
    /* [in] */ const String& instructionSet,
    /* [in] */ Boolean defer)
{
    Boolean isExists = FALSE;
    if (filename != NULL) {
        AutoPtr<IFile> file;
        CFile::New(filename, (IFile**)&file);
        file->Exists(&isExists);
    }
    if (!isExists) {
        Slogger::E(TAG, "DexFile_isDexOptNeeded file '%s' does not exist", filename.string());
        // ScopedLocalRef<jclass> fnfe(env, env->FindClass("java/io/FileNotFoundException"));
        // const char* message = (filename == nullptr) ? "<empty file name>" : filename;
        // env->ThrowNew(fnfe.get(), message);
        return UP_TO_DATE;
    }

    for (Int32 i = 0; i < sBootClassPath->GetLength(); i++) {
        if ((*sBootClassPath)[i].Equals(filename)) {
            if (kVerboseLogging) {
                Slogger::D(TAG, "DexFile_isDexOptNeeded ignoring boot class path file: %s", filename.string());
            }
            return UP_TO_DATE;
        }
    }

    Boolean force_system_only = FALSE;
    Boolean require_system_version = FALSE;

    // Check the profile file.  We need to rerun dex2oat if the profile has changed significantly
    // since the last time, or it's new.
    // If the 'defer' argument is TRUE then this will be retried later.  In this case we
    // need to make sure that the profile file copy is not made so that we will get the
    // same result second time.
    // std::string profile_file;
    // std::string prev_profile_file;
    // Boolean should_copy_profile = FALSE;
    // if (Runtime::Current()->GetProfilerOptions().IsEnabled() && (pkgname != nullptr)) {
    //     profile_file = GetDalvikCacheOrDie("profiles", FALSE /* create_if_absent */)
    //         + std::string("/") + pkgname;
    //     prev_profile_file = profile_file + std::string("@old");

    //     struct stat profstat, prevstat;
    //     int e1 = stat(profile_file.c_str(), &profstat);
    //     int e1_errno = errno;
    //     int e2 = stat(prev_profile_file.c_str(), &prevstat);
    //     int e2_errno = errno;
    //     if (e1 < 0) {
    //     if (e1_errno != EACCES) {
    //         // No profile file, need to run dex2oat, unless we find a file in system
    //         if (kReasonLogging) {
    //         LOG(INFO) << "DexFile_isDexOptNeededInternal profile file " << profile_file << " doesn't exist. "
    //                     << "Will check odex to see if we can find a working version.";
    //         }
    //         // Force it to only accept system files/files with versions in system.
    //         require_system_version = TRUE;
    //     } else {
    //         LOG(INFO) << "DexFile_isDexOptNeededInternal recieved EACCES trying to stat profile file "
    //                 << profile_file;
    //     }
    //     } else if (e2 == 0) {
    //     // There is a previous profile file.  Check if the profile has changed significantly.
    //     // A change in profile is considered significant if X% (change_thr property) of the top K%
    //     // (compile_thr property) samples has changed.
    //     double top_k_threshold = Runtime::Current()->GetProfilerOptions().GetTopKThreshold();
    //     double change_threshold = Runtime::Current()->GetProfilerOptions().GetTopKChangeThreshold();
    //     double change_percent = 0.0;
    //     ProfileFile new_profile, old_profile;
    //     Boolean new_ok = new_profile.LoadFile(profile_file);
    //     Boolean old_ok = old_profile.LoadFile(prev_profile_file);
    //     if (!new_ok || !old_ok) {
    //         if (kVerboseLogging) {
    //         LOG(INFO) << "DexFile_isDexOptNeededInternal Ignoring invalid profiles: "
    //                     << (new_ok ?  "" : profile_file) << " " << (old_ok ? "" : prev_profile_file);
    //         }
    //     } else {
    //         std::set<std::string> new_top_k, old_top_k;
    //         new_profile.GetTopKSamples(new_top_k, top_k_threshold);
    //         old_profile.GetTopKSamples(old_top_k, top_k_threshold);
    //         if (new_top_k.empty()) {
    //         if (kVerboseLogging) {
    //             LOG(INFO) << "DexFile_isDexOptNeededInternal empty profile: " << profile_file;
    //         }
    //         // If the new topK is empty we shouldn't optimize so we leave the change_percent at 0.0.
    //         } else {
    //         std::set<std::string> diff;
    //         std::set_difference(new_top_k.begin(), new_top_k.end(), old_top_k.begin(), old_top_k.end(),
    //             std::inserter(diff, diff.end()));
    //         // TODO: consider using the usedPercentage instead of the plain diff count.
    //         change_percent = 100.0 * static_cast<double>(diff.size()) / static_cast<double>(new_top_k.size());
    //         if (kVerboseLogging) {
    //             std::set<std::string>::iterator end = diff.end();
    //             for (std::set<std::string>::iterator it = diff.begin(); it != end; it++) {
    //             LOG(INFO) << "DexFile_isDexOptNeededInternal new in topK: " << *it;
    //             }
    //         }
    //         }
    //     }

    //     if (change_percent > change_threshold) {
    //         if (kReasonLogging) {
    //         LOG(INFO) << "DexFile_isDexOptNeededInternal size of new profile file " << profile_file <<
    //         " is significantly different from old profile file " << prev_profile_file << " (top "
    //         << top_k_threshold << "% samples changed in proportion of " << change_percent << "%)";
    //         }
    //         should_copy_profile = !defer;
    //         // Force us to only accept system files.
    //         force_system_only = TRUE;
    //     }
    //     } else if (e2_errno == ENOENT) {
    //     // Previous profile does not exist.  Make a copy of the current one.
    //     if (kVerboseLogging) {
    //         LOG(INFO) << "DexFile_isDexOptNeededInternal previous profile doesn't exist: " << prev_profile_file;
    //     }
    //     should_copy_profile = !defer;
    //     } else {
    //     PLOG(INFO) << "Unable to stat previous profile file " << prev_profile_file;
    //     }
    // }

    const InstructionSet target_instruction_set = GetInstructionSetFromString(instructionSet.string());
    if (target_instruction_set == kNone) {
        // ScopedLocalRef<jclass> iae(env, env->FindClass("java/lang/IllegalArgumentException"));
        // std::string message(StringPrintf("Instruction set %s is invalid.", instruction_set));
        // env->ThrowNew(iae.get(), message.c_str());
        return 0;
    }

    // Get the filename for odex file next to the dex file.
    String odex_filename(DexFilenameToOdexFilename(filename, target_instruction_set));
    // Get the filename for the dalvik-cache file
    String cache_dir;
    Boolean have_android_data = FALSE;
    Boolean dalvik_cache_exists = FALSE;
    Boolean is_global_cache = FALSE;
    GetDalvikCache(instructionSet.string(), FALSE, &cache_dir, &have_android_data, &dalvik_cache_exists,
                    &is_global_cache);
    String cache_filename;  // was cache_location
    Boolean have_cache_filename = FALSE;
    if (dalvik_cache_exists) {
        String error_msg;
        have_cache_filename = GetDalvikCacheFilename(filename, cache_dir.string(), &cache_filename,
                                                    &error_msg);
        if (!have_cache_filename && kVerboseLogging) {
            Slogger::I(TAG, "DexFile_isDexOptNeededInternal failed to find cache file for dex file %s: %s",
                filename.string(), error_msg.string());
        }
    }

    // Boolean should_relocate_if_possible = Runtime::Current()->ShouldRelocate();

    Byte dalvik_cache_decision = -1;
    // Lets try the cache first (since we want to load from there since thats where the relocated
    // versions will be).
    if (have_cache_filename && !force_system_only) {
        // We can use the dalvik-cache if we find a good file.
        dalvik_cache_decision = IsDexOptNeededForFile(cache_filename, filename, target_instruction_set);
        // We will only return DexOptNeeded if both the cache and system return it.
        if (dalvik_cache_decision != DEXOPT_NEEDED && !require_system_version) {
        // CHECK(!(dalvik_cache_decision == kPatchoatNeeded && !should_relocate_if_possible))
        //     << "May not return PatchoatNeeded when patching is disabled.";
            return dalvik_cache_decision;
        }
        // We couldn't find one thats easy. We should now try the system.
    }

    Byte system_decision = IsDexOptNeededForFile(odex_filename, filename, target_instruction_set);
    // CHECK(!(system_decision == kPatchoatNeeded && !should_relocate_if_possible))
    //     << "May not return PatchoatNeeded when patching is disabled.";

    if (require_system_version && system_decision == PATCHOAT_NEEDED
                                && dalvik_cache_decision == UP_TO_DATE) {
        // We have a version from system relocated to the cache. Return it.
        return dalvik_cache_decision;
    }

    // if (should_copy_profile && system_decision == DEXOPT_NEEDED) {
    //     CopyProfileFile(profile_file.c_str(), prev_profile_file.c_str());
    // }

    return system_decision;
}

// from dex_file.cc from art

static Int32 OpenAndReadMagic(const char* filename, uint32_t* magic, String* error_msg)
{
    assert(magic != NULL);
    Int32 fd = open(filename, O_RDONLY, 0);
    if (fd == -1) {
        String msg("");
        msg.AppendFormat("Unable to open '%s' : %s", filename, strerror(errno));
        *error_msg = msg;
        return -1;
    }
    Int32 n = TEMP_FAILURE_RETRY(read(fd, magic, sizeof(*magic)));
    if (n != sizeof(*magic)) {
        TEMP_FAILURE_RETRY(close(fd));
        String msg("");
        msg.AppendFormat("Failed to find magic in '%s'", filename);
        *error_msg = msg;
        return -1;
    }
    if (lseek(fd, 0, SEEK_SET) != 0) {
        TEMP_FAILURE_RETRY(close(fd));
        String msg("");
        msg.AppendFormat("Failed to seek to beginning of file '%s' : %s", filename,
                strerror(errno));
        *error_msg = msg;
        return -1;
    }
    return fd;
}

const Byte DexFile::sDexMagic[] = { 'd', 'e', 'x', '\n' };
const Byte DexFile::sDexMagicVersion[] = { '0', '3', '5', '\0' };
const char* DexFile::sClassesDex = "classes.dex";
const char DexFile::sMultiDexSeparator = ':';

Boolean DexFile::GetChecksum(
    /* [in] */ const char* filename,
    /* [in] */ uint32_t* checksum,
    /* [in] */ String* error_msg)
{
    assert(checksum != NULL);
    uint32_t magic;

    // Strip ":...", which is the location
    const char* zip_entry_name = sClassesDex;
    const char* file_part = filename;
    String file_part_storage;

    if (DexFile::IsMultiDexLocation(filename)) {
        file_part_storage = GetBaseLocation(filename);
        file_part = file_part_storage.string();
        zip_entry_name = filename + file_part_storage.GetByteLength() + 1;
        assert(zip_entry_name[-1] == sMultiDexSeparator);
    }

    Int32 fd = OpenAndReadMagic(file_part, &magic, error_msg);
    if (fd == -1) {
        assert(!error_msg->IsNullOrEmpty());
        return FALSE;
    }
    if (IsZipMagic(magic)) {
        AutoPtr<ZipArchive> zip_archive = ZipArchive::OpenFromFd(fd, filename, error_msg);
        if (zip_archive == NULL) {
            String msg("");
            msg.AppendFormat("Failed to open zip archive '%s'", file_part);
            *error_msg = msg;
            return FALSE;
        }
        AutoPtr<Elastos::Droid::Server::Pm::Dex::ZipEntry> zip_entry = zip_archive->Find(zip_entry_name, error_msg);
        if (zip_entry == NULL) {
            String msg("");
            msg.AppendFormat("Zip archive '%s' doesn't contain %s (error msg: %s)", file_part,
                    zip_entry_name, error_msg->string());
            *error_msg = msg;
            return FALSE;
        }
        *checksum = zip_entry->GetCrc32();
        return TRUE;
    }
    if (IsDexMagic(magic)) {
        AutoPtr<DexFile> dex_file = DexFile::OpenFile(fd, filename, FALSE, error_msg);
        if (dex_file == NULL) {
            return FALSE;
        }
        *checksum = dex_file->GetHeader().mChecksum;
        return TRUE;
    }
    TEMP_FAILURE_RETRY(close(fd));
    String msg("");
    msg.AppendFormat("Expected valid zip or dex file: '%s'", filename);
    *error_msg = msg;
    return FALSE;
}

AutoPtr<DexFile> DexFile::OpenFile(
    /* [in] */ Int32 fd,
    /* [in] */ const char* location,
    /* [in] */ Boolean verify,
    /* [in] */ String* error_msg)
{
    assert(location != NULL);
    AutoPtr<MemMap> map;

    struct stat sbuf;
    memset(&sbuf, 0, sizeof(sbuf));
    if (fstat(fd, &sbuf) == -1) {
        if (fd != -1) TEMP_FAILURE_RETRY(close(fd));
        String msg("");
        msg.AppendFormat("DexFile: fstat '%s' failed: %s", location, strerror(errno));
        *error_msg = msg;
        return NULL;
    }
    if (S_ISDIR(sbuf.st_mode)) {
        TEMP_FAILURE_RETRY(close(fd));
        String msg("");
        msg.AppendFormat("Attempt to mmap directory '%s'", location);
        *error_msg = msg;
        return NULL;
    }
    size_t length = sbuf.st_size;
    map = MemMap::MapFile(length, PROT_READ, MAP_PRIVATE, fd, 0, location, error_msg);
    if (map == NULL) {
        TEMP_FAILURE_RETRY(close(fd));
        assert(!error_msg->IsNullOrEmpty());
        return NULL;
    }

    if (map->Size() < sizeof(DexFile::Header)) {
        String msg("");
        msg.AppendFormat("DexFile: failed to open dex file '%s' that is too short to have a header", location);
        *error_msg = msg;
        return NULL;
    }

    const Header* dex_header = reinterpret_cast<const Header*>(map->Begin());

    AutoPtr<DexFile> dex_file = OpenMemory(String(location), dex_header->mChecksum, map, error_msg);
    if (dex_file == NULL) {
        String msg("");
        msg.AppendFormat("Failed to open dex file '%s' from memory: %s", location,
                error_msg->string());
        *error_msg = msg;
        return NULL;
    }

    if (verify && !DexFileVerifier::Verify(dex_file, dex_file->Begin(), dex_file->Size(), location,
                                         error_msg)) {
        return NULL;
    }

    return dex_file;
}

AutoPtr<DexFile> DexFile::OpenMemory(
    /* [in] */ const String& location,
    /* [in] */ uint32_t location_checksum,
    /* [in] */ MemMap* mem_map,
    /* [out] */ String* error_msg)
{
    return OpenMemory(mem_map->Begin(),
                    mem_map->Size(),
                    location,
                    location_checksum,
                    mem_map,
                    error_msg);
}

AutoPtr<DexFile> DexFile::OpenMemory(
    /* [in] */ const Byte* base,
    /* [in] */ size_t size,
    /* [in] */ const String& location,
    /* [in] */ uint32_t location_checksum,
    /* [in] */ MemMap* mem_map,
    /* [out] */ String* error_msg)
{
    assert((reinterpret_cast<const uintptr_t>(base) & 3) == 0); // various dex file structures must be word aligned
    AutoPtr<DexFile> dex_file = new DexFile(base, size, location, location_checksum, mem_map);
    if (!dex_file->Init(error_msg)) {
       return NULL;
    }
    else {
       return dex_file;
    }
}

DexFile::DexFile(
    /* [in] */ const Byte* base,
    /* [in] */ size_t size,
    /* [in] */ const String& location,
    /* [in] */ uint32_t location_checksum,
    /* [in] */ MemMap* mem_map)
    : mBegin(base)
    , mSize(size)
    , mLocation(location)
    , mHeader(reinterpret_cast<const Header*>(base))
{

}

Boolean DexFile::Init(
    /* [out] */ String* error_msg)
{
    if (!CheckMagicAndVersion(error_msg)) {
        return FALSE;
    }
    return TRUE;
}

Boolean DexFile::CheckMagicAndVersion(
    /* [out] */ String* error_msg) const
{
    if (!IsMagicValid(mHeader->mMagic)) {
        String msg("");
        msg.AppendFormat("Unrecognized magic number in %s: %d %d %d %d",
                GetLocation().string(), mHeader->mMagic[0],
                mHeader->mMagic[1], mHeader->mMagic[2], mHeader->mMagic[3]);
        *error_msg = msg;
        return FALSE;
    }
    if (!IsVersionValid(mHeader->mMagic)) {
        String msg("");
        msg.AppendFormat("Unrecognized version number in %s: %d %d %d %d",
                GetLocation().string(), mHeader->mMagic[4],
                mHeader->mMagic[5], mHeader->mMagic[6], mHeader->mMagic[7]);
        *error_msg = msg;
        return FALSE;
    }
    return TRUE;
}

DexFile::~DexFile()
{

}

String DexFile::GetLocation() const
{
    return mLocation;
}

String DexFile::GetBaseLocation(
    /* [in] */ const char* location)
{
    const char* pos = strrchr(location, sMultiDexSeparator);
    if (pos == NULL) {
        return String(location);
    }
    else {
        return String(location, pos - location);
    }
}

const DexFile::Header& DexFile::GetHeader() const
{
    assert(mHeader != NULL);
    return *mHeader;
}

Boolean DexFile::IsMagicValid(
    /* [in] */ const byte* magic)
{
    return (memcmp(magic, sDexMagic, sizeof(sDexMagic)) == 0);
}

Boolean DexFile::IsVersionValid(
    /* [in] */ const Byte* magic)
{
    const byte* version = &magic[sizeof(sDexMagic)];
    return (memcmp(version, sDexMagicVersion, sizeof(sDexMagicVersion)) == 0);
}

const Byte* DexFile::Begin() const
{
    return mBegin;
}

size_t DexFile::Size() const
{
    return mSize;
}

Boolean DexFile::IsMultiDexLocation(
    /* [in] */ const char* location)
{
    return strrchr(location, sMultiDexSeparator) != NULL;
}

String DexFile::GetDexCanonicalLocation(
    /* [in] */ const char* dex_location)
{
    assert(dex_location != NULL);
    String base_location = GetBaseLocation(dex_location);
    const char* suffix = dex_location + base_location.GetByteLength();
    assert(suffix[0] == 0 || suffix[0] == sMultiDexSeparator);
    char* path = realpath(base_location.string(), NULL);
    if (path != NULL && path != base_location.string()) {
        String ret = String(path) + String(suffix);
        free(path);
        return ret;
    }
    else if (suffix[0] == 0) {
        return base_location;
    }
    else {
        return String(dex_location);
    }
}

// from class_linker.cc in art

Boolean DexFile::VerifyOatAndDexFileChecksums(
    /* [in] */ const OatFile* oat_file,
    /* [in] */ const char* dex_location,
    /* [in] */ uint32_t dex_location_checksum,
    /* [in] */ InstructionSet instruction_set,
    /* [in] */ String* error_msg)
{
    // if (!VerifyOatChecksums(oat_file, instruction_set, error_msg)) {
    //     return false;
    // }

    AutoPtr<OatFile::OatDexFile> oat_dex_file = oat_file->GetOatDexFile(dex_location,
            &dex_location_checksum);
    if (oat_dex_file == NULL) {
        String msg("");
        msg.AppendFormat("oat file '%s' does not contain contents for '%s' with checksum 0x%x",
                oat_file->GetLocation().string(), dex_location, dex_location_checksum);
        *error_msg = msg;
        const Vector< AutoPtr<OatFile::OatDexFile> >& oat_dex_files = oat_file->GetOatDexFiles();
        Vector< AutoPtr<OatFile::OatDexFile> >::ConstIterator it;
        for (it = oat_dex_files.Begin(); it != oat_dex_files.End(); ++it) {
            AutoPtr<OatFile::OatDexFile> oat_dex_file = *it;
            String msg("");
            msg.AppendFormat("\noat file '%s' contains contents for '%s' with checksum 0x%x",
                                oat_file->GetLocation().string(),
                                oat_dex_file->GetDexFileLocation().string(),
                                oat_dex_file->GetDexFileLocationChecksum());
            *error_msg += msg;
        }
        return FALSE;
    }

    if (dex_location_checksum != oat_dex_file->GetDexFileLocationChecksum()) {
        String msg("");
        msg.AppendFormat("oat file '%s' mismatch (0x%x) with '%s' (0x%x)",
                            oat_file->GetLocation().string(),
                            oat_dex_file->GetDexFileLocationChecksum(),
                            dex_location, dex_location_checksum);
        *error_msg = msg;
        return FALSE;
    }
    return TRUE;
}

Boolean DexFile::IsZipMagic(
    /* [in] */ uint32_t magic)
{
    return (('P' == ((magic >> 0) & 0xff)) &&
          ('K' == ((magic >> 8) & 0xff)));
}

Boolean DexFile::IsDexMagic(
    /* [in] */ uint32_t magic)
{
    return IsMagicValid(reinterpret_cast<const byte*>(&magic));
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
