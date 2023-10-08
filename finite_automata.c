//
// Created by antos07 on 9/26/23.
//

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "finite_automata.h"

#define insure_allocated(ptr) if (!ptr) {fprintf(stderr, "Failed to allocate memory!"); exit(EXIT_FAILURE);}

void init_finite_automata_states(struct finite_automata *fa) {
    if (!fa) {
        return;
    }

    // Allocate memory for fa->state_number states.
    fa->states = calloc(fa->state_number, sizeof(struct finite_automata_state));
    insure_allocated(fa->states);
}

static struct finite_automata_transition *create_finite_automate_transition(char symbol, size_t to_state) {
    // Allocate memory for a new transition.
    struct finite_automata_transition *transition = malloc(sizeof(struct finite_automata_transition));
    insure_allocated(transition);

    // Init transition.
    transition->symbol = symbol;
    transition->to_state = to_state;
    transition->next = NULL;

    return transition;
}

void add_finite_automata_transition(struct finite_automata *fa, size_t from_state, char symbol, size_t to_state) {
    if (!fa) {
        return;
    }

    // Allocate and init a new transition.
    struct finite_automata_transition *new_transition = create_finite_automate_transition(symbol, to_state);

    // Insert the transition at the beginning of the linked list of transitions
    // of the from_state state.
    new_transition->next = fa->states[from_state].first_transition;
    fa->states[from_state].first_transition = new_transition;
}

void set_finite_automata_final_state(struct finite_automata *fa, size_t state) {
    if (!fa) {
        return;
    }

    fa->states[state].is_final = true;
}

static void destroy_finite_automata_state(struct finite_automata_state *state) {
    if (!state) {
        return;
    }

    // Destroy the linked list of transitions.
    while (state->first_transition) {
        struct finite_automata_transition *next = state->first_transition->next;
        free(state->first_transition);
        state->first_transition = next;
    }
}

void destroy_finite_automata(struct finite_automata *fa) {
    if (!fa) {
        return;
    }

    // Destroy each state.
    for (size_t i = 0; i < fa->state_number; ++i) {
        destroy_finite_automata_state(&fa->states[i]);
    }
    // Deallocate the array of states.
    free(fa->states);

    // Insure states are removed from the finite automata.
    fa->state_number = 0;
    fa->states = NULL;
}

void print_finite_automata(struct finite_automata *fa, FILE *output_file) {
    // Print the alphabet size, the state number and the initial state.
    fprintf(output_file, "%d\n%ld\n%ld\n", (int) fa->alphabet_size, fa->state_number, fa->initial_state);

    // Calculate the final state number.
    size_t final_state_number = 0;
    for (size_t i = 0; i < fa->state_number; ++i) {
        final_state_number += fa->states[i].is_final;
    }

    // Print the final state number and each of the final states.
    fprintf(output_file, "%ld", final_state_number);
    for (size_t i = 0; i < fa->state_number; ++i) {
        if (fa->states[i].is_final) {
            fprintf(output_file, "% ld", i);
        }
    }
    fprintf(output_file, "\n");

    // Print transitions.
    for (size_t i = 0; i < fa->state_number; ++i) {
        struct finite_automata_transition *current = fa->states[i].first_transition;
        while (current) {
            fprintf(output_file, "%ld %c %ld\n", i, current->symbol, current->to_state);
            current = current->next;
        }
    }
}

struct dfa_state {
    size_t nfa_state_number;
    int64_t *bitset;
};
#define BITSET_ELEMENT_SIZE 64

struct dfa_state create_dfa_state(size_t nfa_state_number) {
    struct dfa_state dfa_state;

    dfa_state.nfa_state_number = nfa_state_number;

    size_t bitset_element_number = nfa_state_number / BITSET_ELEMENT_SIZE + 1;
    dfa_state.bitset = calloc(bitset_element_number, sizeof(int64_t));
    insure_allocated(dfa_state.bitset);

    return dfa_state;
}

bool is_dfa_state_empty(const struct dfa_state *dfa_state) {
    size_t bitset_element_number = dfa_state->nfa_state_number / BITSET_ELEMENT_SIZE + 1;
    for (size_t i = 0; i < bitset_element_number; ++i) {
        if (dfa_state->bitset[i] != 0) {
            return false;
        }
    }
    return true;
}

bool dfa_state_contains_nfa_state(struct dfa_state *dfa_state, size_t nfa_state) {
    size_t bitset_element = nfa_state / BITSET_ELEMENT_SIZE;
    int bitset_element_bit = nfa_state % BITSET_ELEMENT_SIZE;
    return dfa_state->bitset[bitset_element] & (1ll << bitset_element_bit);
}

void add_nfa_state_to_dfa_state(struct dfa_state *dfa_state, size_t nfa_state) {
    size_t bitset_element = nfa_state / BITSET_ELEMENT_SIZE;
    int bitset_element_bit = nfa_state % BITSET_ELEMENT_SIZE;
    dfa_state->bitset[bitset_element] |= (1ll << bitset_element_bit);
}

void destroy_dfa_state(struct dfa_state *dfa_state) {
    if (!dfa_state) return;

    free(dfa_state->bitset);
    dfa_state->bitset = NULL;
    dfa_state->nfa_state_number = 0;
}

struct dfa_state copy_dfa_state(struct dfa_state state) {
    struct dfa_state copy;
    copy.nfa_state_number = state.nfa_state_number;

    size_t bitset_element_number = state.nfa_state_number / BITSET_ELEMENT_SIZE + 1;
    copy.bitset = malloc(bitset_element_number * sizeof(int64_t));
    insure_allocated(copy.bitset);
    memcpy(copy.bitset, state.bitset, bitset_element_number * sizeof(int64_t));

    return copy;
}

bool compare_dfa_states(struct dfa_state a, struct dfa_state b) {
    if (a.nfa_state_number != b.nfa_state_number)
        return false;
    size_t bitset_element_number = a.nfa_state_number / BITSET_ELEMENT_SIZE + 1;
    for (size_t i = 0; i < bitset_element_number; ++i) {
        if (a.bitset[i] != b.bitset[i])
            return false;
    }
    return true;
}

struct dfa_state_registry {
    size_t size;
    size_t capacity;
    struct dfa_state *data;
};

void init_dfa_state_registry(struct dfa_state_registry *registry) {
    registry->size = 0;
    registry->capacity = 1;
    registry->data = calloc(registry->capacity, sizeof(struct dfa_state));
    insure_allocated(registry->data);
}

bool is_dfa_state_in_registry(struct dfa_state_registry *registry, struct dfa_state state) {
    for (size_t i = 0; i < registry->size; ++i) {
        if (compare_dfa_states(state, registry->data[i])) {
            return true;
        }
    }
    return false;
}

size_t add_dfa_state_to_registry(struct dfa_state_registry *registry, struct dfa_state state) {
    for (size_t i = 0; i < registry->size; ++i) {
        if (compare_dfa_states(state, registry->data[i])) {
            return i;
        }
    }

    if (registry->size >= registry->capacity) {
        // Grow data.
        struct dfa_state *new_data = calloc(registry->capacity * 2, sizeof(struct dfa_state));
        insure_allocated(new_data);
        memmove(new_data, registry->data, registry->capacity * sizeof(struct dfa_state));
        free(registry->data);
        registry->data = new_data;
        registry->capacity *= 2;
    }

    registry->data[registry->size] = copy_dfa_state(state);
    return registry->size++;
}

struct dfa_state get_dfa_state_from_registry_by_id(struct dfa_state_registry *registry, size_t id) {
    return registry->data[id];
}

size_t get_registry_state_number(const struct dfa_state_registry *registry) {
    return registry->size;
}

size_t get_state_id_from_registry(const struct dfa_state_registry *registry, struct dfa_state state) {
    for (size_t i = 0; i < registry->size; ++i) {
        if (compare_dfa_states(state, registry->data[i])) {
            return i;
        }
    }
    // Return garbage.
    return -1;
}

void destroy_registry(struct dfa_state_registry *registry) {
    for (size_t i = 0; i < registry->size; ++i) {
        destroy_dfa_state(&registry->data[i]);
    }
    free(registry->data);
    registry->data = NULL;
    registry->size = registry->capacity = 0;
}

void convert_nfa_to_dfa(struct finite_automata *input_nfa, struct finite_automata *output_dfa) {
    // Copy alphabet size as it won't change.
    output_dfa->alphabet_size = input_nfa->alphabet_size;

    struct dfa_state_registry registry;
    init_dfa_state_registry(&registry);

    // Add initial state to the registry.
    struct dfa_state initial_nfa_state = create_dfa_state(input_nfa->state_number);
    add_nfa_state_to_dfa_state(&initial_nfa_state, input_nfa->initial_state);
    output_dfa->initial_state = add_dfa_state_to_registry(&registry, initial_nfa_state);

    for (size_t output_state_id = 0; output_state_id < get_registry_state_number(&registry); ++output_state_id) {
        struct dfa_state dfa_state = get_dfa_state_from_registry_by_id(&registry, output_state_id);

        // Create a new dfa state per each transition.
        struct dfa_state transition_to[MAX_ALPHABET_SIZE];
        for (int i = 0; i < input_nfa->alphabet_size; ++i) {
            transition_to[i] = create_dfa_state(input_nfa->state_number);
        }

        // Fill new dfa transitions.
        for (size_t input_state = 0; input_state < input_nfa->state_number; ++input_state) {
            if (!dfa_state_contains_nfa_state(&dfa_state, input_state)) {
                continue;
            }

            struct finite_automata_transition *transition = input_nfa->states[input_state].first_transition;
            while (transition) {
                add_nfa_state_to_dfa_state(&transition_to[transition->symbol - 'a'], transition->to_state);

                transition = transition->next;
            }
        }

        // Add new transition states
        for (int i = 0; i < input_nfa->alphabet_size; ++i) {
            if (is_dfa_state_empty(&transition_to[i])
                || is_dfa_state_in_registry(&registry, transition_to[i]))
                continue;

            size_t state_id = add_dfa_state_to_registry(&registry, transition_to[i]);

            destroy_dfa_state(&transition_to[i]);
        }
    }

    // Init dfa states.
    output_dfa->state_number = get_registry_state_number(&registry);
    init_finite_automata_states(output_dfa);

    // Iterate again filling the dfa with transitions
    for (size_t output_state_id = 0; output_state_id < output_dfa->state_number; ++output_state_id) {
        struct dfa_state dfa_state = get_dfa_state_from_registry_by_id(&registry, output_state_id);

        // Create a new dfa state per each transition.
        struct dfa_state transition_to[MAX_ALPHABET_SIZE];
        for (int i = 0; i < input_nfa->alphabet_size; ++i) {
            transition_to[i] = create_dfa_state(input_nfa->state_number);
        }

        // Fill new dfa transitions.
        for (size_t input_state = 0; input_state < input_nfa->state_number; ++input_state) {
            if (!dfa_state_contains_nfa_state(&dfa_state, input_state)) {
                continue;
            }

            if (input_nfa->states[input_state].is_final) {
                set_finite_automata_final_state(output_dfa, output_state_id);
            }

            struct finite_automata_transition *transition = input_nfa->states[input_state].first_transition;
            while (transition) {
                add_nfa_state_to_dfa_state(&transition_to[transition->symbol - 'a'], transition->to_state);

                transition = transition->next;
            }
        }

        // Add new transition states
        for (int i = 0; i < input_nfa->alphabet_size; ++i) {
            if (is_dfa_state_empty(&transition_to[i])) {
                destroy_dfa_state(&transition_to[i]);
                transition_to[i] = copy_dfa_state(dfa_state);
            }

            size_t to_state = get_state_id_from_registry(&registry, transition_to[i]);
            add_finite_automata_transition(output_dfa, output_state_id, 'a' + i, to_state);

            destroy_dfa_state(&transition_to[i]);
        }
    }

    destroy_registry(&registry);
}
