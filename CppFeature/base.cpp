#include <iostream>

int func1(int a) {
    return a * 10;
}

int func2(int a) {
    return a * 20;
}


int localTest(int _b) {
    int arr[4] = {1, 2, 3, 4};
    int nextInt = 5;

    int *p = arr;

    std::cout << "localTest: " << *(arr + 1) << "  " << *(p + 1) << std::endl;

    int arr2d[2][2] = {11, 22, 33, 44};
    int nextIntTest = 55;

    int *p2 = *arr2d;

    std::cout << "localTest: " << *(arr2d + 1) << "  " << *(p2 + 1) << std::endl;


    int a = 0, b = 6, c = 7;

    int d = a + c + ++c; // 7 + 8, 从左往右

    int e = d + + d;

    std::cout << "d: " << d << std::endl;
    std::cout << "e: " << e << std::endl;

    int aa = 3, bb = 2, cc = 1;

    std::cout << aa << " " << bb << " " << cc << std::endl;

    printf("%d %d\n", c, c++); //从右往左
    //va_list

    std::cout << "d: " << d << std::endl;

    int ccccc = d += b;

    std::cout << "cccc: " << ccccc << std::endl;

    long long cnt = 0;
    for(char c = 1; c < 10; c--) cnt++;

    std::cout << "cnt: " << cnt << std::endl;

    for(unsigned int i = 0; i >= 0; i--);

    return 0;
}



int main() {
    long l;
    std::cout << "long: " << sizeof(l) << std::endl;
    long long ll;
    std::cout << "long long: " << sizeof(ll) << std::endl;


    struct{
        char a;
        long long b;
        int c;
    } st;

    std::cout << "struct: " << sizeof(st) << std::endl;

    struct{
        char a;
        int b;
        long long c;
    }st2;



    std::cout << "struct: " << sizeof(st2) << std::endl;


    struct{
        char a;
        int c;
    }st3;

    std::cout << "struct: " << sizeof(st3) << std::endl;

    struct{
        int c;
        long long d;
        //char f;
    }st4;

    std::cout << "struct: " << sizeof(st4) << std::endl;

    unsigned int _a = 0x1234;
    unsigned char _b = (unsigned char)_a;

    std::cout << "endian: " << (_b == 0x34) << std::endl;


    union{
        char a[4];
        unsigned int b;
    }endian_test = { {'l', '?', '?', 'b'} };

    std::cout << (char)endian_test.b << std:: endl;


    union{
        int a;
        char b;
    }un;

    un.a = 1;

    std::cout << (unsigned int)un.b << std::endl;


    int (*f)(int) = func1;

    std::cout << "func pointer: " << f(123) << std::endl;

    int (*funcs[2])(int) = {func1, func2};
    for(int i = 0; i < 2; ++i) {
        std::cout << "funcs pointer " << i << " " << funcs[i](10) << std::endl;
    }


    int arr[3] = {1, 2, 3};

    for(int i = 0; i < 3; ++i) {
        std::cout << arr[i] << std::endl;
    }

    int (*arrPtr)[3] = &arr;
    for(int i = 0; i < 3; ++i) {
        std::cout << (*arrPtr)[i] << std::endl;
    }


    int *pp1; //野指针, 未初始化

    int *pp2 = new int(10);
    delete pp2; //悬垂指针

    int *pp3 = nullptr; //空指针

    //在c语言中()是被用来赋值运算用的,那么编译器就会以为(0,1)中仅仅当作一个值传给a[0][0],根据查看汇编源码,把最近处理的1作为结果被赋值了!
    //逗号运算符确保操作数被顺序地处理：先计算左边的操作数，再计算右边的操作数。右操作数的类型和值作为整个表达式的结果。
    long long llkh = ('c', 'd', 'e', 1);

    std::cout << "llkh: " << llkh << std::endl;


    localTest(123);

    return 0;
}


