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

Organization* best_neighbor_delete(Organization *org, int update_id)
{
    cout << "Delete - ";
    Organization *new_org;
    Organization *best_org = NULL;

    for(int level = 2; level < org->all_states.size(); level++)
    {
        new_org = org->copy();
        new_org->delete_parent(level, 0, update_id);
        if(best_org == NULL || new_org->effectiveness > best_org->effectiveness)
            best_org = new_org;
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
    cout << "Add - ";
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
Organization* local_search(Organization *org)
{
    vector<best_neighbor_func> neighborhood {&best_neighbor_add, &best_neighbor_delete};
    // random_shuffle(neighborhood.begin(), neighborhood.end());
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(neighborhood.begin(), neighborhood.end(), std::default_random_engine(seed));

    Organization *new_org;
    unsigned k = 0;
    int update_id = 1;
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
            cout << "Accepted" << endl;
        } else {
            k++;
            cout << "Rejected" << endl;
        }
    }
    return org;
}


int main()
{
    Instance * instance = Instance::read_instance();
    float gamma = 1.0;
    Organization *org;

    //PERFORMANCE EVALUATION
    time_t start, end;

    time(&start);

    org = Organization::generate_organization_by_clustering(instance, gamma);
    org = local_search(org);

    time(&end);

    cout << org->effectiveness << ", " << difftime(end, start) << "\n";

    return 0;
}