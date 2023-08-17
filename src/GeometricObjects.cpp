#include <queue>
#include "AStarFlipDistance/GeometricObjects.hpp"

GeometricObjects::GeometricObjects(std::vector<Point_2> vertices, bool compute_distance){
    _vertices=std::move(vertices);
    n=(int)_vertices.size();
    std::vector<std::vector<int>> edge_matrix(n, std::vector<int>(n, -1));
    _edges_matrix=edge_matrix;
    std::vector<std::vector<std::vector<int>>> triangle_matrix(n, std::vector<std::vector<int>>(n, std::vector<int>(n, -1)));
    _triangle_matrix=triangle_matrix;

    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            int ind=(int)_edge_list.size();
            _edge_list.emplace_back(i,j);
            _edges_matrix[i][j]=ind;
        }
    }

    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            for(int k=j+1;k<n;k++){
                int ind=(int)_triangle_list.size();
                Triangle tmp(i,j,k);
                bool is_empty= is_triangle_empty_as_indices_in_set(i,j,k,_vertices);
                _triangle_list.emplace_back(tmp);
                _triangle_matrix[i][j][k]=ind;
                _triangle_matrix[i][k][j]=ind;
                _triangle_matrix[j][k][i]=ind;
                _triangle_matrix[j][i][k]=ind;
                _triangle_matrix[k][j][i]=ind;
                _triangle_matrix[k][i][j]=ind;
                _triangle_is_allowed_list.emplace_back((int)is_empty);
            }
        }
    }

    m=(int)_edge_list.size();
    std::vector<std::vector<int>> quad_matrix(m, std::vector<int>(m, -1));
    _allowed_flips=quad_matrix;

    for(int i=0;i<m;i++){
        for(int j=i+1;j<m;j++){
            bool allowed=false;



            Edge e_1=_edge_list[i];
            Edge e_2=_edge_list[j];


            if(e_1.src!=e_2.src &&e_1.src!=e_2.dst&&e_1.dst!=e_2.src&&e_1.dst!=e_2.dst){

                bool convex = is_quadrailateral_convex(_vertices[e_1.src],_vertices[e_1.dst],_vertices[e_2.src],_vertices[e_2.dst]);
                Triangle t1(e_1.src,e_1.dst,e_2.src);
                Triangle t2(e_1.src,e_1.dst,e_2.dst);

                bool triangles_empty= is_triangle_empty_as_indices_in_set(t1.vertices[0],t1.vertices[1],t1.vertices[2],_vertices)
                        &&is_triangle_empty_as_indices_in_set(t2.vertices[0],t2.vertices[1],t2.vertices[2],_vertices);

                //std::cout<<convex<<std::endl;
                //std::cout<<triangles_empty<<std::endl;

                allowed=convex&&triangles_empty;
            }
            else{
                allowed=false;
            }

            if(!do_arcs_intersect(_vertices[e_1.src],_vertices[e_1.dst],_vertices[e_2.src],_vertices[e_2.dst])){
                allowed=false;
            }

            _allowed_flips[i][j]=allowed;

        }
    }
    if(compute_distance==true){
        compute_distance_matrix();
    }



}

void GeometricObjects::print_stuff() {

    for (const auto& row : _edges_matrix) {
        for (const auto& element : row) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }
    for(auto e:_edge_list){
        e.print();
        std::cout<<" ";
    }
    std::cout<<std::endl;

    for(auto e:_triangle_list){
        e.print();
        std::cout<<" ";
    }
    std::cout<<std::endl;
    for(auto e:_triangle_is_allowed_list){
        std::cout<<e<<" ";
    }


    std::cout<<std::endl;
    std::cout<<_triangle_list.size()<<" size";
    std::cout<<std::endl;
    std::cout<<_vertices.size()<<" size";
    std::cout<<std::endl;

    for (const auto& row : _allowed_flips) {
        for (const auto& element : row) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }



}

int GeometricObjects::get_number_of_triangles() {
    return _triangle_list.size();
}

Triangle GeometricObjects::get_triangle(int i) {
    return _triangle_list[i];
}

int GeometricObjects::get_triangle(Triangle t) {
    return _triangle_matrix[t.vertices[0]][t.vertices[1]][t.vertices[2]];
}

bool GeometricObjects::is_flip_valid(int e_0, int e_1, int f_0, int f_1) {
    int edge_1=_edges_matrix[std::min(e_0,e_1)][std::max(e_0,e_1)];
    int edge_2=_edges_matrix[std::min(f_0,f_1)][std::max(f_0,f_1)];
    return (_allowed_flips[std::min(edge_1,edge_2)][std::max(edge_1,edge_2) ]);
}

bool GeometricObjects::is_flip_valid(int edge_1,int edge_2) {
    return (_allowed_flips[std::min(edge_1,edge_2)][std::max(edge_1,edge_2) ]);
}

int GeometricObjects::get_number_of_vertices() {
    return _vertices.size();
}

int GeometricObjects::get_edge(int e_0, int e_1) {
    return _edges_matrix[std::min(e_0,e_1)][std::max(e_0,e_1)];;
}

Edge GeometricObjects::get_edge(int e_index) {
    return _edge_list[e_index];
}

int GeometricObjects::get_number_of_edges() {
    return _edge_list.size();
}

int GeometricObjects::get_triangle(int i, int j, int k) {
    return _triangle_matrix[i][j][k];
}

void GeometricObjects::compute_distance_matrix() {
    std::vector<int> visited(_edge_list.size(),-1);
    _edge_distance_matrix= std::vector<std::vector<int>>(_edge_list.size(), std::vector<int>(_edge_list.size(), {10000}));
    std::queue<std::pair<int,int>> queue;
    for(int start=0;start<_edge_list.size();start++){
        queue=std::queue<std::pair<int,int>>();
        queue.emplace(start,0);
        visited[start]=start;
        while(!queue.empty()){
            auto current=queue.front();
            queue.pop();
            _edge_distance_matrix[start][current.first]=current.second;

            for(int next=0;next<_edge_list.size();next++){
                if(is_flip_valid(current.first,next) && visited[next]!=start){
                    queue.emplace(next,current.second+1);
                    visited[next]=start;
                }
            }
        }
    }

    /*for(int i=0;i<_edge_distance_matrix.size();i++){
        for(int j=0;j<_edge_distance_matrix.size();j++){
            std::cout<< _edge_distance_matrix[i][j]<< " ";
        }
        std::cout<<std::endl;
    }*/
}

int GeometricObjects::get_distance(int edge_ind_1, int edge_ind_2) {
    return _edge_distance_matrix[edge_ind_1][edge_ind_2];
}

std::vector<Point_2> GeometricObjects::get_vertices() {
    return _vertices;
}
