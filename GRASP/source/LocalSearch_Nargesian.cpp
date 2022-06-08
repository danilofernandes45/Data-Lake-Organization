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

// Organization* best_neighbor_delete(Organization *org, int update_id)
// {
//     // cout << "Delete - ";
//     Organization *new_org;
//     Organization *best_org = NULL;

//     // cout << "org: " <<  org->effectiveness << endl;

//     for(int level = 2; level < org->all_states.size(); level++)
//     {
//         new_org = org->copy();
//         new_org->delete_parent(level, 0, update_id);
//         // cout << "new_org: " <<  new_org->effectiveness << endl;
//         if(best_org == NULL || new_org->effectiveness > best_org->effectiveness){
//             best_org = new_org;
//             // cout << "best_org: " << best_org->effectiveness << endl;
//         }
//     }
//     return best_org;
// }

Organization* best_neighbor_delete(Organization *org, int update_id)
{
    // cout << "Delete - ";
    Organization *aux; 
    Organization *best_org = org->copy();
    Organization *new_org = org->copy();

    vector<State*> best_deleted_states, deleted_states;
    int best_level = -1;

    // cout << "org: " <<  org->effectiveness << endl;
    // print_organization(org);

    for(int level = 2; level < org->all_states.size(); level++)
    {
        deleted_states = new_org->delete_parent(level, 0, update_id);
        // cout << "new_org: " <<  new_org->effectiveness << endl;
        if( new_org->effectiveness > best_org->effectiveness ) {
            aux = best_org;
            best_org = new_org;
            new_org = aux;

            if( best_level >= 0 )
                new_org->undo_delete_parent(org, best_deleted_states, best_level );

            best_level = level;
            best_deleted_states = deleted_states;

            // cout << "best_org: " << best_org->effectiveness << endl;
            // cout << "new_org: " << new_org->effectiveness << endl; 
        } else {
            new_org->undo_delete_parent(org, deleted_states, level);
            // print_organization(new_org);
        }
    }
    return best_org;
}

// Organization* best_neighbor_add(Organization *org, int update_id)
// {
//     Organization *new_org;
//     Organization *best_org = NULL;

//     cout << "org: " <<  org->effectiveness << endl;

//     for(int level = 2; level < org->all_states.size(); level++)
//     {
//         for(int level_id = 0; level_id < org->all_states[level].size(); level_id++)
//         {
//             new_org = org->copy();
//             new_org->add_parent(level, level_id, update_id);
//             cout << "new_org: " <<  new_org->effectiveness << endl;
//             if(best_org == NULL || new_org->effectiveness > best_org->effectiveness)
//                 best_org = new_org;
//         }
//     }
//     return best_org;
// }

Organization* best_neighbor_add(Organization *org, int update_id)
{
    // cout << "Add - ";
    Organization *aux; 
    Organization *best_org = org->copy();
    Organization *new_org = org->copy();
    int best_level, best_level_id, min_level;
    int best_min_level = -1;

    // cout << "org: " <<  org->effectiveness << endl;

    for(int level = 2; level < org->all_states.size(); level++)
    {
        for(int level_id = 0; level_id < org->all_states[level].size(); level_id++)
        { 
            min_level = new_org->add_parent(level, level_id, update_id);
            // cout << "new_org: " <<  new_org->effectiveness << endl;
            if( new_org->effectiveness > best_org->effectiveness ) {
                aux = best_org;
                best_org = new_org;
                new_org = aux;

                if( best_min_level >= 0 )
                    new_org->undo_add_parent(org, best_level, best_level_id, best_min_level);

                best_level = level;
                best_level_id = level_id;
                best_min_level = min_level;

                // cout << "best_org: " << best_org->effectiveness << endl;
                // cout << "new_org: " << new_org->effectiveness << endl; 
            } else if( min_level >= 0 ){
                new_org->undo_add_parent(org, level, level_id, min_level);
            }
        }
    }
    return best_org;
}

//RVND Local Search
pair<Organization*, int> local_search(Organization *org, int update_id)
{
    vector<best_neighbor_func> neighborhood {&best_neighbor_add, &best_neighbor_delete};
    // random_shuffle(neighborhood.begin(), neighborhood.end());
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(neighborhood.begin(), neighborhood.end(), std::default_random_engine(seed));

    Organization *new_org;
    unsigned k = 0;
    while (k < neighborhood.size())
    {
        new_org = neighborhood[k](org, update_id);
        if( new_org->effectiveness > org->effectiveness ){
            // random_shuffle(neighborhood.begin(), neighborhood.end());
            seed = std::chrono::system_clock::now().time_since_epoch().count();
            shuffle(neighborhood.begin(), neighborhood.end(), std::default_random_engine(seed));
            k = 0;
            org = new_org;
            update_id++;
            // cout << "Accepted" << endl;
        } else {
            k++;
            // cout << "Rejected" << endl;
        }
    }
    return {org, update_id};
}

Organization* perturbation(Organization *org, int update_id)
{
    Organization *new_org = org->copy();
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand(seed);

    int level = rand() % org->all_states.size();
    level = max(level, 2);
    int level_id = rand() % org->all_states[level].size();

    if( rand() % 2 == 0 )
        new_org->add_parent(level, level_id, update_id);
    else
        new_org->delete_parent(level, level_id, update_id);
    
    return new_org;
}

void sa_perturbation(Organization *org, int update_id)
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

Organization* simulated_annealing(Organization *org, int max_iter, float alpha)
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

    while( T > T_min ) {
        for(int i = 0; i < max_iter; i++) {
            sa_perturbation(new_org, update_id);
            delta = org->effectiveness - new_org->effectiveness;
            if( delta < 0 || distribution(generator) < exp(-delta/T) ) {
                org = new_org->copy();
                update_id++;
                if( org->effectiveness > best_org->effectiveness )
                    best_org = org;
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

Organization* multistart_sa(Instance *instance, float gamma, int num_restarts, int max_iter, float alpha) {
    Organization *org = Organization::generate_organization_by_clustering(instance, gamma);
    Organization *best_org = org;
    Organization *new_org;
    for (int i = 0; i < num_restarts; i++)
    {
        new_org = simulated_annealing(org, max_iter, alpha);
        if( new_org->effectiveness > best_org->effectiveness )
            best_org = new_org;
    }
    return best_org;    
}

Organization* iterated_local_search(Instance *instance, float gamma, int max_iter)
{
    int update_id = 1;
    Organization *new_org;
    pair<Organization*, int> local_opt;

    Organization *best_org = Organization::generate_organization_by_clustering(instance, gamma);
    local_opt = local_search(best_org, update_id);
    best_org = local_opt.first;
    update_id = local_opt.second;

    for(int i = 0; i < max_iter; i++)
    {
        new_org = perturbation(best_org, update_id);
        local_opt = local_search(new_org, update_id + 1);
        if( local_opt.first->effectiveness > best_org->effectiveness ){
            best_org = local_opt.first;
            update_id = local_opt.second;
        }
    }
    return best_org;
}

Organization* grasp(Instance *instance, float gamma, int max_iter)
{
    double best_effectiveness = -1;
    Organization *new_org, *best_org = NULL;
    pair<Organization*, int> local_opt;

    for(int i = 0; i < max_iter; i++)
    {
        new_org = Organization::generate_organization_by_heuristic(instance, gamma);
        local_opt = local_search(new_org, 1);
        if( local_opt.first->effectiveness > best_effectiveness ){
            best_org = local_opt.first;
            best_effectiveness = best_org->effectiveness;
        }
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

    // org = iterated_local_search(instance, gamma, 5);

    org = simulated_annealing(org, 30, 0.001);
    // org = multistart_sa(instance, gamma, 10, 50, 0.001);

    time(&end);

    cout << org->effectiveness << ", " << org->all_states.size() << ", " << difftime(end, start) << "\n";

    return 0;
}