
#include "FbrAlgorithm.h"


FbrAlgorithm::FbrAlgorithm(size_t size, double f_old, double f_new) : ICacheAlgorithm( 0 )
{
    new_Lru = new LruAlgorithm(((size_t) (f_new * size)));
    old_Lfu = new Lfu(((size_t) (size * f_old)));
    m_exist = (1 - (f_new + f_old) != 0);
    if (m_exist)
    {
        m_Lru = new LruAlgorithm((size_t) (size * (1 - (f_new + f_old))));
    }
}

FbrAlgorithm::~FbrAlgorithm()
{
    delete new_Lru;
    delete m_Lru;
}


DataType FbrAlgorithm::Get(KeyType key)
{
    FbrNode *node = static_cast<FbrNode *>(new_Lru->Get(key));
    // checks if new contains the  key, if true: regular lru get.
    if (node != nullptr)
    {
        return node->_blockBuff;
    }
    if (m_exist)
    {
        auto temp = m_Lru->FbrGet(key);
        auto block_pair = std::make_pair(temp.first, static_cast<FbrNode *>(temp.second));
        //checks if M contains the  key, if true: count++, and set it in the new list.
        if (block_pair.second != nullptr)
        {
            block_pair.second->_count++;
            SetNew(block_pair.first, block_pair.second);
            return block_pair.second->_blockBuff;
        }
    }
    auto temp = GetOld(key);
    auto block_pair = std::make_pair(temp.first, temp.second);
    if (block_pair.second != nullptr)
    {
        block_pair.second->_count++;
        SetNew(block_pair.first, block_pair.second);
        return block_pair.second->_blockBuff;
    }
    return nullptr;
}

void FbrAlgorithm::SetNew(KeyType key, FbrNode *node)
{

    node->_type = NEW;
    auto temp = new_Lru->FbrSet(key ,node);
    auto block_pair = std::make_pair(temp.first, static_cast<FbrNode *>(temp.second));

    if (block_pair.second != nullptr)
    {
        if (m_exist)
        {
            SetM(block_pair.first, block_pair.second);
        }
        else
        {
            SetOld(block_pair.first, block_pair.second);
        }
    }
};


void FbrAlgorithm::SetM(KeyType key, FbrNode *node)
{
    node->_type = MIDDLE;
    auto temp = m_Lru->FbrSet(key, node);
    auto block_pair = std::make_pair(temp.first, static_cast<FbrNode *>(temp.second));
    if (block_pair.second != nullptr)
    {
        SetOld(block_pair.first, block_pair.second);
    }
};

void FbrAlgorithm::SetOld(KeyType key, FbrNode *node)
{
    node->_type = OLD;

    old_Lfu->Cache.insert({key, node});
    old_Lfu->lfu.insert({key, node->_count});
    if (old_Lfu->lfu.size() == old_Lfu->_capacity)
    {
        auto temp = *(old_Lfu->lfu.rbegin());
        FbrNode* oldnode = old_Lfu->Cache[temp.first];
        FreeFbrNode(oldnode);
        old_Lfu->Cache.erase(temp.first);
        old_Lfu->lfu.erase(temp);
    }
};

std::pair<KeyType, FbrNode*> FbrAlgorithm::GetOld(KeyType key)
{
    auto item = old_Lfu->Cache.find(key);
    if (item == old_Lfu->Cache.end())
        return std::make_pair(key, (FbrNode *) nullptr);

    std::cout << "before evict: " << std::endl;
    PrintCache();
    FbrNode *node = item->second;
    old_Lfu->lfu.erase({key, node->_count});
    std::cout << "after evict: " << std::endl;
    PrintCache();
    std::cout << "evicted" << std::endl;
    node->_count++;
    old_Lfu->Cache.erase(item);
    return std::make_pair(key, node);
};

int FbrAlgorithm::Set(KeyType key, DataType data)
{
    SetNew(key, new FbrNode(data));
    return 0;
};

void FbrAlgorithm::PrintCache()
{
    new_Lru->PrintCache();
    if (m_exist)
    {
        m_Lru->PrintCache();
    }
    for (auto &i : old_Lfu->lfu) {

        std::cout << i.first.first << " " << i.first.second << std::endl;
    }

}
