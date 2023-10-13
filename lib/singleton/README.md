# Implementation of singleton pattern template
There are two ways to use it:
1. use `singleton<T>` as a base class
    ```cpp
    // first example 
    #include <iostream>
    #include "singleton.h"
    
    class Test final : public singleton<Test>
    {
    public:
        void print()
        {
            std::cout << "Hello world!" << std::endl;
        }
    private:
        Test() = default;
        ~Test() = default;
    
        friend class singleton<Test>;
    };
    
    int main()
    {
        // first example
        Test::instance()->print();
        return 0;
    }
    ```

2. use `singleton<T>::get_instance()` to get the instance
    ```cpp
    // second example 
    #include <iostream>
    #include "singleton.h"

    class Test
    {
    public:
        Test() = default;
        ~Test() = default;

        void print()
        {
            std::cout << "Hello world!" << std::endl;
        }
    };

    int main()
    {
        // second example
        singleton<Test>::instance()->print();

        return 0;
    }
    ```