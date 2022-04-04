#include <stdio.h>
#include <bits/stdc++.h>
#include <vector>
#include <iostream>     // std::cout
#include <algorithm>    // std::shuffle
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

using namespace std;

class Table
{
    public:
        int nrows; //NUMBER OF ROWS
        int ncols; //NUMBER OF COLUMNS
        // EACH COLUMN IS REPRESENTED BY A SUM VECTOR. SUM VECTOR = nrows * TOPIC VECTOR
        // A TOPIC VECTOR IS THE MEAN OF WORD EMBEDDING VECTORS OF A COLUMN VALUES
        float **sum_vectors; 
};

class Instance
{
    public:
        int num_tables; //NUMBER OF TABLES INTO DATA LAKE
        int total_num_columns; // TOTAL NUMBER OF COLUMNS
        int embedding_dim; // WORD VECTOR EMBEDDING DIMENSION
        Table **tables = NULL; // TABLES FROM DATA LAKE
};

class State
{
    public:
        float *sum_vector; //SUM VECTOR REPRESENTATION OF THE STATE
        int sample_size;  // NUMBER OF VALUES (WORD VECTORS) UNDER ITS DOMAIN
        int level;        // THE SHORTEST PATH LENGTH FROM THE ROOT
        float *reach_probs; // REACHABILITY PROBABILITIES GIVEN EACH INTERESTING ATTRIBUTE (ONLY FOR NON-LEAF NODES)
        float overall_reach_prob; // OVERALL REACHABILITY PROBABILITY OF THE STATE (ONLY FOR NON-LEAF NODES)
        float discover_prob; // DISCOVER PROBABILITY OF A ATTRIBUTE REPRESENTED BY THE STATE (ONLY FOR LEAF NODES)
        vector<State*> parents;
        vector<State*> children;
        int table_id = -1;
        int column_id= -1;
};

class Organization
{
    public:
        int embedding_dim;
        State *root = NULL;
        float gamma; // HYPERPARAMETER USED IN PROBABILITY ESTIMATION

};

float cossine_similarity(float *vector_1, float *vector_2, int dim){
    float inner_product = 0;
    float norm_1 = 0;
    float norm_2 = 0;

    for (int i = 0; i < dim; i++)
    {
        inner_product += vector_1[i] * vector_2[i];
        norm_1 += vector_1[i] * vector_1[i];
        norm_2 += vector_2[i] * vector_2[i];
    }

    norm_1 = sqrt(norm_1);
    norm_2 = sqrt(norm_2);

    return inner_product / (norm_1 * norm_2);    

}

//GENERATE THE BASELINE ORGANIZATION
Organization generate_basic_organization(Instance * inst, float gamma)
{

    //Organizatio setup
    Organization org;
    org.embedding_dim = inst->embedding_dim;
    org.gamma = gamma;
    //Root node creation
    org.root = new State;
    org.root->sum_vector = new float[inst->embedding_dim]; // VECTOR INITIALIZED WITH ZEROS
    org.root->sample_size = 0;
    org.root->level = 0;
    org.root->overall_reach_prob = 1;

    State *state;
    float aux;
    float ratio;

    //Leaf nodes (columns representation) creation
    for (int i = 0; i < inst->num_tables; i++)
    {
        org.root->sample_size += inst->tables[i]->nrows * inst->tables[i]->ncols;

        for (int j = 0; j < inst->tables[i]->ncols; j++)
        {
            state = new State;
            state->table_id = i;
            state->column_id = j;
            state->parents.push_back( org.root );
            state->sum_vector = inst->tables[i]->sum_vectors[j];
            state->level = 1;
            state->sample_size = inst->tables[i]->nrows;

            ratio = org.gamma / inst->total_num_columns;
            state->discover_prob = exp(ratio);

            //Estimating the discover probability of a column
            aux = 0;            
            for (int k = 0; k < inst->num_tables; k++)
            {
                for (int l = 0; l < inst->tables[k]->ncols; l++)
                {
                    if ( k == i && l == j ) {
                        aux += state->discover_prob;
                    } else {
                        aux += exp( ratio * cossine_similarity(state->sum_vector, inst->tables[k]->sum_vectors[l], org.embedding_dim ) );
                    }
                }
            }

            state->discover_prob /= aux;
            // printf("Discover Similarity T%d-C%d: %.5f\n\n", i, j, state->discover_prob);

            //root sum vector incrementation 
            for (int d = 0; d < inst->embedding_dim; d++)
                org.root->sum_vector[d] += state->sum_vector[d];

            //Adding the leaf to the organization as child of the root
            org.root->children.push_back(state);   
        }   
    }

    return org;
    
}

//GENERATE A ORGANIZATION BY HIERARQUICAL CLUSTERING
Organization generate_organization_by_clustering(Instance * inst, float gamma)
{
    Organization org = generate_basic_organization(inst, gamma);

    

    return org;
}

Instance read_instance()
{
    Instance instance;

    scanf("%d %d", &instance.num_tables, &instance.embedding_dim);
    instance.total_num_columns = 0;
    instance.tables = new Table*[instance.num_tables];

    Table *table;

    for (int i = 0; i < instance.num_tables; i++)
    {
        table = new Table;
        instance.tables[i] = table;

        scanf("%d %d", &table->nrows, &table->ncols);
        instance.total_num_columns += table->ncols;
        table->sum_vectors = new float*[table->nrows];

        for (int j = 0; j < table->ncols; j++)
        {
            table->sum_vectors[j] = new float[instance.embedding_dim];
            for (int d = 0; d < instance.embedding_dim; d++)
                scanf("%f", &table->sum_vectors[j][d]);
        }
    }

    // printf("%d %d %d\n", instance.num_tables, instance.embedding_dim, instance.total_num_columns);
    
    // for (int i = 0; i < instance.num_tables; i++)
    // {
    //     Table *table = instance.tables[i];

    //     printf("%d %d\n", table->nrows, table->ncols);

    //     for (int j = 0; j < table->ncols; j++)
    //     {
    //         for (int d = 0; d < instance.embedding_dim; d++)
    //             printf("%f ", table->sum_vectors[j][d]); 
    //         printf("\n");
    //     }
    // }

    return instance;
}

int main()
{
    Instance instance = read_instance();
    float gamma = 1.0;

    Organization org = generate_basic_organization(&instance, gamma);

    return 0;
}