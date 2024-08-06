#include "clib.h"
#include "utils.h"

CstrArray make_cstr_array(size_t capacity, Cstr first, ...)
{
    CstrArray result = {0};
    result.capacity = capacity;
    result.count = 0;

    va_list args;
    va_start(args, first);
    for(Cstr next = va_arg(args, Cstr); next != NULL && result.count < capacity; next = va_arg(args, Cstr)) {
        result.count++;
    }
    va_end(args);

    result.items = (Cstr*) malloc(sizeof(Cstr) * result.count);
    result.count = 0;

    va_start(args, first);
    for(Cstr next = va_arg(args, Cstr); next != NULL && result.count < capacity; next = va_arg(args, Cstr)) {
        result.items[result.count++] = next;
    }
    va_end(args);

    return result;
}

void cstr_array_append(CstrArray* array, Cstr item)
{
    if(array->count >= array->capacity) {
        ERRO("Array is at max capacity: %zu", array->capacity);
        return;
    }

    if(item == NULL) {
        ERRO("Cannot add a NULL item to the list");
        return;
    }

    array->items[array->count++] = item;
}

void cstr_array_remove(CstrArray* array, Cstr item) 
{
    if (array->count == 0) {
        WARN("Nothing to remove");
        return;
    }

    if(item == NULL) {
        WARN("Cannot remove a NULL item to the list");
        return;
    }

    int index = -1;
    for(index = 0; index < (int) array->count; ++index) {
        if(strcmp(item, array->items[index]) == 0) break;
    }

    if(index == -1) return;

    for(size_t i = index; i < array->count; ++i) {
        if(i != array->count -1)
            array->items[i] = array->items[i + 1];
        else 
            array->items[i] = NULL;
    }
}
void cstr_array_remove_index(CstrArray* array, size_t index)
{
    if (array->count == 0) {
        WARN("Nothing to remove");
        return;
    }

    if(index > array->count || index > array->capacity) {
        ERRO("There aren't that many elements in the array");
        return;
    }

    for(size_t i = index; i < array->count; ++i) {
        if(i != array->count -1)
            array->items[i] = array->items[i + 1];
        else 
            array->items[i] = NULL;
    }
    
}
