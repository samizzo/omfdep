#include <stdio.h>
#include <algorithm>
#include "depfile.h"

bool Depfile::Load(const char* filename)
{
    FILE* fp = nullptr;
    fopen_s(&fp, filename, "r");
    if (!fp)
        return false;

    bool foundColon = false;

    while (!feof(fp))
    {
        char c;

        if (foundColon)
        {
            // Read until first space.
            do
            {
                if (fread(&c, 1, 1, fp) != 1)
                {
                    if (feof(fp))
                        break;
                    return false;
                }
            }
            while (c == ' ');

            std::string dependency;
            dependency.append(1, c);

            // Read chars until a space.
            do
            {
                if (fread(&c, 1, 1, fp) != 1)
                {
                    if (feof(fp))
                        break;
                    return false;
                }

                if (c == ' ')
                    break;

                dependency.append(1, c);
            }
            while (1);

            dependency.erase(std::remove(dependency.begin(), dependency.end(), '\n'), dependency.end());
            dependency.erase(std::remove(dependency.begin(), dependency.end(), '\r'), dependency.end());

            if (dependency.size() > 0)
                m_dependencies.emplace_back(dependency);
        }
        else
        {
            if (fread(&c, 1, 1, fp) != 1)
            {
                if (feof(fp))
                    break;
                return false;
            }

            if (c == ':')
                foundColon = true;
        }
    }

    return true;
}
