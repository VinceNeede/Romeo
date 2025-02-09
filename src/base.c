#include "base.h"
#include "token.h"
#include "callable.h"
#include "LinkList_Literal.h"
#include "LinkList_string.h"
#include "HT_var.h"

#define binary_op(name,type, op) \
    Literal *name##_##type##_##type(List_Literal* args){ \
        Literal *res = newLiteral(#type, malloc(sizeof(type)),1); \
        *((type*)res->data) = *((type*)args->head->data->data) op *((type*)args->head->next->data->data); \
        return res; \
    }

#define binary_op2(name, type1, type2, ret_type, op) \
    Literal *name##_##type1##_##type2(List_Literal* args){ \
        Literal *res = newLiteral(#ret_type, malloc(sizeof(ret_type)),1); \
        *((ret_type*)res->data) = *((type1*)args->head->data->data) op *((type2*)args->head->next->data->data); \
        return res; \
    }\
    Literal *name##_##type2##_##type1(List_Literal* args){ \
        Literal *res = newLiteral(#ret_type, malloc(sizeof(ret_type)),1); \
        *((ret_type*)res->data) = *((type2*)args->head->data->data) op *((type1*)args->head->next->data->data); \
        return res; \
    }
#define comparison_op(name, type, op) \
    Literal *name##_##type##_##type(List_Literal* args){ \
        Literal *res = newLiteral("bool", malloc(sizeof(int)),1); \
        *((int*)res->data) = *((type*)args->head->data->data) op *((type*)args->head->next->data->data); \
        return res; \
    }
#define unary_op(name, type, op) \
    Literal *name##_##type(List_Literal* args){ \
        Literal *res = newLiteral(#type, malloc(sizeof(type)),1); \
        *((type*)res->data) = op *((type*)args->head->data->data); \
        return res; \
    }

comparison_op(eq, int, ==)
comparison_op(neq, int, !=)
comparison_op(lt, int, <)
comparison_op(gt, int, >)
comparison_op(le, int, <=)
comparison_op(ge, int, >=)
// comparison_op(eq, double, ==) Double should not be compared
comparison_op(neq, double, !=)
comparison_op(lt, double, <)
comparison_op(gt, double, >)
comparison_op(le, double, <=)
comparison_op(ge, double, >=)


binary_op(add, int, +)
binary_op(sub, int, -)
binary_op(mul, int, *)
binary_op(div, int, /)
binary_op(add, double, +)
binary_op(sub, double, -)
binary_op(mul, double, *)
binary_op(div, double, /)
binary_op2(add, int, double, double, +)
binary_op2(sub, int, double, double, -)
binary_op2(mul, int, double, double, *)
binary_op2(div, int, double, double, /)

unary_op(negate, int, -)
unary_op(negate, double, -)
unary_op(not, int, !)
unary_op(not, bool, !)

#define  C_to_Romeo_fun(fun_name, ret_type) \
    args_types = newList_string(); \
    add_string(args_types, strdup(#ret_type)); \
    add_string(args_types, strdup(#ret_type)); \
    key = (key_field*)malloc(sizeof(key_field)); \
    key->type = FUNCTION; \
    key->field.function.name = strdup(#fun_name); \
    key->field.function.args_types = args_types; \
    key->field.function.non_optional_args = args_types->size; \
    callee = newCallable(NULL, NULL, #ret_type); \
    callee->function = fun_name##_##ret_type##_##ret_type; \
    addHT_var(r##ret_type->look_up_table, newRvariable(rfunction, key, callee),0);  

#define  C_to_Romeo_fun2(fun_name, type1, type2, ret_type) \
    args_types = newList_string(); \
    add_string(args_types, strdup(#type1)); \
    add_string(args_types, strdup(#type2)); \
    key = (key_field*)malloc(sizeof(key_field)); \
    key->type = FUNCTION; \
    key->field.function.name = strdup(#fun_name); \
    key->field.function.args_types = args_types; \
    key->field.function.non_optional_args = args_types->size; \
    callee = newCallable(NULL, NULL, #ret_type); \
    callee->function = fun_name##_##type1##_##type2; \
    addHT_var(r##type1->look_up_table, newRvariable(rfunction, key, callee),0);
#define C_to_Romeo_unary_fun(fun_name, arg_type) \
    args_types = newList_string(); \
    add_string(args_types, strdup(#arg_type)); \
    key = (key_field*)malloc(sizeof(key_field)); \
    key->type = FUNCTION; \
    key->field.function.name = strdup(#fun_name); \
    key->field.function.args_types = args_types; \
    key->field.function.non_optional_args = args_types->size; \
    callee = newCallable(NULL, NULL, #arg_type); \
    callee->function = fun_name##_##arg_type; \
    addHT_var(r##arg_type->look_up_table, newRvariable(rfunction, key, callee),0);
void base_functions(){
    List_string *args_types;
    key_field *key;
    Callable *callee;
    Rtype *rint = searchHT_Rtype(types, "int");
    Rtype *rdouble = searchHT_Rtype(types, "double");
    Rtype *rbool = searchHT_Rtype(types, "bool");
    Rtype *rfunction = searchHT_Rtype(types, "function");

    C_to_Romeo_fun(add, int)
    C_to_Romeo_fun(sub, int)
    C_to_Romeo_fun(mul, int)
    C_to_Romeo_fun(div, int)
    C_to_Romeo_fun(add, double)
    C_to_Romeo_fun(sub, double)
    C_to_Romeo_fun(mul, double)
    C_to_Romeo_fun(div, double)
    C_to_Romeo_fun2(add, int, double, double)
    C_to_Romeo_fun2(sub, int, double, double)
    C_to_Romeo_fun2(mul, int, double, double)
    C_to_Romeo_fun2(div, int, double, double)
    C_to_Romeo_fun2(add, double, int, double)
    C_to_Romeo_fun2(sub, double, int, double)
    C_to_Romeo_fun2(mul, double, int, double)
    C_to_Romeo_fun2(div, double, int, double)
    C_to_Romeo_fun2(eq, int, int, bool)
    C_to_Romeo_fun2(neq, int, int, bool)
    C_to_Romeo_fun2(lt, int, int, bool)
    C_to_Romeo_fun2(gt, int, int, bool)
    C_to_Romeo_fun2(le, int, int, bool)
    C_to_Romeo_fun2(ge, int, int, bool)
    C_to_Romeo_fun2(neq, double, double, bool)
    C_to_Romeo_fun2(lt, double, double, bool)
    C_to_Romeo_fun2(gt, double, double, bool)
    C_to_Romeo_fun2(le, double, double, bool)
    C_to_Romeo_fun2(ge, double, double, bool)
    C_to_Romeo_unary_fun(negate, int)
    C_to_Romeo_unary_fun(negate, double)
    C_to_Romeo_unary_fun(not, int)
    C_to_Romeo_unary_fun(not, bool)
}



