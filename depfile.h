#pragma once

#include <vector>
#include <string>

class Depfile
{
    public:
        bool Load(const char* filename);

        const std::string& GetDependency(size_t index) const; 
        size_t GetDependencyCount() const;

    private:
        std::vector<std::string> m_dependencies;
};

inline const std::string& Depfile::GetDependency(size_t index) const
{
    return m_dependencies[index];
}

inline size_t Depfile::GetDependencyCount() const
{
    return m_dependencies.size();
}
