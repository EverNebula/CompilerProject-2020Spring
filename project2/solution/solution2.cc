// this is a silly solution
// just to show you how different
// components of this framework work
// please bring your wise to write
// a 'real' solution :)

#include <iostream>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <dirent.h>
#include <json/json.h>

#include "parser.h"
#include "IRVisitor.h"
using std::string;

// debug output
#define VERBOSE

#ifdef VERBOSE
#define dprintf(format, ...) printf(format, ##__VA_ARGS__)
#else
#define dprintf(format, ...)
#endif

void real();
void readjson(std::ifstream &ifile);

int main() {
    std::cout << "call real()" << std::endl;
    real();
    return 0;
}

void real(){
    //std::cout << "Try to open directory PROJ_ROOT_DIR/project1/cases" << std::endl;

    DIR *dir;
    struct  dirent *ent;
    if ((dir = opendir ("../project2/cases/")) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {

            char jsonfilename[256] = "../project2/cases/";
            if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name,"..") == 0)
                continue;
            strcat(jsonfilename, ent->d_name);
            printf ("%s\n", jsonfilename);
            std::ifstream ifile(jsonfilename, std::ios::in);
            readjson(ifile);
            // break;
        }
        closedir (dir);
    } else {
    /* could not open directory */
        std::cout << "Could not open directory \"cases\"" << std::endl;
        return ;
    }
}

void readjson(std::ifstream &ifile){
    Json::CharReaderBuilder rbuilder;
    rbuilder["collectComments"] = false;
    Json::Value root_group;
    Json::String errs;
    bool parse_ok = Json::parseFromStream(rbuilder, ifile, &root_group, &errs);
    if(!parse_ok){
        std::cout << "Json::parseFromStream  failed!" << std::endl;
        return;
    }
    std::string name = root_group["name"].asString();
    std::string data_type = root_group["data_type"].asString();
    std::string kernel = root_group["kernel"].asString();
    std::vector<string> insvec;
    std::vector<string> outvec;
    std::vector<string> gradto;

    for(auto &insval : root_group["ins"])
        insvec.push_back(insval.asString());
    for(auto &outval : root_group["outs"])
        outvec.push_back(outval.asString());
    for(auto &gradval : root_group["grad_to"])
        gradto.push_back(gradval.asString());

    Parser psr(name, insvec, outvec, gradto,data_type, kernel);
    psr.build_Kernel();
    // Expr tref = psr.parse_RHS("A<4>[k]+B<7,8>[j+1,j+2]");
    
    // parse_P(string("A<16, 32>[i, j] = A<16, 32>[i, j] + alpha<1> * (B<16, 32>[i, k] * C<32, 32>[k, j]); A<16, 32>[i, j] = A<16, 32>[i, j] + beta<1> * D<16, 32>[i, j];"));
}