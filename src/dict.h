#ifndef _LOX_DICT_H_
#define _LOX_DICT_H_

#define DICT_INITIAL_CAPACITY 16
#include "uthash.h"

typedef struct key_value_pair_t {
    char* key;
    void* value;    
    struct key_value_pair_t* next;
} KeyValuePair;

typedef struct dict_t {
    KeyValuePair* buckets[DICT_INITIAL_CAPACITY];
    int capacity;
    int count;
} Dictionary;

typedef struct EnvDict {
    const char* key; // hash key
    void* value;
    UT_hash_handle hh;
} EnvDict;

Dictionary* dict();

// int dictAdd(Dictionary* dict, const char* key, void* value);
int dictAdd(EnvDict** dict, const char* key, void* value);
int dictRemove(Dictionary* dict, const char* key);
void* dictGet(EnvDict** dict, const char* key);
void dictDestroy(Dictionary* dict);
int dictContains(Dictionary* dict, const char* key);
int dictSet(EnvDict** dict, const char* key, void* value);
#endif
