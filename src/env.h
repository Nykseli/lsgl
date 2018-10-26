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

// typedef void* (*FuncCall)(LiteralExpr** args, int argLen, void* declaration, ExcecutionEnv* env);

// typedef struct callable {
//     unsigned int arity;
//     FuncCall call;
//     void* declaration;
//     ExcecutionEnv* closure;
// } Callable;

// //TODO: CALLEE_OBJECT
// typedef enum calleetype {
//     CALLEE_FUNC,
//     CALLEE_OBJ,
// } CalleeType;

// typedef struct callee {
//     CalleeType type;
//     void* callee; 
// }Callee;

#endif
