#include <string>
#include <sstream>

class Printer{
 public:
    Printer() {
        tabn = 0;
        index = 4;
    }
    std::string print(const Expr&);
    std::string print(const Stmt&);
    std::string print(const Group&);

    void print_indent() {
        for (int i = 0; i < tabn; ++i)
            oss << "\t";
    }

    void enter() {
        tabn += 1;
        index += 4;
    }

    void exit() {
        tabn -= 1;
        index -= 4;
    }

 private:
    std::ostringstream oss;
    int tabn;
    int index; 
};

