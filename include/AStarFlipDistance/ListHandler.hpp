
#ifndef A_STAR_FOR_FLIPDISTANCE_LISTHANDLER_HPP
#define A_STAR_FOR_FLIPDISTANCE_LISTHANDLER_HPP
#include <unordered_map>
#include <vector>
#include <iostream>

class Hasher
{
public:

    std::size_t operator() (std::vector<int16_t> const& key) const
        {
            std::size_t seed = key.size();

            // Combine each element and its position to compute the hash
            for (std::size_t i = 0; i < key.size(); ++i) {
                std::hash<int16_t> hasher;
                seed ^= hasher(key[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2) + i;
            }

            return seed;
        }

//    std::size_t operator() (std::vector<int16_t> const& key) const
//    {
//        std::size_t hash = 0;
//
//        for (const auto& element : key) {
//            std::hash<int> hasher;
//            hash = (hash * 31) + hasher(element);
//        }
//
//        return hash;
//    }

//    std::size_t operator() (std::vector<int16_t> const& key) const
//    {
//        int hash = 0;
//        for(int i=0; i<key.size(); i++)
//        {
//            hash += (53*hash + key[i]);
//            //hash += (11483*hash + key[i]);
//        }
//        return hash;
//    }

};
class EqualFn
{
public:
    bool operator() (std::vector<int16_t>  const& t1, std::vector<int16_t>  const& t2) const
    {
        return std::equal(t1.begin(), t1.end(), t2.begin());

//        for(int i=0;i<t1.size();i++){
//            if(t1[i]!=t2[i]){
//                return false;
//            }
//        }
//        return true;
    }
};


class ListHandler {
public:
    ListHandler() = default;
    std::pair<std::vector<int16_t>, std::pair<int,int>> extract_min();
    bool insert_or_decrease_key_if_not_in_closed(const std::vector<int16_t>& triangulation, int p_new_estimated_Distance ,int shortest_path_distance);
    int get_nr_of_extended_nodes();
    int get_nr_of_opened_nodes();

private:

    std::unordered_map<std::vector<int16_t>,int,Hasher,EqualFn> _open;
    std::unordered_map<std::vector<int16_t>,int,Hasher,EqualFn> _closed;
    std::vector<std::pair<std::vector<int16_t>,std::pair<int,int>>> _heap;


    void insert(const std::vector<int16_t>& triangulation, int p_estimated_Distance,int shortest_path_distance);
    void decrease_Key(const std::vector<int16_t>& triangulation, int p_new_estimated_Distance ,int shortest_path_distance);

    bool is_Empty();
    static int Parent(int i);
    static int LeftChild(int i);
    static int RightChild(int i);




};




#endif //A_STAR_FOR_FLIPDISTANCE_LISTHANDLER_HPP
