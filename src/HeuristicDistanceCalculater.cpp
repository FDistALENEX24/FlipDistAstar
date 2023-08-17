
#include <utility>

#include "AStarFlipDistance/HeuristicDistanceCalculator.hpp"

HeuristicDistanceCalculator::HeuristicDistanceCalculator(std::vector<Point_2> vertices,
                                                         std::vector<Triangle> triangulation_1,
                                                         std::vector<Triangle> triangulation_2) :_objects(GeometricObjects(vertices)), _start(vertices,_objects),
                                                                                                  _target(_objects,triangulation_2){
    _start.init_triangulation(triangulation_1);
    _vertices=vertices;
}

void HeuristicDistanceCalculator::run() {
    auto copy_triang=_start.get_triangle_representation();
    simple_heuristic=_start.get_simple_heuristic(_target);
    eppstein_heuristic=_start.get_eppstein_heuristic_parent(_target);
    hanke_heuristic=_start.get_heuristic_upper_bound(_target);
    _start.init_triangulation(copy_triang);
}

HeuristicDistanceCalculator::HeuristicDistanceCalculator(std::vector<Point_2> vertices,
                                                         std::vector<Triangle> triangulation_1,
                                                         std::vector<Triangle> triangulation_2,
                                                         GeometricObjects objects):_objects(std::move(objects)), _start(vertices,_objects),
                                                                                   _target(_objects,triangulation_2){
    _start.init_triangulation(triangulation_1);
    _vertices=vertices;

}
