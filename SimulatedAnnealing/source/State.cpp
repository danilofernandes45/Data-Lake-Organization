#include "State.hpp"

State::~State(){
    delete this->sum_vector;
    delete this->reach_probs;
    delete this->sum_children_probs; 
    delete this->similarities;
    delete this->reachable_states; 
}

template<typename T>
bool CompareLevel::operator()(const T *state_1, const T *state_2) {
    if(state_1->level == state_2->level)
        return state_1->abs_column_id < state_2->abs_column_id;
    return state_1->level < state_2->level; 
}

template<typename T>
bool CompareID::operator()(const T *state_1, const T *state_2) {
    return state_1->abs_column_id < state_2->abs_column_id; 
}

// template<typename T>
// bool CompareProb::operator()(const T *state_1, const T *state_2) {
//     return state_1->overall_reach_prob > state_2->overall_reach_prob; 
// }

bool State::compare(const State *state_1, const State *state_2) {
    return state_1->overall_reach_prob < state_2->overall_reach_prob; 
}

bool State::compare_id(const State *state_1, const State *state_2) {
    return state_1->abs_column_id < state_2->abs_column_id; 
}

void State::add_parenthood(State *parent, State *child, int embedding_dim)
{
    parent->reachable_states[child->abs_column_id] = 1;

    parent->children.insert(child);
    child->parents.insert(parent);

    for(int i = 0; i < embedding_dim; i++) {
        parent->sum_vector[i] += child->sum_vector[i];
    }
}

void State::compute_similarities(Instance *inst)
{
    int table_id;
    int col_id;
    float *sum_vector_i;
    
    for (int i = 0; i < inst->total_num_columns; i++)
    {
        table_id = inst->map[i][0];
        col_id = inst->map[i][1];
        sum_vector_i = inst->tables[table_id]->sum_vectors[col_id];
        this->similarities[i] = cossine_similarity(this->sum_vector, sum_vector_i, inst->embedding_dim); 
    }
}

State* State::build(Instance *inst, int id, int i, int j)
{
    State *state = new State;
    state->update_id = -1;
    state->level = -1;
    state->abs_column_id = id;
    state->similarities = new float[inst->total_num_columns];
    state->reach_probs = new float[inst->total_num_columns];
    state->sum_children_probs = new float[inst->total_num_columns];
    state->is_tag = false;
    
    if( inst->num_tags > 0 )
        state->reachable_states = new bool[inst->total_num_columns + 2*inst->num_tags - 1]{0};
    else
        state->reachable_states = new bool[2*inst->total_num_columns-1]{0};

    if ( i >= 0 && j >= 0 ) {
        state->sum_vector = inst->tables[i]->sum_vectors[j];
        // state->sample_size = inst->tables[i]->nrows;
        state->reachable_states[id] = 1;
    } else {
        state->sum_vector = new float[inst->embedding_dim];
    }

    return state;
}

State* State::copy(int total_num_columns, int max_num_states, int embedding_dim)
{
    State *copy = new State;
    copy->level = this->level;
    copy->abs_column_id = this->abs_column_id;
    copy->update_id = this->update_id;
    copy->is_tag = this->is_tag;
    copy->overall_reach_prob = this->overall_reach_prob;

    copy->sum_vector = new float[embedding_dim];
    for (int i = 0; i < embedding_dim; i++)
        copy->sum_vector[i] = this->sum_vector[i];

    copy->reach_probs = new float[total_num_columns];
    for (int i = 0; i < total_num_columns; i++)
        copy->reach_probs[i] = this->reach_probs[i];

    copy->sum_children_probs = new float[total_num_columns];
    for (int i = 0; i < total_num_columns; i++)
        copy->sum_children_probs[i] = this->sum_children_probs[i];

    copy->similarities = new float[total_num_columns];
    for (int i = 0; i < total_num_columns; i++)
        copy->similarities[i] = this->similarities[i];

    copy->reachable_states = new bool[max_num_states];
    for (int i = 0; i < max_num_states; i++)
        copy->reachable_states[i] = this->reachable_states[i];

    // DON'T COPY PARENTS AND CHILDREN HERE
    return copy;  
}

void State::update_level()
{
    int old_level, new_level;
    if( this->parents.size() > 0 )
    {   
        old_level = this->level;
        new_level = (*(this->parents.begin()))->level + 1;
        
        if(old_level != new_level){
            //UPDATE THE BINARY IN ITS CHILDREN IN ORDER TO KEEP THE LEVEL ORDER OF PARENTS
            for(State * child : this->children)
                child->parents.erase(this);

            this->level = new_level;

            for(State * child : this->children)
                child->parents.insert(this);
        }
    }   
}

void State::update_reach_probs(float gamma, int total_num_columns)
{
    //float sum_probs;
    float prob;
    int num_children;
    int reachable_cols_count = 0;
    set<State*>::iterator iter_p, iter_c;
    if( this->parents.size() > 0 ) {   
        this->overall_reach_prob = 0;
        //FOR EACH INTERESTING TOPIC
        for (int i = 0; i < total_num_columns; i++) {
            if(this->reachable_states[i]) { //ALOPRATION
                reachable_cols_count++;
                //FOR EACH PARENT
                this->reach_probs[i] = 0;
                for( State * parent : this->parents )
                {
                    num_children = parent->children.size();
                    //sum_probs = 0;
                    //WARNING: THIS CAN BE OPTIMIZED
                    //for(State * child : parent->children )
                    //    sum_probs += exp( gamma * child->similarities[i] / num_children);
                            
                    prob = exp( gamma * this->similarities[i] / num_children ) / parent->sum_children_probs[i];
                    this->reach_probs[i] += prob * parent->reach_probs[i];

                    //cout << sum_probs - parent->sum_children_probs[i] << endl; 

                    //ALOPRATION
                    // sum_probs = 0;
                    // for(State * child : parent->children )
                    //     sum_probs += (1 + child->similarities[i]);
                    // prob = (1 + this->similarities[i]) / sum_probs;
                    // this->reach_probs[i] += prob * parent->reach_probs[i];
                }

                this->overall_reach_prob += this->reach_probs[i];
            }
        }
        // this->overall_reach_prob /= total_num_columns;
        this->overall_reach_prob /= reachable_cols_count; // ALOPRATION
    }
    //COMPUTE CHILDREN PROBS SUM
    for (int i = 0; i < total_num_columns; i++) {
        if(this->reachable_states[i]) { // ALOPRATION
            this->sum_children_probs[i] = 0;
            num_children = this->children.size();
            for(State * child : this->children )
                this->sum_children_probs[i] += exp( gamma * child->similarities[i] / num_children);
        }
    }
}