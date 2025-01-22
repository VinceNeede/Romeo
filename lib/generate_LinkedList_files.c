#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_help(char *name) {
    printf("Usage: %s [options] -t <type> [-I <headers to include>]\n"
              "Options:\n"
              "  -h\t\t\tPrint this help message\n"
              "  -H <header dir>\tDirectory to store header files\n"
              "  -S <src dir>\t\tDirectory to store source files\n"
              "  -s <subfix>\t\tSubfix to append to the file names\n"
              "  -t <type>\t\tType of the data to store\n"
              "  -f <free function>\tFunction to free the data, must be contained in the headers\n"
              "  -a adds a bool controller to free functions"
            //   "  -c <copy function>\tFunction to copy the data, must be contained in the headers\n"
              "  -u\t\t\tUse user defined data type, if -f is not selected, it is selected as \"free<subfix>\"\n"
            //   "     if -c is not selected, it is selected as \"copy<subfix>\"\n"
              "  -I <headers to include>\tHeaders to include in the header file\n"
              , name);
}

int usage(int argc, char *argv[], char **header_dir, char **src_dir, char **subfix, char **type,
        char** free_fun, char **copy_fun, char*** headers, int* n_headers, int* is_u_def, int *add_to_free) {
    int i=0;
    for (i=0; i<argc; i++) {
        if (argv[i][0]=='-'){
            switch (argv[i][1]){
            case 'h':
                print_help(argv[0]);
                return 1;
            case 'H':
                *header_dir = argv[i+1];
                break;
            case 'S':
                *src_dir = argv[i+1];
                break;
            case 's':
                *subfix = argv[i+1];
                break;
            case 't':
                *type = argv[i+1];
                break;
            case 'f':
                *free_fun = argv[i+1];
                break;
            case 'c':
                *copy_fun = argv[i+1];
                break;
            case 'a':
                *add_to_free = 1;
                break;
            case 'I':
                i++;
                *n_headers = argc-i;
                *headers = (char **)malloc(*n_headers*sizeof(char *));
                for (int j=0; j<*n_headers; j++) {
                    (*headers)[j] = argv[i+j];
                }
                break;
            case 'u':
                *is_u_def = 1;
                break;
            default:
                fprintf(stderr, "Unknown option: %s\nExecute %s -h for help", argv[i], argv[0]);
                return 1;
        }
        if (argv[i][1]=='I') break;
        }
    }

    return 0;
}


int main(int argc, char *argv[]) {
    int is_user_defined=0, add_free=0;
    char *header_dir, *src_dir, *subfix, *type, *free_fun, *copy_fun, **headers;
    int n_headers=0;
    header_dir = src_dir = subfix = type = free_fun = copy_fun = NULL;
    if (usage(argc, argv, &header_dir, &src_dir, &subfix, &type, &free_fun, &copy_fun, 
    &headers, &n_headers, &is_user_defined, &add_free)) return 1;

    if (header_dir == NULL && src_dir == NULL)
        src_dir=header_dir = ".";
    else if (header_dir == NULL)
        header_dir = src_dir;
    else if (src_dir == NULL)
        src_dir = header_dir;
    if (subfix == NULL) subfix = type;
    if (type == NULL) {
        fprintf(stderr, "Type must be specified\n");
        return 2;
    }

    char *path_header = (char *)malloc(strlen(header_dir) + strlen("/LinkList_") + strlen(subfix) + strlen(".h") + 1);
    char *path_c = (char *)malloc(strlen(src_dir) + strlen("/LinkList_") + strlen(subfix) + strlen(".c") + 1);

    sprintf(path_header, "%s/LinkList_%s.h", header_dir, subfix);
    sprintf(path_c, "%s/LinkList_%s.c", src_dir, subfix);

    FILE *header = fopen(path_header, "w");
    FILE *src = fopen(path_c, "w");

    fprintf(header,
            "#ifndef LINKLIST_%s_H\n"
            "#define LINKLIST_%s_H\n",
            subfix, subfix);


    fprintf(header, "#include \"forward_decl.h\"\n");
    for (int i=0; i<n_headers; i++) {
        fprintf(header, "#include \"%s\"\n", headers[i]);
    }
    fprintf(header, "typedef struct node_%s {\n"
            "    %s data;\n"
            "    struct node_%s *next;\n"
            "} Node_%s;\n"
            "\n"
            "Node_%s* newNode_%s(%s data);\n",subfix, type, subfix, subfix,
            subfix, subfix, type);

    char tmp[100];
    if (add_free){
        sprintf(tmp,"\tNode_%s* free_lit_from;\n", subfix);
    }
    else{
        tmp[0]='\0';
    }
    fprintf(header,
            "\n"
            "typedef struct List_%s {\n"
            "\tNode_%s *head;\n"
            "\tNode_%s *tail;\n"
            "\tint size;\n"
            "%s"
            "} List_%s;\n"
            "\n"
            "List_%s* newList_%s();\n"
            "void add_%s(List_%s *list, %s data);\n"
            "%s getIndex_%s(List_%s *list, int index);\n"
            "List_%s* copyList_%s(List_%s *list);\n"
            ,subfix, subfix, subfix, 
            tmp, subfix,
            subfix, subfix, subfix, subfix, type, type,
            subfix, subfix, 
            subfix, subfix, subfix);

    if (add_free)
        fprintf(header,
            "void freeNode_%s(Node_%s *node, int control);\n"
            "void freeList_%s(List_%s *list);\n", subfix, subfix, subfix, subfix);
    else
        fprintf(header,
            "void freeNode_%s(Node_%s *node);\n"
            "void freeList_%s(List_%s *list);\n", subfix, subfix, subfix, subfix);

    fprintf(src,
            "#include <stdio.h>\n"
            "#include <stdlib.h>\n"
            "#include \"LinkList_%s.h\"\n"
            "\n"
            "Node_%s* newNode_%s(%s data) {\n"
            "    Node_%s *node = (Node_%s *)malloc(sizeof(Node_%s));\n"
            "    if (node == NULL) {\n"
            "        printf(\"Memory allocation failed\\n\");\n"
            "        exit(1);\n"
            "    }\n"
            "    node->data = data;\n"
            "    node->next = NULL;\n"
            "    return node;\n"
            "}\n"
            "\n",
            subfix,
            subfix, subfix, type,
            subfix, subfix, subfix);
    if (add_free)
        fprintf(src,"void freeNode_%s(Node_%s *node, int control) {\n",subfix, subfix);
    else
        fprintf(src,"void freeNode_%s(Node_%s *node) {\n",subfix, subfix);


    if (add_free){
        sprintf(tmp, ", control");
    }
    else{
        tmp[0]='\0';
    }
    if (is_user_defined) {
        if (free_fun == NULL)
            fprintf(src,"    if(node->data!=NULL) free%s(node->data%s);\n", subfix,tmp);
        else
            fprintf(src,"    if(node->data!=NULL) %s(node->data%s);\n", free_fun,tmp);
    } else if (free_fun != NULL)
        fprintf(src,"    if(node->data!=NULL) %s(node->data%s);\n", free_fun,tmp);

    // if (is_struct) fprintf(src,"    free%s(node->data);\n", subfix);

    if (add_free)
        sprintf(tmp, "\t\tlist->free_lit_from = list->head;\n");
    else
        tmp[0]='\0';

    fprintf(src,"    free(node);\n"
            "}\n"
            "\n"
            "List_%s* newList_%s() {\n"
            "    List_%s *list = (List_%s *)malloc(sizeof(List_%s));\n"
            "    if (list == NULL) {\n"
            "        printf(\"Memory allocation failed\\n\");\n"
            "        exit(1);\n"
            "    }\n"
            "    list->head = NULL;\n"
            "    list->tail = NULL;\n"
            "    list->size = 0;\n"
            "    return list;\n"
            "}\n"
            "\n"
            "void add_%s(List_%s *list, %s data) {\n"
            "    Node_%s *node = newNode_%s(data);\n"
            "    if (list->tail == NULL) {\n"
            "        list->head = node;\n"
            "        list->tail = list->head;\n"
            "%s"
            "    } else {\n"
            "        list->tail->next = node;\n"
            "        list->tail = list->tail->next;\n"
            "    }\n"
            "    list->size++;\n"
            "}\n"
            "\n"
            "%s getIndex_%s(List_%s *list, int index) {\n"
            "    if (index < 0 || index >= list->size) {\n"
            "        printf(\"Index out of bounds\\n\");\n"
            "        exit(1);\n"
            "    }\n"
            "    Node_%s *current = list->head;\n"
            "    for (int i = 0; i < index; i++) {\n"
            "        current = current->next;\n"
            "    }\n"
            "    return current->data;\n"
            "}\n"
            "\n",            
            subfix, subfix,
            subfix, subfix, subfix,
            subfix, subfix, type,
            subfix, subfix, tmp,
            type, subfix, subfix, 
            subfix);

    if (add_free){
        fprintf(src,"void freeList_%s(List_%s *list) {\n", subfix, subfix);
        fprintf(src, "int control = 0;\n");
        sprintf(tmp, ", control");
    }else{
        fprintf(src,"void freeList_%s(List_%s *list) {\n", subfix, subfix);
        tmp[0]='\0';
    }
    fprintf(src,
            "    Node_%s *current = list->head;\n"
            "    while (current != NULL) {\n", subfix);
    if (add_free)
        fprintf(src,"        if (current == list->free_lit_from) control = 1;\n");
    fprintf(src,
            "        Node_%s *next = current->next;\n"
            "        freeNode_%s(current%s);\n"
            "        current = next;\n"
            "    }\n"
            "    free(list);\n"
            "}\n",
            subfix, 
            subfix, tmp);

    // implement copyList_<subfix>
    if (add_free)
        sprintf(tmp, "new_list->free_lit_from = new_list->head;\n");
    else
        tmp[0]='\0';
    fprintf(src,
                "List_%s* copyList_%s(List_%s *list){\n"
                "    List_%s *new_list = newList_%s();\n"
                "    Node_%s *current = list->head;\n"
                "    while (current != NULL){\n", 
                subfix, subfix, subfix, 
                subfix, subfix, subfix);
    if (is_user_defined && copy_fun == NULL)
            fprintf(src,"        add_%s(new_list, copy%s(current->data));\n", subfix, subfix);
    else if (copy_fun != NULL)
            fprintf(src,"        add_%s(new_list, %s(current->data));\n", subfix, copy_fun);
    else
            fprintf(src,"        add_%s(new_list, current->data);\n", subfix);
    fprintf(src,
                "        current = current->next;\n"
                "    }\n"
                "%s"
                "    return new_list;\n"
                "}\n",tmp);


    fprintf(header, "#endif // LINKLIST_%s_H\n", subfix);
    fclose(header);
    fclose(src);
    free(path_header);
    free(path_c);
    if (n_headers > 0) free(headers);
    return 0;
}
