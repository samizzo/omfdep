#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include "omf.h"

bool Omf::Record::Read(FILE* fp)
{
    if (fread(&type, sizeof(type), 1, fp) != 1)
        return false;

    if (fread(&length, sizeof(length), 1, fp) != 1)
        return false;

    contents.resize(length - 1);
    if (fread(contents.data(), 1, length - 1, fp) != length - 1)
        return false;

    if (fread(&checksum, sizeof(checksum), 1, fp) != 1)
        return false;

    return true;
}

bool Omf::Record::Write(FILE* fp)
{
    if (fwrite(&type, sizeof(type), 1, fp) != 1)
        return false;

    if (fwrite(&length, sizeof(length), 1, fp) != 1)
        return false;

    if (fwrite(contents.data(), 1, length - 1, fp) != length - 1)
        return false;

    if (fwrite(&checksum, sizeof(checksum), 1, fp) != 1)
        return false;

    return true;
}

bool Omf::Load(const char* filename)
{
    m_filename = filename;

    FILE* fp = nullptr;
    fopen_s(&fp, filename, "rb");
    if (!fp)
        return false;

    while (!feof(fp))
    {
        Record record;
        if (!record.Read(fp))
        {
            if (feof(fp))
                break;
            return false;
        }
        m_records.emplace_back(record);
    }

    fclose(fp);

    return true;
}

bool Omf::Write()
{
    if (m_filename.empty())
        return false;

    FILE* fp = nullptr;
    fopen_s(&fp, m_filename.c_str(), "wb");
    if (!fp)
        return false;

    for (auto& record : m_records)
        record.Write(fp);

    fclose(fp);

    return true;
}

void Omf::AddDependency(const char* filename)
{
    Record record;

    // COMENT record.
    record.type = 0x88;

    // Add bytes to indicate Borland-style dependency info data.
    record.contents.push_back(0x80);
    record.contents.push_back(0xe9);

    struct stat attr;
    stat(filename, &attr);

    struct tm lt;
    localtime_s(&lt, &attr.st_mtime);

    uint16_t time, date;
    time = (lt.tm_hour << 11) | (lt.tm_min << 5) | (lt.tm_sec >> 1);
    uint16_t year = (lt.tm_year + 1900) - 1980;
    date = (year << 9) | ((lt.tm_mon + 1) << 5) | lt.tm_mday;

    // Add file timestamp to record.
    record.contents.push_back(time & 0xff);
    record.contents.push_back((time >> 8) & 0xff);
    record.contents.push_back(date & 0xff);
    record.contents.push_back((date >> 8) & 0xff);

    // Add filename length.
    size_t filenameLength = strlen(filename);
    record.contents.push_back((uint8_t)filenameLength);

    // Add filename.
    for (size_t i = 0; i < filenameLength; i++)
        record.contents.push_back(filename[i]);

    // Set length.
    record.length = (uint16_t)record.contents.size() + 1;

    // Calculate checksum.
    int32_t checksum = record.type + record.length;
    for (size_t i = 0; i < record.contents.size(); i++)
        checksum += record.contents[i];

    record.checksum = -checksum;

    // Find the THEADR record, insert after it.
    bool inserted = false;
    for (size_t i = 0; i < m_records.size(); i++)
    {
        if (m_records[i].type == 0x80)
        {
            m_records.insert(m_records.begin() + i + 1, record);
            inserted = true;
        }
    }

    if (!inserted)
        m_records.insert(m_records.begin(), record);
}
