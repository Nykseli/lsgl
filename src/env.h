#ifndef _LSGL_ENV_H_
#define _LSGL_ENV_H_

typedef struct EnvDict {
    const char* key; // hash key
    void* value;
    UT_hash_handle hh;
} EnvDict;


typedef struct exce_env {
    EnvDict* variables;
    struct exce_env* enclosing;
} ExcecutionEnv;

#endif
