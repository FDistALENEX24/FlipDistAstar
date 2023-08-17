
#ifndef A_STAR_FOR_FLIPDISTANCE_GEOMETRICOBJECTS_HPP
#define A_STAR_FOR_FLIPDISTANCE_GEOMETRICOBJECTS_HPP

#include <utility>

#include "vector"
#include "AStarFlipDistance/BasicDataStructures.hpp"
#include "AStarFlipDistance/Geometry.hpp"


class GeometricObjects {

public:
     GeometricObjects()=default;
     GeometricObjects(std::vector<Point_2> vertices,bool compute_distance=true);

    void print_stuff();

    Edge get_edge(int e_index);
    int get_edge(int e_0,int e_1);
    int get_number_of_vertices();
    int get_number_of_triangles();
    Triangle get_triangle(int i);
    int get_triangle(Triangle t);
    int get_triangle(int i,int j, int k);
    bool is_flip_valid(int e_0, int e_1,int f_0,int f_1);
    bool is_flip_valid(int edge_0, int edge_1);

    int get_number_of_edges();
    void compute_distance_matrix();

    int get_distance(int edge_ind_1,int edge_ind_2);

    std::vector<Point_2> get_vertices();

private:
    int n=0;
    int m=0;
    std::vector<Point_2> _vertices;
    std::vector<Edge> _edge_list;
    std::vector<std::vector<int>> _edges_matrix;
    std::vector<Triangle> _triangle_list;
    std::vector<int> _triangle_is_allowed_list;
    std::vector<std::vector<std::vector<int>>> _triangle_matrix;
    std::vector<std::vector<int>> _allowed_flips;
    std::vector<std::vector<int>> _edge_distance_matrix;


};


#endif //A_STAR_FOR_FLIPDISTANCE_GEOMETRICOBJECTS_HPP
