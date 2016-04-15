#include <elastos.h>
#include <stdio.h>
#include <new>
#include <Elastos.Utility.Zip.h>
#include <elautoptr.h>

using namespace Elastos;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::Zip::IZipEntry;
using Elastos::Utility::Zip::IZipFile;
using Elastos::Utility::Zip::CZipFile;

class EnterLeaveLog
{
public:
    EnterLeaveLog(const char* str)
    {
        mString = String(str);
        printf("\n>>>> ENTER %s ====\n", str);
    }
    ~EnterLeaveLog()
    {
        printf("<<<< LEAVE %s ====\n", mString.string());
    }

private:
    String mString;
};

void testGetEntries(IZipFile *pZipFile)
{
    EnterLeaveLog log("test GetEntries");

    AutoPtr<IObjectContainer> container;
    ECode ec = pZipFile->GetEntries((IObjectContainer **) &container);
    if (FAILED(ec)) {
        printf(" >> Cann't Get Entries ec is %x!\n", ec);
        return;
    }

    AutoPtr<IObjectEnumerator> emu;
    container->GetObjectEnumerator((IObjectEnumerator**)&emu);

    String name;
    Boolean hasNext;
    while(emu->MoveNext(&hasNext), hasNext) {
        AutoPtr<IInterface> itf;
        emu->Current((IInterface**)&itf);
        AutoPtr<IZipEntry> entry = IZipEntry::Probe(itf);
        entry->GetName(&name);
        printf(" >> the entry name is %s\n", name.string());
    }
}

void testGetSize(IZipFile *pZipFile)
{
    EnterLeaveLog log("test GetSize");

    Int32 nSize;
    pZipFile->GetSize(&nSize);
    printf(" >> the size is %d\n", nSize);
}

void testGetEntry(IZipFile *pZipFile)
{
    EnterLeaveLog log("test GetEntry");

    String entryName("testZip/dir1/");
    AutoPtr<IZipEntry> pEntry;
    ECode ec = pZipFile->GetEntry(entryName, (IZipEntry**)&pEntry);
    if (FAILED(ec) || pEntry == NULL) {
        printf(" >> Failed to get entry: [%s]\n", entryName.string());
    }
    assert(pEntry != NULL);

    pEntry = NULL;
    entryName = String("testZip/dir2/file1");
    ec = pZipFile->GetEntry(entryName, (IZipEntry**)&pEntry);
    if (FAILED(ec) || pEntry == NULL) {
        printf(" >> Failed to get entry: [%s]\n", entryName.string());
    }
    assert(pEntry != NULL);

    pEntry = NULL;
    entryName = String("not_exist");
    ec = pZipFile->GetEntry(entryName, (IZipEntry**)&pEntry);
    if (FAILED(ec) || pEntry == NULL) {
        printf("Failed to get entry: [%s]\n", entryName.string());
    }
    assert(pEntry == NULL);
}

void testReadZipFile(IZipFile *pZipFile, const String& entryName)
{
    EnterLeaveLog log("test ReadZipFile");

    AutoPtr<IZipEntry> pEntry;
    ECode ec = pZipFile->GetEntry(entryName, (IZipEntry**)&pEntry);
    if (pEntry == NULL) {
        printf(" >> Cann't Find entry: [%s]!\n", entryName.string());
        return;
    }

    AutoPtr<IInputStream> is;
    Int32 len;
    AutoPtr<ArrayOf<Byte> > buf;

    ec = pZipFile->GetInputStream(pEntry, (IInputStream **) &is);
    if (FAILED(ec)) {
        printf(" >> Cann't Get ZipFile InputStream!\n");
        goto _EXIT_;
    }

    buf = ArrayOf<Byte>::Alloc(1024);
    ec = is->ReadBytesEx(buf, 0, 1024, &len);
    while(SUCCEEDED(ec) && len > 0) {
        String tmpStr((const char*)(buf->GetPayload()), len);
        printf(" >> Read %d bytes: [%s]\n", len, tmpStr.string());
        ec = is->ReadBytesEx(buf, 0, 1024, &len);
    }

_EXIT_:
    if (is != NULL) is->Close();
}

int main(int argc, char *argv[])
{
/*
testZip
    |-dir1
    |   |-file1
    |   |-file2
    |-dir2
        |-file1
        |-file2
 */

    String zipFilename("/data/data/com.elastos.runtime/elastos/testZip.zip");

    AutoPtr<IZipFile> pZipFile;
    ECode ec = CZipFile::New(zipFilename, (IZipFile**)&pZipFile);
    if (FAILED(ec)) {
        printf("Cann't open the file %s, ec = %x!\n", zipFilename.string(), ec);
        return -1;
    }

    testGetSize(pZipFile);
    testGetEntries(pZipFile);
    testGetEntry(pZipFile);

    String entryName("testZip/dir2/file1");
    testReadZipFile(pZipFile, entryName);

    return 0;
}

