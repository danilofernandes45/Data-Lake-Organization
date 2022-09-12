#include "utils.hpp"

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

void add_parenthood(State *parent, State *child, int embedding_dim)
{
    parent->reachable_states[child->abs_column_id] = 1;

    parent->children.insert(child);
    child->parents.insert(parent);

    for(int i = 0; i < embedding_dim; i++) {
        parent->sum_vector[i] += child->sum_vector[i];
    }
}