#ifndef STATE_HPP
#define STATE_HPP

#include <bits/stdc++.h>
#include<vector>
#include "Instance.hpp"
#include "utils.hpp"
using namespace std;

class CompareLevel
{
    public:
        template<typename T>
        bool operator()(const T *state_1, const T *state_2);
};

class CompareID
{
    public:
        template<typename T>
        bool operator()(const T *state_1, const T *state_2);
};

// class CompareProb
// {
//     public:
//         template<typename T>
//         bool operator()(const T *state_1, const T *state_2);
// };

class State
{
    public:
        float *sum_vector; //SUM VECTOR REPRESENTATION OF THE STATE
        // int sample_size;  // NUMBER OF VALUES (WORD VECTORS) UNDER ITS DOMAIN
        int level;        // THE SHORTEST PATH LENGTH FROM THE ROOT
        float *reach_probs; // REACHABILITY PROBABILITIES GIVEN EACH INTERESTING ATTRIBUTE
        float overall_reach_prob; // OVERALL REACHABILITY PROBABILITY OF THE STATE
        set<State*, CompareLevel> parents;
        set<State*, CompareID> children;
        float *similarities; //VECTOR WITH SIMILARITIES BETWEEN THIS STATE AND ALL INTERESTING TOPICS IN DL
        // bool *domain; // BINARY VECTOR WHICH DEFINES THE COLUMNS ARE CONTAINED BY THE STATE
        bool *reachable_states; // BINARY VECTOR WHICH DEFINES THE REACHABLE STATES FROM THIS. THE FIRST |A| POSITIONS INDICATE THE COLUMNS, THEN THESE DEFINES THE STATE DOMAIN
        int abs_column_id; //ABSOLUTE COLUMN ID (ONLY FOR LEAF NODES)
        int update_id; //ID OF LAST reach_probs UPDATE. IT'S USED INTO TOPOLOGICAL SORT
        bool is_tag; //IF THE STATE REPRESENTS A TAG

        void update_reach_probs(float gamma, int total_num_columns);
        void compute_similarities(Instance *inst);
        State* copy(int total_num_columns, int embedding_dim);

        static State* build(Instance *inst, int id, int i, int j);
        static void add_parenthood(State *parent, State *child, int embedding_dim);
        static bool compare(const State *state_1, const State *state_2);
        static bool compare_id(const State *state_1, const State *state_2);
        // static bool compare_level(const State& state_1, const State& state_2);
};

#endif