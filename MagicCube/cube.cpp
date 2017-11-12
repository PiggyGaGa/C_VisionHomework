#include "MagicCube.hpp"
#include <iostream>
#include <vector>
#include <thread>
using std::cout; using std::cin; using std::endl;
using std::vector;

int main()
{
    Cube myCube;
    
    std::thread task0(&Cube::ImageProducer, myCube);
    std::thread task1(&Cube::ImageShow, myCube);

    task0.join();
    task1.join();

    return 0;
}
