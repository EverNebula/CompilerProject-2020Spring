// this is a silly solution
// just to show you how different
// components of this framework work
// please bring your wise to write
// a 'real' solution :)

#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <cstring>
#include <dirent.h>
#include <json/json.h>
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
    std::string cheat_src =
"// this is supposed to be generated by codegen tool\n\
#include \"../run.h\"\n\
\n\
void kernel_example(float (&B)[32][16], float (&C)[32][16], float (&A)[32][16]) {\n\
    for (int i = 0; i < 32; ++i) {\n\
        for (int j = 0; j < 16; ++j) {\n\
            A[i][j] = B[i][j] * C[i][j];\n\
        }\n\
    }\n\
}";
    std::ofstream ofile("./kernels/kernel_example.cc", std::ios::out);
    ofile << cheat_src;
    ofile.close();
    return 0;
}

void real(){
    //std::cout << "Try to open directory PROJ_ROOT_DIR/project1/cases" << std::endl;

    DIR *dir;
    struct  dirent *ent;
    if ((dir = opendir ("../project1/cases/")) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            char jsonfilename[256] = "../project1/cases/";
            if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name,"..") == 0)
                continue;
            strcat(jsonfilename, ent->d_name);
            printf ("%s\n", jsonfilename);
            std::ifstream ifile(jsonfilename, std::ios::in);
            readjson(ifile);
            break;
        }
        closedir (dir);
    } else {
    /* could not open directory */
        std::cout << "1Could not open directory \"cases\"" << std::endl;
        return ;
    }
}


void parse_Id(string str)
{
    dprintf("Id: %s\n", str.c_str());
}

void parse_IntV(string str)
{
    dprintf("IntV: %s\n", str.c_str());
}

void parse_FloatV(string str)
{
    dprintf("FloatV: %s\n", str.c_str());
}

void parse_IdExpr(string str)
{
    dprintf("IdExpr: %s\n", str.c_str());
}

void parse_Const(string str)
{
    dprintf("Const: %s\n", str.c_str());

    int idx = 0;

    if((idx = str.find('.', idx)) != string::npos)
    {
        parse_FloatV(str);
    }
    else
    {
        parse_IntV(str);
    }
}

void parse_AList(string str)
{
    dprintf("AList: %s\n", str.c_str());
    int idx = 0, las = 0;

    while((idx = str.find(',', idx)) != string::npos)
    {
        parse_IdExpr(str.substr(las, idx-las));
        las = ++idx;
    }
    parse_IdExpr(str.substr(las));
}

void parse_CList(string str)
{
    dprintf("CList: %s\n", str.c_str());
    int idx = 0, las = 0;

    while((idx = str.find(',', idx)) != string::npos)
    {
        parse_IntV(str.substr(las, idx-las));
        las = ++idx;
    }
    parse_IntV(str.substr(las));
}

void parse_TRef(string str)
{
    dprintf("TRef: %s\n", str.c_str());

    int idx = 0, las = 0;
    // id part
    if((idx = str.find('<', idx)) != string::npos)
    {
        parse_Id(str.substr(0, idx));
        las = idx + 1;

        if((idx = str.find('[', idx)) != string::npos)
        {
            parse_CList(str.substr(las, idx-las-1));
            parse_AList(str.substr(idx+1, str.length()-idx-2));
        }
        else
        {
            std::cout << "invalid statement. (without [)" << std::endl;
        }
    }
    else
    {
        std::cout << "invalid statement. (without <)" << std::endl;
    }

}

void parse_SRef(string str)
{
    dprintf("SRef: %s\n", str.c_str());

    int idx = 0;
    // id part
    if((idx = str.find('<', idx)) != string::npos)
    {
        parse_Id(str.substr(0, idx));
        parse_CList(str.substr(idx+1, str.length()-idx-2));
    }
    else
    {
        std::cout << "invalid statement. (without <)" << std::endl;
    }

}

void parse_LHS(string str)
{
    dprintf("LHS: %s\n", str.c_str());
    parse_TRef(str);
}

void parse_RHS(string str)
{
    dprintf("RHS: %s\n", str.c_str());

    int idx = 0, las = 0, len = str.length();
    int bkt = 0;

    // +   -
    for (idx = 0; idx < len; ++idx)
    {
        if (str[idx] == '(')
            bkt++;
        else if (str[idx] == ')')
            bkt--;
        else if (bkt == 0)
        {
            if (str[idx] == '+' || str[idx] == '-')
                break;
        }
    }

    if (idx != len)
    {
        parse_RHS(str.substr(idx+1));
        parse_RHS(str.substr(0, idx));
        return;
    }

    // *   /   %   //($)
    for (idx = 0; idx < len; ++idx)
    {
        if (str[idx] == '(')
            bkt++;
        else if (str[idx] == ')')
            bkt--;
        else if (bkt == 0)
        {
            if (str[idx] == '*' || str[idx] == '/' 
                || str[idx] == '%' || str[idx] == '$')
                break;
        }
    }

    if (idx != len)
    {
        parse_RHS(str.substr(idx+1));
        parse_RHS(str.substr(0, idx));
        return;
    }


    // bracket
    if (str[0] == '(' && str[len-1] == ')')
    {
        parse_RHS(str.substr(1, len-2));
        return;
    }


    // TRef
    if ((idx = str.find('[', 0)) != string::npos)
    {
        parse_TRef(str);
        return;   
    }

    // SRef
    if ((idx = str.find('<', 0)) != string::npos)
    {
        parse_SRef(str);
        return;
    }

    // Const
    parse_Const(str);
}

void parse_S(string str)
{
    dprintf("S: %s\n", str.c_str());
    int idx = 0;

    if((idx = str.find('=', idx)) != string::npos)
    {
        parse_LHS(str.substr(0, idx));
        parse_RHS(str.substr(idx+1));
    }
    else
    {
        std::cout << "invalid statement. (without =)" << std::endl;
    }
}

void parse_P(string str)
{
    int idx = 0, las = 0;

    // erase all space
    while((idx = str.find(' ', idx)) != string::npos)
    {
        str.erase(idx,1);
    }
    idx = 0;
    while((idx = str.find("//", idx)) != string::npos)
    {
        str.replace(idx, 2, "$");
    }
    dprintf("P: %s\n", str.c_str());

    idx = 0;
    while((idx = str.find(';', idx)) != string::npos)
    {
        parse_S(str.substr(las, idx-las));
        las = ++idx;
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
    std::cout << root_group["name"] << std::endl;
    std::cout << root_group["kernel"] << std::endl;
    
    parse_P(string("A<16, 32>[i, j] = A<16, 32>[i, j] + alpha<1> * (B<16, 32>[i, k] * C<32, 32>[k, j]); A<16, 32>[i, j] = A<16, 32>[i, j] + beta<1> * D<16, 32>[i, j];"));
}