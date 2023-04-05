#include <iostream>
#include <cstring>

class String {
private:
    char* string = nullptr;
    size_t size = 0;
    size_t capacity;

public:
    String() {
        capacity = 10;
        string = new char[capacity];
    }

    String(size_t value, char element) : string(new char[2 * value]), size(value), capacity(2 * value) {
        memset(string, element, value);
    }

    String(const String &other_string) : string(new char[other_string.capacity]), size(other_string.size), capacity(other_string.capacity) {
        memcpy(string, other_string.string, size);
    }

    String& operator=(const String& other_string) {
        String copy = other_string;
        swap(copy);
        return *this;
    }

    void swap(String& other_string) {
        std::swap(other_string.string, string);
        std::swap(other_string.size, size);
        std::swap(other_string.capacity, capacity);

    }

    String(const char* c_string) {
        size_t size_c_string = strlen(c_string);
        string = new char[2 * size_c_string];
        capacity = 2 * size_c_string;
        size = size_c_string;
        for (size_t i = 0; i < size_c_string; i++) {
            string[i] = c_string[i];
        }
    }

    ~String() {
        if (capacity != 0){
            delete[] string;
        }
    }

    char& operator[](int i) {
        return string[i];
    }

    const char& operator[](int i) const {
        return string[i];
    }

    bool operator==(const String& other_string) {
        if (other_string.size != size) return false;
        for (size_t i = 0; i < size; i++) {
            if (other_string[i] != string[i]) return false;
        }
        return true;
    }

    int length() const {
        return size;
    }

    void push_back(char value) {
        if (size == capacity) {
            capacity *= 2;
            char* k = new char[capacity];
            memcpy(k, string, size);
            if (capacity != 0) delete[] string;
            string = k;
        }
        string[size] = value;
        size++;
    }

    void pop_back() {
        if (size != 0) size--;
    }

    String& operator+=(const String& other_string) {
        for (size_t i = 0; i < other_string.size; ++i) {
            push_back(other_string[i]);
        }
        return *this;
    }

    String& operator+=(const char &symbol) {
        push_back(symbol);
        return *this;
    }

    size_t find(const String& other_string) const {
        size_t subz = 0;
        for (size_t i = 0; i < size; i++) {
            if (string[i] == other_string[subz]) {
                subz++;
                if (subz == other_string.size) return (size_t)(i - subz);
            }
            else subz = 0;
            if (other_string[subz] == string[i]) ++subz;
        }
        return size;
    }

    size_t rfind(const String &other_string) const {
        size_t subz = 0;
        for (size_t i = 0; i < size; i++) {
            if (string[size - i - 1] == other_string[other_string.size - subz - 1]) {
                ++subz;
                if (subz == other_string.size) return (size_t)(size - i - 1);
            }
            else subz = 0;
            if (other_string[other_string.size - subz - 1] == string[size - i]) ++subz;
        }
        return size;
    }

    bool empty() {
        return size == 0;
    }

    void clear() {
        if (capacity != 0){
            capacity = 0;
            size = 0;
            delete[] string;
        }
    }

    char& front() {
        return string[0];
    }

    const char& front() const {
        return string[0];
    }

    char& back() {
        return string[size - 1];
    }
    const char& back() const {
        return string[size - 1];
    }

    String substr(size_t index, int count) const {
        String ans;
        for (size_t i = index; i < index + count; ++i){
            ans.push_back(string[i]);
        }
        return ans;
    }
    String operator+(char symbol) {
        String new_string = *this;
        new_string.push_back(symbol);
        return new_string;
    }

};

String operator+(const String&first, const String&second) {
    String result(first);
    result += second;
    return result;
}

String operator+(char symbol, const String&other_string) {
    String new_string;
    new_string.push_back(symbol);
    new_string += other_string;
    return new_string;
}

std::ostream& operator<<(std::ostream &out, const String &other_string) {
    for (size_t i = 0; i < (size_t)other_string.length(); ++i) {
        out << other_string[i];
    }
    return out;
}

String& operator>>(std::istream &input, String &other_string) {
    while (isspace(input.peek())) input.get();
    for (char element; !isspace(input.peek()) && input >> element;) {
        other_string.push_back(element);
    }
    return other_string;
}
