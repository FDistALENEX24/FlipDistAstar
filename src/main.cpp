#include <iostream>
#include <filesystem>
#include <regex>
#include "AStarFlipDistance/Geometry.hpp"
#include "AStarFlipDistance/BasicDataStructures.hpp"
#include "AStarFlipDistance/GeometricObjects.hpp"
#include "AStarFlipDistance/TriangulationHandler.hpp"
#include "AStarFlipDistance/AStarFlipDistance.hpp"
#include "AStarFlipDistance/HeuristicDistanceCalculator.hpp"
#include "AStarFlipDistance/HigherOrderDelaunayObjects.hpp"
#include "AStarFlipDistance/FlipIlpEdgeBased.hpp"
#include "AStarFlipDistance/BiDirectionalBfs.hpp"

const int SIMPLE=0;
const int EPPSTEIN=1;
const int HEURISTIC=2;
const int BFS=3;
const int ILPEDGE=4;

std::vector<std::string> get_file_names_in_folder(std::string foldername){


    std::vector<std::string> fileNames;

    for (const auto& entry : std::filesystem::directory_iterator(foldername)) {
        if (entry.is_regular_file()) {
            fileNames.push_back(entry.path().filename().string());
        }
    }
    std::sort(fileNames.begin(),fileNames.end());
    return fileNames;
}

std::vector<std::pair<std::string,std::string>> get_file_pairs_in_folder(std::string foldername){
    auto names= get_file_names_in_folder(foldername);
    std::vector<std::pair<std::string,std::string>> result;
    for(int i=0;i<names.size();i++){
        for(int j=i+1;j<names.size();j++){
            result.emplace_back(foldername+"/"+names[i],foldername+"/"+names[j]);
        }
    }
    std::sort(result.begin(),result.end());
    return result;
}

std::vector<Point_2> load_vertices(std::string name){
    std::vector<Point_2> points;
    std::ifstream file(name);

    if (!file.is_open()) {
        std::cout << "Failed to open file: " << name << std::endl;
        return points;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        double x;
        double y;
        if (!(iss >> x >> y)) {
            std::cout << "Invalid line: " << line << std::endl;
            continue;
        }
        points.emplace_back(x,y);
    }

    file.close();
    return points;
}

std::vector<Triangle> load_triangulation(std::string name){
    std::ifstream file(name);
    std::string line;
    std::vector<Triangle> result;

    if (file.is_open()) {
        while (getline(file, line)) {
            std::istringstream iss(line);
            int num1, num2, num3;

            if (iss >> num1 >> num2 >> num3) {
                result.emplace_back(num1,num2,num3);
            } else {
                std::cout << "Invalid line format: " << line << std::endl;
            }
        }
        file.close();
    } else {
        std::cout << "Unable to open file: " << name << std::endl;
    }
    return result;
}


int get_year(const std::string& filename){
    std::regex yearRegex(R"(_(\d{4})_)");

    std::smatch match;
    int year=-1;
    if (std::regex_search(filename, match, yearRegex)) {
        std::string yearString = match.str(1);
        year = std::stoi(yearString);
    }




    return year;
}

std::vector<std::string> do_one_product_of_instances_sea_level(const std::string& folder_path,int type){
    std::string outputfile=folder_path+"/results";
    std::string header;
    std::vector<std::string> result;
    if(type==SIMPLE){
        std::cout<<"doing_simple"<<std::endl;
        outputfile+="_simple.csv";
        header="file1 file2 flip heuristic runtime closed open\n";
    }
    else if (type==EPPSTEIN){
        std::cout<<"doing_eppstein"<<std::endl;
        outputfile+="_eppstein.csv";
        header="file1 file2 flip heuristic runtime closed open\n";
    }
    else if (type==HEURISTIC){
        std::cout<<"doing_heuristic"<<std::endl;
        outputfile+="_heuristic.csv";
        header="file1 file2 simple eppstein hanke \n";
    }

    else if (type==BFS){
        std::cout<<"doing_heuristic"<<std::endl;
        outputfile+="_bfs.csv";
        header="file1 file2 flip runtime closed open \n";
    }
    else if (type==ILPEDGE){
        std::cout<<"doing_ILP"<<std::endl;
        outputfile+="_ilp.csv";
        header="file1 file2 flip runtime\n";
    }

    std::string triangulation_path=folder_path+"/triangulation";
    std::string points_path=folder_path+"/points.csv";
    auto file_pairs= get_file_pairs_in_folder(triangulation_path);
    auto points=load_vertices(points_path);

    std::vector<Triangle> loaded_triangulation_1= load_triangulation(file_pairs[0].first);
    std::vector<Triangle> loaded_triangulation_2= load_triangulation(file_pairs[0].second);
    HeuristicDistanceCalculator dists(points,loaded_triangulation_1,loaded_triangulation_2);

    std::ofstream file(outputfile, std::ios::app);
    std::cout<<"outputfile   "<<outputfile<<std::endl;
    file<< header;

    for(auto p:file_pairs){
        int x= get_year(p.first);
        int y= get_year(p.second);
        if(x<2000||x>=2010||y<2000||y>=2010){
            std::cout<<x<<" "<<y<<std::endl;
            continue;
        }





        loaded_triangulation_1= load_triangulation(p.first);
        loaded_triangulation_2= load_triangulation(p.second);



        if(type==EPPSTEIN){
            AStarFlipDistance a_star_epp(points,loaded_triangulation_1,loaded_triangulation_2);
            a_star_epp.run_epptein();
            std::string to_append=p.first+" "+p.second+" "+a_star_epp.get_data_as_string();
            file << to_append<<std::endl;
            result.emplace_back(to_append);
            std::cout<<to_append<<std::endl;
        }
        else if (type==SIMPLE){
            AStarFlipDistance a_star_simple(points,loaded_triangulation_1,loaded_triangulation_2);
            a_star_simple.run_simple();
            std::string to_append=p.first+" "+p.second+" "+a_star_simple.get_data_as_string();
            file << to_append<<std::endl;
            result.emplace_back(to_append);
            std::cout<<to_append<<std::endl;
        }
        else if (type==HEURISTIC){
            dists.set_new_start_triangulation(loaded_triangulation_1);
            dists.set_new_target_triangulation(loaded_triangulation_2);
            dists.run();
            std::string to_append=p.first+" "+p.second+" "+std::to_string(dists.simple_heuristic)+" "+std::to_string(dists.eppstein_heuristic)+" "+std::to_string(dists.hanke_heuristic);
            file << to_append<<std::endl;
            result.emplace_back(to_append);
            std::cout<<to_append<<std::endl;
        }
        else if (type==BFS){
            BiDirectionalBfs bfs(points,loaded_triangulation_1,loaded_triangulation_2);
            bfs.run();
            dists.set_new_start_triangulation(loaded_triangulation_1);
            dists.set_new_target_triangulation(loaded_triangulation_2);
            dists.run();
            std::string to_append=p.first+" "+p.second+" "+bfs.get_data_as_string();
            file << to_append<<std::endl;
            result.emplace_back(to_append);
            std::cout<<to_append<<std::endl;
        }
        else if (type==ILPEDGE){
            HigherOrderDelaunayObjects obj(points);
            obj.compute_useful_order_k_edges_and_triangles(points.size());
            FlipIlpEdgeBased flip(GeometricTriangulation(loaded_triangulation_1,points),GeometricTriangulation(loaded_triangulation_2,points),obj);
            flip.solve();
            std::string to_append=p.first+" "+p.second+" "+std::to_string((int)flip.get_number_of_flips())+" "+ std::to_string((int)(flip.runtime*1000)) ;
            file << to_append<<std::endl;
            result.emplace_back(to_append);
            std::cout<<to_append<<std::endl;

        }

    }
    return result;

}


std::vector<std::string> do_one_product_of_instances(const std::string& folder_path,int type){
    std::string outputfile=folder_path+"/results";
    std::string header;
    std::vector<std::string> result;
    if(type==SIMPLE){
        std::cout<<"doing_simple"<<std::endl;
        outputfile+="_simple.csv";
        header="file1 file2 flip heuristic runtime closed open\n";
    }
    else if (type==EPPSTEIN){
        std::cout<<"doing_eppstein"<<std::endl;
        outputfile+="_eppstein.csv";
        header="file1 file2 flip heuristic runtime closed open\n";
    }
    else if (type==HEURISTIC){
        std::cout<<"doing_heuristic"<<std::endl;
        outputfile+="_heuristic.csv";
        header="file1 file2 simple eppstein hanke \n";
    }
    else if (type==BFS){
        std::cout<<"doing_BFS"<<std::endl;
        outputfile+="_bfs.csv";
        header="file1 file2 flip runtime closed open \n";
    }
    else if (type==ILPEDGE){
        std::cout<<"doing_ILP"<<std::endl;
        outputfile+="_ilp.csv";
        header="file1 file2 flip runtime\n";
    }

    std::string triangulation_path=folder_path+"/triangulation";
    std::string points_path=folder_path+"/points";
    auto file_pairs= get_file_pairs_in_folder(triangulation_path);
    auto points=load_vertices(points_path);

    std::vector<Triangle> loaded_triangulation_1= load_triangulation(file_pairs[0].first);
    std::vector<Triangle> loaded_triangulation_2= load_triangulation(file_pairs[0].second);
    HeuristicDistanceCalculator dists(points,loaded_triangulation_1,loaded_triangulation_2);

    std::ofstream file(outputfile, std::ios::app);
    std::cout<<"outputfile   "<<outputfile<<std::endl;
    file<< header;

    for(auto p:file_pairs){
        loaded_triangulation_1= load_triangulation(p.first);
        loaded_triangulation_2= load_triangulation(p.second);

        if(type==EPPSTEIN){
            AStarFlipDistance a_star_epp(points,loaded_triangulation_1,loaded_triangulation_2);
            a_star_epp.run_epptein();
            std::string to_append=p.first+" "+p.second+" "+a_star_epp.get_data_as_string();
            file << to_append<<std::endl;
            result.emplace_back(to_append);
            std::cout<<to_append<<std::endl;
        }
        else if (type==SIMPLE){
            AStarFlipDistance a_star_simple(points,loaded_triangulation_1,loaded_triangulation_2);
            a_star_simple.run_simple();
            std::string to_append=p.first+" "+p.second+" "+a_star_simple.get_data_as_string();
            file << to_append<<std::endl;
            result.emplace_back(to_append);
            std::cout<<to_append<<std::endl;
        }
        else if (type==HEURISTIC){
            dists.set_new_start_triangulation(loaded_triangulation_1);
            dists.set_new_target_triangulation(loaded_triangulation_2);
            dists.run();
            std::string to_append=p.first+" "+p.second+" "+std::to_string(dists.simple_heuristic)+" "+std::to_string(dists.eppstein_heuristic)+" "+std::to_string(dists.hanke_heuristic);
            file << to_append<<std::endl;
            result.emplace_back(to_append);
            std::cout<<to_append<<std::endl;
        }
        else if (type==BFS){
            BiDirectionalBfs bfs(points,loaded_triangulation_1,loaded_triangulation_2);
            bfs.run();
            dists.set_new_start_triangulation(loaded_triangulation_1);
            dists.set_new_target_triangulation(loaded_triangulation_2);
            dists.run();
            std::string to_append=p.first+" "+p.second+" "+bfs.get_data_as_string();
            file << to_append<<std::endl;
            result.emplace_back(to_append);
            std::cout<<to_append<<std::endl;
        }
        else if (type==ILPEDGE){
            HigherOrderDelaunayObjects obj(points);
            obj.compute_useful_order_k_edges_and_triangles(points.size());
            FlipIlpEdgeBased flip(GeometricTriangulation(loaded_triangulation_1,points),GeometricTriangulation(loaded_triangulation_2,points),obj);
            flip.solve();

            std::string to_append=p.first+" "+p.second+" "+std::to_string((int)flip.get_number_of_flips())+" "+ std::to_string((int)(flip.runtime*1000)) ;
            file << to_append<<std::endl;
            result.emplace_back(to_append);
            std::cout<<to_append<<std::endl;

        }

    }
    return result;

}


void do_all_products_for_size_random(std::string folder_path,int type){
    std::string outputfile=folder_path+"/results";
    std::string header;
    if(type==SIMPLE){
        std::cout<<"doing_simple"<<std::endl;
        outputfile+="_simple.csv";
        header="file1 file2 flip heuristic runtime closed open\n";
    }
    else if (type==EPPSTEIN){
        std::cout<<"doing_eppstein"<<std::endl;
        outputfile+="_eppstein.csv";
        header="file1 file2 flip heuristic runtime closed open\n";
    }
    else if (type==HEURISTIC){
        std::cout<<"doing_heuristic"<<std::endl;
        outputfile+="_heuristic.csv";
        header="file1 file2 simple eppstein hanke \n";
    }
    else if (type==BFS){
        std::cout<<"doing_BFS"<<std::endl;
        outputfile+="_bfs.csv";
        header="file1 file2 flip runtime closed open \n";
    }
    else if (type==ILPEDGE){
        std::cout<<"doing_ILP"<<std::endl;
        outputfile+="_ilp.csv";
        header="file1 file2 flip runtime\n";
    }

    std::ofstream file(outputfile, std::ios::app);
    file<< header;

    for(int i=0;i<10;i++){
        std::cout<<"Set_"<<i<<std::endl;
        std::string path_to_current_set=folder_path+"/set_"+std::to_string(i);
        auto rows=do_one_product_of_instances(path_to_current_set,type);
        for(auto x:rows){
            file<<x<<std::endl;
        }
    }

}



void do_random_experiments_fixed_one_set_with_arg(std::string type,std::string dataset,int which ,int which_secondary =1, const std::string& path_to_data="../data"){
    int type_int=-22;
    if(type=="eppstein"){
        type_int=EPPSTEIN;
    }
    if(type=="heuristic"){
        type_int=HEURISTIC;
    }
    if(type=="simple"){
        type_int=SIMPLE;
    }

    if(type=="bfs"){
        type_int=BFS;
    }
    if(type=="ilp"){
        type_int=ILPEDGE;
    }

    std::cout<<"your path: "+path_to_data<<std::endl;

    if(dataset=="sealevel"){
        std::vector<int> valid={25,30,42};
        if( std::find(valid.begin(), valid.end(), which) == valid.end()){
            std::cout<<"wrong args 1 \n";
            return;
        }
        std::vector<int> valid_orders{1,2,3,4,5,6,7,which};
        if( std::find(valid_orders.begin(), valid_orders.end(), which_secondary) == valid_orders.end()){
            std::cout<<"wrong args 2\n";
            return;
        }
        std::string folder=path_to_data+"/sealevel_experiments_paper/s_"+std::to_string(which)+"/s_"+std::to_string(which)+"_"+std::to_string(which_secondary);
        do_one_product_of_instances_sea_level(folder,type_int);

    } else if(dataset=="random"){
        std::vector<int> valid={10,15,20,25,30,35,40};
        if( std::find(valid.begin(), valid.end(), which) == valid.end()){
            return;
        }
        std::string conv_or_point="";
        if(which_secondary==1){
            conv_or_point="n_";
            std::string folder=path_to_data+"/random_experiments_paper/"+conv_or_point+std::to_string(which);
            do_all_products_for_size_random(folder,type_int);
        }else if(which_secondary==0){
            conv_or_point="c_";
            std::string folder=path_to_data+"/random_experiments_paper/"+conv_or_point+std::to_string(which)+"/set_0";
            do_one_product_of_instances(folder,type_int);
        }
        else{
            std::cout<<"wrong args \n";
        }



    } else{
        std::cout<<"wrong args \n";
    }



}



int main(int argc, char *argv[]){

    std::cout<<std::endl<<argc<<std::endl;
    if(argc>6||argc<3){
        std::cout<<"to many arguments:\nfirst:  simple eppstein heuristic bfs ilp\nsecond: random sealevel\nthird:  size \nfourth: order/convexorPointset \n fifth: path to data e.g. xyz/abc/data";
        return -22;
    }
    if(argc>5){
        std::string type=argv[1];
        std::string exp=argv[2];
        int size =std::stoi(argv[3]);
        int second =std::stoi(argv[4]);
        std::string path=argv[5];
        do_random_experiments_fixed_one_set_with_arg(type,exp,size,second,path);

    }

    if(argc==5){
        std::string type=argv[1];
        std::string exp=argv[2];
        int size =std::stoi(argv[3]);
        int second =std::stoi(argv[4]);
        do_random_experiments_fixed_one_set_with_arg(type,exp,size,second);

    }
    if(argc==4){
        std::string type=argv[1];
        std::string exp=argv[2];
        int size =std::stoi(argv[3]);
        do_random_experiments_fixed_one_set_with_arg(type,exp,size);

    }

    return 0;


}
