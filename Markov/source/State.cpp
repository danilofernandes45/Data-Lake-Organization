#include "State.hpp"

bool CompareLevel::operator()(const State *state_1, const State *state_2) {
    return state_1->level > state_2->level; 
}

bool State::compare(const State *state_1, const State *state_2) {
    return state_1->overall_reach_prob < state_2->overall_reach_prob; 
}

bool State::compare_id(const State *state_1, const State *state_2) {
    return state_1->abs_column_id < state_2->abs_column_id; 
}

void State::destroy()
{
    delete [] this->sum_vector; //SUM VECTOR REPRESENTATION OF THE STATE
    delete [] this->reach_probs; // REACHABILITY PROBABILITIES GIVEN EACH INTERESTING ATTRIBUTE
    delete [] this->similarities; //VECTOR WITH SIMILARITIES BETWEEN THIS STATE AND ALL INTERESTING TOPICS IN DL
    delete [] this->domain; // BINARY VECTOR WHICH DEFINES THE COLUMNS ARE CONTAINED BY THE STATE

    vector<State*>().swap(this->parents);
    vector<State*>().swap(this->children);

    delete this;
}

State* State::build(Instance *inst, int id, int i, int j)
{
    State *state = new State;
    state->update_id = -1;
    state->abs_column_id = id;
    state->similarities = new float[inst->total_num_columns];
    state->reach_probs = new float[inst->total_num_columns];
    state->domain = new int[inst->total_num_columns];

    if ( i >= 0 && j >= 0 ) {
        state->sum_vector = inst->tables[i]->sum_vectors[j];
        state->sample_size = inst->tables[i]->nrows;
        state->domain[id] = 1;
    } else {
        state->sum_vector = new float[inst->embedding_dim];
    }

    return state;
}

State* State::copy(int total_num_columns, int embedding_dim)
{
    State *copy = new State;
    copy->sample_size = this->sample_size;
    copy->level = this->level;
    copy->abs_column_id = this->abs_column_id;
    copy->update_id = this->update_id;
    copy->overall_reach_prob = this->overall_reach_prob;

    copy->sum_vector = new float[embedding_dim];
    for (int i = 0; i < embedding_dim; i++)
        copy->sum_vector[i] = this->sum_vector[i];

    copy->reach_probs = new float[total_num_columns];
    for (int i = 0; i < total_num_columns; i++)
        copy->reach_probs[i] = this->reach_probs[i];

    copy->similarities = new float[total_num_columns];
    for (int i = 0; i < total_num_columns; i++)
        copy->similarities[i] = this->similarities[i];

    copy->domain = new int[total_num_columns];
    for (int i = 0; i < total_num_columns; i++)
        copy->domain[i] = this->domain[i];

    // DON'T COPY PARENTS AND CHILDREN HERE
    return copy;  
}

void State::update_reach_probs(float gamma, int total_num_columns)
{
    State *parent;
    float sum_probs, prob;
    int num_children;

    if( this->parents.size() != 0 )
    {
        //CONSIDERATION: ALL PARENTS ARE IN THE SAME LEVEL
        this->level = this->parents[0]->level + 1;
        this->overall_reach_prob = 0;
        //FOR EACH INTERESTING TOPIC
        for (int i = 0; i < total_num_columns; i++)
        {
            //FOR EACH PARENT
            this->reach_probs[i] = 0; 
            for(int j = 0; j < this->parents.size(); j++)
            {
                parent = this->parents[j];
                num_children = parent->children.size();
                sum_probs = 0;
                //WARNING: THIS CAN BE OPTIMIZED
                for(int k = 0; k < num_children; k++)
                    sum_probs += exp( gamma * parent->children[k]->similarities[i] / num_children);
                        
                prob = exp( gamma * this->similarities[i] / num_children ) / sum_probs;
                this->reach_probs[i] += prob * parent->reach_probs[i];
            }
            this->overall_reach_prob += this->reach_probs[i];
        }
        this->overall_reach_prob /= total_num_columns;
    }
}