#include "Cluster.hpp"

void add_parenthood(State *parent, State *child, int embedding_dim)
{
    parent->reachable_states[child->abs_column_id] = 1;

    parent->children.insert(child);
    child->parents.insert(parent);

    for(int i = 0; i < embedding_dim; i++) {
        parent->sum_vector[i] += child->sum_vector[i];
    }
}

Cluster* Cluster::init_clusters(Instance * inst)
{
    Cluster* active_clusters = NULL;
    vector<State*> tags;

    int id = 0;
    int tag_id;
    State *state;
    Cluster *current, *previous;

    //Create non-leaf nodes with tags, when they exists
    for (int i = 0; i < inst->num_tags; i++)
    {
        state = State::build(inst, - inst->total_num_columns - i, -1, -1);
        state->is_tag = true;
        tags.push_back(state);

        current = new Cluster;
        current->state = state;
        current->id = i;

        if(active_clusters == NULL)
            active_clusters = current;
        else
            previous->next = current;

        previous = current;
    }
     
    //Leaf nodes (columns representation) creation
    for (int i = 0; i < inst->num_tables; i++)
    {
        for (int j = 0; j < inst->tables[i]->ncols; j++)
        {
            state = State::build(inst, id, i, j);
            
            if ( tags.empty() )
            {
                current = new Cluster;
                current->state = state;
                current->id = id;
                current->cardinality = 1;

                if(active_clusters == NULL)
                    active_clusters = current;
                else
                    previous->next = current;

                previous = current;

            } else {
                state->compute_similarities(inst);
                //IF THE TAGS ARE RELATE TO THE TABLE
                for (int k = 0; k < inst->tables[i]->tags_table.size(); k++)
                {
                    tag_id = inst->tables[i]->tags_table[k];
                    add_parenthood(tags[tag_id], state, inst->embedding_dim);
                }
                //IF THE TAGS ARE RELATED TO THE COLUMNS
                for (int k = 0; k < inst->tables[i]->tags_cols[j].size(); k++)
                {
                    tag_id = inst->tables[i]->tags_cols[j][k];
                    add_parenthood(tags[tag_id], state, inst->embedding_dim);
                }
            }
            id++;
        }   
    }
    return active_clusters;
}

float** Cluster::init_dist_matrix(Cluster* active_clusters, int num_clusters, int embedding_dim)
{
    Cluster *cluster_i = active_clusters;
    Cluster *cluster_j;

    int size = 2 * num_clusters - 1;
    int id_i, id_j;
    float **dist_matrix = new float*[size];
    float similarity;

    //Init dist_matrix with zeros
    for(int i = 0; i < size; i++)
        dist_matrix[i] = new float[size];

    for(int i = 0; i < num_clusters; i++)
    {
        id_i = cluster_i->state->abs_column_id;
        cluster_j = cluster_i->next;

        for(int j = i+1; j < num_clusters; j++)
        {
            similarity = cossine_similarity(cluster_i->state->sum_vector, cluster_j->state->sum_vector, embedding_dim);
            
            id_j = cluster_j->state->abs_column_id;
            if( id_i >= 0 && id_j >= 0 ) {
                cluster_i->state->similarities[ id_j ] = similarity;
                cluster_j->state->similarities[ id_i ] = similarity;
            }

            dist_matrix[i][j] = 1 - similarity;
            dist_matrix[j][i] = dist_matrix[i][j];
            cluster_j = cluster_j->next;
        }
        if( id_i >= 0 )
            cluster_i->state->similarities[ id_i ] = 1.0;
        else
            cluster_i->cardinality = cluster_i->state->children.size();

        // <TEST>
        // printf("ID: %d\n", cluster_i->state->abs_column_id);
        // for (int z = 0; z < total_num_columns; z++)
        //     printf("%.2f ", cluster_i->state->similarities[z]);
        // printf("\n\n");
        // </TEST>

        cluster_i = cluster_i->next;
    }
    return dist_matrix;
}

//MERGE THE LAST TWO CLUSTER OF NN CHAIN AND ADD THE NEW CLUSTER INTO UNMERGED CLUSTERS LIST 
Cluster* Cluster::merge_clusters(Cluster *stack, float **dist_matrix, int cluster_id, int state_id, Instance *inst)
{
    int id_1 = stack->id;
    int id_2 = stack->is_NN_of->id;

    int card_1 = stack->cardinality;
    int card_2 = stack->is_NN_of->cardinality;

    State *state_1 = stack->state;
    State *state_2 = stack->is_NN_of->state;

    //CREATE A NEW STATE IN THE ORGANIZATION WHICH WILL BE PARENT OF RNN STATES
    State *new_state = State::build(inst, state_id, -1, -1);

    int table_id, col_id, iter;

    for (int i = 0; i < inst->total_num_columns; i++) {
        new_state->reachable_states[i] = state_1->reachable_states[i] | state_2->reachable_states[i];

        if ( new_state->reachable_states[i] == 1 ) {

            table_id = inst->map[i][0];
            col_id = inst->map[i][1];

            for (int d = 0; d < inst->embedding_dim; d++)
                new_state->sum_vector[d] += inst->tables[table_id]->sum_vectors[col_id][d];
        }
    }

    iter = inst->total_num_columns;
    while( iter < 2 * inst->total_num_columns - 1 ) 
         new_state->reachable_states[iter] = state_1->reachable_states[iter] | state_2->reachable_states[iter];       
    
    new_state->reachable_states[ abs(state_id) ] = 1;

    new_state->compute_similarities(inst);

    new_state->children.insert(state_1);
    new_state->children.insert(state_2);

    state_1->parents.insert(new_state);
    state_2->parents.insert(new_state);   

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
