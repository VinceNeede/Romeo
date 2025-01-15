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
              "  -u\t\t\tUse user defined data type, if -f is not selected, it is selected as \"free<subfix>\"\n"
              "  -I <headers to include>\tHeaders to include in the header file\n"
              , name);
}

int usage(int argc, char *argv[], char **header_dir, char **src_dir, char **subfix, char **type,
        char** free_fun, char*** headers, int* n_headers, int* is_u_def) {
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
    int is_user_defined=0;
    char *header_dir, *src_dir, *subfix, *type, *free_fun, **headers;
    int n_headers=0;
    header_dir = src_dir = subfix = type = free_fun = NULL;
    if (usage(argc, argv, &header_dir, &src_dir, &subfix, &type, &free_fun, &headers, &n_headers, &is_user_defined)) return 1;

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

    for (int i=0; i<n_headers; i++) {
        fprintf(header, "#include \"%s\"\n", headers[i]);
    }

    fprintf(header, "typedef struct node_%s {\n"
            "    %s data;\n"
            "    struct node_%s *next;\n"
            "} Node_%s;\n"
            "\n"
            "Node_%s* newNode_%s(%s data);\n"
            "void freeNode_%s(Node_%s *node);\n"
            "\n"
            "typedef struct List_%s {\n"
            "    Node_%s *head;\n"
            "    int size;\n"
            "} List_%s;\n"
            "\n"
            "List_%s* newList_%s();\n"
            "void add_%s(List_%s *list, %s data);\n"
            "%s getIndex_%s(List_%s *list, int index);\n"
            "void freeList_%s(List_%s *list);\n"
            "#endif // LINKLIST_%s_H\n",
            subfix, type, subfix, subfix,
            subfix, subfix, type, subfix, subfix,
            subfix, subfix, subfix,
            subfix, subfix, subfix, subfix, type, type, subfix, subfix, subfix, subfix, subfix);

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
            "\n"
            "void freeNode_%s(Node_%s *node) {\n",
            subfix,
            subfix, subfix, type,
            subfix, subfix, subfix,
            subfix, subfix);
    if (is_user_defined) {
        if (free_fun == NULL)
            fprintf(src,"    free%s(node->data);\n", subfix);
        else
            fprintf(src,"    %s(node->data);\n", free_fun);
    } else if (free_fun != NULL)
        fprintf(src,"    %s(node->data);\n", free_fun);

    // if (is_struct) fprintf(src,"    free%s(node->data);\n", subfix);
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
            "    list->size = 0;\n"
            "    return list;\n"
            "}\n"
            "\n"
            "void add_%s(List_%s *list, %s data) {\n"
            "    Node_%s *node = newNode_%s(data);\n"
            "    if (list->head == NULL) {\n"
            "        list->head = node;\n"
            "    } else {\n"
            "        Node_%s *current = list->head;\n"
            "        while (current->next != NULL) {\n"
            "            current = current->next;\n"
            "        }\n"
            "        current->next = node;\n"
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
            "\n"
            "void freeList_%s(List_%s *list) {\n"
            "    Node_%s *current = list->head;\n"
            "    while (current != NULL) {\n"
            "        Node_%s *next = current->next;\n"
            "        freeNode_%s(current);\n"
            "        current = next;\n"
            "    }\n"
            "    free(list);\n"
            "}\n",
            subfix, subfix,
            subfix, subfix, subfix,
            subfix, subfix, type,
            subfix, subfix, subfix, type, 
            subfix, subfix, subfix, subfix, subfix, subfix, subfix, subfix
            );

    fclose(header);
    fclose(src);
    free(path_header);
    free(path_c);
    if (n_headers > 0) free(headers);
    return 0;
}
