#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage(char *name) {
    printf("Usage: %s <header dir> <src dir> <subfix> [struct] <type>\n", name);
}

int is_pointer(char *type) {
    return type[strlen(type) - 1] == '*';
}

int main(int argc, char *argv[]) {
    char *header_dir, *src_dir, *subfix, *type;
    int free_type = 0;
    switch (argc)
    {
    case (5):
        header_dir = argv[1];
        src_dir = argv[2];
        subfix = argv[3];
        type = argv[4];
        if (!is_pointer(type)) {
            fprintf(stderr, "Type must be either a struct or a pointer\n");
            return 2;
        }
        break;  
    case (6):
        if (strcmp(argv[4], "struct") != 0) {
            usage(argv[0]);
            return 1;
        }
        header_dir = argv[1];
        src_dir = argv[2];
        subfix = argv[3];
        type = malloc(strlen(argv[5]) + 2);
        free_type = 1;
        sprintf(type, "%s*", argv[5]);
        break;
    default:
        usage(argv[0]);
        return 1;    
    }
    char *path_header = (char *)malloc(strlen(header_dir) + strlen("/HT_") + strlen(subfix) + strlen(".h") + 1);
    char *path_c = (char *)malloc(strlen(src_dir) + strlen("/HT_") + strlen(subfix) + strlen(".c") + 1);

    sprintf(path_header, "%s/HT_%s.h", header_dir, subfix);
    sprintf(path_c, "%s/HT_%s.c", src_dir, subfix);

    FILE *header = fopen(path_header, "w");
    FILE *src = fopen(path_c, "w");

    fprintf(header,
            "#ifndef HT_%s_H\n"
            "#define HT_%s_H\n"
            "#include \"LinkList_%s.h\"\n"
            "#include<stdio.h>\n"
            "#include<stdlib.h>\n",
            subfix, subfix, subfix);
    
    // define struct HT_subfix
    fprintf(header,
            "typedef struct HT_%s{\n"
            "\t%s *items;\n"
            "\tList_%s **overflow_buckets;\n"
            "\tint size;\n"
            "\tint count;\n"
            "\tsize_t (*hash)(const %s);\n"
            "\tint (*cmp)(const %s, const %s);\n"
            "\t void (*free_item)(%s);\n"
            "}HT_%s;\n",
            subfix, 
            type,
            subfix,
            type,
            type,type,
            type,
            subfix);
    
    // define HT_subfix_init
    fprintf(header,
            "HT_%s* HT_%s_init(int size, size_t (*hash)(const %s), int (*cmp)(const %s, const %s), void (*free_item)(%s));\n",
            subfix, subfix, type, type, type, type);
    // define HT_subfix_free
    fprintf(header,
            "void HT_%s_free(HT_%s *ht);\n",
            subfix, subfix);
    // define HT_subfix_insert
    fprintf(header,
            "void HT_%s_insert(HT_%s *ht, %s item);\n",
            subfix, subfix, type);
    // define HT_subfix_search
    fprintf(header,
            "%s HT_%s_search(HT_%s *ht, %s item);\n",
            type, subfix, subfix, type);
    // define HT_subfix_delete
    fprintf(header,
            "void HT_%s_delete(HT_%s *ht, %s item);\n",
            subfix, subfix, type);
    fprintf(header, "#endif\t//HT_%s_H\n", subfix);
    fclose(header);

    fprintf(src,"#include \"HT_%s.h\"\n",subfix);

    // define HT_subfix_init
    fprintf(src,
            "HT_%s* HT_%s_init(int size, size_t (*hash)(const %s), int (*cmp)(const %s, const %s), void (*free_item)(%s)) {\n"
            "\tHT_%s *ht = (HT_%s*)malloc(sizeof(HT_%s));\n"
            "\tht->items = (%s*)malloc(size * sizeof(%s));\n"
            "\tht->overflow_buckets = (List_%s**)malloc(size * sizeof(List_%s*));\n"
            "\tfor (int i = 0; i < size; i++) {\n",
            subfix, subfix, type, type, type, type,
            subfix, subfix, subfix,
            type, type,
            subfix, subfix);
    fprintf(src,"\t\tht->items[i] = NULL;\n");
    fprintf(src,
            "\t\tht->overflow_buckets[i] = newList_%s();\n"
            "\t}\n"
            "\tht->size = size;\n"
            "\tht->count = 0;\n"
            "\tht->hash = hash;\n"
            "\tht->cmp = cmp;\n"
            "\tht->free_item = free_item;\n"
            "\treturn ht;\n"
            "}\n",
            subfix);

    // define HT_subfix_free
    fprintf(src,
            "void HT_%s_free(HT_%s *ht) {\n"
            "\tfor (int i = 0; i < ht->size; i++) {\n"
            "\t\tht->free_item(ht->items[i]);\n"
            "\t\tfreeList_%s(ht->overflow_buckets[i]);\n"
            "\t}\n"
            "\tfree(ht->items);\n"
            "\tfree(ht->overflow_buckets);\n"
            "\tfree(ht);\n"
            "}\n",
            subfix, subfix, subfix);

    // define HT_subfix_insert
    fprintf(src,
            "void HT_%s_insert(HT_%s *ht, %s item) {\n"
            "\tsize_t index = ht->hash(item) %% ht->size;\n",
            subfix, subfix, type);
    
    fprintf(src,"\tif (ht->items[index] == NULL) {\n"
                "\t\tht->items[index] = item;\n"
                "\t\tht->count++;\n"
                "\t} else if (ht->cmp(ht->items[index], item)) {\n"
                "\t\tht->free_item(ht->items[index]);\n"
                "\t\tht->items[index] = item;\n"     // for now update it, later we will separate statitc and dynamic
                "\t\treturn;\n"
                "\t} else {\n"
                "\t\tadd_%s(ht->overflow_buckets[index], item);\n"
                "\t}\n"
                "}\n",
            subfix);

        // define HT_subfix_search
    fprintf(src,
            "%s HT_%s_search(HT_%s *ht, %s item) {\n"
            "\tsize_t index = ht->hash(item) %% ht->size;\n"
            "\tif (ht->items[index] == NULL) return NULL;\n"
            "\tif (ht->cmp(ht->items[index], item)) {\n"
            "\t\treturn ht->items[index];\n"
            "\t} else {\n"
            "\t\tfor (int i=0; i<ht->overflow_buckets[index]->size; i++) {\n"
            "\t\t\t%s bucket_item = getIndex_%s(ht->overflow_buckets[index],i);\n"
            "\t\t\tif (ht->cmp(bucket_item, item)) {\n"
            "\t\t\t\treturn bucket_item;\n"
            "\t\t\t}\n"
            "\t\t}\n"
            "\t}\n"
            "\treturn NULL;\n"
            "}\n",
            type, subfix, subfix, type,
            type, subfix);
        
        // define HT_subfix_delete
    fprintf(src,
            "void HT_%s_delete(HT_%s *ht, %s item) {\n"
            "\tsize_t index = ht->hash(item) %% ht->size;\n"
            "\tif (ht->items[index] == NULL) return;\n"
            "\tif (ht->cmp(ht->items[index], item)) {\n"
            "\t\tht->free_item(ht->items[index]);\n"
            "\t\tht->items[index] = NULL;\n"
            "\t} else {\n"
            "\t\tfor (int i=0; i<ht->overflow_buckets[index]->size; i++) {\n"
            "\t\t\t%s bucket_item = getIndex_%s(ht->overflow_buckets[index],i);\n"
            "\t\t\tif (ht->cmp(bucket_item, item)) {\n"
            "\t\t\t\tfreeNode_%s(ht->overflow_buckets[index]->head);\n"
            "\t\t\t\treturn;\n"
            "\t\t\t}\n"
            "\t\t}\n"
            "\t}\n"
            "}\n",
            subfix, subfix, type,
            type,
            subfix,subfix);
    fclose(src);
    free(path_header);
    free(path_c);
    if (free_type) free(type);
    return 0;
}