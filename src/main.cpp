#include "tfy/tasks.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc,char** argv) {
    try {
        int task=0; bool all=(argc==1);
        std::size_t limit=3;
        std::filesystem::path dotDir;
        for(int i=1;i<argc;++i) {
            std::string arg=argv[i];
            if(arg=="--all") all=true;
            else if(arg=="--task" && i+1<argc)task=std::stoi(argv[++i]);
            else if(arg=="--limit" && i+1<argc)limit=std::stoul(argv[++i]);
            else if(arg=="--dot-dir" && i+1<argc)dotDir=argv[++i];
            else if(arg=="--help"||arg=="-h") {
                std::cout << "TFY RGR, variant 17 (C++17)\n"
                  << "Usage: rgr --task N [--limit 1..5] [--dot-dir DIR]\n"
                  << "       rgr --all [--limit 1..5] [--dot-dir DIR]\n";
                return 0;
            } else throw std::invalid_argument("Unknown or incomplete option: "+arg);
        }
        if(all) for(int n=1;n<=12;++n)tfy::runTask(n,std::cout,limit,dotDir);
        else if(task)tfy::runTask(task,std::cout,limit,dotDir);
        else throw std::invalid_argument("Specify --task N or --all");
    } catch(const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';return 1;
    }
    return 0;
}
