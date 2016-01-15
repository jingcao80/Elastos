#include<wtypes.h>
#include<stdio.h>
#include<clsdef.h>

static CLSModule* s_pModule;
static FILE* s_pOutFile;

void GenXML()
{
    char* uri = "uri";

    fprintf(s_pOutFile, "<?xml version = \"1.0\" encoding=\"GB2312\" ?>\n\n");
    fprintf(s_pOutFile, "<appInfo>\n");
    fprintf(s_pOutFile, "    <version>1.0</version>\n");
    fprintf(s_pOutFile, "    <resourcesPath>resource</resourcesPath>\n");
    fprintf(s_pOutFile, "    <bigIconName>big</bigIconName>\n");
    fprintf(s_pOutFile, "    <smallIconName>small</smallIconName>\n");
    fprintf(s_pOutFile, "    <server>playhhhh</server>\n");
    fprintf(s_pOutFile, "    <uri>%s</uri>\n", uri);
    fprintf(s_pOutFile, "    <contentType>5</contentType>\n");
    fprintf(s_pOutFile, "    <groupId>6</groupId>\n");
    fprintf(s_pOutFile, "    <displayIndex>7</displayIndex>\n");
    fprintf(s_pOutFile, "<appInfo>\n");

    fclose(s_pOutFile);

    DisposeFlattedCLS(s_pModule);
}

int main(int argc, char** argv)
{
    int ret, len;

    if (2 != argc) {
        printf("Arguments error!\n");
        exit(0);
    }

    len = strlen(argv[1]);
    if (_stricmp(argv[1] + len - 4, ".cls")) {
        printf("Arguments error!\n");
        exit(0);
    }

    ret = LoadCLS(argv[1], &s_pModule);
    if (ret < 0) {
        printf("Can't load CLS file %s.\n", argv[1]);
        exit(1);
    }

    s_pOutFile = fopen("deploy.xml", "w");
    if (NULL == s_pOutFile) {
        printf("Can't create .xml file.\n");
        exit(1);
    }

    GenXML();

    return 0;
}
