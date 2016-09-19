
#include "elastos/droid/server/pm/DexFile.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <Elastos.CoreLibrary.IO.h>
#include <stdlib.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Utility::Logging::Logger;

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

// Spammy logging for kUpToDate
static const Boolean kVerboseLogging = FALSE;
// Logging of reason for returning kDexoptNeeded or kPatchoatNeeded.
static const Boolean kReasonLogging = TRUE;

enum InstructionSet {
  kNone,
  kArm,
  kArm64,
  kThumb2,
  kX86,
  kX86_64,
  kMips,
  kMips64
};

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
            Logger::D(TAG, "Unknown ISA %d", isa);
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
    Logger::D("chenxihao", "IsDexOptNeededForFile oat_filename = %s, filename = %s", oat_filename.string(), filename);
    AutoPtr<IFile> file;
    CFile::New(oat_filename, (IFile**)&file);
    Boolean isExists;
    file->Exists(&isExists);
    if (isExists) {
        return DexFile::UP_TO_DATE;
    }
    return DexFile::DEXOPT_NEEDED;
    // String error_msg;
    // std::unique_ptr<const OatFile> oat_file(OatFile::Open(oat_filename, oat_filename, nullptr,
    //                                                         false, &error_msg));
    // if (oat_file.get() == nullptr) {
    //     if (kReasonLogging) {
    //     LOG(INFO) << "DexFile_isDexOptNeeded failed to open oat file '" << oat_filename
    //         << "' for file location '" << filename << "': " << error_msg;
    //     }
    //     error_msg.clear();
    //     return kDexoptNeeded;
    // }
    // bool should_relocate_if_possible = Runtime::Current()->ShouldRelocate();
    // uint32_t location_checksum = 0;
    // const art::OatFile::OatDexFile* oat_dex_file = oat_file->GetOatDexFile(filename, nullptr,
    //                                                                         kReasonLogging);
    // if (oat_dex_file != nullptr) {
    //     // If its not possible to read the classes.dex assume up-to-date as we won't be able to
    //     // compile it anyway.
    //     if (!DexFile::GetChecksum(filename, &location_checksum, &error_msg)) {
    //     if (kVerboseLogging) {
    //         LOG(INFO) << "DexFile_isDexOptNeeded found precompiled stripped file: "
    //             << filename << " for " << oat_filename << ": " << error_msg;
    //     }
    //     if (ClassLinker::VerifyOatChecksums(oat_file.get(), target_instruction_set, &error_msg)) {
    //         if (kVerboseLogging) {
    //         LOG(INFO) << "DexFile_isDexOptNeeded file " << oat_filename
    //                     << " is up-to-date for " << filename;
    //         }
    //         return kUpToDate;
    //     } else if (should_relocate_if_possible &&
    //                 ClassLinker::VerifyOatImageChecksum(oat_file.get(), target_instruction_set)) {
    //         if (kReasonLogging) {
    //         LOG(INFO) << "DexFile_isDexOptNeeded file " << oat_filename
    //                     << " needs to be relocated for " << filename;
    //         }
    //         return kPatchoatNeeded;
    //     } else {
    //         if (kReasonLogging) {
    //         LOG(INFO) << "DexFile_isDexOptNeeded file " << oat_filename
    //                     << " is out of date for " << filename;
    //         }
    //         return kDexoptNeeded;
    //     }
    //     // If we get here the file is out of date and we should use the system one to relocate.
    //     } else {
    //     if (ClassLinker::VerifyOatAndDexFileChecksums(oat_file.get(), filename, location_checksum,
    //                                                     target_instruction_set, &error_msg)) {
    //         if (kVerboseLogging) {
    //         LOG(INFO) << "DexFile_isDexOptNeeded file " << oat_filename
    //                     << " is up-to-date for " << filename;
    //         }
    //         return kUpToDate;
    //     } else if (location_checksum == oat_dex_file->GetDexFileLocationChecksum()
    //                 && should_relocate_if_possible
    //                 && ClassLinker::VerifyOatImageChecksum(oat_file.get(), target_instruction_set)) {
    //         if (kReasonLogging) {
    //         LOG(INFO) << "DexFile_isDexOptNeeded file " << oat_filename
    //                     << " needs to be relocated for " << filename;
    //         }
    //         return kPatchoatNeeded;
    //     } else {
    //         if (kReasonLogging) {
    //         LOG(INFO) << "DexFile_isDexOptNeeded file " << oat_filename
    //                     << " is out of date for " << filename;
    //         }
    //         return kDexoptNeeded;
    //     }
    //     }
    // } else {
    //     if (kReasonLogging) {
    //     LOG(INFO) << "DexFile_isDexOptNeeded file " << oat_filename
    //                 << " does not contain " << filename;
    //     }
    //     return kDexoptNeeded;
    // }
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
        Logger::E(TAG, "DexFile_isDexOptNeeded file '%s' does not exist", filename.string());
        // ScopedLocalRef<jclass> fnfe(env, env->FindClass("java/io/FileNotFoundException"));
        // const char* message = (filename == nullptr) ? "<empty file name>" : filename;
        // env->ThrowNew(fnfe.get(), message);
        return UP_TO_DATE;
    }

    for (Int32 i = 0; i < sBootClassPath->GetLength(); i++) {
        if ((*sBootClassPath)[i].Equals(filename)) {
            if (kVerboseLogging) {
                Logger::D(TAG, "DexFile_isDexOptNeeded ignoring boot class path file: %s", filename.string());
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
            Logger::I(TAG, "DexFile_isDexOptNeededInternal failed to find cache file for dex file %s: %s",
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

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
