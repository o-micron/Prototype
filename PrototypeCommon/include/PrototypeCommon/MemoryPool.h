/// This code is distributed under the MIT License, which is reproduced below and at the top of the project files. This pretty
/// much means you can do whatever you want with the code, but I will not be liable for ANY kind of damage that this code might
/// cause. Here is the full license which you should read before using the code
///
/// Copyright (c) 2013 Cosku Acay,
/// http://www.coskuacay.com Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
/// associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
/// to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this
/// permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS",
/// WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR
/// A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
/// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
/// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

/// This is a modified version of the original code 
/// Copyright 2021 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <climits>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

template<typename T, size_t BlockSizeMultipler = 2>
struct MemoryPool
{
  public:
    /* Member types */
    typedef T               value_type;
    typedef T*              pointer;
    typedef T&              reference;
    typedef const T*        const_pointer;
    typedef const T&        const_reference;
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;
    typedef std::false_type propagate_on_container_copy_assignment;
    typedef std::true_type  propagate_on_container_move_assignment;
    typedef std::true_type  propagate_on_container_swap;

    template<typename U>
    struct rebind
    {
        typedef MemoryPool<U> other;
    };

    /* Member functions */
    MemoryPool() noexcept;
    MemoryPool(const MemoryPool& memoryPool) noexcept;
    MemoryPool(MemoryPool&& memoryPool) noexcept;
    template<class U>
    explicit MemoryPool(const MemoryPool<U>& memoryPool) noexcept;

    ~MemoryPool() noexcept;
    void clear() noexcept;

    MemoryPool& operator=(const MemoryPool& memoryPool) = delete;
    MemoryPool& operator                                =(MemoryPool&& memoryPool) noexcept;

    pointer       address(reference x) const noexcept;
    const_pointer address(const_reference x) const noexcept;

    size_type max_size() const noexcept;

    template<class U, class... Args>
    void construct(U* p, Args&&... args);
    template<class U>
    void destroy(U* p);

    template<class... Args>
    pointer newElement(Args&&... args);
    void    deleteElement(pointer p);

  private:
    union Slot_
    {
        value_type element;
        Slot_*     next;
    };

    typedef char*  data_pointer_;
    typedef Slot_  slot_type_;
    typedef Slot_* slot_pointer_;

    slot_pointer_ currentBlock_;
    slot_pointer_ currentSlot_;
    slot_pointer_ lastSlot_;
    slot_pointer_ freeSlots_;

    // Can only allocate one object at a time. n and hint are ignored
    pointer allocate(size_type n = 1, const_pointer hint = 0);
    void    deallocate(pointer p, size_type n = 1);

    size_type padPointer(data_pointer_ p, size_type align) const noexcept;
    void      allocateBlock();

    static_assert(BlockSizeMultipler >= 2, "BlockSizeMultipler must be at least 2.");
};

#ifndef MEMORY_BLOCK_TCC
#define MEMORY_BLOCK_TCC

template<typename T, size_t BlockSizeMultipler>
inline typename MemoryPool<T, BlockSizeMultipler>::size_type
MemoryPool<T, BlockSizeMultipler>::padPointer(data_pointer_ p, size_type align) const noexcept
{
    auto result = reinterpret_cast<uintptr_t>(p);
    return ((align - result) % align);
}

template<typename T, size_t BlockSizeMultipler>
MemoryPool<T, BlockSizeMultipler>::MemoryPool() noexcept
{
    currentBlock_ = nullptr;
    currentSlot_  = nullptr;
    lastSlot_     = nullptr;
    freeSlots_    = nullptr;
}

template<typename T, size_t BlockSizeMultipler>
MemoryPool<T, BlockSizeMultipler>::MemoryPool(const MemoryPool& memoryPool) noexcept
  : MemoryPool()
{}

template<typename T, size_t BlockSizeMultipler>
MemoryPool<T, BlockSizeMultipler>::MemoryPool(MemoryPool&& memoryPool) noexcept
{
    currentBlock_            = memoryPool.currentBlock_;
    memoryPool.currentBlock_ = nullptr;
    currentSlot_             = memoryPool.currentSlot_;
    lastSlot_                = memoryPool.lastSlot_;
    freeSlots_               = memoryPool.freeSlots_;
}

template<typename T, size_t BlockSizeMultipler>
template<class U>
MemoryPool<T, BlockSizeMultipler>::MemoryPool(const MemoryPool<U>& memoryPool) noexcept
  : MemoryPool()
{}

template<typename T, size_t BlockSizeMultipler>
MemoryPool<T, BlockSizeMultipler>&
MemoryPool<T, BlockSizeMultipler>::operator=(MemoryPool&& memoryPool) noexcept
{
    if (this != &memoryPool) {
        std::swap(currentBlock_, memoryPool.currentBlock_);
        currentSlot_ = memoryPool.currentSlot_;
        lastSlot_    = memoryPool.lastSlot_;
        freeSlots_   = memoryPool.freeSlots;
    }
    return *this;
}

template<typename T, size_t BlockSizeMultipler>
MemoryPool<T, BlockSizeMultipler>::~MemoryPool() noexcept
{
    clear();
}

template<typename T, size_t BlockSizeMultipler>
void
MemoryPool<T, BlockSizeMultipler>::clear() noexcept
{
    slot_pointer_ curr = currentBlock_;
    while (curr != nullptr) {
        slot_pointer_ prev = curr->next;
                      operator delete(reinterpret_cast<void*>(curr));
        curr = prev;
    }
    currentBlock_ = nullptr;
    currentSlot_  = nullptr;
    lastSlot_     = nullptr;
    freeSlots_    = nullptr;
}

template<typename T, size_t BlockSizeMultipler>
inline typename MemoryPool<T, BlockSizeMultipler>::pointer
MemoryPool<T, BlockSizeMultipler>::address(reference x) const noexcept
{
    return &x;
}

template<typename T, size_t BlockSizeMultipler>
inline typename MemoryPool<T, BlockSizeMultipler>::const_pointer
MemoryPool<T, BlockSizeMultipler>::address(const_reference x) const noexcept
{
    return &x;
}

template<typename T, size_t BlockSizeMultipler>
void
MemoryPool<T, BlockSizeMultipler>::allocateBlock()
{
    // Allocate space for the new block and store a pointer to the previous one
    auto newBlock = reinterpret_cast<data_pointer_>(operator new(sizeof(T) * BlockSizeMultipler));
    reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;
    currentBlock_                                   = reinterpret_cast<slot_pointer_>(newBlock);
    // Pad block body to staisfy the alignment requirements for elements
    data_pointer_ body        = newBlock + sizeof(slot_pointer_);
    size_type     bodyPadding = padPointer(body, alignof(slot_type_));
    currentSlot_              = reinterpret_cast<slot_pointer_>(body + bodyPadding);
    lastSlot_ = reinterpret_cast<slot_pointer_>(newBlock + (sizeof(T) * BlockSizeMultipler) - sizeof(slot_type_) + 1);
}

template<typename T, size_t BlockSizeMultipler>
inline typename MemoryPool<T, BlockSizeMultipler>::pointer
MemoryPool<T, BlockSizeMultipler>::allocate(size_type n, const_pointer hint)
{
    if (freeSlots_ != nullptr) {
        auto result = reinterpret_cast<pointer>(freeSlots_);
        freeSlots_  = freeSlots_->next;
        return result;
    } else {
        if (currentSlot_ >= lastSlot_) allocateBlock();
        return reinterpret_cast<pointer>(currentSlot_++);
    }
}

template<typename T, size_t BlockSizeMultipler>
inline void
MemoryPool<T, BlockSizeMultipler>::deallocate(pointer p, size_type n)
{
    if (p != nullptr) {
        reinterpret_cast<slot_pointer_>(p)->next = freeSlots_;
        freeSlots_                               = reinterpret_cast<slot_pointer_>(p);
    }
}

template<typename T, size_t BlockSizeMultipler>
inline typename MemoryPool<T, BlockSizeMultipler>::size_type
MemoryPool<T, BlockSizeMultipler>::max_size() const noexcept
{
    size_type maxBlocks = -1 / (sizeof(T) * BlockSizeMultipler);
    return ((sizeof(T) * BlockSizeMultipler) - sizeof(data_pointer_)) / sizeof(slot_type_) * maxBlocks;
}

template<typename T, size_t BlockSizeMultipler>
template<class U, class... Args>
inline void
MemoryPool<T, BlockSizeMultipler>::construct(U* p, Args&&... args)
{
    new (p) U(std::forward<Args>(args)...);
}

template<typename T, size_t BlockSizeMultipler>
template<class U>
inline void
MemoryPool<T, BlockSizeMultipler>::destroy(U* p)
{
    p->~U();
}

template<typename T, size_t BlockSizeMultipler>
template<class... Args>
inline typename MemoryPool<T, BlockSizeMultipler>::pointer
MemoryPool<T, BlockSizeMultipler>::newElement(Args&&... args)
{
    pointer result = allocate();
    construct<value_type>(result, std::forward<Args>(args)...);
    return result;
}

template<typename T, size_t BlockSizeMultipler>
inline void
MemoryPool<T, BlockSizeMultipler>::deleteElement(pointer p)
{
    if (p != nullptr) {
        p->~value_type();
        deallocate(p);
    }
}

#endif // MEMORY_BLOCK_TCC

#endif // MEMORY_POOL_H
