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
              "  -t <type>\t\tType of the data\n"
              "  -kt <key type>\t\tType of the key\n"
              "  -kf <key field>\t\tField of the key in case type is a struct\n"
              "  -u\t\t\tUse user defined data type, if -f is not selected, it is selected as \"free<subfix>\"\n"
              "  -I <headers to include>\tHeaders to include in the header file. LinkList_subfix is imported by default\n"
              , name);
}

int usage(int argc, char *argv[], char **header_dir, char **src_dir, char **subfix, char **type,
			char **key_type, char** key_field, char*** headers, int* n_headers, int* is_u_def) {
	if (argc == 1) print_help(argv[0]);
    int i=0;
    for (i=1; i<argc; i++) {
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
            case 'k':
				if (strcmp(argv[i]+1, "kt")==0) {
					*key_type = argv[i+1];
				} else if (strcmp(argv[i]+1, "kf")==0) {
					*key_field = argv[i+1];
				}
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
                fprintf(stderr, "Unknown option: %s\nExecute %s -h for help\n", argv[i], argv[0]);
                return 1;
        }
        if (argv[i-1][1]=='I') break;
        }
    }

    return 0;
}


int main(int argc, char *argv[]) {
    int is_user_defined=0;
    char *header_dir, *src_dir, *subfix, *type, *key_type, *key_field, **headers;
    int n_headers=0;
    header_dir = src_dir = subfix = type = key_type = key_field = NULL;
	headers = NULL;
    if (usage(argc, argv, &header_dir, &src_dir, &subfix, &type, &key_type, &key_field,
	 &headers, &n_headers, &is_user_defined)) return 1;

    if (header_dir == NULL && src_dir == NULL)
        src_dir=header_dir = ".";
    else if (header_dir == NULL)
        header_dir = src_dir;
    else if (src_dir == NULL)
        src_dir = header_dir;
    if (subfix == NULL) subfix = type;
    if (type == NULL) {
        fprintf(stderr, "Type, key_type and key_field must be specified\n");
        return 2;
    }
	if (key_type == NULL) key_type = type;
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
            "typedef struct {\n"
            "\t%s *items;\n"
            "\tList_%s **overflow_buckets;\n"
            "\tint size;\n"
            "\tint count;\n"
            "\tsize_t (*hash)(const %s);\n"
            "\tint (*cmp)(const %s, const %s);\n"
            "\tvoid (*update)(%s, const %s);\n"
            "\t void (*free_item)(%s);\n"
            "}HT_%s;\n",
            type,
            subfix,
            key_type,
            key_type,key_type,
            type,type,
            type,
            subfix);
    
    // define HT_subfix_init
    fprintf(header,
            "HT_%s* newHT_%s(int size, size_t (*hash)(const %s), int (*cmp)(const %s, const %s), void (*update)(%s, const %s), void (*free_item)(%s));\n",
            subfix, subfix, key_type, type, type, type, type, type);
    // define HT_subfix_free
    fprintf(header,
            "void freeHT_%s(HT_%s *ht);\n",
            subfix, subfix);
    // define HT_subfix_insert
    fprintf(header,
            "void addHT_%s(HT_%s *ht, %s item);\n",
            subfix, subfix, type);
    // define HT_subfix_search
    fprintf(header,
            "%s searchHT_%s(HT_%s *ht, %s key);\n",
            type, subfix, subfix, key_type);
    // define HT_subfix_delete
    fprintf(header,
            "void rmHT_%s(HT_%s *ht, %s key);\n",
            subfix, subfix, key_type);
    fprintf(header, "#endif\t//HT_%s_H\n", subfix);
    fclose(header);

    fprintf(src,"#include \"HT_%s.h\"\n",subfix);

    // define HT_subfix_init
    fprintf(src,
            "HT_%s* newHT_%s(int size, size_t (*hash)(const %s), int (*cmp)(const %s, const %s), void (*update)(%s, const %s), void (*free_item)(%s)) {\n"
            "\tHT_%s *ht = (HT_%s*)malloc(sizeof(HT_%s));\n"
            "\tht->items = (%s*)malloc(size * sizeof(%s));\n"
            "\tht->overflow_buckets = (List_%s**)malloc(size * sizeof(List_%s*));\n"
            "\tfor (int i = 0; i < size; i++) {\n",
            subfix, subfix, key_type, type, type, type, type, type,
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
            "\tht->update = update;\n"
            "\tht->free_item = free_item;\n"
            "\treturn ht;\n"
            "}\n",
            subfix);

    // define HT_subfix_free
    fprintf(src,
            "void freeHT_%s(HT_%s *ht) {\n"
            "\tfor (int i = 0; i < ht->size; i++) {\n"
            "\t\tht->free_item(ht->items[i]);\n"
            "\t\tfreeList_%s(ht->overflow_buckets[i]);\n"
            "\t}\n"
            "\tfree(ht->items);\n"
            "\tfree(ht->overflow_buckets);\n"
            "\tfree(ht);\n"
            "}\n",
            subfix, subfix, subfix);



	char tmp[100];
	if (key_field == NULL)
		sprintf(tmp,"ht -> cmp(current->data,item)");
	else
		sprintf(tmp,"ht -> cmp(current->data->%s,item->%s)",key_field,key_field);
	// define addToBucket_subfix

	fprintf(src, 	"void addToBucket_%s(HT_%s *ht, size_t index, %s item) {\n"
					"\tNode_%s *current = ht->overflow_buckets[index]->head;\n"
					"\twhile(current != NULL && ! %s) {\n"
					"\t\tcurrent = current -> next;\n"
					"\t}\n"
					"\tif(current == NULL) current = newNode_%s(item);\n"
					"\telse if (ht->update != NULL) %s;\n"
					"}\n",
			subfix, subfix, type, 
			subfix,
			tmp,
			subfix, tmp);


    // define HT_subfix_insert
    fprintf(src,"void addHT_%s(HT_%s *ht, %s item) {\n", subfix, subfix, type);
	if (key_field != NULL){
	    fprintf(src,"\tsize_t index = ht->hash(item -> %s) %% ht->size;\n",key_field);
		sprintf(tmp,"ht -> cmp(ht->items[index]->%s,item->%s)",key_field,key_field);
	}else{
		fprintf(src,"\tsize_t index = ht->hash(item) %% ht->size;\n");
		sprintf(tmp,"ht -> cmp(ht->items[index],item)");
	}
    fprintf(src,"\tif (ht->items[index] == NULL) {\n"
                "\t\tht->items[index] = item;\n"
                "\t\tht->count++;\n"
                "\t} else if (%s) {\n"
                "\t\tif(ht->update!=NULL) ht->update(ht->items[index], item);\n"
                "\t\treturn;\n"
                "\t} else addToBucket_%s(ht, index, item);\n"
                "}\n",
            tmp, subfix);

	// define HT_subfix_search
    fprintf(src,
            "%s searchHT_%s(HT_%s *ht, %s key) {\n"
            "\tsize_t index = ht->hash(key) %% ht->size;\n"
            "\tif (ht->items[index] == NULL) return NULL;\n",type, subfix, subfix, key_type);
	
	if (key_field != NULL){
        fprintf(src,"\tif (ht->cmp(ht->items[index]->%s, key)) {\n",key_field);
		sprintf(tmp,"ht -> cmp(current->data->%s,key)",key_field);
	}else{
		fprintf(src,"\tif (ht->cmp(ht->items[index], key)) {\n");
		sprintf(tmp,"ht -> cmp(current->data,key)");
	}
	fprintf(src,
            "\t\treturn ht->items[index];\n"
            "\t} else {\n"
			"\t\tNode_%s *current = ht->overflow_buckets[index]->head;\n"
			"\t\twhile(current != NULL && !%s)\n"
			"\t\t\tcurrent = current -> next;\n"
			"\t\tif(current != NULL) return current -> data;\n"
			"\t}\n"
			"\treturn NULL;\n"
            "}\n",
            subfix, tmp);
        
    // define HT_subfix_delete
    fprintf(src,
            "void rmHT_%s(HT_%s *ht, %s key) {\n"
            "\tsize_t index = ht->hash(key) %% ht->size;\n"
            "\tif (ht->items[index] == NULL) return NULL;\n", subfix, subfix, key_type);
	
	if (key_field != NULL){
        fprintf(src,"\tif (ht->cmp(ht->items[index]->%s, key)) {\n",key_field);
		sprintf(tmp,"ht -> cmp(current->data->%s,key)",key_field);
	}else{
		fprintf(src,"\tif (ht->cmp(ht->items[index], key)) {\n");
		sprintf(tmp,"ht -> cmp(current->data,key)");
	}
	fprintf(src,
			"\t\tif (ht->free_item != NULL) ht->free_item(ht->items[index]);\n"
            "\t\tht->items[index] = NULL;\n"
            "\t} else {\n"
			"\t\tNode_%s *current = ht->overflow_buckets[index]->head;\n"
			"\t\tNode_%s *prev = NULL;\n"
			"\t\twhile(current != NULL && !%s){\n"
			"\t\t\tprev = current;\n"
			"\t\t\tcurrent = current -> next;\n"
			"\t\t}\n"
			"\t\tif(current != NULL){\n"
			"\t\t\tif(prev == NULL) ht->overflow_buckets[index]->head = current -> next;\n"
			"\t\t\telse prev -> next = current -> next;\n"
			"\t\t\tfreeNode_%s(current);\n"
			"\t\t}\n"
			"\t}\n"
            "}\n",
            subfix, subfix, tmp, subfix);
	fclose(src);
    free(path_header);
    free(path_c);
    return 0;
}