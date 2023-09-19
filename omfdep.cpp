#include <stdio.h>
#include <stdint.h>
#include <vector>
#include "omf.h"
#include "depfile.h"

static void showUsage()
{
    printf("usage: omfdep -d <.d path> <.obj path>\n");
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        showUsage();
        return 0;
    }

    const char* depPath = nullptr;
    const char* objPath = nullptr;

    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-d") && i+1 < argc)
        {
            i++;
            depPath = argv[i];
        }
        else
        {
            objPath = argv[i];
        }
    }

    if (!depPath || !objPath)
    {
        showUsage();
        return 0;
    }

    Depfile depfile;
    if (!depfile.Load(depPath))
    {
        printf("error: failed to load '%s'\n", depPath);
        return 0;
    }

    Omf omf;
    if (!omf.Load(objPath))
    {
        printf("error: failed to load '%s'\n", objPath);
        return 0;
    }

    for (size_t i = 0; i < depfile.GetDependencyCount(); i++)
        omf.AddDependency(depfile.GetDependency(i).c_str());

    if (!omf.Write())
    {
        printf("error: failed to write '%s'\n", objPath);
        return 0;
    }

    return 0;
}
