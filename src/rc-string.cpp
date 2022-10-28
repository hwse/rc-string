#include <rcstr/rc-string.hpp>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <stdexcept>

// DEBUG
#include <iostream>

namespace rcstr
{

// === utility ===


char * char_malloc(size_t count)
{
    std::cout << "char_malloc(" << count << ");" << std::endl;
    return static_cast<char *>(std::malloc(sizeof(char) * (count + 1)));
}

char* char_realloc(char * buffer, size_t new_size)
{
    std::cout << "char_realloc(..., " << new_size << ");" << std::endl;
    return static_cast<char *>(std::realloc(static_cast<void*>(buffer), sizeof(char) * (new_size + 1)));
}

void char_free(char * buffer)
{
    std::cout << "char_free(...);" << std::endl;
    std::free(buffer);
}


// === string_data implementation ===

class string_data
{
public:
    string_data(const char * c, size_t length);

    string_data(const string_data & other);
    ~string_data();
    void inc_ref_count();
    bool dec_ref_count();
    void ensure_capacaty(size_t wanted_capacity);

    size_t ref_count;
    char* buffer;
    size_t length;
    size_t capacity;
};

string_data::string_data(const char * c, size_t length)
    : ref_count(1),
    buffer(nullptr),
    length(length),
    capacity(length)
{
    assert(length != 0);
    buffer = char_malloc(length);
    std::copy(c, c + length, buffer);
    buffer[length] = '\0';
}

string_data::string_data(const string_data & other)
    : ref_count(1),
    buffer(nullptr),
    length(other.length),
    capacity(other.length)
{
    buffer = char_malloc(length);
    std::copy(other.buffer, other.buffer + other.length, buffer);
    buffer[length] = '\0';
}

string_data::~string_data()
{
    char_free(buffer);
}

void string_data::inc_ref_count()
{
    ref_count++;
}

bool string_data::dec_ref_count()
{
    assert(ref_count != 0);
    ref_count--;
    return ref_count == 0;
}

void string_data::ensure_capacaty(size_t wanted_capacity)
{
    if (wanted_capacity > capacity)
    {
        buffer = char_realloc(buffer, wanted_capacity);
        capacity = wanted_capacity;
    }
}

// === string_data utils === 

string_data * make_single_ref(string_data * data)
{
    assert(data->ref_count != 0);
    // already only one user, reuse
    if (data->ref_count == 1)
    {
        return data;
    }
    // copy the data
    auto result = new string_data(*data);
    data->dec_ref_count();
    return result;
}

// === string implementation ===

string::string() : shared_data(nullptr)
{   
}

string::string(const string & other) : shared_data(nullptr)
{
    if (other.shared_data != nullptr)
    {
        shared_data = other.shared_data;
        shared_data->inc_ref_count();
    }
}

string::string(const char * text) : shared_data(nullptr)
{
    const auto len = std::strlen(text);
    if (len != 0)
    {
        shared_data = new string_data(text, len);
    }
}

string::~string()
{
    if (!shared_data)
    {
        return;
    }

    if (!shared_data->dec_ref_count())
    {
        return;
    }

    delete shared_data;
}

const char & string::at(std::size_t pos) const
{
    if (shared_data == nullptr || pos >= shared_data->length)
    {
        throw std::out_of_range("out of index");
    }
    return shared_data->buffer[pos];
}

const char * string::c_str() const
{
    static constexpr char * EMPTY = ""; 
    if (shared_data == nullptr)
    {
        return EMPTY;
    }
    return shared_data->buffer;
}

void string::push_back(char c)
{
    shared_data = make_single_ref(shared_data);

    if (shared_data->capacity <= shared_data->length)
    {
        const size_t new_capa = shared_data->capacity < 16 ? 16 : shared_data->capacity * 3 / 2;
        shared_data->ensure_capacaty(new_capa);
    }

    assert(shared_data->capacity > shared_data->length);
    shared_data->buffer[shared_data->length] = c;
    shared_data->buffer[shared_data->length+1] = '\0';
    shared_data->length++;
}


} // namespace rcstr
