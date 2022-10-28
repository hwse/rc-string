#include <iostream>
#include <rcstr/rc-string.hpp>

int main()
{
    rcstr::string text = "Hello World";
    const auto text2 = text;
    auto text3 = text2;
    text3.push_back('!');
    std::cout << text.c_str() << std::endl;
    return 0;
}