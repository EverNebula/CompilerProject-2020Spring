#include <string>
#include <sstream>

class Printer{
 public:
    Printer() {
        tabn = 0;
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
    }

    void exit() {
        tabn -= 1;
    }

 private:
    std::ostringstream oss;
    int tabn;
};

