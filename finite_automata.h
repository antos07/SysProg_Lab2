//
// Created by antos07 on 9/26/23.
//

#ifndef SYSPROG_LAB2_FINITE_AUTOMATA_H
#define SYSPROG_LAB2_FINITE_AUTOMATA_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define MAX_ALPHABET_SIZE 26

struct finite_automata_transition {
    char symbol;
    size_t to_state;
    struct finite_automata_transition *next;
};

struct finite_automata_state {
    bool is_final;
    struct finite_automata_transition *first_transition;
};

struct finite_automata {
    char alphabet_size;
    size_t state_number;
    size_t initial_state;
    struct finite_automata_state *states;
};

void init_finite_automata_states(struct finite_automata *fa);

void add_finite_automata_transition(struct finite_automata *fa, size_t from_state, char symbol, size_t to_state);

void set_finite_automata_final_state(struct finite_automata *fa, size_t state);

void destroy_finite_automata(struct finite_automata *fa);

void print_finite_automata(struct finite_automata *fa, FILE *output_file);

void convert_nfa_to_dfa(struct finite_automata *input_nfa, struct finite_automata *output_dfa);

#endif //SYSPROG_LAB2_FINITE_AUTOMATA_H
