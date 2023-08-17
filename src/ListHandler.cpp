
#include "AStarFlipDistance/ListHandler.hpp"




int ListHandler::Parent(int i) { return (i - 1) / 2; }

int ListHandler::LeftChild(int i) { return 2 * i + 1; }

int ListHandler::RightChild(int i) { return 2 * i + 2; }


void ListHandler::insert(const std::vector<int16_t>& triangulation, int p_new_estimated_Distance ,int shortest_path_distance){
    _heap.emplace_back(triangulation, std::pair(p_new_estimated_Distance,shortest_path_distance));
    int i = _heap.size() - 1;
    _open[triangulation] = i;

    while (i > 0 and std::get<1>(_heap[Parent(i)]).first > std::get<1>(_heap[i]).first){
        std::swap(_heap[i], _heap[Parent(i)]);
        _open[std::get<0>(_heap[i])] = i;
        _open[std::get<0>(_heap[Parent(i)])] = Parent(i);
        i = Parent(i);
    }
}


void ListHandler::decrease_Key(const std::vector<int16_t>&  triangulation, int p_new_estimated_Distance ,int shortest_path_distance ){
    int i = _open[triangulation];

    if(_heap[i].second.second<=shortest_path_distance){
        return;
    }
    _heap[i] =  std::pair(triangulation, std::pair(p_new_estimated_Distance,shortest_path_distance));

    while (i > 0 and std::get<1>(_heap[Parent(i)]).first > std::get<1>(_heap[i]).first){
        std::swap(_heap[i], _heap[Parent(i)]);
        _open[std::get<0>(_heap[i])] = i;
        _open[std::get<0>(_heap[Parent(i)])] = Parent(i);
        i = Parent(i);
    }
}


bool ListHandler::is_Empty(){
    if(_heap.size() > 0){
        return false;
    }else{
        return true;
    }
}

std::pair<std::vector<int16_t>, std::pair<int,int>> ListHandler::extract_min(){
    if(_heap.size() > 1) {
        std::pair<std::vector<int16_t>, std::pair<int,int>> min = _heap[0];
        _heap[0] = _heap.back();
        _heap.pop_back();
        _open.erase(std::get<0>(min));
        _open[std::get<0>(_heap[0])] = 0;
        _closed.insert(std::pair(min.first,1));

        int i = 0;
        while (LeftChild(i) < _heap.size()) {
            int j = LeftChild(i);

            if (j + 1 < _heap.size() && std::get<1>(_heap[j + 1]).first < std::get<1>(_heap[j]).first) {
                ++j;
            }

            if (std::get<1>(_heap[i]).first > std::get<1>(_heap[j]).first) {
                std::swap(_heap[i], _heap[j]);
                _open[std::get<0>(_heap[i])] = i;
                _open[std::get<0>(_heap[j])] = j;
                i = j;
            } else {
                break;
            }
        }

        return min;
    }else{
        std::pair<std::vector<int16_t>, std::pair<int,int>> min = _heap[0];
        _heap[0] = _heap.back();
        _heap.pop_back();
        _open.erase(std::get<0>(min));
        return min;

    }
}

bool ListHandler::insert_or_decrease_key_if_not_in_closed(const std::vector<int16_t>& triangulation, int p_new_estimated_Distance ,int shortest_path_distance) {
    if(_closed.find(triangulation)!= _closed.end()){
        return false;
    }
    if(_open.find(triangulation)==_open.end()){
        insert(triangulation,p_new_estimated_Distance,shortest_path_distance);
    }
    else{
        decrease_Key(triangulation,p_new_estimated_Distance,shortest_path_distance);
    }
    return true;
}

int ListHandler::get_nr_of_extended_nodes() {
    return _closed.size();
}

int ListHandler::get_nr_of_opened_nodes() {
    return _open.size();
}
