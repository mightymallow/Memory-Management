#ifndef __KALLOCATOR_H__
#define __KALLOCATOR_H__

enum allocation_algorithm {FIRST_FIT, BEST_FIT, WORST_FIT};

void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm);

void* kalloc(int _size);
void kfree(void* _ptr);
int available_memory();
void print_statistics();
int compact_allocation(void** _before, void** _after);
void destroy_allocator();
void* firstFit(int size);
void* bestFit(int size);
void* worstFit(int size);

#endif
