#ifndef PROJECT_FILENAMEKEY_HPP
#define PROJECT_FILENAMEKEY_HPP

#include "MapReduceClient.h"
#include <string>

class FileNameKey : public k1Base, public k2Base, public k3Base,
                    public v1Base, public v2Base, public v3Base
{
    std::string _data;

public:
    FileNameKey() : _data("") {}
    FileNameKey(std::string data) : _data(data) {}

    ~FileNameKey() override
    { _data.clear(); }

    inline std::string GetData()const
    {
        return _data;
    }

    virtual bool operator<(const FileNameKey &other) const final
    {
        return _data < other._data;
    }
};

#endif //PROJECT_FILENAMEKEY_HPP
