#include "SearchMapReduce.h"

void SearchMapReduce::Map(const k1Base *const key, const v1Base *const val) const
{
    try
    {
        auto _key = dynamic_cast<const FileNameKey *const>(key);
        auto path = fs::path(_key->GetData());
        const fs::directory_iterator end{};

        for (fs::directory_iterator iter{path}; iter != end; ++iter)
        {
            auto file = iter->path().filename().string();
            if (file.find(_searchStr) != std::string::npos)
            {
                auto k2 = FileNameKey(iter->path());
                auto v2 = FileNameKey(file);
                Emit2(&k2, &v2);
            }
        }
    }
    catch (std::exception &e)
    { throw e; }

// TODO: Test if this experimental shit works.
//        DIR* dir;
//        struct dirent* ent;
//        if((dir = opendir(_dir.c_str())) != NULL)
//        {
//            while((ent = readdir(dir)) != NULL)
//            {
//
//            }
//            closedir(dir);
//        }
//        else
//        {
//            _logger.Log("invalid directory");
//        }
}

void SearchMapReduce::Reduce(const k2Base *const key, const V2_VEC &vals) const
{
    try
    {
        auto _key = dynamic_cast<const FileNameKey *const>(key);

        for (auto val : vals)
        {
            auto _val = dynamic_cast<const FileNameKey *const>(val);
            Emit3(_key, _val);
        }
    }
    catch (std::exception &e)
    { throw e; }

}
