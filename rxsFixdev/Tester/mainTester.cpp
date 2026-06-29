#include "uintTester.h"
#include <stdio.h>
#include <typeinfo>
#include <string>
#include <stdarg.h>

#define FUNCTION_INFO(Type)	printf("ERROR:%s LINE:%d %s(%s) \n", __FILE__, __LINE__, __FUNCTION__,typeid(Type).name());

#define FUNCTION_INFO(...)	function_info_myp(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__);





void function_info_myp(const char* filenName, int line, const char* funName, ...);



void function_info_myp(const char* filenName, int line, const char* funName, ...) {
	va_list valist;
    int i;
    int num = 3;

    std::string params;
    std::string p;

    va_start(valist, num);
    for (i = 0; i < num; i++)
    {
        p = va_arg(valist, char *);

        typeid(p.c_str()).name();

        params[params.length() - 1] = ',';
    }
    va_end(valist);

	printf("ERROR:%s LINE:%d %s(%s) \n", filenName, line, funName, typeid(Type).name());
}



void Test1(void* a) {
	FUNCTION_INFO(a)
}


int main() {

	std::string x = " hello ";
	Test1(&x);
 }




