
#ifndef A_STAR_FOR_FLIPDISTANCE_ASTARFLIPDISTANCE_HPP
#define A_STAR_FOR_FLIPDISTANCE_ASTARFLIPDISTANCE_HPP

#include "AStarFlipDistance/GeometricObjects.hpp"
#include "AStarFlipDistance/StaticTriangulation.hpp"
#include "AStarFlipDistance/TriangulationHandler.hpp"
#include "ListHandler.hpp"
#include <chrono>
class AStarFlipDistance {
public:
    AStarFlipDistance(std::vector<Point_2> vertices,  std::vector<Triangle> start_triangulation,  std::vector<Triangle> target_triangulation );
    void run();
    void run_epptein();
    void run_simple();
    int get_flipdistance();
    int get_runtime();
    std::string get_data_as_string();

    ListHandler _list;

private:

    static bool are_triangulation_representations_equal(const std::vector<int16_t>& t_1, const std::vector<int16_t>&t_2) ;

    TriangulationHandler _triangulation;
    StaticTriangulation _target_triangulation;
    int _flip_distance;
    bool has_run=false;
    int _runtime;
    int _initial_heuristic=-22;

};


#endif //A_STAR_FOR_FLIPDISTANCE_ASTARFLIPDISTANCE_HPP
