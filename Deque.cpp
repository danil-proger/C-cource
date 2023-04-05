#include <stdexcept>
#include <type_traits>
#include <vector>

template<typename Iterator>
class reverse_iter {

    Iterator reverse_iterator;

public:
    reverse_iter() = default;

    reverse_iter(const reverse_iter& other) = default;

    explicit reverse_iter(const Iterator& reverse_iterator): Iterator(reverse_iterator) {}

    reverse_iter& operator--() {
        return ++reverse_iterator;
    }

    reverse_iter operator--(int) {
        return reverse_iterator--;
    }

    reverse_iter& operator++() {
        return --reverse_iterator;
    }
    reverse_iter operator++(int) {
        return reverse_iterator--;
    }

    reverse_iter& operator+=(int value) {
        return reverse_iterator -= value;
    }

    reverse_iter& operator-=(int value) {
        return reverse_iterator += value;
    }

    reverse_iter operator+(int value) {
        return reverse_iterator - value;
    }

    reverse_iter operator-(int value) {
        return reverse_iterator + value;
    }

    reverse_iter& operator*() {
        return *reverse_iterator;
    }

    reverse_iter* operator->() {
        return reverse_iterator.operator->();
    }

};


template <typename T>
class Deque {
public:
    Deque();

    ~Deque();

    Deque(const Deque& copy);

    explicit Deque(int n, const T& val = T());

    void push_back(T value);

    void pop_back();

    void push_front(T value);

    void pop_front();

    Deque& operator=(const Deque& copy);

    bool operator==(const Deque& copy);

    const T& operator[](size_t index) const;

    T& operator[](size_t index);

    size_t size() const;

    bool empty();

    T& at(size_t index);

    const T& at(size_t index) const;

    template<bool Const>
    class Iterator {
    public:
        Iterator(T** _iter, size_t _index) : iter(_iter), index(_index) {};

        Iterator& operator++();

        Iterator& operator--();

        Iterator<Const> operator+(const int& n) const {
            auto col = (n + index) / container_size;
            return Iterator<Const>(col + iter, (n + index) % container_size);
        };

        Iterator<Const> operator-(const int& n) const;

        bool operator<(const Iterator<Const>& other) const;

        bool operator>(const Iterator<Const>& other) const;

        bool operator>=(const Iterator<Const>& other) const {
            return !(*this < other);
        }

        bool operator==(const Iterator<Const>& other) const {
            if (iter == other.iter && index == other.index) {
                return true;
            }
            return false;
        }

        int operator-(const Iterator<Const>& other) const {
            return index - other.index + (iter - other.iter) * container_size;
        }

        bool operator!=(const Iterator<Const>& other) const {
            return !(*this == other);
        }

        bool operator<=(const Iterator<Const>& other) const {
            return !(*this > other);
        }

        std::conditional_t<Const, const T&, T&> operator*() const {
            return *(*iter + index);
        }

        std::conditional_t<Const, const T*, T*> operator->() {
            return *iter + index;
        }
    private:
        T** iter;
        size_t index;
    };

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using reverse_const_iterator = reverse_iter<Iterator<true>>;
    using reverse_iterator = reverse_iter<Iterator<false>>;

    void insert(iterator iter, const T& value);

    void erase(iterator iter);

    iterator begin() {
        return iterator(front_array.chunk_no + array, front_array.current);
    }

    const_iterator begin() const {
        return const_iterator(front_array.chunk_no + array, front_array.current);
    }

    const_iterator cbegin() const {
        return const_iterator(array + front_array.chunk_no, front_array.current);
    }

    reverse_const_iterator rend() const {
        return std::reverse_iterator(end());
    }

    iterator end() {
        auto col = array + back_array.chunk_no;
        if (back_array.current != back_array.last_elem_pos) {
            return iterator(col, back_array.current + 1);
        }
        return iterator(col + 1, back_array.first_elem_pos);
    }

    const_iterator end() const;

    const_iterator cend() const;

    reverse_iterator rbegin() {
        return std::reverse_iterator(begin());
    }

    reverse_const_iterator crend() const {
        return std::reverse_iterator(end());
    }

    reverse_const_iterator rbegin() const {
        return std::reverse_iterator(begin());
    }

    reverse_iterator rend() {
        return std::reverse_iterator(end());
    }
private:

    struct Chunk_coord {
        size_t chunk_no;
        size_t first_elem_pos;
        size_t last_elem_pos;
        size_t current;

        Chunk_coord() = default;

        Chunk_coord(size_t _chunk_no, size_t _first_elem_pos, size_t _last_elem_pos, size_t _current):
                chunk_no(_chunk_no), first_elem_pos(_first_elem_pos), last_elem_pos(_last_elem_pos), current(_current) {};

        Chunk_coord(const Chunk_coord& copy) = default;

        bool operator==(const Chunk_coord& copy);
    };

    void reserve(bool is_start, bool is_finish);
    std::vector<size_t> pos(size_t index) const;

    const static size_t container_size = 32;
    T** array;
    Chunk_coord front_array;
    Chunk_coord back_array;
    size_t length = 0;
    size_t capacity = 2;
};

template<typename T>
bool Deque<T>::Chunk_coord::operator==(const Deque::Chunk_coord &coord) {
    if (coord.chunk_no == chunk_no && coord.first_elem_pos == first_elem_pos && coord.last_elem_pos == last_elem_pos
        && coord.current == current) {
        return true;
    }
    return false;
}

template<typename T>
template<bool Const>
bool Deque<T>::Iterator<Const>::operator<(const Deque::Iterator<Const> &other) const {
    if (iter < other.iter) {
        return true;
    }
    if (iter == other.iter && index < other.index){
        return true;
    }
    return false;
}

template<typename T>
template<bool Const>
bool Deque<T>::Iterator<Const>::operator>(const Deque::Iterator<Const> &other) const {
    if (iter > other.iter) {
        return true;
    }
    if (iter == other.iter && index > other.index) {
        return true;
    }
    return false;
}

template<typename T>
template<bool Const>
typename Deque<T>::template Iterator<Const> &
Deque<T>::Iterator<Const>::operator++() {
    if (index != container_size - 1) {
        ++index;
    } else {
        ++iter;
        index = 0;
    }
    return *this;
}

template<typename T>
template<bool Const>
typename Deque<T>::template Iterator<Const> &
Deque<T>::Iterator<Const>::operator--() {
    if (index != 0) {
        --index;
    } else {
        --iter;
        index = container_size - 1;
    }
    return (*this);
}

template<typename T>
template<bool Const>
typename Deque<T>::template Iterator<Const>
Deque<T>::Iterator<Const>::operator-(const int &n) const {
    auto copy = static_cast<size_t>(n);
    if (copy > index) {
        return Iterator<Const>(iter -  1 - (copy - index) /
                                       container_size, container_size - (copy - index) % container_size);
    }
    return Iterator<Const>(iter, index - copy);
}


template<typename T>
size_t Deque<T>::size() const {
    return length;
}

template<typename T>
Deque<T>::Deque(): length(0), capacity(2) {
    array = new T*[capacity];
    back_array = Chunk_coord(0, 0, container_size - 1, container_size-1);
    front_array = Chunk_coord(0, container_size - 1, 0, container_size-1);
    for (int row = 0; row < 2; ++row) {
        array[row] = reinterpret_cast<T*>(new uint8_t[container_size * sizeof(T)]);
    }
}

template<typename T>
Deque<T>::Deque(const Deque &copy):length(copy.length), capacity(copy.capacity) {
    array = new T*[capacity];
    front_array = Chunk_coord(copy.front_array);
    back_array = Chunk_coord(copy.back_array);
    for (size_t row = front_array.chunk_no; row < back_array.chunk_no + 1; ++row) {
        array[row] = reinterpret_cast<T*>(new uint8_t[container_size * sizeof(T)]);
        size_t value;

        if (row == front_array.chunk_no) {
            value = front_array.current;
        } else {
            value = 0;
        }
        for (size_t column = value; column < container_size; ++column) {
            if (row == back_array.chunk_no) {
                if (column < back_array.current + 1) {
                    new (array[row] + column) T(copy.array[row][column]);
                }
            } else {
                if (column < container_size) {
                    new (array[row] + column) T(copy.array[row][column]);
                }
            }
        }
    }
}

template<typename T>
Deque<T>& Deque<T>::operator=(const Deque &copy) {
    if (copy == *this) {
        return *this;
    }
    Deque<T> c = copy;
    std::swap(front_array, c.front_array);
    std::swap(back_array, c.back_array);
    std::swap(array, c.array);
    std::swap(length, c.length);
    std::swap(capacity, c.capacity);
    return *this;
}

template<typename T>
std::vector<size_t> Deque<T>::pos(size_t index) const {
    if (front_array.current + index >= container_size) {
        index += front_array.current;
        index -= container_size;
        size_t chunk_no = index / container_size + 1 + front_array.chunk_no;
        return {chunk_no, index % container_size};
    }
    return {front_array.chunk_no, index + front_array.current};
}

template<typename T>
T& Deque<T>::operator[](size_t index) {
    return array[pos(index)[0]][pos(index)[1]];
}

template<typename T>
T &Deque<T>::at(size_t index) {
    if (index < size()) {
        return array[pos(index)[0]][pos(index)[1]];
    }
    throw std::out_of_range("at");
}

template<typename T>
void Deque<T>::reserve(bool is_start, bool is_finish) {
    size_t argument = (length + container_size - 1) / container_size;
    size_t first_elem_pos;
    size_t last_elem_pos;
    if (is_start) {
        first_elem_pos = argument;
    } else {
        first_elem_pos = 0;
    }
    if (is_finish) {
        last_elem_pos = argument;
    } else {
        last_elem_pos = 0;
    }

    T** new_array = new T*[first_elem_pos + last_elem_pos + capacity];
    for (size_t row = 0; row < capacity; ++row) {
        new_array[first_elem_pos + row] = array[row];
    }

    for (size_t row = first_elem_pos + capacity; row < first_elem_pos + last_elem_pos + capacity; ++row) {
        new_array[row] = reinterpret_cast<T*>(new uint8_t[container_size * sizeof(T)]);
    }

    for (size_t row = 0; row < first_elem_pos; ++row) {
        new_array[row] = reinterpret_cast<T*>(new uint8_t[container_size * sizeof(T)]);
    }

    delete array;
    capacity = first_elem_pos + last_elem_pos + capacity;
    front_array.chunk_no += first_elem_pos;
    back_array.chunk_no += first_elem_pos;
    array = new_array;
}

template<typename T>
void Deque<T>::erase(Deque::iterator iter) {
    T p;
    for (auto row = iter; row < end() - 1; ++row) {
        p = *row;
        *row = *(row + 1);
        *(row + 1) = p;
    }
    pop_back();
}

template<typename T>
void Deque<T>::insert(Deque::iterator iter, const T& value) {
    push_back(value);
    T p;
    for (auto row = end() - 1; row > iter; --row) {
        p = *row;
        *row = *(row - 1);
        *(row - 1) = p;
    }
}

template<typename T>
void Deque<T>::push_back(T value) {
    if (!empty()) {
        if (back_array.last_elem_pos != back_array.current) {
            ++back_array.current;
        } else {
            if (back_array.chunk_no == capacity-1) {
                reserve(0, true);
            }
            ++back_array.chunk_no;
            back_array.current = 0;
        }
    }
    new(array[back_array.chunk_no] + back_array.current) T(value);
    ++length;
}

template<typename T>
void Deque<T>::pop_back() {
    if (!empty()) {
        auto col = array[back_array.chunk_no] + back_array.current;
        col->~T();
        if (length != 1) {
            if (back_array.current != back_array.first_elem_pos) {
                --back_array.current;
            }
            else {
                --back_array.chunk_no;
                back_array.current = back_array.last_elem_pos;
            }
        }
        --length;
    }
}

template<typename T>
void Deque<T>::push_front(T value) {
    if (!empty()) {
        if (front_array.last_elem_pos != front_array.current) {
            --front_array.current;
        } else {
            if (front_array.chunk_no == 0) {
                reserve(true, 0);
            }
            front_array.current = container_size - 1;
            --front_array.chunk_no;
        }
    }
    new(array[front_array.chunk_no] + front_array.current) T(value);
    length += 1;
}

template<typename T>
void Deque<T>::pop_front() {
    if (!empty()) {
        (array[front_array.chunk_no] + front_array.current)->~T();
        if (length != 1) {
            if (front_array.current != front_array.first_elem_pos) {
                ++front_array.current;
            } else {
                ++front_array.chunk_no;
                front_array.current = front_array.last_elem_pos;
            }
        }
    }
    --length;
}

template<typename T>
const T &Deque<T>::operator[](size_t index) const {
    return array[pos(index)[0]][pos(index)[1]];
}

template<typename T>
const T &Deque<T>::at(size_t index) const {
    if (index < size()) {
        return array[pos(index).first_elem_pos][pos(index).second];
    }
    throw std::out_of_range("at");
}

template<typename T>
Deque<T>::Deque(int n, const T& val): length(n) {
    auto copy_size = static_cast<size_t>(n);
    if (copy_size > container_size) {
        capacity = (copy_size + container_size - 1) / container_size;
        back_array = Chunk_coord(capacity - 1, 0, container_size - 1, (copy_size - 1) % container_size);
        front_array = Chunk_coord(0, container_size - 1, 0, 0);
    } else {
        front_array = Chunk_coord(0, container_size - 1, 0, container_size - copy_size);
        back_array = Chunk_coord(0, 0, container_size - 1, container_size - 1);
        capacity = 2;
    }
    array = new T*[capacity];
    for (size_t row = 0; row < capacity; ++row) {
        array[row] = reinterpret_cast<T*>(new uint8_t[container_size * sizeof(T)]);
    }
    for (size_t row = front_array.chunk_no; row < back_array.chunk_no + 1; ++row) {
        size_t value;
        if (row == front_array.chunk_no) {
            value = front_array.current;
        } else {
            value = 0;
        }
        for (size_t column = value; column < container_size; ++column) {
            if (column < back_array.current + 1) {
                new (array[row] + column) T(val);
            } else {
                if (column < container_size) {
                    new (array[row] + column) T(val);
                }
            }
        }
    }
}

template<typename T>
typename Deque<T>::const_iterator
Deque<T>::end() const {
    auto col = array + back_array.chunk_no;
    if (back_array.current != back_array.last_elem_pos) {
        return const_iterator(col, back_array.current + 1);
    }
    return const_iterator(col + 1, back_array.first_elem_pos);
}

template<typename T>
typename Deque<T>::const_iterator
Deque<T>::cend() const {
    auto col = array + back_array.chunk_no;
    if (back_array.current != back_array.last_elem_pos) {
        return const_iterator(col, back_array.current + 1);
    }
    return const_iterator(col + 1, back_array.first_elem_pos);
}

template<typename T>
bool Deque<T>::empty() {
    if (length == 0) {
        return true;
    }
    return false;
}



template<typename T>
Deque<T>::~Deque() {
    for (size_t row = front_array.chunk_no; row <= back_array.chunk_no; ++row) {
        for (size_t column = ((row == front_array.chunk_no) ? front_array.current : 0);
             column < ((row == back_array.chunk_no) ? back_array.current + 1: capacity); ++column) {
            (array[row] + column)->~T();
        }
    }
    delete array;
}

template <typename T>
bool operator==(const Deque<T>& lhs, const Deque<T>& rhs) {
    return lhs.begin() == rhs.begin();
}
