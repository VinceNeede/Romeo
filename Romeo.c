#include "scanner.h"
#include "parser.h"
#include "interpreter.h"
#include "types.h"

char* readFile(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file %s\n", path);
        exit(74);
    }

    // Move the file pointer to the end of the file
    fseek(file, 0, SEEK_END);
    // Get the current position of the file pointer (which is the size of the file)
    size_t length = (size_t)ftell(file);
    // Move the file pointer back to the beginning of the file
    fseek(file, 0, SEEK_SET);

    // Allocate memory to hold the entire file content
    char *content = (char *)malloc(length + 1);
    if (content == NULL) {
        fprintf(stderr, "Not enough memory to read file %s\n", path);
        exit(74);
    }

    // Read the entire file into the allocated memory
    fread(content, 1, length, file);
    // Null-terminate the string
    content[length] = '\0';

    // Close the file
    fclose(file);

    return content;
}

void run (char* source, Scanner *scanner, Parser *parser){
    List_Stmt *stmts;
    set_source(scanner, source);
    scanTokens(scanner);
    stmts = parse(parser);
    Interpret(stmts);
    stmts->free_lit_from = NULL;
    freeList_Stmt(stmts);
}

void runFile(char *path, Scanner *scanner, Parser *parser){
    char *source = readFile(path);
    run(source, scanner, parser);
    free(source);
}

void runPrompt(Scanner *scanner, Parser *parser){
    char *line = NULL;
    size_t len = 0;
    size_t read;
    printf("Welcome to Romeo Programming Language\n");
    while(1){
        printf("> ");
        read = getline(&line, &len, stdin);
        if (read == (size_t)-1) break;
        if (strcmp(line, "exit\n")==0) break;
        run(line, scanner, parser);
        scanner->line++;
    }
    free(line);
}

int main(int argc, char *argv[]){
    if (argc>2){
        printf("Usage: %s [source_file]\n", argv[0]);
        return 1;
    }
    Scanner* scanner;
    Parser* parser;
    
    scanner = newScanner();
    parser = newParser(scanner->tokens);
    types_init();
    interpreter_init();

    if(argc==2) runFile(argv[1], scanner, parser);
    else runPrompt(scanner, parser);

    freeParser(parser);
    freeInterpreter();
    freeScanner(scanner);
    free_types();

    return 0;
}