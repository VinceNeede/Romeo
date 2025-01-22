#include<stdio.h>
#include<stdlib.h>
#include<string_utilities.h>


char* get_type_AST(const char* line){
    List_string* slist = splitString(line, ':');
    char* type = trim(getIndex_string(slist, 0));
    freeList_string(slist);
    return type;
}

void defineVisitor(FILE* file, const char* baseName, const char* types[]){
    fprintf(file, "typedef struct {\n");
    while (*types){
        char* type = get_type_AST(*types);
        fprintf(file, "\tvoid* (*visit%s)(%s*);\n", type, baseName);
        free(type);
        types++;
    }
    fprintf(file, "} %sVisitor;\n\n",baseName);
}

void defineEnum(FILE* file, const char* baseName, const char* types[]){
    fprintf(file, "typedef enum {\n");
    char* upper_baseName = upper(baseName);
    char* type = NULL;
    char* upper_type = NULL;

    while (*types) {
        type = get_type_AST(*types);
        upper_type = upper(type);
        fprintf(file, "\t%s_%s,\n", upper_baseName, upper_type);
        types++;
        free(type);
        free(upper_type);
    }
    fprintf(file, "} %sType;\n\n", baseName);
    free(upper_baseName);
}

void define_baseName_struct(FILE* file, const char* baseName, const char* types[]){
    fprintf(file, "typedef struct %s {\n", baseName);
    fprintf(file, "\t%sType type;\n", baseName);
    fprintf(file, "\tunion {\n");

    List_string* slist = NULL;
    List_string* arguments = NULL;
    char *sign = NULL;
    char *type = NULL;
    char *lower_type = NULL;
    char *lower_baseName = lower(baseName);
    while (*types){
        fprintf(file, "\t\tstruct {\n");
        slist = splitString(*types, ':');
        type = trim(getIndex_string(slist, 0));
        lower_type = lower(type);
        sign = getIndex_string(slist, 1);
        arguments = splitString(sign, ',');
        for (int i = 0; i < arguments->size; i++){
            fprintf(file, "\t\t\t%s;\n", getIndex_string(arguments, i));
        }
        fprintf(file, "\t\t} %s;\n", lower_type);
        freeList_string(arguments);
        freeList_string(slist);
        free(type);
        free(lower_type);
        types++;
    }
    fprintf(file, "\t} %s;\n", lower_baseName);
    fprintf(file, "\tvoid* (*%sAccept)(%sVisitor*, %s*);\n", baseName, baseName, baseName);
    fprintf(file, "} %s;\n", baseName);
    free(lower_baseName);
}

void defineAST_header_only(const char *outputDir, const char *baseName, 
    const char *types_[], const char *includes[], const char *forwardDeclares[]){
    FILE *file;
    char *path = (char*)malloc(strlen(outputDir) + strlen(baseName) + 5);
    char *baseName_upper = upper(baseName);
    sprintf(path, "%s/%s.h", outputDir, baseName);
    file = fopen(path, "w");
    fprintf(file,   "#ifndef %s_H\n"
                    "#define %s_H\n\n", baseName_upper, baseName_upper);
    while (*includes!=NULL) fprintf(file, "#include \"%s\"\n\n",*includes++);
    while (*forwardDeclares!=NULL) fprintf(file, "%s\n",*forwardDeclares++);
    // include files
    // fprintf(file,   "#include <stdio.h>\n"
    //                 "#include \"token.h\"\n"
    //                 "#include <string.h>\n"
    //                 "#include \"string_utilities.h\"\n\n");
    fprintf(file, "typedef struct %s %s;\n\n", baseName, baseName);
    defineVisitor(file, baseName, types_);
    defineEnum(file, baseName, types_);
    define_baseName_struct(file, baseName, types_);
    fclose(file);
    free(path);
}

void define_Accept(const char* headerDir, const char* sourceDir, const char* baseName, const char* types[]){
    FILE * header, *source;
    char *headerPath = (char*)malloc(strlen(headerDir) + strlen(baseName) + 5);
    char *sourcePath = (char*)malloc(strlen(sourceDir) + strlen(baseName) + 5);
    sprintf(headerPath, "%s/%s.h", headerDir, baseName);
    sprintf(sourcePath, "%s/%s.c", sourceDir, baseName);
    header = fopen(headerPath, "a");
    source = fopen(sourcePath, "w");
    fprintf(source, "#include\"%s.h\"\n", baseName);
    fprintf(source, "#include\"LinkList_Expr.h\"\n");
    fprintf(source, "#include\"LinkList_Stmt.h\"\n");
    const char **head = types;
    char *type=NULL, *upper_type=NULL;
    char *lower_baseName = lower(baseName); char *upper_baseName = upper(baseName);
    List_char *StringBuilder = NULL;
    char* line = NULL;
    while(*types){
        StringBuilder = newList_char();
        type = get_type_AST(*types);
        concat_string_to_list(StringBuilder, "void* Accept");
        concat_string_to_list(StringBuilder, type);
        concat_string_to_list(StringBuilder, "(");
        concat_string_to_list(StringBuilder, baseName);
        concat_string_to_list(StringBuilder, "Visitor* visitor, ");
        concat_string_to_list(StringBuilder, baseName);
        concat_string_to_list(StringBuilder, "* ");
        concat_string_to_list(StringBuilder, lower_baseName);
        concat_string_to_list(StringBuilder, ")");
        line = charListToString(StringBuilder);
        fprintf(header, "%s;\n", line);
        fprintf(source, "%s{\n", line);
        free(line);
        fprintf(source, "\treturn visitor->visit%s(%s);\n}\n", type, lower_baseName);
        free(type);
        freeList_char(StringBuilder);
        types++;
    }
    types=head;
    fprintf(header, "void* %sAccept(%sVisitor* visitor, %s* %s);\n", baseName, baseName, baseName, lower_baseName);
    fprintf(source, "void* %sAccept(%sVisitor* visitor, %s* %s){\n", baseName, baseName, baseName, lower_baseName);
    fprintf(source, "\tswitch(%s->type){\n", lower_baseName);
    while(*types){
        type = get_type_AST(*types);
        upper_type = upper(type);
        fprintf(source, "\tcase %s_%s:\n", upper_baseName, upper_type);
        fprintf(source, "\t\treturn Accept%s(visitor, %s);\n", type, lower_baseName);
        free(type);
        free(upper_type);
        types++;
    }
    fprintf(source, "\t}return NULL;\n}\n");
    free(lower_baseName);
    free(upper_baseName);
    fclose(header);
    fclose(source);
    free(headerPath);
    free(sourcePath);
}
void define_constructors(const char* headerDir, const char* sourceDir, const char* baseName, const char* types[]){
    FILE * header, *source;
    char *headerPath = (char*)malloc(strlen(headerDir) + strlen(baseName) + 5);
    char *sourcePath = (char*)malloc(strlen(sourceDir) + strlen(baseName) + 5);
    sprintf(headerPath, "%s/%s.h", headerDir, baseName);
    sprintf(sourcePath, "%s/%s.c", sourceDir, baseName);
    header = fopen(headerPath, "a");
    source = fopen(sourcePath, "a");

    char *type = NULL, *upper_type = NULL, *lower_type = NULL;
    char *lower_baseName = lower(baseName); char *upper_baseName = upper(baseName);
    List_string* slist = NULL;
    char *tmp = NULL;
    List_string* arguments = NULL;
    char *argument_name=NULL;
    while(*types){
        slist = splitString(*types, ':');
        type = trim(getIndex_string(slist, 0));
        upper_type = upper(type);
        lower_type = lower(type);
        fprintf(header, "%s* new%s%s(%s);\n", baseName, type, baseName,getIndex_string(slist,1));
        fprintf(source, "%s* new%s%s(%s){\n", baseName, type, baseName,getIndex_string(slist,1));

        fprintf(source, "\t%s* %s = (%s*)malloc(sizeof(%s));\n", baseName, lower_baseName, baseName, baseName);
        fprintf(source, "\tif (%s == NULL) {\n", lower_baseName);
        fprintf(source, "\t\tfprintf(stderr, \"Memory allocation failed\\n\");\n");
        fprintf(source, "\t\texit(1);\n");
        fprintf(source, "\t}\n");
        fprintf(source, "\t%s->type = %s_%s;\n", lower_baseName, upper_baseName, upper_type);

        arguments = splitString(getIndex_string(slist,1), ',');
        freeList_string(slist);
        for (int i = 0; i < arguments->size; i++){
            tmp = trim(getIndex_string(arguments, i));
            slist = splitString(tmp, ' ');
            argument_name = getIndex_string(slist, 1);
            fprintf(source, "\t%s->%s.%s.%s = %s;\n", lower_baseName, lower_baseName, lower_type, argument_name,argument_name);
            freeList_string(slist);
            free(tmp);
        }

        fprintf(source,"\treturn %s;\n}\n",lower_baseName);
        freeList_string(arguments);
        free(type);
        free(upper_type);
        free(lower_type);
        types++;
    }


    fclose(header);
    fclose(source);
    free(lower_baseName);
    free(upper_baseName);
    free(headerPath);
    free(sourcePath);
}

void define_destructor(const char* headerDir, const char* sourceDir, const char* baseName, const char* types[]){
    FILE * header, *source;
    char *headerPath = (char*)malloc(strlen(headerDir) + strlen(baseName) + 5);
    char *sourcePath = (char*)malloc(strlen(sourceDir) + strlen(baseName) + 5);
    sprintf(headerPath, "%s/%s.h", headerDir, baseName);
    sprintf(sourcePath, "%s/%s.c", sourceDir, baseName);
    header = fopen(headerPath, "a");
    source = fopen(sourcePath, "a");

    char *type = NULL, *upper_type = NULL, *lower_type = NULL;
    char *lower_baseName = lower(baseName); char *upper_baseName = upper(baseName);
    List_string* slist = NULL;
    char *tmp = NULL;
    List_string* arguments = NULL;
    char *argument_name=NULL, *argument_type=NULL;


    fprintf(header, "void free%s(%s*,int free_literal);\n", baseName, baseName);
    fprintf(source, "void free%s(%s* %s,int free_literal){\n", baseName, baseName, lower_baseName);
    fprintf(source, "\tif (%s == NULL) return;\n", lower_baseName);
    fprintf(source, "\tswitch(%s->type){\n", lower_baseName);
    while(*types){
        slist = splitString(*types, ':');
        type = trim(getIndex_string(slist, 0));
        upper_type = upper(type);
        lower_type = lower(type);
        fprintf(source, "\tcase %s_%s:\n", upper_baseName, upper_type);
        arguments = splitString(getIndex_string(slist,1), ',');
        freeList_string(slist);
        for (int i = 0; i < arguments->size; i++){
            tmp = trim(getIndex_string(arguments, i));
            slist = splitString(tmp, ' ');
            argument_type = getIndex_string(slist, 0);
            argument_name = getIndex_string(slist, 1);
            free(tmp);
            tmp = replace(argument_type, '*','\0');

            if (strcmp(tmp, "List_Token") != 0)              // Is this rule general?
                if (argument_type[strlen(argument_type)-1] == '*'){
                    if (strcmp(tmp,"Literal")==0)
                        fprintf(source,"\t\tif (free_literal) freeLiteral(%s -> %s.%s.%s,1);\n",lower_baseName, lower_baseName, lower_type, argument_name);
                    else if (strcmp(tmp,"Expr")==0)
                        fprintf(source,"\t\tfree%s (%s -> %s.%s.%s, free_literal);\n",tmp, lower_baseName, lower_baseName, lower_type, argument_name);
                    else if (strcmp(tmp,"Token")==0){
                        fprintf(source,"\t\tif (free_literal) freeLiteral(%s -> %s.%s.%s->literal,1);\n",lower_baseName, lower_baseName, lower_type, argument_name);
                        fprintf(source,"\t\tfree%s (%s -> %s.%s.%s);\n",tmp, lower_baseName, lower_baseName, lower_type, argument_name);
                    }
                    else if (strstr(tmp,"List_")!=NULL){
                        fprintf(source,"\t\tif (!free_literal) %s -> %s.%s.%s->free_lit_from = NULL;\n",lower_baseName, lower_baseName, lower_type, argument_name);
                        fprintf(source,"\t\tfree%s (%s -> %s.%s.%s);\n",tmp, lower_baseName, lower_baseName, lower_type, argument_name);
                    }
                    else
                        fprintf(source,"\t\tfree%s (%s -> %s.%s.%s);\n",tmp, lower_baseName, lower_baseName, lower_type, argument_name);
                }
            free(tmp);
            freeList_string(slist);
        }
        fprintf(source, "\t\tbreak;\n");
        freeList_string(arguments);
        free(type);
        free(upper_type);
        free(lower_type);
        types++;
    }
    fprintf(source, "\t}\n");   // end switch
    fprintf(source, "\tfree(%s);\n}\n", lower_baseName);    //free the struct
    free(lower_baseName);
    free(upper_baseName);
    fclose(header);
    fclose(source);
    free(headerPath);
    free(sourcePath);
}

void define_copy(const char* headerDir, const char* sourceDir, const char* baseName, const char* types[]){
    FILE * header, *source;
    char *headerPath = (char*)malloc(strlen(headerDir) + strlen(baseName) + 5);
    char *sourcePath = (char*)malloc(strlen(sourceDir) + strlen(baseName) + 5);
    sprintf(headerPath, "%s/%s.h", headerDir, baseName);
    sprintf(sourcePath, "%s/%s.c", sourceDir, baseName);
    header = fopen(headerPath, "a");
    source = fopen(sourcePath, "a");

    char *type = NULL, *upper_type = NULL, *lower_type = NULL;
    char *lower_baseName = lower(baseName); char *upper_baseName = upper(baseName);
    List_string* slist = NULL;
    char *tmp = NULL;
    List_string* arguments = NULL;
    char *argument_name=NULL, *argument_type=NULL;

    fprintf(header, "%s* copy%s(%s*);\n", baseName, baseName, baseName);
    fprintf(source, "%s* copy%s(%s* %s){\n", baseName, baseName, baseName, lower_baseName);
    fprintf(source, "\tif (%s == NULL) return NULL;\n", lower_baseName);
    fprintf(source, "\t%s* copy;\n", baseName);
    fprintf(source, "\tswitch(%s->type){\n", lower_baseName);
    while(*types){
        slist = splitString(*types, ':');
        type = trim(getIndex_string(slist, 0));
        upper_type = upper(type);
        lower_type = lower(type);
        fprintf(source, "\tcase %s_%s:\n", upper_baseName, upper_type);
        arguments = splitString(getIndex_string(slist,1), ',');
        freeList_string(slist);
        fprintf(source, "\t\tcopy = new%s%s(\n", type, baseName);
        for (int i = 0; i < arguments->size; i++){
            tmp = trim(getIndex_string(arguments, i));
            slist = splitString(tmp, ' ');
            argument_type = getIndex_string(slist, 0);
            argument_name = getIndex_string(slist, 1);
            free(tmp);
            tmp = replace(argument_type, '*','\0');
            if (i < arguments->size - 1) {
                fprintf(source,"\t\t\tcopy%s (%s -> %s.%s.%s),\n",tmp, lower_baseName, lower_baseName, lower_type, argument_name);
            } else {
                fprintf(source,"\t\t\tcopy%s (%s -> %s.%s.%s)\n",tmp, lower_baseName, lower_baseName, lower_type, argument_name);
            }            
            free(tmp);
            freeList_string(slist);
        }
        fprintf(source, "\t\t);\n");
        fprintf(source, "\t\tbreak;\n");
        freeList_string(arguments);
        free(type);
        free(upper_type);
        free(lower_type);
        types++;
    }
    fprintf(source, "\tdefault: fprintf(stderr, \"not valid type\");\n");
    fprintf(source, "\t}\n");   // end switch
    fprintf(source, "\treturn copy;\n}\n");
    fprintf(header, "#endif // %s_H\n", upper_baseName);
    free(lower_baseName);
    free(upper_baseName);
    fclose(header);
    fclose(source);
    free(headerPath);
    free(sourcePath);

}
void defineAST(const char *includeDir,const char *srcDir, const char *baseName, 
    const char *types[], const char *includes[], const char *forwardDeclares[]){
    defineAST_header_only(includeDir, baseName, types, includes, forwardDeclares);
    define_Accept(includeDir, srcDir, baseName, types);
    define_constructors(includeDir, srcDir, baseName, types);
    define_destructor(includeDir, srcDir, baseName, types);
    define_copy(includeDir, srcDir, baseName, types);
    // defineAST_source(srcDir, baseName, types);
}
int main(int argc, char **argv){
    if (argc != 3){
        fprintf(stderr, "Usage: generateAST <include_dir> <output_dir>\n");
        return 1;
    }
    char *includeDir = argv[1];
    char *srcDir = argv[2];

    const char *ExprNames[] = {
        "Assign   : Token* name, Expr* value",
        "Call     : Expr* callee, List_Expr* arguments",
        "Grouping : Expr* expression",
        "Literal  : Literal* value",
        "Variable : Token* name",
        "Ret      : Expr* value",
        "Reference: Token* name",
        NULL
    };
    const char *ExprIncludes[] = {"token.h", NULL};
    const char *ExprForwardDeclares[] = {"typedef struct List_Expr List_Expr;",
                                        "typedef struct node_Expr Node_Expr;",
                                        "void freeList_Expr(List_Expr *list);",
                                        "List_Expr* copyList_Expr(List_Expr* expr);",
                                        NULL};

    defineAST(includeDir, srcDir, "Expr", ExprNames, ExprIncludes, ExprForwardDeclares);

    const char *StmtNames[] = {
        "Block      : List_Stmt* statements",
        "Expression : Expr* expression",
        "Print      : Expr* expression",
        "Var        : Token* type, Token* name, Expr* initializer",
        NULL
    };
    const char *StmtIncludes[] = {"Expr.h","LinkList_Token.h", NULL};
    const char *StmtForwardDeclares[] = {
                                        "typedef struct List_Stmt List_Stmt;",
                                        "typedef struct node_Stmt Node_Stmt;",
                                        "void freeList_Stmt(List_Stmt *);",
                                        "List_Stmt* copyList_Stmt(List_Stmt* stmt);",
                                        NULL};
    defineAST(includeDir, srcDir, "Stmt", StmtNames, StmtIncludes, StmtForwardDeclares);

    return 0;
}