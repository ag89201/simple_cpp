#pragma once
#include <stdint.h>
#include <stddef.h>

namespace heap
{
    constexpr size_t HEAP_ALIGN = sizeof(int);
    constexpr size_t HEAP_ALIGN_MASK = HEAP_ALIGN - 1;
    
    
    template <typename Guard>
    class scope_guard
    {
    public:
        explicit scope_guard(Guard &g) : guard_(g) { guard_.lock(); }
        ~scope_guard() { guard_.unlock(); }

        scope_guard(const scope_guard &) = delete;
        scope_guard &operator=(const scope_guard &) = delete;

    private:
        Guard &guard_;
    };

    struct default_guard
    {
        void lock() { /*std::cout << "lock" << std::endl;*/ }
        void unlock() { /*std::cout << "unlock" << std::endl;*/ }
    };

    template <size_t N>
    struct pool
    {
        int Pool[N / sizeof(int)];
    };

    struct memControlBlock
    {
        enum
        {
            FREE = 0,
            ALLOCATED = 1,
        };
        struct type_size
        {
            size_t available : 1;
            size_t size : 31;
        };

        memControlBlock *next;
        memControlBlock *prev;
        type_size ts;

        /// @brief split current memory chunk.
        /// @param size
        /// @param start
        /// @return the pointer to new MCB
        memControlBlock *split(size_t size, memControlBlock *start) noexcept
        {
            size = align_size(size);
            memControlBlock *new_block = reinterpret_cast<memControlBlock *>(reinterpret_cast<uintptr_t>(this) + size);

            new_block->next = next;
            new_block->prev = this;

            new_block->ts.size = ts.size - size;
            new_block->ts.available = FREE;

            next = new_block;
            ts.size = size;
            ts.available = ALLOCATED;

            if (new_block->next != start)
            {
                (new_block->next)->prev = new_block;
            }
            return new_block;
        }

        /// @brief join current memory chunk with the next one
        /// @param start
        void merge_with_next(memControlBlock *start) noexcept
        {
            memControlBlock *other = next;
            ts.size += other->ts.size;
            other = next = other->next;
            if (other != start)
            {
                other->prev = this;
            }
        }
        void *pool() noexcept { return this + 1; }

        size_t align_size(size_t size) const noexcept
        {
            return (size + HEAP_ALIGN_MASK) & ~HEAP_ALIGN_MASK;
        }
    };

    template <typename Guard = default_guard>
    class allocator
    {
    private:
        void init(memControlBlock *pstart, size_t size_bytes) noexcept
        {
            if (size_bytes < sizeof(memControlBlock))
            {
                return;
            }
            pstart->next = pstart;
            pstart->prev = pstart;

            pstart->ts.size = size_bytes - sizeof(memControlBlock);
            pstart->ts.available = memControlBlock::FREE;
        }

        memControlBlock *start_;
        memControlBlock *freemem_;
        Guard guard_;

    public:
        struct Summary
        {
            struct Info
            {
                size_t blocks;
                size_t block_max_size;
                size_t size;

            } used, free;
        };

        Summary info() noexcept
        {
            Summary s;
            s.used.size = 0;
            s.used.blocks = 0;
            s.used.block_max_size = 0;

            s.free.size = 0;
            s.free.blocks = 0;
            s.free.block_max_size = 0;

            memControlBlock *mcb = freemem_;
            do
            {
                auto *info = (mcb->ts.available == memControlBlock::FREE) ? &s.free : &s.used;
                ++info->blocks;
                info->size += mcb->ts.size;
                if (info->block_max_size < mcb->ts.size)
                {
                    info->block_max_size = mcb->ts.size;
                }
                mcb = mcb->next;

            } while (mcb != start_);
            return s;
        }

        template <size_t size_bytes>
        explicit allocator(pool<size_bytes> &pool) noexcept
            : start_((memControlBlock *)pool.Pool),
              freemem_((memControlBlock *)pool.Pool),
              guard_()
        {
            init(start_, sizeof(pool));
        }

        virtual ~allocator() = default;

        void *allocate(size_t size) noexcept
        {

            size = (size + sizeof(memControlBlock) + (HEAP_ALIGN - 1)) & ~(HEAP_ALIGN - 1);
            if (size > freemem_->ts.size)
            {

                return nullptr;
            }

            memControlBlock *xptr;

            xptr = NULL;

            void *Allocated = nullptr;

            size_t free_cnt = 0;

            scope_guard<Guard> ScopeGuard(guard_);

            memControlBlock *tptr = freemem_;

            for (;;)
            {
                if (tptr->ts.available == memControlBlock::FREE)
                {

                    if (tptr->ts.size >= size && tptr->ts.size <= size + sizeof(memControlBlock) + HEAP_ALIGN)
                    {
                        tptr->ts.available = memControlBlock::ALLOCATED;
                        Allocated = tptr->pool();

                        ++free_cnt;

                        break;
                    }
                    else
                    {

                        if (xptr == NULL)
                        {
                            if (tptr->ts.size >= size)
                            {
                                xptr = tptr;
                            }
                            ++free_cnt;
                        }
                    }
                }

                tptr = tptr->next;
                if (tptr == start_)
                {
                    if (xptr != 0)
                    {
                        tptr = xptr;
                        xptr = tptr->split(size, start_);
                        Allocated = tptr->pool();
                        break;
                    }
                }
            }

            if ((free_cnt == 1) && (Allocated))
            {
                freemem_ = tptr->next;
            }

            return Allocated;
        }
        void deallocate(void *pool) noexcept
        {
            if (pool == nullptr || ((uintptr_t)pool & (HEAP_ALIGN - 1)))
            {
                return;
            }

            memControlBlock *xptr;
            memControlBlock *tptr = (memControlBlock *)pool - 1;

            scope_guard<Guard> ScopeGuard(guard_);

            xptr = tptr->prev;
            if ((xptr != tptr && xptr->next != tptr) || pool < start_)
            {
                return;
            }

            tptr->ts.available = memControlBlock::FREE;

            xptr = tptr->next;

            if (xptr && xptr->ts.available == memControlBlock::FREE && xptr != start_)
            {
                tptr->merge_with_next(start_);
            }

            xptr = tptr->prev;

            if (xptr && xptr->ts.available == memControlBlock::FREE && tptr != start_)
            {
                xptr->merge_with_next(start_);
                tptr = xptr;
            }

            if (tptr < freemem_)
            {
                freemem_ = tptr;
            }
        }

        size_t max_size() noexcept
        {
            return info().free.size;
        }
    };

    template <typename T, typename Allocator = allocator<default_guard>>
    class mallocator : public Allocator
    {
    public:
        using value_type = T;

        template <size_t size_bytes>
        explicit mallocator(pool<size_bytes> &pool) : Allocator(pool) {}

        T *allocate(size_t n)
        {
            size_t size = sizeof(T) * n;

            if (size > Allocator::max_size())
            {
                return nullptr;
            }
            return static_cast<T *>(Allocator::allocate(n * sizeof(T)));
        }

        void deallocate(T *p, size_t n)
        {
            Allocator::deallocate((void *)p);
        }

        void destroy(T *p)
        {
            if (p)
            {
                p->~T();
            }
        }
    };
}
