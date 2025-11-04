//code written by Aoibhe Hegarty
//student number: 23487084
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_States 25
#define MAX_Symbols 3
#define Epsilon '\0'

typedef struct DFA_State {
    bool nfa_States[MAX_States];
    bool is_Accept;               
    bool is_Marked;               
} DFA_State;

typedef struct DFA_Transition{
    int from_Dfa;
    char symbol_Dfa;
    int to_Dfa;
} DFA_Transition;

typedef struct Transition_funct{
    int from_State;
    char trans_Symbol; 
    int to_State;
} Transition_funct;

typedef struct Automaton {
    int num_States;
    bool is_DFA;
    bool is_Start[MAX_States];
    bool is_Accept[MAX_States];
    int num_Trans; // number of transitions
    Transition_funct transitions[200];
    char alphabet[MAX_Symbols];
    int alpha_Size; // size of the automatons alphabet
    int num_Accept;//number of acceptanc estates
    int start_State;
    DFA_State states[MAX_States];
    int accept_State[MAX_States];
    DFA_Transition transition[200];
} Automaton;

void addState(Automaton * Fin_auto, int new_State, bool start_State, bool accept_State);
void addTransition(Automaton *Fin_state, int begin_State, char transition, int end_State);
bool accepts(Automaton *fin_State, char *in);
void closure(Automaton *fin_State, int given_State, bool visited[]);
void next(Automaton *fin_State, int given_State, char symbol, bool reachable[]);
bool deterministic(Automaton *fin_State);
bool sameSet(const bool *a, const bool *b);
void copySet(bool *dest, const bool *src);
int initial_Dfa(Automaton *n_fin_State, DFA_State dfa_States[]);
Automaton toDFA(Automaton *nfa);

// funtion adds new state to the machine
void addState(Automaton * Fin_auto, int new_State, bool start_State, bool accept_State){
    // if the new state is larger than the previous number of states...
    if (new_State >= Fin_auto->num_States){
        // increase number of states by 1 + the value of the new state
        Fin_auto->num_States = new_State + 1;
    }
    Fin_auto->is_Start[new_State] = start_State;
    Fin_auto->is_Accept[new_State] = accept_State;
}

// function adds new transition to automaton between specifired states
void addTransition(Automaton *Fin_state, int begin_State, char transition, int end_State){
    Fin_state->transitions[Fin_state->num_Trans].from_State = begin_State;
    Fin_state->transitions[Fin_state->num_Trans].trans_Symbol = transition;
    Fin_state->transitions[Fin_state->num_Trans].to_State = end_State;
    Fin_state->num_Trans++;

}

//function tells whether a given input is accepted by fsa
bool accepts(Automaton *fin_State, char *in){
    bool curr[MAX_States] = {false};
    bool next_State[MAX_States] = {false};

    for(int i = 0; i < fin_State->num_States; i++){
        if(fin_State->is_Start[i]){
            closure(fin_State, i, curr);
        }
    }

    for(int j = 0; in[j] != Epsilon; j++){
        char c = in[j];
        memset(next_State, 0, sizeof(next_State));

        for(int k = 0; k < fin_State->num_States; k++){
            if(curr[k]){
                next(fin_State, j, c, next_State);
            }
        }

        memcpy(curr, next_State, sizeof(next_State));
    }

    for(int l = 0; l < fin_State->num_States; l++){
        if(curr[l] && fin_State->is_Accept[l]){
            return true;
        }
    }

    return false;
}

//function returns list of states that can be reached from given state by following only empty transitions
void closure(Automaton *fin_State, int given_State, bool visited[]){
    visited[given_State] = true;
    for (int i = 0; i < fin_State->num_Trans; i++) {
        Transition_funct t = fin_State->transitions[i];
        if (t.from_State == given_State && t.trans_Symbol == Epsilon && !visited[t.to_State]) {
            closure(fin_State, t.to_State, visited);
        }
    }
}

//function returns list of states that can be ereached from a given state following an input symbol
void next(Automaton *fin_State, int given_State, char symbol, bool reachable[]){
    bool closed_States[MAX_States] = {false};
    closure(fin_State, given_State, closed_States);

    for (int i = 0; i < fin_State->num_Trans; i++) {
        Transition_funct t = fin_State->transitions[i];
        if (closed_States[t.from_State] && t.trans_Symbol == symbol) {
            bool dest_Closure[MAX_States] = {false};
            closure(fin_State, t.to_State, dest_Closure);  // Step 2: follow ε from destination
            for (int j = 0; j < fin_State->num_States; j++) {
                if (dest_Closure[j]) reachable[j] = true;
            }
        }
    }
}

int initial_Dfa(Automaton *n_fin_State, DFA_State dfa_States[]){
    bool start_Close[MAX_States] = {false};
    for (int i = 0; i < n_fin_State->num_States; i++){
        if(n_fin_State->is_Start[i]){
            closure(n_fin_State, i, start_Close);
        }
    }

    copySet(dfa_States[0].nfa_States, start_Close);

    dfa_States[0].is_Accept = false;
    for(int i = 0; i < n_fin_State->num_States; i++){
        if(start_Close[i] && n_fin_State->is_Accept[i]){
            dfa_States[0].is_Accept = true;
        }
    }

    dfa_States[0].is_Marked = false;
}

//function tells whether the fsa is deterministic or not
bool deterministic(Automaton *fin_State){
    for(int i = 0; i < fin_State->num_Trans; i++){
        if(fin_State->transitions[i].trans_Symbol == Epsilon){
            return false;
        }
        for(int j = i + 1; j < fin_State->num_Trans; j++){
            // it is at this point that i regret my naming convention
            if(fin_State->transitions[i].from_State == fin_State->transitions[j].from_State && fin_State->transitions[i].trans_Symbol == fin_State->transitions[j].trans_Symbol){
                return false;
            }
        }
    }

    return true;
}

bool sameSet(const bool *a, const bool *b){
    for (int i = 0; i < MAX_States; i++)
        if (a[i] != b[i]){
            return false;
        }
    return true;
}

void copySet(bool *dest, const bool *src){
    for (int i = 0; i < MAX_States; i++)
        dest[i] = src[i];
}

Automaton toDFA(Automaton *nfa) {
    Automaton dfa = {0};
    dfa.alpha_Size = nfa->alpha_Size;
    memcpy(dfa.alphabet, nfa->alphabet, nfa->alpha_Size);

    dfa.num_States = initial_Dfa(nfa, dfa.states);
    dfa.start_State = 0;

    while (1) {
        int unmarked = -1;

        // Find any unmarked states
        for (int i = 0; i < dfa.num_States; i++) {
            if (!dfa.states[i].is_Marked) {
                unmarked = i;
                break;
            }
        }
        if (unmarked == -1){
            break;
        } // No unmarked states left

        dfa.states[unmarked].is_Marked = true;

        // For each input symbol (no epsilon)
        for (int a = 0; a < nfa->alpha_Size; a++) {
            char symbol = nfa->alphabet[a];
            bool reachable[MAX_States] = {false};

            // Compute next states for this symbol
            for (int i = 0; i < nfa->num_States; i++) {
                if (dfa.states[unmarked].nfa_States[i]){
                    next(nfa, i, symbol, reachable);
                }
            }

            // Check if this set is new
            bool exists = false;
            int targetIndex = -1;

            for (int j = 0; j < dfa.num_States; j++) {
                if (sameSet(reachable, dfa.states[j].nfa_States)) {
                    exists = true;
                    targetIndex = j;
                    break;
                }
            }

            if (!exists) {
                targetIndex = dfa.num_States;
                copySet(dfa.states[targetIndex].nfa_States, reachable);

                // Mark accepting if any NFA state is accepting
                dfa.states[targetIndex].is_Accept = false;
                for (int i = 0; i < nfa->num_States; i++) {
                    if (reachable[i] && nfa->is_Accept[i]){
                        dfa.states[targetIndex].is_Accept = true;
                    }
                }

                dfa.states[targetIndex].is_Marked = false;
                dfa.num_States++;
            }

            // Add DFA transition
            dfa.transition[dfa.num_Trans++] = (DFA_Transition){unmarked, symbol, targetIndex};
        }
    }

    // Record accepting states
    for (int i = 0; i < dfa.num_States; i++) {
        if (dfa.states[i].is_Accept){
            dfa.accept_State[dfa.num_Accept++] = i;
        }
    }

    return dfa;
}


int main() {
    printf("Finite State Machine Implementation\n");
    Automaton finite_State = {0};
    finite_State.num_States = 11;
    finite_State.alpha_Size = 3;
    finite_State.alphabet[0] = 'a';
    finite_State.alphabet[1] = 'b';

    addState(&finite_State, 0, true, false);
    addState(&finite_State, 10, false, true);

    addTransition(&finite_State, 0, Epsilon, 1);
    addTransition(&finite_State, 0, Epsilon, 7);
    addTransition(&finite_State, 1, Epsilon, 2);
    addTransition(&finite_State, 1, Epsilon, 4);
    addTransition(&finite_State, 2, 'a', 3);
    addTransition(&finite_State, 4, 'b', 5);
    addTransition(&finite_State, 3, Epsilon, 6);
    addTransition(&finite_State, 5, Epsilon, 6);
    addTransition(&finite_State, 6, Epsilon, 1);
    addTransition(&finite_State, 6, Epsilon, 7);
    addTransition(&finite_State, 7, 'a', 8);
    addTransition(&finite_State, 8, 'b', 9);
    addTransition(&finite_State, 9, 'b', 10);

    printf("accepts(\"abb\"): %s\n", accepts(&finite_State, "abb") ? "true":"false");
    printf("deterministic: %s\n", deterministic(&finite_State) ? "true" : "false");

    Automaton det_fin_State = {0};
    toDFA(&det_fin_State);

    printf("deterministic: %s\n", deterministic(&det_fin_State) ? "true" : "false");

    return 0;
}

