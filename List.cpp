#include <memory>
#include <cmath>

template<size_t N>
class StackStorage {
public:
    StackStorage() = default;

    StackStorage(const StackStorage &other) = delete;

    StackStorage &operator=(const StackStorage &other) = delete;

    char *allocate(size_t n, size_t stack);

    void deallocate(char *, size_t) {};

private:
    char storage_[N];
    char *curr{storage_};
};

template<size_t N>
char *StackStorage<N>::allocate(size_t n, size_t stack) {
    char *lst = curr + (n - reinterpret_cast<size_t>(curr)) % stack;
    curr = lst + n;
    return lst;
}

template<typename T, size_t N>
class StackAllocator {
public:
    using value_type = T;

    template<typename U>
    struct rebind {
        using other = StackAllocator<U, N>;
    };

    explicit StackAllocator() = default;

    ~StackAllocator() = default;

    StackAllocator(StackStorage<N> &stackStorage) : storage_(&stackStorage) {};

    template<typename U>
    StackAllocator(const StackAllocator<U, N> &other): storage_(other.get_storage()) {};

    void deallocate(T *pointer, size_t n);

    T *allocate(size_t n);

    StackStorage<N> *get_storage() const {
        return storage_;
    }

private:
    StackStorage<N> *storage_;
};

template<typename T, size_t N>
T *StackAllocator<T, N>::allocate(size_t n) {
    return static_cast<T *>(static_cast<void *>(storage_->allocate(n * sizeof(T), alignof(T))));
}

template<typename T, class Allocator = std::allocator<T>>
class List {
private:
    struct BaseNode;
    struct Node;

    using StackAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using FullStackAllocator = std::allocator_traits<StackAllocator>;

public:
    template<bool Const>
    class Iter {
    public:
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = std::conditional_t<Const, const T &, T &>;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;

        BaseNode *iter;

        Iter() : iter(nullptr) {}

        explicit Iter(BaseNode *node) : iter(node) {}

        Iter(const Iter<false> &other) : iter(other.iter) {}

        Iter operator--(int) &{
            Iter copy = *this;
            iter = iter->prev;
            return copy;
        };

        Iter operator++(int) &{
            Iter copy = *this;
            iter = iter->next;
            return copy;
        };

        Iter &operator++() &{
            iter = iter->next;
            return *this;
        };

        Iter &operator--() {
            iter = iter->prev;
            return *this;
        };

        reference operator*() const {
            return static_cast<std::conditional_t<Const, const Node *, Node *>>(iter)->value;
        }

        pointer operator->() const {
            return &*this;
        }

        bool operator!=(const Iter<Const> &other) const {
            return !(*this == other);
        }

        bool operator==(const Iter<Const> &other) const {
            return (iter == other.iter);
        }
    };

public:
    using const_iterator = Iter<true>;
    using iterator = Iter<false>;
    using const_reverse_iterator = std::reverse_iterator<Iter<true>>;
    using reverse_iterator = std::reverse_iterator<Iter<false>>;

    List() = default;

    explicit List(Allocator &_stack) : stack_(_stack) {}

    List(int n, Allocator &_stack);

    List(const List &other);

    List(int n, const T &value, const Allocator &_stack);

    List(int n, const T &value);

    explicit List(int n);

    ~List() {
        while (!empty()) {
            pop_back();
        }
    }

    List &operator=(const List &other);

    const_iterator cend() const {
        return const_iterator(&basenode_);
    }

    const_iterator end() const {
        return const_iterator(&basenode_);
    }

    iterator begin() {
        return iterator(basenode_.next);
    }

    const_iterator begin() const {
        return const_iterator(basenode_.next);
    }

    iterator end() {
        return iterator(&basenode_);
    }

    const_reverse_iterator crbegin() const {
        return reverse_iterator(cend());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(cend());
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_iterator cbegin() const {
        return const_iterator(basenode_.next);
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }

    Allocator get_allocator() const {
        return stack_;
    }

    bool empty() {
        return size_ == 0;
    }

    size_t size() const {
        return size_;
    }

    void push_back(const T &value) {
        insert(end(), value);
    }

    void push_front(const T &value) {
        insert(begin(), value);
    }

    void pop_back() {
        erase(--end());
    }

    void pop_front() {
        erase(begin());
    }

    void insert(const_iterator iter, const T &value);

    void emplase_back(const_iterator iter);

    void erase(const_iterator iter);

private:
    StackAllocator stack_;
    size_t size_ = 0;
    mutable BaseNode basenode_;
};


template<typename T, class Allocator>
void List<T, Allocator>::insert(List::const_iterator iter, const T &value) {
    Node *res = FullStackAllocator::allocate(stack_, 1);

    try {
        auto help = iter.iter->prev;
        FullStackAllocator::construct(stack_, res, value, help, iter.iter);
    } catch (...) {
        FullStackAllocator::deallocate(stack_, FullStackAllocator::allocate(stack_, 1), 1);
        throw;
    }

    ++size_;
    res->connectNodes();
}

template<typename T, class Allocator>
void List<T, Allocator>::erase(List::const_iterator iter) {
    auto help = static_cast<Node *>(iter.iter);
    iter.iter->next->prev = iter.iter->prev;
    FullStackAllocator::destroy(stack_, help);
    iter.iter->prev->next = iter.iter->next;
    FullStackAllocator::deallocate(stack_, help, 1);
    --size_;
}


template<typename T, class Allocator>
List<T, Allocator>::List(int n) {
    auto size = static_cast<size_t>(n);
    try {
        for (size_t i = 0; i < size; ++i) {
            emplase_back(end());
        }
    } catch (...) {
        while (!empty()) {
            pop_back();
        }
    }
}

template<typename T, class Allocator>
List<T, Allocator>::List(int n, const T &value) {
    auto size = static_cast<size_t>(n);

    for (size_t i = 0; i < size; ++i) {
        push_back(value);
    }

}

template<typename T, class Allocator>
List<T, Allocator>::List(int n, const T &value, const Allocator &_stack) {
    auto size = static_cast<size_t>(n);
    stack_ = _stack;
    for (size_t i = 0; i < size; ++i) {
        push_back(value);
    }
}

template<typename T, class Allocator>
List<T, Allocator>::List(int n, Allocator &_stack) {
    auto size = static_cast<size_t>(n);
    stack_ = _stack;
    for (size_t i = 0; i < size; ++i) {
        emplase_back(end());
    }
}

template<typename T, class Allocator>
List<T, Allocator> &List<T, Allocator>::operator=(const List &other) {

    if (&other == this) {
        return *this;
    }

    size_t count_help = 0;
    size_t prev_size = size_;

    if (FullStackAllocator::propagate_on_container_copy_assignment::value) {
        stack_ = other.stack_;
    }

    try {
        auto help = other.begin();
        while (help != other.end()) {
            push_back(*help);
            ++count_help;
            ++help;
        }
    } catch (...) {
        for (size_t i = 0; i < count_help; ++i) {
            pop_back();
        }
        throw;
    }

    for (size_t i = 0; i < prev_size; ++i) {
        pop_front();
    }

    return *this;
}

template<typename T, class Allocator>
List<T, Allocator>::List(const List &other) {
    stack_ = FullStackAllocator::select_on_container_copy_construction(other.stack_);
    try {
        auto help = other.begin();
        while (help != other.end()) {
            push_back(*help);
            ++help;
        }
    } catch (...) {
        while (!empty()) {
            pop_back();
        }
        throw;
    }
}

template<typename T, class Allocator>
void List<T, Allocator>::emplase_back(List::const_iterator iter) {
    Node *node = FullStackAllocator::allocate(stack_, 1);

    FullStackAllocator::construct(stack_, node);
    node->next = iter.iter;
    node->prev = iter.iter->prev;

    node->connectNodes();
    ++size_;
}

template<typename T, size_t N>
void StackAllocator<T, N>::deallocate(T *pointer, size_t n) {
    auto answer = static_cast<char *>(static_cast<void *>(pointer));
    storage_->deallocate(answer, n * sizeof(T));
}

template<typename T, class Allocator>
struct List<T, Allocator>::BaseNode {
    BaseNode *next;
    BaseNode *prev;

    BaseNode(BaseNode *_prev, BaseNode *_next) : next(_next), prev(_prev) {};

    void connectNodes() {
        prev->next = this;
        next->prev = this;
    }

    BaseNode() {
        prev = this;
        next = this;
    };
};

template<typename T, class Allocator>
struct List<T, Allocator>::Node : BaseNode {
    T value;

    Node() = default;

    Node(const T &_value, BaseNode *_prev, BaseNode *_next) : BaseNode(_prev, _next), value(_value) {};
};
