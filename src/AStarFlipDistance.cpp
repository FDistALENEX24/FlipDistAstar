
#include "AStarFlipDistance/AStarFlipDistance.hpp"
#include "AStarFlipDistance/HeuristicDistanceCalculator.hpp"

AStarFlipDistance::AStarFlipDistance( std::vector<Point_2> vertices, std::vector<Triangle> start_triangulation,
                                     std::vector<Triangle> target_triangulation) : _triangulation(vertices,GeometricObjects(vertices)),
                                                                                          _target_triangulation(GeometricObjects(vertices),target_triangulation){
    _triangulation.init_triangulation(start_triangulation);
}



void AStarFlipDistance::run() {

}

void AStarFlipDistance::run_epptein() {
    auto t1 = std::chrono::high_resolution_clock::now();

    bool found=false;
    //initial values for start_triangulation
    int heuristic_distance=_triangulation.get_eppstein_heuristic_parent(_target_triangulation);
    _initial_heuristic=heuristic_distance;
    int shortest_path_distance=0;
    int estimated_distance=heuristic_distance*1000100;
    if(heuristic_distance==0){
        _flip_distance=0;
        has_run=true;
        return;
    }
    _list.insert_or_decrease_key_if_not_in_closed(_triangulation.get_representation(),estimated_distance,shortest_path_distance);

    while(true){
        auto t_timeout = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::high_resolution_clock ::now() - t1;
        if(duration > std::chrono::minutes(3)){
            std::cout<<" timed out";
            std::vector<Triangle> cur;
            for(auto x:_triangulation.get_representation()){
                cur.emplace_back(_triangulation._objects.get_triangle(x));
            }

            HeuristicDistanceCalculator tmp(_triangulation._objects.get_vertices(),cur,_target_triangulation._triangle_representation,_triangulation._objects);
            tmp.run();
            _flip_distance=-tmp.hanke_heuristic-shortest_path_distance;
            _runtime=(int)std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            has_run=true;
            return;
        }
        auto current=_list.extract_min();
        _triangulation.init_triangulation(current.first);
        shortest_path_distance=current.second.second;
        auto possible_flips=_triangulation.get_edge_representation();
        int parent_h=_triangulation.get_eppstein_heuristic_parent(_target_triangulation);

        if(_triangulation.equals_triangulation(_target_triangulation)){
            _flip_distance=shortest_path_distance;
            auto t2 = std::chrono::high_resolution_clock::now();
            _runtime=(int)std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
            return;
        }

        for(auto e:possible_flips){

            auto f=_triangulation.do_flip_implicit(_triangulation._objects.get_edge(e));
            if(f.src<0){
                continue;
            }



            heuristic_distance=_triangulation.get_eppstein_heuristic_child(_target_triangulation,e,_triangulation._objects.get_edge(f.src,f.dst));

            int x=(parent_h-heuristic_distance);
            if (abs(x)>1 ) {
                throw std::invalid_argument( "not consistent " +std::to_string(abs(x))+" "+ std::to_string(heuristic_distance)+ " " + std::to_string(parent_h));
            }
            estimated_distance=heuristic_distance*1000100+(shortest_path_distance+1)*1000000;
            _list.insert_or_decrease_key_if_not_in_closed(_triangulation.get_representation(),estimated_distance,shortest_path_distance+1);

            _triangulation.do_flip_implicit(f);
        }
    }
}

void AStarFlipDistance::run_simple() {
    auto t1 = std::chrono::high_resolution_clock::now();


    bool found=false;
    //initial values for start_triangulation
    int heuristic_distance=_triangulation.get_simple_heuristic(_target_triangulation);
    _initial_heuristic=heuristic_distance;
    int shortest_path_distance=0;
    int estimated_distance=heuristic_distance*1000100;
    //int estimated_distance=heuristic_distance;

    if(heuristic_distance==0){
        _flip_distance=0;
        has_run=true;
        return;
    }

    _list.insert_or_decrease_key_if_not_in_closed(_triangulation.get_representation(),estimated_distance,shortest_path_distance);
    while(!found){

        auto duration = std::chrono::high_resolution_clock ::now() - t1;
        if(duration > std::chrono::minutes(3)){
            std::cout<<" timed out";
            _flip_distance=-1;
            _runtime=(int)std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            has_run=true;
            return;
        }

        auto current=_list.extract_min();
        _triangulation.init_triangulation(current.first);
        shortest_path_distance=current.second.second;

        int parent_heuristic=current.second.first-shortest_path_distance*1000000;
        //int parent_heuristic=current.second.first-shortest_path_distance;

        auto possible_flips=_triangulation.get_edge_representation();

        if(_triangulation.equals_triangulation(_target_triangulation)){
            _flip_distance=shortest_path_distance;
            auto t2 = std::chrono::high_resolution_clock::now();
            _runtime=(int)std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
            return;
        }

        for(auto e:possible_flips){
            auto f=_triangulation.do_flip_implicit_for_simple_heuristic(_triangulation._objects.get_edge(e),_target_triangulation);
            if(f.first.empty()){
                continue;
            }



            int delta_heuristic=1000100*f.second;
            //int delta_heuristic=f.second;;

            estimated_distance=parent_heuristic+delta_heuristic+(shortest_path_distance+1)*1000000;
            //estimated_distance=parent_heuristic+delta_heuristic+(shortest_path_distance+1);
            _list.insert_or_decrease_key_if_not_in_closed(f.first,estimated_distance,shortest_path_distance+1);
        }
    }
}

/*void AStarFlipDistance::run_explizit() {
    auto t1 = std::chrono::high_resolution_clock::now();

    bool found=false;
    //initial values for start_triangulation
    int heuristic_distance=_triangulation.get_simple_heuristic(_target_triangulation);
    int shortest_path_distance=0;
    int estimated_distance=heuristic_distance*1000100;

    if(heuristic_distance==0){
        _flip_distance=0;
        has_run=true;
        return;
    }

    _list.insert_or_decrease_key_if_not_in_closed(_triangulation.get_representation(),estimated_distance,shortest_path_distance);
    while(!found){
        auto current=_list.extract_min();
        _triangulation.init_triangulation(current.first);
        shortest_path_distance=current.second.second;
        auto possible_flips=_triangulation.get_edge_representation();
        for(auto e:possible_flips){
            auto f=_triangulation.do_flip_implicit(e);
            if(f.src<0){
                continue;
            }

            if(_triangulation.equals_triangulation(_target_triangulation)){
                _flip_distance=shortest_path_distance+1;
                auto t2 = std::chrono::high_resolution_clock::now();
                _runtime=(int)std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
                return;
            }

            heuristic_distance=_triangulation.get_simple_heuristic(_target_triangulation);
            estimated_distance=heuristic_distance*1000100+(shortest_path_distance+1)*1000000;
            _list.insert_or_decrease_key_if_not_in_closed(_triangulation.get_representation(),estimated_distance,shortest_path_distance+1);

            _triangulation.do_flip_implicit(f);
        }
    }
}*/


int AStarFlipDistance::get_flipdistance() {
    return _flip_distance;
}

int AStarFlipDistance::get_runtime() {
    return _runtime;
}

bool AStarFlipDistance::are_triangulation_representations_equal(const std::vector<int16_t>& t_1, const std::vector<int16_t>&t_2) {
    for (int i = 0; i < t_1.size(); i++) {
        if (t_2[i] != t_1[i]) {
            return false;
        }
    }
    return true;
}

std::string AStarFlipDistance::get_data_as_string() {
    std::string result=std::to_string(get_flipdistance())+" "+std::to_string( _initial_heuristic)+" "+std::to_string(get_runtime())+" "+ std::to_string(_list.get_nr_of_extended_nodes())+ " "+ std::to_string(_list.get_nr_of_opened_nodes());

    return result;
}

