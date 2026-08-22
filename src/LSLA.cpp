#include <iostream>

namespace LSLA
{
    void HelloWorld()
    {
        std::cout << "Hello, World!" << std::endl;
    }

    class Vector
    {
    private:
        int size;
        float *data;
    public:
        Vector(int size)
        {
            this->size = size;
            this->data = new float[size];
        }
        ~Vector(){
            delete [] data;
        }
        float& operator[](int index);
    };
}