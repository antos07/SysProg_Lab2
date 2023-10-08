#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "finite_automata.h"

#define FAILED_TO_PROCESS_FILE (-1)
#define SUCCESSFUL_FILE_PROCESSING 0

int process_file(FILE *file) {
    struct finite_automata fa;

    // Read alphabet size, state number, initial state and final state number
    int alphabet_size;
    size_t final_state_number;
    if (fscanf(file, "%d%ld%ld%ld", &alphabet_size, &fa.state_number,
               &fa.initial_state, &final_state_number) == EOF) {
        return FAILED_TO_PROCESS_FILE;
    }

    // Check bounds.
    if (alphabet_size < 1 || alphabet_size > MAX_ALPHABET_SIZE || fa.initial_state < 0 ||
        fa.initial_state >= fa.state_number) {
        return FAILED_TO_PROCESS_FILE;
    }
    fa.alphabet_size = (char) alphabet_size;

    // Allocate and initialize states
    init_finite_automata_states(&fa);

    // Set final states
    for (size_t i = 0; i < final_state_number; ++i) {
        size_t state;
        if (fscanf(file, "%ld", &state) == EOF) {
            destroy_finite_automata(&fa);
            return FAILED_TO_PROCESS_FILE;
        }
        set_finite_automata_final_state(&fa, state);
    }

    // Set transitions
    size_t from_state, to_state;
    char symbol;
    int ret = fscanf(file, "%ld %c%ld", &from_state, &symbol, &to_state);
    while (ret == 3) {
        add_finite_automata_transition(&fa, from_state, symbol, to_state);
        ret = fscanf(file, "%ld %c%ld", &from_state, &symbol, &to_state);
    }

    printf("Input FA:\n");
    print_finite_automata(&fa, stdout);

    struct finite_automata dfa;
    convert_nfa_to_dfa(&fa, &dfa);

    printf("Output FA:\n");
    print_finite_automata(&dfa, stdout);

    destroy_finite_automata(&fa);
    destroy_finite_automata(&dfa);

    return SUCCESSFUL_FILE_PROCESSING;
}

int main(int argc, char **argv) {
    // Check valid usage.
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path to file>", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the given file.
    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        // Couldn't open the file.
        perror(argv[0]);
        return EXIT_FAILURE;
    }

    // Process the given file.
    int ret = process_file(input_file);
    if (ret != SUCCESSFUL_FILE_PROCESSING) {
        fprintf(stderr, "Failed to process the file");
    }

    // Close the opened file.
    fclose(input_file);

    return EXIT_SUCCESS;
}
