#include <stdio.h>
#include <bits/stdc++.h>
#include <vector>
#include <iostream>     // std::cout
#include <random>       // std::default_random_engine
#include <time.h>
using namespace std;

//PROJECT FILES
#include "Instance.hpp"
#include "Organization.hpp"

#define best_neighbor_func Organization* (*)(Organization*, int)

void print_organization(Organization *org)
{
    for (int i = 0; i < org->all_states.size(); i++)
    {
        for (State * state : org->all_states[i])
        {
            cout << state->abs_column_id << " (" << state->level << ")\nParents => ";
            for(State * parent : state->parents)
                cout << parent->abs_column_id << " ";
            
            cout << "\nChildren => ";
            for(State * child : state->children)
                cout << child->abs_column_id << " ";
            cout << "\n";
        }
        
    }
    cout << "\n";

    for (int i = 0; i < org->all_states.size(); i++)
    {
        for (State * state : org->all_states[i])
        {
            cout << "~ " << state->abs_column_id << " ~" << endl;
            for(int p = 0; p < org->instance->total_num_columns; p++)
                cout << state->reach_probs[p] << " ";
            cout << "\n" << state->overall_reach_prob << endl;
        }
        
    }
    cout << "\n";
}

void perturbation(Organization *org, int update_id)
{
    //GENERATOR OF RANDOM NUMBERS
    random_device rand_dev;
    mt19937 generator(rand_dev());
    uniform_real_distribution<double> distribution(0.0, 1.0);
    double x, sum = 0;
    int level, level_id, i;

    //Harmonic number
    double H_n = 0;
    for(double i = 2; i < org->all_states.size(); i++)
        H_n = H_n + (1 / i);

    //RANDOM SELECTION OF LEVEL (ROULLETE WHEEL)
    x = distribution(generator) * H_n;
    for(double i = 2; i < org->all_states.size(); i++) {
        sum = sum + (1 / i);
        if( x < sum ){
            level = i;
            break;
        }
    }

    H_n = 0.0;
    for(State * state : org->all_states[level])
        H_n = H_n + ( 1 / state->overall_reach_prob );

    //RANDOM SELECTION OF LEVEL (ROULLETE WHEEL)
    sum = 0.0;
    i = 0;
    x = distribution(generator) * H_n;
    for(State * state : org->all_states[level]) {
        sum = sum + ( 1 / state->overall_reach_prob );
        if( x < sum ){
            level_id = i;
            break;
        }
        i++;
    }
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand(seed);
    if( rand() % 2 == 0 )
        org->add_parent(level, level_id, update_id);
    else
        org->delete_parent(level, level_id, update_id);
}

Organization* simulated_annealing(Organization *org, int max_iter, float alpha, int K_max)
{
    int update_id = 1;
    Organization *best_org = org;
    Organization *new_org = org->copy();
    float T = org->effectiveness;
    float T_min = pow(alpha, 10000);
    float delta;
    //GENERATOR OF RANDOM NUMBERS
    random_device rand_dev;
    mt19937 generator(rand_dev());
    uniform_real_distribution<float> distribution(0.0, 1.0);

    int count = 0;

    while( T > T_min && count < K_max ) {
        for(int i = 0; i < max_iter; i++) {
            perturbation(new_org, update_id);
            count++;
            delta = org->effectiveness - new_org->effectiveness;
            if( delta < 0 || distribution(generator) < exp(-delta/T) ) {
                org = new_org->copy();
                update_id = update_id + 2;
                if( org->effectiveness > best_org->effectiveness ) {
                    best_org = org;
                    count = 0;
                }
            } else {
                // new_org->undo_last_operation(org);
                new_org = org->copy();
            }
            if( new_org->all_states.size() == 2 )
                new_org = best_org->copy();
        }
        T = alpha * T;
    }
    return best_org;
}

Organization* multistart_sa(Instance *instance, float gamma, int num_restarts, int max_iter, float alpha, int K_max) {
    Organization *best_org = NULL;
    Organization *new_org;
    for (int i = 0; i < num_restarts; i++)
    {
        new_org = Organization::generate_organization_by_clustering(instance, gamma);
        new_org = simulated_annealing(new_org, max_iter, alpha, K_max);
        if( best_org == NULL || new_org->effectiveness > best_org->effectiveness )
            best_org = new_org;
    }
    return best_org;    
}

int main()
{
    Instance * instance = Instance::read_instance();
    float gamma = 1.0;
    Organization *org;

    //PERFORMANCE EVALUATION
    time_t start, end;

    time(&start);

    // org = Organization::generate_basic_organization(instance, gamma);
    // cout << org->effectiveness << " " << org->all_states.size() << endl;
    org = Organization::generate_organization_by_clustering(instance, gamma);
    // cout << org->effectiveness << " " << org->all_states.size() << endl;
    // org = Organization::generate_organization_by_heuristic(instance, gamma);
    // cout << org->effectiveness << " " << org->all_states.size() << endl;
    // org = grasp(instance, gamma, 10);

    // org = local_search(org, 1).first;
    // org = iterated_local_search(instance, gamma, 5);

    // org = simulated_annealing(org, 30, 0.001, 100);
    // org = multistart_sa(instance, gamma, 10, 20, 0.001, 20);

    for(State * state : org->root->parents)
        cout << state->abs_column_id << " - ";

    cout << endl;

    org = multistart_sa(instance, gamma, 1, 2, 0.001, 20);

    time(&end);

    cout << org->effectiveness << ", " << org->all_states.size() << ", " << difftime(end, start) << "\n";

    org->success_probabilities();

    return 0;
}