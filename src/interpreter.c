#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "expr.h"
#include "dict.h"
#include "visitor.h"
#include "uthash.h"

static void* visitLiteralExpr(void* expr);
static void* visitGroupingExpr(void* expr);
static void* visitUnaryExpr(void* expr);
static void* visitBinaryExpr(void* expr);
static void* visitVarExpr(void* expr);
static void* visitAssignExpr(void* stmt);
static void* visitLogicalExpr(void* stmt);
static void* visitCallExpr(void* expr);

//static void* functionCall(Token paren, Callee callee, Expr** args, int argLen);
static void* functionCall();
static char* isTruthy(LiteralExpr* expr);

static void* visitExpressionStmt(Stmt* stmt);
static void* visitPrintStmt(Stmt* stmt);
static void* visitVarStmt(Stmt* stmt);
static void* visitBlockStmt(Stmt* stmt);
static void* visitIfStmt(Stmt* stmt);
static void* visitWhileStmt(Stmt* stmt);
static void* visitFunctionStmt(Stmt* stmt);
static void* visitReturnStmt(Stmt* stmt);
static void* execute(Stmt* stmt);

static void* evaluate(Expr* expr);

static ExcecutionEnv* newEnv();

static Dictionary* enviromentDict;

//TODO: global functions in global exce env
static ExcecutionEnv* currentEnv = NULL;

//static EnvDict* currentEnv->enclosing = NULL;


ExpressionVisitor EvalExpressionVisitor = {
    .visitLiteral = visitLiteralExpr,
    .visitGrouping = visitGroupingExpr,
    .visitUnary = visitUnaryExpr,
    .visitBinary = visitBinaryExpr,
    .visitVar = visitVarExpr,
    .visitAssign = visitAssignExpr,
    .visitLogical = visitLogicalExpr,
    .visitCall = visitCallExpr,
};

StatementVisitor EvalStatmentVisitor = {
    .visitExpression = visitExpressionStmt,
    .visitPrint = visitPrintStmt,
    .visitVar = visitVarStmt,
    .visitBlock = visitBlockStmt,
    .visitIf = visitIfStmt,
    .visitWhile = visitWhileStmt,
    .visitFunction = visitFunctionStmt,
    .visitReturn = visitReturnStmt,
};

/**
* This function is used to set the CallExpr Callee variable
* This "executes" the definied function 
*/
//static void* functionCall(Token paren, Callee callee, Expr** args, int argLen){
static void* functionCall(Token* params, BlockStmt* body, int paramLen, LiteralExpr** arguments){
    //TODO: i need stmt block that basicly contains the function
    // i also need values of the argumens so i can assing then to the block scope
    // after the arguments are assigned to block scope, i just run the scope
    // i deal with the function return value later

    /*
BlockStmt* bStmt = (BlockStmt*)stmt->stmt;
    ExcecutionEnv* prev = currentEnv, *env = newEnv();
    env->enclosing = prev;
    currentEnv = env;
    for(int i = 0; i< bStmt->stmtLen; i++){
        execute(bStmt->statements[i]);
    }

    currentEnv = prev;
    */

   BlockStmt* bStmt = body;
   ExcecutionEnv* prev = currentEnv, *env = newEnv();
   env->enclosing = prev;
   currentEnv = env;
   LiteralExpr* returnValue = NULL;
   for(int i = 0; i < paramLen; i++){
       dictAdd(&currentEnv->variables, params[i].lexeme, arguments[i]);
   }

   for(int i = 0; i< bStmt->stmtLen; i++){
        returnValue = execute(bStmt->statements[i]);
        if(returnValue != NULL) break;
    }

    currentEnv = prev;
    if(returnValue != NULL) return returnValue;

    return NULL;
}


static char* isTruthy(LiteralExpr* lExpr){

    if(lExpr){
        return 1;
    }
    return 0;
}

static const char* isNonTruthy(LiteralExpr* expr){
    if(expr->type == NULL_L) return FALSE_KEY;

    if(expr->type == LITERAL_BOOL){
        return strcmp(TRUE_KEY, (const char*)expr->value) != 0 ? TRUE_KEY : FALSE_KEY;
    }

    return FALSE_KEY;
}

static int isTrue(LiteralExpr* expr){
    if(expr->type == NULL_L) return 0;

    if(expr->type == LITERAL_BOOL){
        return strcmp(TRUE_KEY, (const char*)expr->value) != 0 ? 0 : 1;
    }

    if(expr->type == LITERAL_NUMBER) {
        //value zero is consired as false
        return *((double*)expr->value) == 0 ? 0 : 1;
    }
    
    if(expr->type == LITERAL_STRING){
        //empty string is consired as false
        return ((const char*)expr->value)[0] == '\0' ? 0 : 1;
    }

    return 0;
}

static int isEqual(LiteralExpr* left, LiteralExpr* right){
    if(left->type == LITERAL_NULL && right->type == LITERAL_NULL){
        return 0;
    }else if(left->type == LITERAL_STRING && right->type == LITERAL_STRING){
        return strcmp((char*)left->value, (const char*)right->value) != 0 ? 1 : 0;
    }else if(left->type == LITERAL_NUMBER && right->type == LITERAL_NUMBER){
        if( *((double*)left->value) == *((double*)right->value)) return  0;
        return  1;
    }


    return 1;
}

static void* evaluate(Expr* expr){
    if(expr == NULL)
    printf("eval typeis null \n");
    return acceptExpr(EvalExpressionVisitor, expr);
}

static void* visitLiteralExpr(void* expr){
    const LiteralExpr* lExpr = (LiteralExpr*)expr;
    return lExpr;
}

static void* visitGroupingExpr(void* expr){
    const GroupingExpr* gExpr = (GroupingExpr*)expr;
    return evaluate(gExpr);

}
static void* visitUnaryExpr(void* expr){
    const UnaryExpr* uExpr = (UnaryExpr*)expr;
    char* truthy = NULL;
    LiteralExpr* right = evaluate(uExpr->right);
    double* value = malloc(sizeof(double));
    switch(uExpr->op.type){
        case MINUS:
            value = right->value;
            *value = -*value;
            right->value = value;
            break;
        case BANG:
            truthy = isNonTruthy(right);
            right->type = LITERAL_BOOL;
            right->value = truthy;
            break;        

    }
    return right;

}
static void* visitBinaryExpr(void* expr){
    const BinaryExpr* bExpr = (BinaryExpr*)expr;
    LiteralExpr* right = (LiteralExpr*)evaluate(bExpr->right);
    LiteralExpr* left = (LiteralExpr*)evaluate(bExpr->left);
    LiteralExpr* result = malloc(sizeof(LiteralExpr));

    char* resultStr;
    double* value = malloc(sizeof(double*));
    int valueLength;
    double leftValue, rightValue;

    switch(bExpr->op.type){
        case MINUS:
            if(right->type == LITERAL_NUMBER && left->type == LITERAL_NUMBER){
                result->type = LITERAL_NUMBER;
                *value = *((double*)left->value) - *((double*)right->value);
                result->value = value;
                break;
            }else{
                //TODO: ERROR
            }
        case PLUS:
            //TODO support plus between strings and numbers
            if(right->type == LITERAL_NUMBER && left->type == LITERAL_NUMBER){
                result->type = LITERAL_NUMBER;
                *value = *((double*)left->value) + *((double*)right->value);
                result->value = value;
                break;
            }else if(right->type == LITERAL_STRING && left->type == LITERAL_STRING){
                valueLength = strlen((char*) right->value) + strlen((char*) left->value);
                valueLength++;
                resultStr = malloc(sizeof(char) * valueLength);
                strcpy(resultStr, (char*) left->value);
                strcat(resultStr, (char*) right->value);
                result->value = resultStr;
                result->valueSize = valueLength;
                result->type = LITERAL_STRING;
            }else{
                //TODO: ERROR
            }
        case GREATER:
            if(right->type == LITERAL_NUMBER && left->type == LITERAL_NUMBER){
                result->type = LITERAL_BOOL;
                leftValue = *((double*)left->value);
                rightValue = *((double*)right->value);
                result->value = leftValue > rightValue ? TRUE_KEY : FALSE_KEY;
            }else{
                //TODO: ERROR
            }
            break;
        case GREATER_EQUAL:
            if(right->type == LITERAL_NUMBER && left->type == LITERAL_NUMBER){
                result->type = LITERAL_BOOL;
                leftValue = *((double*)left->value);
                rightValue = *((double*)right->value);
                result->value = leftValue >= rightValue ? TRUE_KEY : FALSE_KEY;
            }else{
                //TODO: ERROR
            }
            break;
        case LESS:
            if(right->type == LITERAL_NUMBER && left->type == LITERAL_NUMBER){
                result->type = LITERAL_BOOL;
                leftValue = *((double*)left->value);
                rightValue = *((double*)right->value);
                result->value = leftValue < rightValue ? TRUE_KEY : FALSE_KEY;
            }else{
                //TODO: ERROR
            }
            break;
        case LESS_EQUAL:
            if(right->type == LITERAL_NUMBER && left->type == LITERAL_NUMBER){
                result->type = LITERAL_BOOL;
                leftValue = *((double*)left->value);
                rightValue = *((double*)right->value);
                result->value = leftValue <= rightValue ? TRUE_KEY : FALSE_KEY;
            }else{
                //TODO: ERROR
            }
            break;
        case EQUAL_EQUAL:
            result->type = LITERAL_BOOL;
            if(isEqual(left, right) == 0){
                result->value = TRUE_KEY;
            }else{
                result->value = FALSE_KEY;
            }
            break;
        case BANG_EQUAL:
            result->type = LITERAL_BOOL;
            if(isEqual(left, right) != 0){
                result->value = TRUE_KEY;
            }else{
                result->value = FALSE_KEY;
            }
            break;
    }

    return result;

}

static void* visitVarExpr(void* expr){
    //TODO: refactor this to support functions (and objects)
    //note that dictGet returns void* maybe just return that and
    //let the "reciever" handle the type conversion since everything does
    //that allready
    VarExpr* vExpr = (VarExpr*) expr;
    LiteralExpr* lExpr = (LiteralExpr*)dictGet(currentEnv, vExpr->name.lexeme);
    if(lExpr == NULL){
        printf("lExpr from dict is NULL\n");
    }
     return lExpr;
}

static void* visitAssignExpr(void* expr){
    AssignExpr* aExpr = (AssignExpr*) expr;
    LiteralExpr* lExpr = (LiteralExpr*)evaluate(aExpr->value);
    // VarExpr* vExpr = malloc(sizeof(VarExpr));
    // vExpr->name = aExpr->name;
    
    if(dictSet(&currentEnv->variables, aExpr->name.lexeme, lExpr) == 1){
        return lExpr;
    }
    
    return NULL;
}

static void* visitLogicalExpr(void* expr) {
    LogicalExpr* loExpr = (LogicalExpr*) expr;
    LiteralExpr* left = evaluate(loExpr->left);

    if(loExpr->op.type == OR){
        if(isTrue(left)) return left;
    }else{
        if(!isTrue(left)) return left;
    } 

    return evaluate(loExpr->right);
}

static void* visitCallExpr(void* expr){
    CallExpr* cExpr = (CallExpr*)expr;

    //TODO: here evaluate gets a void pointer that refers to 
    // function statement that we have stored in to the
    // excecution hasmap. so this should to FunctionStmt
    //TODO: should this evaluate somekind of struct that has functions and obejcts?

    //TODO: callee will visit visitVarExpr function tthat returns literalExpr
    //visitVarExpr should be changed to return void* so we can utilize it here
    //this would mean that we would need to think alot of the hasmap stuff
    //other option is that we add LITERAL_FUNCTION to LiteralType enum
    //and then cast LITERAL_FUNTIONS as FunctStmt*. it is not pretty and may not work
    //so lets just rework the visitVarExpr
    LiteralExpr* callee = evaluate(cExpr->callee);
    //TODO: result type?
    FunctionStmt* callable = NULL;
    LiteralExpr* result = NULL;
    if(callee == NULL){
        printf("callee is null\n");
        //TODO: runtime error function / class not defined
    }

    if(callee->type != LITERAL_FUNCTION && callee->type != LITERAL_OBJECT){
        //TODO: runtime error
    }

    if(callee->type == LITERAL_FUNCTION){
        callable = (FunctionStmt*)callee->value;
    }
    
    LiteralExpr** arguments = NULL;
    for(int i = 0; i < cExpr->argLen; i++){
        arguments = realloc(arguments, sizeof(LiteralExpr) * (i + 1));
        arguments[i] = evaluate(cExpr->arguments[i]);
    }

    result = callable->call(callable->params, callable->body, callable->paramLen, arguments);
    //result = call->call(arguments, cExpr->argLen, call->declaration, call->closure);
    return result;
}


static void* visitExpressionStmt(Stmt* stmt){
    ExprStmt* eStmt = (ExprStmt*) stmt->stmt;
    LiteralExpr* lExpr = (LiteralExpr*)evaluate(eStmt->expr);
    if(lExpr == NULL){
       //TODO: functions returns null so lExpr is null when function is called
        printf("EXPRESSSION = NULL\n");
    }
    return lExpr;
}

static void* visitPrintStmt(Stmt* stmt){
    PrintStmt* pStmt = (PrintStmt*) stmt->stmt;
    LiteralExpr* lExpr = (LiteralExpr*)evaluate(pStmt->expr);
    if(lExpr == NULL) printf("kakels byls");
    
    if(lExpr->type == NULL) return NULL;

    if(lExpr->type == LITERAL_STRING){
        //printf("Interpret string: %s\n", (char*) lExpr->value);
        printf("%s\n", (char*) lExpr->value);
    }else if(lExpr->type == LITERAL_NUMBER){
        printf("%f\n", *((double*) lExpr->value));
    }else if(lExpr->type == LITERAL_BOOL){
        printf("%s\n", (char*) lExpr->value);
    }
    return NULL;
}

static void* visitVarStmt(Stmt* stmt){
    VarStmt* vStmt = (VarStmt*) stmt->stmt;
    LiteralExpr* lExpr = (LiteralExpr*)evaluate(vStmt->expr);
    
    dictAdd(&currentEnv->variables, vStmt->name.lexeme, lExpr);
    
}

static ExcecutionEnv* newEnv(){
    ExcecutionEnv* env = malloc(sizeof(ExcecutionEnv));
    env->variables = NULL;
    return env;
}


static void* visitBlockStmt(Stmt* stmt){
    BlockStmt* bStmt = (BlockStmt*)stmt->stmt;
    ExcecutionEnv* prev = currentEnv, *env = newEnv();
    env->enclosing = prev;
    currentEnv = env;
    for(int i = 0; i< bStmt->stmtLen; i++){
        execute(bStmt->statements[i]);
    }

    currentEnv = prev;
    return NULL;
}

static void* visitIfStmt(Stmt* stmt){
    IfStmt* iStmt = (IfStmt*)stmt->stmt;
    LiteralExpr* lExpr = (LiteralExpr*) evaluate(iStmt->condition);
    LiteralExpr* result = NULL;
    //if(strcmp(isNonTruthy(lExpr), (char*)TRUE_KEY) == 0){
    if(isTrue(lExpr)){
        result = malloc(sizeof(LiteralExpr));
        result = execute(iStmt->thenBranch);
    }else if(iStmt->elseBranch != NULL){
        result = malloc(sizeof(LiteralExpr));
        execute(iStmt->elseBranch);
    }
    return result;
}

static void* visitWhileStmt(Stmt* stmt){
   // Stmt* sStmt = (Stmt*)stmt->stmt;
    WhileStmt* wStmt = (WhileStmt*)stmt->stmt;
    

    while(isTrue((LiteralExpr*)evaluate(wStmt->condition))){
        execute(wStmt->body);
    }

    return NULL;
}


static void* visitFunctionStmt(Stmt* stmt){
    //TODO: save function statment to hasmap so you can
    //reference it with callExpr
    FunctionStmt* fStmt = (FunctionStmt*)stmt->stmt;
    fStmt->call = functionCall;
    // Values must be saved to dict as LiteralExpr*
    LiteralExpr* lExpr = malloc(sizeof(LiteralExpr));
    lExpr->type = LITERAL_FUNCTION;
    lExpr->value = fStmt;
    dictAdd(&currentEnv->variables, fStmt->name.lexeme, lExpr);
    return NULL;
}

static void* visitReturnStmt(Stmt* stmt){
    ReturnStmt* rStmt = (ReturnStmt*) stmt->stmt;
    LiteralExpr* value = NULL;

    if(rStmt->value != NULL){
        value = evaluate(rStmt->value);
    }

    return value;

}

static void* execute(Stmt* stmt){
    acceptStmt(EvalStatmentVisitor, stmt);
}

void interpret(ParsedStmt stmts){
    enviromentDict = dict();
    currentEnv = realloc(currentEnv, sizeof(currentEnv));
    currentEnv->enclosing = NULL;
    currentEnv->variables = NULL;
    int i;
    for(i = 0; i < stmts.stmtLen; i++){
        execute(stmts.stmt[i]);
    }
}
