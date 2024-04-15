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
        return Iterator(head_);
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator(nullptr);
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator(head_);
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

    SingleLinkedList() : head_(nullptr), size_(0), before_head_() {
        before_head_.next_node = head_;
    }


    ~SingleLinkedList() {
        Clear();
    }

    void PushFront(const Type& value) {
        Node* new_node = new Node(value, head_);
        head_ = new_node;
        before_head_.next_node = head_;
        ++size_;
    }

    void Clear() noexcept {
        Node* current = head_;
        while (current != nullptr) {
            Node* next = current->next_node;
            delete current;
            current = next;
        }
        head_ = nullptr;
        before_head_.next_node = nullptr;
        size_ = 0;
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    SingleLinkedList(std::initializer_list<Type> values) : head_(nullptr), size_(0) {
        auto it = values.end();
        while (it != values.begin()) {
            --it;
            PushFront(*it);
        }
    }

    SingleLinkedList(const SingleLinkedList& other) : head_(nullptr), size_(0) {
        CloneFrom(other);
        size_ = other.size_;
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            SingleLinkedList temp(rhs);
            swap(temp);
        }
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_, other.head_);
        std::swap(size_, other.size_);
        std::swap(before_head_.next_node, other.before_head_.next_node);
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(&before_head_);
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&before_head_));
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&before_head_));
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.node_ != nullptr);
        Node* new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        if (pos.node_ == &before_head_) {
            head_ = new_node;
        }
        ++size_;
        return Iterator(new_node);
    }

    void PopFront() noexcept {
        assert(head_ != nullptr);
        Node* old_head = head_;
        head_ = head_->next_node;
        delete old_head;
        --size_;
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.node_ != nullptr && pos.node_->next_node != nullptr && size_ > 0);
        Node* target_node = pos.node_->next_node;
        pos.node_->next_node = target_node->next_node;
        if (pos.node_ == &before_head_) {
            head_ = target_node->next_node;
        }
        delete target_node;
        --size_;
        return Iterator(pos.node_->next_node);
    }

private:
    void CloneFrom(const SingleLinkedList& other) {
        Node** last_ptr = &head_;
        Node* current = other.head_;
        while (current != nullptr) {
            *last_ptr = new Node(current->value);
            last_ptr = &((*last_ptr)->next_node);
            current = current->next_node;
        }
    }
    Node* head_ = nullptr;
    size_t size_ = 0;
    Node before_head_;
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
