#ifndef STATE_HPP
#define STATE_HPP

#include <bits/stdc++.h>
#include<vector>
#include "Instance.hpp"
#include "utils.hpp"
using namespace std;

class State
{
    public:
        float *sum_vector; //SUM VECTOR REPRESENTATION OF THE STATE
        int sample_size;  // NUMBER OF VALUES (WORD VECTORS) UNDER ITS DOMAIN
        int level;        // THE SHORTEST PATH LENGTH FROM THE ROOT
        float *reach_probs; // REACHABILITY PROBABILITIES GIVEN EACH INTERESTING ATTRIBUTE
        float overall_reach_prob; // OVERALL REACHABILITY PROBABILITY OF THE STATE
        vector<State*> parents;
        vector<State*> children;
        float *similarities; //VECTOR WITH SIMILARITIES BETWEEN THIS STATE AND ALL INTERESTING TOPICS IN DL
        int *domain; // BINARY VECTOR WHICH DEFINES THE COLUMNS ARE CONTAINED BY THE STATE
        int abs_column_id; //ABSOLUTE COLUMN ID (ONLY FOR LEAF NODES)
        int update_id; //ID OF LAST reach_probs UPDATE. IT'S USED TO AVOID RECOMPUTATIONS IN THE DAG

        void update_reach_probs(float gamma, int total_num_columns);
        void compute_similarities(Instance *inst);
        State* copy(int total_num_columns, int embedding_dim);
        void destroy();

        static State* build(Instance *inst, int id, int i, int j);
        static bool compare(const State *state_1, const State *state_2);
        static bool compare_id(const State *state_1, const State *state_2);
        // static bool compare_level(const State& state_1, const State& state_2);
};

class CompareLevel
{
    public:
        bool operator()(const State *state_1, const State *state_2);
};

#endif