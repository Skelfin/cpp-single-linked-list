#pragma once

#include <cstddef>
#include <string>
#include <utility>

template <typename Type>
class SingleLinkedList {
    struct Node {
        Type value{};
        Node* next_node = nullptr;

        Node() = default;
        Node(const Type& val, Node* next = nullptr)
            : value(val), next_node(next) {}
    };

    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node) : node_(node) {}

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept
            : node_(other.node_) {}

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }


        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            BasicIterator temp = *this;
            ++(*this);
            return temp;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(const_cast<Node*>(&head_));
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    SingleLinkedList() = default;


    ~SingleLinkedList() {
        Clear();
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void Clear() noexcept {
        while (!IsEmpty()) {
            PopFront();
        }
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    SingleLinkedList(std::initializer_list<Type> values) {
        auto it = values.end();
        while (it != values.begin()) {
            --it;
            PushFront(*it);
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        SingleLinkedList tmp;
        Node* last_node = &tmp.head_;
        for (auto it = other.begin(); it != other.end(); ++it) {
            last_node->next_node = new Node(*it, nullptr);
            last_node = last_node->next_node;
        }
        tmp.size_ = other.size_;
        swap(tmp);
    }
    // Честно скажу, просил помощи у других студетов, чтобы понять как убрать before_head_.
    // И насчет SingleLinkedList не много не понял, у них же код разный, как для них сделать шаблонный метод?
    // И да...из-за before_head_ пришлось поменять несколько методов и SingleLinkedList(const SingleLinkedList& other)
    // Это тоже затронуло (хоть и не так сильно)

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            SingleLinkedList temp(rhs);
            swap(temp);
        }
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        Node* tmp = other.head_.next_node;
        size_t size = other.size_;

        other.head_.next_node = head_.next_node;
        other.size_ = size_;

        head_.next_node = tmp;
        size_ = size;
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.node_ != nullptr);
        pos.node_->next_node = new Node(value, pos.node_->next_node);
        ++size_;
        return Iterator(pos.node_->next_node);
    }

    void PopFront() noexcept {
        if (size_ != 0) {
            Node* next = head_.next_node->next_node;
            delete head_.next_node;
            head_.next_node = next;
            --size_;
        }
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.node_ != nullptr && pos.node_->next_node != nullptr && size_ > 0);
        Node* target_node = pos.node_->next_node;
        pos.node_->next_node = pos.node_->next_node->next_node;
        delete target_node;
        --size_;
        return Iterator(pos.node_->next_node);
    }

private:
    Node head_ = Node();
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (&lhs == &rhs) {
        return true;
    }
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}


template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}
