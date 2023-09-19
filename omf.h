#pragma once

#include <stdint.h>
#include <vector>
#include <string>

class Omf
{
    class Record
    {
        public:
            bool Read(FILE* fp);
            bool Write(FILE* fp);

            uint8_t type;
            uint16_t length;
            std::vector<uint8_t> contents;
            uint8_t checksum;
    };

    public:
        bool Load(const char* filename);
        bool Write();

        void AddDependency(const char* filename);

    private:
        std::vector<Record> m_records;
        std::string m_filename;
};
