#pragma once

#include <cstddef>

namespace rcstr
{

class string_data;

class string
{
public:
    string();

    string(const string & other);
    
    string(const char *text);

    ~string();

    const char & at(std::size_t pos) const;

    const char * c_str() const;
    
    void push_back(char c);
private:
    string_data * shared_data;
};

}