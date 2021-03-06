#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dict.h"
#include "uthash.h"
#include "expr.h"
#include "env.h"

static unsigned int hashCode(const char* key);
static KeyValuePair* dictGetBucket(Dictionary* dict, const char* key);

Dictionary* dict()
{
    Dictionary* dict = (Dictionary*)malloc(sizeof(Dictionary));
    memset(dict, 0, sizeof(Dictionary));
    dict->capacity = DICT_INITIAL_CAPACITY;
    dict->count = 0;
    return dict;
}

// int dictAdd(Dictionary* dict, const char* key, void* value)
// {
//     KeyValuePair *pair = NULL, *bucket = NULL;
//     int hash = 0;
//     if (dict != NULL) {
//         if (!dictContains(dict, key)) {
//             pair = (KeyValuePair*)malloc(sizeof(KeyValuePair));
//             pair->key = key;
//             pair->value = value;
//             pair->next = NULL;
//             hash = hashCode(key);
//             bucket = dict->buckets[hash];
//             if (bucket == NULL) {
//                 dict->buckets[hash] = pair;
//             } else {
//                 pair->next = dict->buckets[hash];
//                 dict->buckets[hash] = pair;
//             }
//             dict->count++;
//             return 1;
//         }
//     }
//     return 0;
// }

int dictAdd(ExcecutionEnv* exceEnv, const char* key, void* value){
    struct EnvDict* d = NULL;
    EnvDict** current = &exceEnv->variables;
    if(current)
        HASH_FIND(hh,*current, key, strlen(key),d);
    if(d == NULL){
        d = (struct EnvDict*)malloc(sizeof(struct EnvDict));
        d->key = key;
        d->value = value;
        HASH_ADD_KEYPTR(hh, *current, d->key, strlen(d->key), d);
        // HASH_ADD_STR(*dict, key, d);
        return 1;
    }else{
        
    }
    
    return 0;
}

int dictRemove(Dictionary* dict, const char* key){
    KeyValuePair *bucket = NULL, *prevBucket = NULL;
    int hash = hashCode(key);
    if (dict == NULL) {
        return 0;
    }

    bucket = dict->buckets[hash];

    if (bucket != NULL) {
        for (bucket = dict->buckets[hash]; bucket != NULL; bucket = bucket->next) {
            if (strcmp(key, bucket->key) == 0) {
                free(bucket->key);
                prevBucket->next = bucket->next;
                free(bucket);
                return 1;
            }
            prevBucket = bucket;
        }
    }

    return 0;
}

int dictContains(Dictionary* dict, const char* key)
{
    return dictGetBucket(dict, key) != NULL;
}

void dictDestroy(Dictionary* dict)
{
    KeyValuePair *bucket = NULL, *nextBucket = NULL;
    int i = 0;
    if (dict != NULL) {
        for (i = 0; i < DICT_INITIAL_CAPACITY; i++) {
            nextBucket = bucket = dict->buckets[i];
            while (bucket != NULL) {
                free(bucket->key);
                nextBucket = bucket->next;
                free(bucket);
                bucket = nextBucket;
            }
        }
        free(dict);
    }
}

static KeyValuePair* dictGetBucket(Dictionary* dict, const char* key)
{
    KeyValuePair* bucket = NULL;
    int hash = 0;
    if (dict == NULL || dict->count == 0) {
        return NULL;
    }
    hash = hashCode(key);
    for (bucket = dict->buckets[hash]; bucket != NULL; bucket = bucket->next) {
        if (strcmp(key, bucket->key) == 0) {
            return bucket;
        }
    }
    return NULL;
}

void* dictGet(ExcecutionEnv* exceEnv, const char* key)
{
    struct EnvDict* bucket;
    EnvDict** current = &exceEnv->variables;
    HASH_FIND_STR(*current, key, bucket);
    
    if(bucket == NULL){
        if (exceEnv->enclosing != NULL){
            return dictGet(exceEnv->enclosing, key);
        }    
    }

    return bucket != NULL ? bucket->value : NULL;
}

int dictSet(ExcecutionEnv* exceEnv, const char* key, void* value){
    struct EnvDict* d;
    struct EnvDict* out;
    EnvDict** current = &exceEnv->variables;
    HASH_FIND_STR(*current, key, d);
    if(d != NULL){
        d = (struct EnvDict*)malloc(sizeof(EnvDict));
        d->key = key;
        d->value = value;
        HASH_REPLACE_STR(*current, key, d, out);
        return 1;
    }
    if(exceEnv->enclosing != NULL){
        return dictSet(exceEnv->enclosing, key, value);
    }
    // memcpy(d->value, value, sizeof(value));

    return 0;
}

static unsigned int hashCode(const char* key)
{
    unsigned int hash = 0, i = 0, length = strlen(key);
    for (i = 0; i < length; i++) {
        hash = 31 * hash + key[i];
    }
    return hash % DICT_INITIAL_CAPACITY;
}
