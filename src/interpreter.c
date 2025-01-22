#include "interpreter.h"
#include "environment.h"
#include "callable.h"
#include "LinkList_string.h"

Interpreter interpreter;

Literal *evaluate(Expr* expr){
    return (Literal *)ExprAccept(interpreter.expr_visitor, expr);
}

void *execute(Stmt* stmt){
    return StmtAccept(interpreter.stmt_visitor, stmt);
}

void* interpret_Literal(Expr* expr){
    return (void*)expr->expr.literal.value;
}

void* interpret_Grouping(Expr* expr){
    return evaluate(expr->expr.grouping.expression);
}

void * interpret_Call(Expr* expr){
    Literal * callee; 
    List_Expr * arguments = expr->expr.call.arguments;
    List_Literal * args = newList_Literal();

    Node_Expr * current = arguments->head;
    List_string * args_types = newList_string();
    Literal * arg;

    while (current != NULL){
        arg = evaluate(current->data);
        add_Literal(args, arg);
        add_string(args_types, strdup(arg->type));
        current = current->next;
    }
    
    if (expr->expr.call.callee->type == EXPR_VARIABLE){
        key_field *key = (key_field*)malloc(sizeof(key_field));
        key->type = FUNCTION;
        key->field.function.name = strdup(expr->expr.call.callee->expr.variable.name->lexeme);
        key->field.function.args_types = args_types;
        key->field.function.non_optional_args = args->size;
        freeLiteral(expr->expr.call.callee->expr.variable.name->literal,1);
        expr->expr.call.callee->expr.variable.name->literal = newLiteral("key_field", (void*)key,1);
    } else {
        fprintf(stderr, "Invalid callee for call\n");
    }
    callee = evaluate(expr->expr.call.callee);

    Callable * function = literal_as_Callable(callee);
    Literal * res = execute_callable(function, &interpreter, args);

    freeLiteral(callee,0);

    Node_Literal * current_literal = args->head;
    Node_Literal * next;
    while (current_literal != NULL){
        next = current_literal->next;
        free(current_literal);
        current_literal = next;
    }
    free(args);
    return res;//(void*)res;
}

void * interpret_Print(Stmt* stmt){
    Literal* value = evaluate(stmt->stmt.print.expression);
    if (cmp_types(value->type, "int")){
        printf("%d\n", *((int*)value->data));
    } else if (cmp_types(value->type, "double")){
        printf("%f\n", *((double*)value->data));
    } else if (cmp_types(value->type, "string")){
        printf("%s\n", (char*)value->data);
    } else if (cmp_types(value->type, "bool")){
        printf("%s\n", *((int*)value->data) ? "true" : "false");
    } else {
        fprintf(stderr, "Invalid type for print\n");
        exit(1);
    }
    freeLiteral(value,0);
    return NULL;
}

void *interpret_Variable(Expr* expr){
    Token* tname = expr->expr.variable.name;
    Rvariable* var = get_var(interpreter.env, tname, 1);
    if (var == NULL){
        fprintf(stderr, "Variable %s not found\n", (char*)tname->literal->data);
        exit(1);
    }
    freeLiteral(tname->literal,1);
    return (void*)var_to_literal(var);
}

void * interpret_VarDeclaration(Stmt* stmt){
    Token* tname = stmt->stmt.var.name;
    Token* type = stmt->stmt.var.type;
    Expr* initializer = stmt->stmt.var.initializer;
    Rtype* t = searchHT_Rtype(types, (char*)type->literal->data);

    char * name = tname->lexeme;

    if (t == NULL){
        fprintf(stderr, "Type %s not found\n", (char*)type->literal->data);
        exit(1);
    }
    if (get_var(interpreter.env, tname, 0)!=NULL){
        fprintf(stderr, "Variable %s already exists in this scope\n", name);
        exit(1);
    }
    if (strcmp(tname->literal->type, "key_field") != 0){
        fprintf(stderr, "Invalid key_field for variable %s\n", name);
    }
    Rvariable *var;
    key_field* key = (key_field*)tname->literal->data;
    if (t->size == 0)
        var = newRvariable(t->name, key, NULL);
    else
        var = newRvariable(t->name, key, malloc(t->size));

    if (interpreter.env->enclosing == NULL && key -> type == FUNCTION && key->field.function.args_types != NULL && key->field.function.args_types->size > 0){
        // add to lookup only if defined in global scope
        char* type_first_arg = key->field.function.args_types->head->data;
        Rtype *Rtype_first_arg = searchHT_Rtype(types, type_first_arg);
        addHT_var(Rtype_first_arg->look_up_table, var, 0);
    } else addHT_var(interpreter.env->vars, var, 0);
    
    if (initializer != NULL){
        Literal * value = evaluate(initializer);
        update_var_from_Literal(var, value);
        free(value->data);
        free(value->type);
        free(value);
    }
    freeLiteral(type->literal,1);
    free(tname->literal->type);
    free(tname->literal);
    // free(value);
    return NULL;
}

void * interpret_VarAssign(Expr* expr){
    Token* tname = expr->expr.assign.name;
    Literal* value = evaluate(expr->expr.assign.value);
    Rvariable* var = get_var(interpreter.env, tname, 1);

    if (var == NULL){
        Rtype *rtype = searchHT_Rtype(types, value->type);
        key_field *key = (key_field*)malloc(sizeof(key_field));
        key->type = rtype->key_type;
        switch (key->type){
            case NAME:
                key->field.name = strdup(tname->lexeme);
                break;
            case FUNCTION:
                key->field.function.name = strdup(tname->lexeme);
                key->field.function.args_types = NULL;
                break;
        }
        freeLiteral(tname->literal,1);
        tname->literal = newLiteral("key_field", (void*)key,1);

        Literal * tmp = newLiteral("string", strdup(value->type),1);
        Token *type = newToken(TYPE, value->type, tmp, tname->line,1);
        Expr * literal_expr = newLiteralExpr(value);

        Stmt* stmt = newVarStmt(type, tname, literal_expr);
        execute(stmt);

        free(literal_expr);
        free(stmt);
        freeToken(type);
        // freeLiteral(tmp);
    } else {
        update_var_from_Literal(var, value);
        freeLiteral(value,0);
        freeLiteral(tname->literal,0);
    }
    return NULL;
}

void * interpret_Expr(Stmt* stmt){
    return evaluate(stmt->stmt.expression.expression);
}

void *interpret_Block(Stmt* stmt){
    Environment* prev = interpreter.env;
    List_Stmt *stmts = stmt->stmt.block.statements;
    Node_Stmt* stop_value = NULL;
    interpreter.env = newEnvironment(interpreter.env);
    stop_value = Interpret(stmts);
    stmts->free_lit_from = stop_value;
    freeEnvironment(interpreter.env);
    interpreter.env = prev;
    return NULL;
}


void *interpret_Ret(Expr* expr){
    Literal* value = evaluate(expr->expr.ret.value);
    if (strcmp(value->type, "string") == 0 && strcmp(value->data,"nil") == 0){
        freeLiteral(value,0);
    }
    else if (interpreter.ret_value == NULL){
        fprintf(stderr, "obtained a not nil return value but no place to assign it\n");
        exit(1);
    } else {
        *interpreter.ret_value = value;
    }
    interpreter.check_return = 1;
    return NULL;
}

Node_Stmt* Interpret(List_Stmt *stmts){
    Node_Stmt *current = stmts->head;
    while (current != NULL && !interpreter.check_return){
        execute(current->data);
        current = current->next;
    }
    if (interpreter.check_return){
        // if (interpreter.ret_value == NULL){
        //     fprintf(stderr, "obtained a return value but no place to assign it\n");
        //     exit(1);
        // }
        // *interpreter.ret_value = (Literal*)tmp_ret;
        return current;
    }
    return NULL;
}

void interpreter_init(){
    interpreter.expr_visitor = (ExprVisitor*)malloc(sizeof(ExprVisitor));
    interpreter.stmt_visitor = (StmtVisitor*)malloc(sizeof(StmtVisitor));

    // interpreter.expr_visitor->visitBinary    = interpret_Binary;
    interpreter.expr_visitor->visitGrouping  = interpret_Grouping;
    interpreter.expr_visitor->visitLiteral   = interpret_Literal;
    interpreter.expr_visitor->visitVariable  = interpret_Variable;
    interpreter.expr_visitor->visitAssign    = interpret_VarAssign;
    interpreter.expr_visitor->visitCall      = interpret_Call;
    interpreter.expr_visitor->visitRet       = interpret_Ret;


    interpreter.stmt_visitor->visitExpression = interpret_Expr;
    interpreter.stmt_visitor->visitPrint      = interpret_Print;
    interpreter.stmt_visitor->visitVar        = interpret_VarDeclaration;
    interpreter.stmt_visitor->visitBlock      = interpret_Block;

    interpreter.env = newEnvironment(NULL);
    interpreter.ret_value = NULL;
    interpreter.check_return = 0;
    return;
}

void freeInterpreter(){
    free(interpreter.expr_visitor);
    free(interpreter.stmt_visitor);
    freeEnvironment(interpreter.env);
    return;
}

