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
        int **map; // MAP ABSOLUTE (IN DL) IN RELATIVE (IN ITS TABLE) COLUMN ID
};

class State
{
    public:
        float *sum_vector; //SUM VECTOR REPRESENTATION OF THE STATE
        int sample_size;  // NUMBER OF VALUES (WORD VECTORS) UNDER ITS DOMAIN
        int level = -1;        // THE SHORTEST PATH LENGTH FROM THE ROOT
        float *reach_probs; // REACHABILITY PROBABILITIES GIVEN EACH INTERESTING ATTRIBUTE (ONLY FOR NON-LEAF NODES)
        float overall_reach_prob; // OVERALL REACHABILITY PROBABILITY OF THE STATE (ONLY FOR NON-LEAF NODES)
        float discover_prob; // DISCOVER PROBABILITY OF A ATTRIBUTE REPRESENTED BY THE STATE (ONLY FOR LEAF NODES)
        vector<State*> parents;
        vector<State*> children;
        int abs_column_id= -1; //ABSOLUTE COLUMN ID (ONLY FOR LEAF NODES)
        float *similarities_vector; //VECTOR WITH SIMILARITIES BETWEEN THIS STATE AND ALL INTERESTING TOPICS IN DL (ONLY FOR NON-LEAF NODES)

    void update_children_reach_probs(Instance *inst, float gamma);

};

class Cluster
{
    public:
        State *state = NULL;
        int id; // DISTANCES MATRIX ID
        Cluster *is_NN_of = NULL; //IS NEAREST NEIGHBOR OF
        Cluster *next = NULL; // CHAINED LIST OF ACTIVE CLUSTERS
        int cardinality; //NUMBER OF ELEMENTS CONTAINED IN THIS CLUSTERS
};

class Organization
{
    public:
        int embedding_dim;
        State *root = NULL;
        float gamma; // HYPERPARAMETER USED IN PROBABILITY ESTIMATION
    void compute_all_reach_probs(Instance *inst);
};

//IMPLEMENTATION CONSIDERING THAT ALL PARENTS OF A STATE ARE IN THE SAME LEVEL
//UNDER THIS CONSIDERATION, A BREADTH-FIRST SEARCH CAN UPDATE ALL PARENTS BEFORE ITS CHILDREN
void State::update_children_reach_probs(Instance *inst, float gamma)
{
    queue<State*> queue;
    queue.push(this);
    State *current;
    State *parent;
    float sum_probs, prob;

    while( !queue.empty() ){
        //GET THE FIRST STATE FROM THE QUEUE
        current = queue.front();
        queue.pop();

        //COMPUTE ITS REACHABILITY PROBABILITIES
        if( current->level != 0 && current->abs_column_id < 0 )
        {
            //UPDATE LEVEL OF CURRENT NODE. OBS.: ALL PARENTS ARE IN THE SAME LEVEL
            current->level = current->parents[0]->level + 1;

            //TEST!
            printf("Level: %d\nID: %d\n", current->level, current->abs_column_id);
            printf("Reach Probs\n");
            
            current->overall_reach_prob = 0;
            //FOR EACH INTERESTING TOPIC
            for (int i = 0; i < inst->total_num_columns; i++)
            {
                //FOR EACH PARENT
                current->reach_probs[i] = 0; 
                for(int j = 0; j < current->parents.size(); j++)
                {
                    parent = current->parents[j];
                    sum_probs = 0;
                    //WARNING: THIS CAN BE OPTIMIZED
                    for(int k = 0; k < parent->children.size(); k++)
                        sum_probs += exp( gamma * parent->children[k]->similarities_vector[i] / parent->children.size());
                    
                    prob = exp( gamma * current->similarities_vector[i] / parent->children.size() ) / sum_probs;
                    current->reach_probs[i] += prob * parent->reach_probs[i];
                }
                //TEST!
                printf("%.3f ", current->reach_probs[i]);
                
                current->overall_reach_prob += current->reach_probs[i];
            }
            current->overall_reach_prob /= inst->total_num_columns;
            //TEST!
            printf("\nOverall reach prob: %.3f\n\n", current->overall_reach_prob);
        //COMPUTE ITS DISCOVERY PROBABILITY (WHEN IT IS A LEAF)
        } else if ( current->abs_column_id >= 0 ){

            //UPDATE LEVEL OF CURRENT NODE. OBS.: ALL PARENTS ARE IN THE SAME LEVEL
            current->level = current->parents[0]->level + 1;

            //TEST!
            printf("Level: %d\nID: %d\n", current->level, current->abs_column_id);
            printf("Reach Probs\n");

            //FOR EACH PARENT
            current->discover_prob = 0;
            for(int j = 0; j < current->parents.size(); j++)
            {
                parent = current->parents[j];
                sum_probs = 0;
                for(int k = 0; k < parent->children.size(); k++){
                    sum_probs += exp( gamma * parent->children[k]->similarities_vector[current->abs_column_id] / parent->children.size());
                    printf("%d %.3f\n", parent->abs_column_id, parent->children[k]->similarities_vector[current->abs_column_id]);
                }
                prob = exp( gamma * current->similarities_vector[current->abs_column_id] / parent->children.size() ) / sum_probs;
                current->discover_prob += prob * parent->reach_probs[current->abs_column_id];
            }
            printf("Discover probability: %.3f\n\n", current->discover_prob);
        }
        

        //ADD ITS CHILDREN TO THE QUEUE
        for(int i=0; i < current->children.size(); i++ )
            queue.push(current->children[i]);
        
    }

}

void Organization::compute_all_reach_probs(Instance *inst){

    this->root->level = 0;
    this->root->overall_reach_prob = 1;
    this->root->reach_probs = new float(inst->total_num_columns);
    for (int i = 0; i < inst->total_num_columns; i++)
        this->root->reach_probs[i] = 1.0;

    this->root->update_children_reach_probs(inst, this->gamma);
}

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
    int count = 0;

    //Leaf nodes (columns representation) creation
    for (int i = 0; i < inst->num_tables; i++)
    {
        org.root->sample_size += inst->tables[i]->nrows * inst->tables[i]->ncols;

        for (int j = 0; j < inst->tables[i]->ncols; j++)
        {
            state = new State;
            state->abs_column_id = count;
            count++;
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

Cluster* init_clusters(Instance * inst)
{
    Cluster* active_clusters = NULL;

    int id = 0;
    State *state;
    Cluster *current, *previous;
    //Leaf nodes (columns representation) creation
    for (int i = 0; i < inst->num_tables; i++)
    {
        for (int j = 0; j < inst->tables[i]->ncols; j++)
        {
            state = new State;
            state->abs_column_id = id;
            state->sum_vector = inst->tables[i]->sum_vectors[j];
            state->sample_size = inst->tables[i]->nrows;
            state->similarities_vector = new float[inst->total_num_columns];
            state->reach_probs = new float[inst->total_num_columns];

            current = new Cluster;
            current->state = state;
            current->id = id;
            current->cardinality = 1;

            if(active_clusters == NULL)
                active_clusters = current;
            else
                previous->next = current;

            previous = current;
            id++;
        }   
    }
    return active_clusters;
}

float** init_dist_matrix(Cluster* active_clusters, int total_num_columns, int embedding_dim)
{

    Cluster *cluster_i = active_clusters;
    Cluster *cluster_j;

    int size = 2 * total_num_columns - 1;
    float **dist_matrix = new float*[size];

    float similarity;

    //Init dist_matrix with zeros
    for(int i = 0; i < size; i++)
        dist_matrix[i] = new float[size];

    for(int i = 0; i < total_num_columns; i++)
    {
        cluster_j = cluster_i->next;
        for(int j = i+1; j < total_num_columns; j++)
        {
            similarity = cossine_similarity(cluster_i->state->sum_vector, cluster_j->state->sum_vector, embedding_dim);
            cluster_i->state->similarities_vector[ cluster_j->state->abs_column_id ] = similarity;
            cluster_j->state->similarities_vector[ cluster_i->state->abs_column_id ] = similarity;

            dist_matrix[i][j] = 1 - similarity;
            dist_matrix[j][i] = dist_matrix[i][j];
            cluster_j = cluster_j->next;
        }
        cluster_i->state->similarities_vector[ cluster_i->state->abs_column_id ] = 1.0;

        printf("ID: %d\n", cluster_i->state->abs_column_id);
        for (int z = 0; z < total_num_columns; z++)
            printf("%.2f ", cluster_i->state->similarities_vector[z]);
        printf("\n\n");

        cluster_i = cluster_i->next;
    }
    
    //TEST!
    // for(int i=0; i<total_num_columns; i++){
    //     for(int j =0; j<total_num_columns; j++)
    //         printf("%.2f ", dist_matrix[i][j]);
    //     printf("\n");
    // }

    return dist_matrix;
}

//MERGE THE LAST TWO CLUSTER OF NN CHAIN AND ADD THE NEW CLUSTER INTO UNMERGED CLUSTERS LIST 
Cluster* merge_clusters(Cluster *stack, float **dist_matrix, int cluster_id, Instance *inst)
{
    int id_1 = stack->id;
    int id_2 = stack->is_NN_of->id;

    int card_1 = stack->cardinality;
    int card_2 = stack->is_NN_of->cardinality;

    State *state_1 = stack->state;
    State *state_2 = stack->is_NN_of->state;

    //CREATE A NEW STATE IN THE ORGANIZATION WHICH WILL BE PARENT OF RNN STATES
    State *new_state = new State;

    new_state->sum_vector = new float[inst->embedding_dim];
    for (int i = 0; i < inst->embedding_dim; i++)
        new_state->sum_vector[i] = state_1->sum_vector[i] + state_2->sum_vector[i];

    int table_id;
    int col_id;
    float *sum_vector_i;
    new_state->similarities_vector = new float[inst->total_num_columns];    

    for (int i = 0; i < inst->total_num_columns; i++)
    {
        table_id = inst->map[i][0];
        col_id = inst->map[i][1];
        sum_vector_i = inst->tables[table_id]->sum_vectors[col_id];
        new_state->similarities_vector[i] = cossine_similarity(new_state->sum_vector, sum_vector_i, inst->embedding_dim);
        
    }

    new_state->abs_column_id = -cluster_id; //NEGATIVE ID: A NON-LEAF STATE
    new_state->reach_probs = new float[inst->total_num_columns];
    new_state->sample_size = state_1->sample_size + state_2->sample_size;
    new_state->children.push_back(state_1);
    new_state->children.push_back(state_2);

    state_1->parents.push_back(new_state);
    state_2->parents.push_back(new_state);

    //CREATE A NEW CLUSTER CONTAINING THE NEW STATE
    Cluster *new_cluster = new Cluster;
    new_cluster->state = new_state;
    new_cluster->id = cluster_id;
    new_cluster->cardinality = card_1 + card_2;

    //UPDATE DISTANCES MATRIX
    for (int i = 0; i < cluster_id; i++)
    {
        dist_matrix[cluster_id][i] = ( card_1 * dist_matrix[id_1][i] + card_2 * dist_matrix[id_2][i] ) / ( card_1 + card_2 );
        dist_matrix[i][cluster_id] = dist_matrix[cluster_id][i];
    }

    //TEST!
    // for(int i=0; i <= cluster_id; i++){
    //     for(int j =0; j <= cluster_id; j++)
    //         printf("%.2f ", dist_matrix[i][j]);
    //     printf("\n");
    // }

    return new_cluster;
}

//GENERATE A ORGANIZATION BY HIERARQUICAL CLUSTERING
//ALGORITHM: NEAREST NEIGHBORS CHAIN -> O(N²)
//DISTANCE BETWEEN CLUSTERS: UNWEIGHTED PAIR-GROUP METHOD WITH ARITHMETIC MEAN (UPGMA)
Organization generate_organization_by_clustering(Instance * inst, float gamma)
{
    //Organization setup
    Organization org;
    org.embedding_dim = inst->embedding_dim;
    org.gamma = gamma;

    Cluster* active_clusters = init_clusters(inst); // CHAINED LIST OF CLUSTERS AVAILABLE TO BE ADDED TO NN CHAIN
    float** dist_matrix = init_dist_matrix(active_clusters, inst->total_num_columns, inst->embedding_dim); // DISTANCE BETWEEN ALL CLUSTERS

    Cluster *stack = active_clusters; //NEAREST NEIGHBORS CHAIN
    active_clusters = active_clusters->next; // REMOVE THE HEAD FROM CHAINED LIST AND ADD TO NN CHAIN

    Cluster *prev_nn, *nn; // PREVIOUS NN CLUSTER AND NN CLUSTER IN CHAINED LIST
    Cluster *previous, *current; // ITERATORS
    Cluster *new_cluster;

    int cluster_id = inst->total_num_columns; // MATRIX ID OF THE NEXT CLUSTER THAT WILL BE CREATED
    float diff_dists;

    //WHILE THERE ARE CLUSTERS TO MERGE 
    while(stack != NULL)
    {
        // printf("Iteration\n\n");
        // printf("%d ", stack->id);
        prev_nn = NULL;
        nn = active_clusters;
        previous = active_clusters;
        current = active_clusters->next;

        //FIND NEAREST NEIGHBOR TO TOP STACK CLUSTER
        while(current != NULL)
        {
            diff_dists = dist_matrix[stack->id][current->id] - dist_matrix[stack->id][nn->id];
            if( diff_dists < 0 || ( diff_dists == 0 && current->id < nn->id ) )
            {
                prev_nn = previous;
                nn = current;
            }
            previous = current;
            current = current->next;
        }

        //CHECK IF A PAIR OF RNN WERE FOUND
        diff_dists = 1.0;
        if( stack->is_NN_of != NULL ){
            diff_dists = dist_matrix[stack->id][stack->is_NN_of->id] - dist_matrix[stack->id][nn->id];
        }
        if( diff_dists < 0 || ( diff_dists == 0 && stack->is_NN_of->id < nn->id ) )
        {
            // printf("%d\n", stack->is_NN_of->id);
            //MERGE THE RNN INTO A NEW CLUSTER
            new_cluster = merge_clusters(stack, dist_matrix, cluster_id, inst);
            //ADD THE NEW CLUSTER INTO UNMERGED CLUSTER LIST
            new_cluster->next = active_clusters;
            active_clusters = new_cluster;
            cluster_id++;

            //REMOVE THE RNN FROM NN CHAIN
            stack = stack->is_NN_of->is_NN_of;
            if(stack == NULL && active_clusters->next != NULL){
                stack = active_clusters;
                active_clusters = active_clusters->next;
            }
        } else {
            //REMOVE NN FROM THE CHAINED LIST
            if(prev_nn == NULL)
                active_clusters = nn->next;
            else
                prev_nn->next = nn->next;
            
            //ADD NN TO THE NN CHAIN
            // printf("%d\n", nn->id);
            nn->is_NN_of = stack;
            stack = nn;

            if( active_clusters == NULL ) {
                active_clusters = merge_clusters(stack, dist_matrix, cluster_id, inst);
                cluster_id++;
                stack = stack->is_NN_of->is_NN_of;
            }
        }
        //Test!
        current = stack;
        // printf("Stack: ");
        // while(current != NULL){
        //     printf("%d ", current->id);
        //     current = current->is_NN_of;
        // }
        // printf("\n");

        // current = active_clusters;
        // printf("Active Clusters: ");
        // while(current != NULL){
        //     printf("%d ", current->id);
        //     current = current->next;
        // }
        // printf("\n\n");
    }

    org.root = active_clusters->state;
    org.compute_all_reach_probs(inst);
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

    //GENERATE MAP ABSOLUTE ID -> RELATIVE ID
    instance.map = new int*[instance.total_num_columns];
    int count = 0;
    for (int i = 0; i < instance.num_tables; i++)
    {
        for (int j = 0; j < instance.tables[i]->ncols; j++){
            instance.map[count] = new int[2];
            instance.map[count][0] = i;
            instance.map[count][1] = j;
            count++;
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

    // Cluster *active_clusters = init_clusters(&instance);
    // init_dist_matrix(active_clusters, instance.total_num_columns, instance.embedding_dim);
    Organization org = generate_organization_by_clustering(&instance, gamma);

    return 0;
}