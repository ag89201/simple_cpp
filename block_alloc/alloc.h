#pragma once
#include <stdint.h>
#include <stddef.h>

namespace heap
{

    template <typename guard>
    class scope_guard
    {
    public:
        scope_guard(guard &g) : g_(g) { g_.lock(); }
        ~scope_guard() { g_.unlock(); }

    private:
        guard &g_;
    };

    struct default_gurad
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
        memControlBlock *split(size_t size, memControlBlock *start)
        {
            uintptr_t addr = (uintptr_t)this + size;

            memControlBlock *new_block = (memControlBlock *)addr;

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
        void merge_with_next(memControlBlock *start)
        {
            memControlBlock *other = next;
            ts.size += other->ts.size;
            other = next = other->next;
            if (other != start)
            {
                other->prev = this;
            }
        }
        void *pool() { return this + 1; }
    };

    template <typename guard>
    class allocator
    {
    private:
        static size_t const HEAP_ALIGN = sizeof(int);

        void init(memControlBlock *pstart, size_t size_bytes)
        {
            pstart->next = pstart;
            pstart->prev = pstart;

            pstart->ts.size = size_bytes - sizeof(memControlBlock);
            pstart->ts.available = memControlBlock::FREE;
        }

        memControlBlock *start;
        memControlBlock *freemem;
        guard Guard;

    public:
        struct summary
        {
            struct info
            {
                size_t Blocks;
                size_t Block_max_size;
                size_t Size;

            } Used, Free;
        };

        summary info()
        {
            summary s;
            s.Used.Size = 0;
            s.Used.Blocks = 0;
            s.Used.Block_max_size = 0;

            s.Free.Size = 0;
            s.Free.Blocks = 0;
            s.Free.Block_max_size = 0;

            memControlBlock *mcb = freemem;
            do
            {
                auto *info = (mcb->ts.available == memControlBlock::FREE) ? &s.Free : &s.Used;
                ++info->Blocks;
                info->Size += mcb->ts.size;
                if (info->Block_max_size < mcb->ts.size)
                {
                    info->Block_max_size = mcb->ts.size;
                }
                mcb = mcb->next;

            } while (mcb != start);
            return s;
        }

        template <size_t size_bytes>
        allocator(pool<size_bytes> &pool)
            : start((memControlBlock *)pool.Pool),
              freemem((memControlBlock *)pool.Pool),
              Guard()
        {
            init(start, sizeof(pool));
        }

        void *allocate(size_t size)
        {

            size = (size + sizeof(memControlBlock) + (HEAP_ALIGN - 1)) & ~(HEAP_ALIGN - 1);
            if (size > freemem->ts.size)
            {
                std::cout<<"null\n";
                return nullptr;
            }

            memControlBlock *xptr;

            xptr = NULL;

            void *Allocated = nullptr;

            size_t free_cnt = 0;

            scope_guard<guard> ScopeGuard(Guard);

            memControlBlock *tptr = freemem;

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
                if (tptr == start)
                {
                    if (xptr != 0)
                    {
                        tptr = xptr;
                        xptr = tptr->split(size, start);
                        Allocated = tptr->pool();
                        break;
                    }
                }
            }

            if ((free_cnt == 1) && (Allocated))
            {
                freemem = tptr->next;
            }

            return Allocated;
        }
        void deallocate(void *pool)
        {
            if (pool == nullptr || ((uintptr_t)pool & (HEAP_ALIGN - 1)))
            {
                return;
            }

            memControlBlock *xptr;
            memControlBlock *tptr = (memControlBlock *)pool - 1;

            scope_guard<guard> ScopeGuard(Guard);

            xptr = tptr->prev;
            if ((xptr != tptr && xptr->next != tptr) || pool < start)
            {
                return;
            }

            tptr->ts.available = memControlBlock::FREE;

            xptr = tptr->next;

            if (xptr->ts.available == memControlBlock::FREE && xptr != start)
            {
                tptr->merge_with_next(start);
            }

            xptr = tptr->prev;

            if (xptr->ts.available == memControlBlock::FREE && tptr != start)
            {
                xptr->merge_with_next(start);
                tptr = xptr;
            }

            if (tptr < freemem)
            {
                freemem = tptr;
            }
        }
    };

    template <typename T, typename Allocator = allocator<default_gurad>>
    class mallocator : Allocator
    {
    public:
        using value_type = T;

        template <size_t size_bytes>
        mallocator(pool<size_bytes> &pool) : Allocator(pool) {}

        T *allocate(size_t n)
        {
            return static_cast<T *>(Allocator::allocate(n * sizeof(T)));
        }

        void deallocate(T *p, size_t n)
        {
            Allocator::deallocate((void *)p);
        }

        void destroy(T *p)
        {
            p->~T();
        }

        auto info()
        {
            return Allocator::info();
        }
    };
}
