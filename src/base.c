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

#define  C_to_Romeo_fun(fun_name, ret_type) \
    args_types = newList_string(); \
    add_string(args_types, strdup(#ret_type)); \
    add_string(args_types, strdup(#ret_type)); \
    key = (key_field*)malloc(sizeof(key_field)); \
    key->type = FUNCTION; \
    key->field.function.name = strdup(#fun_name); \
    key->field.function.args_types = args_types; \
    key->field.function.non_optional_args = args_types->size; \
    Rfunction = newCallable(NULL, NULL, #ret_type); \
    Rfunction->function = fun_name##_##ret_type##_##ret_type; \
    addHT_var(r##ret_type->look_up_table, newRvariable("function", key, Rfunction),0);  

#define  C_to_Romeo_fun2(fun_name, type1, type2, ret_type) \
    args_types = newList_string(); \
    add_string(args_types, strdup(#type1)); \
    add_string(args_types, strdup(#type2)); \
    key = (key_field*)malloc(sizeof(key_field)); \
    key->type = FUNCTION; \
    key->field.function.name = strdup(#fun_name); \
    key->field.function.args_types = args_types; \
    key->field.function.non_optional_args = args_types->size; \
    Rfunction = newCallable(NULL, NULL, #ret_type); \
    Rfunction->function = fun_name##_##type1##_##type2; \
    addHT_var(r##type1->look_up_table, newRvariable("function", key, Rfunction),0); \
    args_types = newList_string(); \
    add_string(args_types, strdup(#type2)); \
    add_string(args_types, strdup(#type1)); \
    key = (key_field*)malloc(sizeof(key_field)); \
    key->type = FUNCTION; \
    key->field.function.name = strdup(#fun_name); \
    key->field.function.args_types = args_types; \
    key->field.function.non_optional_args = args_types->size; \
    Rfunction = newCallable(NULL, NULL, #ret_type); \
    Rfunction->function = fun_name##_##type2##_##type1; \
    addHT_var(r##type2->look_up_table, newRvariable("function", key, Rfunction),0);

void base_functions(){
    List_string *args_types;
    key_field *key;
    Callable *Rfunction;
    Rtype *rint = searchHT_Rtype(types, "int");
    Rtype *rdouble = searchHT_Rtype(types, "double");

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
}



