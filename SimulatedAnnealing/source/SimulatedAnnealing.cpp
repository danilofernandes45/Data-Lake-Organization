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
        for (int j = 0; j < org->all_states[i].size(); j++)
        {
            cout << org->all_states[i][j]->abs_column_id << " (" << org->all_states[i][j]->level << ")\nParents => ";
            for(int k = 0; k < org->all_states[i][j]->parents.size(); k++)
                cout << org->all_states[i][j]->parents[k]->abs_column_id << " ";
            
            cout << "\nChildren => ";
            for(int k = 0; k < org->all_states[i][j]->children.size(); k++)
                cout << org->all_states[i][j]->children[k]->abs_column_id << " ";
            cout << "\n";
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
    int level, level_id;

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

    H_n = 0;
    for(int i = 0; i < org->all_states[level].size(); i++)
        H_n = H_n + ( 1 / org->all_states[level][i]->overall_reach_prob );

    //RANDOM SELECTION OF LEVEL (ROULLETE WHEEL)
    x = distribution(generator) * H_n;
    for(int i = 0; i < org->all_states[level].size(); i++) {
        sum = sum + ( 1 / org->all_states[level][i]->overall_reach_prob );
        if( x < sum ){
            level_id = i;
            break;
        }
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
                update_id++;
                if( org->effectiveness > best_org->effectiveness ) {
                    best_org = org;
                    count = 0;
                }
            } else {
                new_org->undo_last_operation(org);
            }
            if( new_org->all_states.size() == 2 )
                new_org = best_org->copy();
        }
        T = alpha * T;
    }
    return best_org;
}

Organization* multistart_sa(Instance *instance, float gamma, int num_restarts, int max_iter, float alpha, int K_max) {
    Organization *org = Organization::generate_organization_by_clustering(instance, gamma);
    Organization *best_org = org;
    Organization *new_org;
    for (int i = 0; i < num_restarts; i++)
    {
        new_org = simulated_annealing(org, max_iter, alpha, K_max);
        if( new_org->effectiveness > best_org->effectiveness )
            best_org = new_org;
    }
    return best_org;    
}

int main()
{
    Instance * instance = Instance::read_instance();
    float gamma = 10.0;
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
    org = multistart_sa(instance, gamma, 25, 30, 0.001, 25);

    time(&end);

    cout << org->effectiveness << ", " << org->all_states.size() << ", " << difftime(end, start) << "\n";

    return 0;
}