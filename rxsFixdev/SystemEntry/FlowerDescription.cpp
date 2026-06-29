#include <iostream>
#include <fstream>
#include <string>

#include <vector>


using namespace std;



typedef struct _iflsdb {
    char type;
    std::string Name;
    //std::vec

};



void Anasyer(const char *path) {
    std::ifstream file(path);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {





        }
        file.close();
    }
    else {
        std::cout << "无法打开文件" << std::endl;
    }
}
