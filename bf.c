#include<stdio.h>
#include<stdlib.h>

#define data_array_size 30000
unsigned char data_array[data_array_size] = {0};
unsigned int data_ptr = 0;
unsigned int inst_ptr = 0;

#define b_stack_size 2048
unsigned int b_stack[b_stack_size]       = {0};
unsigned int b_stack_ptr                 = 0;
unsigned int b_left_store[b_stack_size]  = {0};
unsigned int b_right_store[b_stack_size] = {0};

unsigned char* program;

#define symbol_count 9
const char symbols[symbol_count] = "+-><.,[]#";


int err(int error, char* msg) {
    fprintf(stderr, "Error: %s\n", msg);
    return error;
}

unsigned char codeofsym(char symbol) {
    for (unsigned char code = 1; code <= symbol_count; code += 1)
        if (symbol == symbols[code - 1])
            return code;
    return 0;
}

unsigned int b_push(unsigned int data) {
    b_stack[b_stack_ptr] = data;
    b_stack_ptr += 1;
    return b_stack_ptr - 1;
}

unsigned int b_pop() {
    b_stack_ptr -= 1;
    unsigned int tmp = b_stack[b_stack_ptr];
    b_stack[b_stack_ptr] = 0;
    return tmp;
}

unsigned int b_match_lr(unsigned int b_left) {
    for(unsigned int i = 0; i < b_stack_size; i += 1)
        if(b_left_store[i] == b_left) return b_right_store[i];
    return 0;
}

unsigned int b_match_rl(unsigned int b_right) {
    for(unsigned int i = 0; i < b_stack_size; i += 1)
        if(b_right_store[i] == b_right) return b_left_store[i];
    return 0;
}

int main(int argc, char* argv[]) {

    // Initial error checks and file pointer initialization
    if(argc < 2) return err(-1, "No args.");
    FILE * in_file = fopen((const char*)argv[1], "r");
    if(in_file == NULL) return err(-1, "Could not open file.");


    // Get active symbol program size
    unsigned int program_size = 0;
    char chr = fgetc(in_file);
    while(chr != EOF) {
        if (codeofsym(chr)) program_size += 1;
        chr = fgetc(in_file);
    }

    // Copy brainfuck commands from file into program memory array and close file
    program = malloc(program_size + 1);
    program[program_size] = 0;
    fseek(in_file, 0, SEEK_SET);
    chr = fgetc(in_file);
    while (inst_ptr < program_size) {
        for (char i = 0; i < symbol_count; i += 1) {
            if (chr == symbols[i]) {
                program[inst_ptr] = chr;
                inst_ptr += 1;
            }
        }
        chr = fgetc(in_file);
    }
    fclose(in_file);
    inst_ptr = 0;


    // Scan loop syntax commands and save positions to stores
    unsigned int b_store_ptr = 0;
    while(inst_ptr < program_size) {
        if(codeofsym(program[inst_ptr]) == 7) b_push(inst_ptr);
        if(codeofsym(program[inst_ptr]) == 8) {
            if(b_stack_ptr == 0) return err(-1, "Syntax error: unclosed right bracket.");
            b_left_store[b_store_ptr] = b_pop();
            b_right_store[b_store_ptr] = inst_ptr;
            b_store_ptr += 1;
        }
        inst_ptr += 1;
    }
    if(b_stack_ptr) return err(-1, "Syntax error: unclosed left bracket.");


    fprintf(stderr, "%s\nprogram_size: %d\n", program, program_size);
    inst_ptr = 0;
    // Brainfuck interpreter
    while(inst_ptr < program_size) {
        switch(codeofsym(program[inst_ptr])) {
            case 1: data_array[data_ptr] += 1;                       break;
            case 2: if (data_array[data_ptr] > 0) data_array[data_ptr] -= 1; break;
            case 3: if (data_ptr < data_array_size-1) data_ptr += 1; break;
            case 4: if (data_ptr > 0) data_ptr -= 1;                 break;
            case 5: fprintf(stdout, "%c", data_array[data_ptr]);     break;
            case 6: data_array[data_ptr] = fgetc(stdin);             break;
            case 7:
                if (data_array[data_ptr] == 0) inst_ptr = b_match_lr(inst_ptr);
                break;
            case 8:
                if (data_array[data_ptr] != 0) inst_ptr = b_match_rl(inst_ptr);
                break;
            case 9:
                fprintf(stderr, "inst_ptr: %d | ", inst_ptr);
                for(int i = 0; i < 10; i++) fprintf(stderr, "%d ", data_array[i]);
                fprintf(stderr, "\n");
            default: break;
        }
        inst_ptr += 1;
    }
    free(program);
    return 0;
}
