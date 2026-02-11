#include "malloc.h"
#include "print_string.h"   // untuk debug

typedef struct block_header {
    size_t size;
    int free;
    struct block_header* next;
} block_t;

static block_t* heap_start = 0;
static block_t* heap_end = 0;

void heap_init(void* start, size_t size) {
    heap_start = (block_t*)start;
    heap_start->size = size - sizeof(block_t);
    heap_start->free = 1;
    heap_start->next = 0;
    heap_end = heap_start;
}

void* malloc(size_t size) {
    // align size ke 4/8 byte
    size = (size + 3) & ~3;
    if (size < 16) size = 16;
    
    block_t* curr = heap_start;
    while (curr) {
        if (curr->free && curr->size >= size) {
            // split jika sisa cukup besar
            if (curr->size >= size + sizeof(block_t) + 16) {
                block_t* new_block = (block_t*)((char*)(curr + 1) + size);
                new_block->size = curr->size - size - sizeof(block_t);
                new_block->free = 1;
                new_block->next = curr->next;
                
                curr->size = size;
                curr->next = new_block;
            }
            curr->free = 0;
            return (void*)(curr + 1);
        }
        curr = curr->next;
    }
    return 0;   // out of memory
}

void free(void* ptr) {
    if (!ptr) return;
    block_t* block = (block_t*)ptr - 1;
    block->free = 1;
    
    // coalesce dengan next block jika free
    if (block->next && block->next->free) {
        block->size += sizeof(block_t) + block->next->size;
        block->next = block->next->next;
    }
    // coalesce dengan prev block? perlu doubly linked list atau scan dari awal
    // (opsional: implementasi sederhana dulu)
}
