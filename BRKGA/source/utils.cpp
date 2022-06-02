#include "utils.hpp"

float cossine_similarity(vector<double> vector_1, vector<double> vector_2){
    float inner_product = 0;
    float norm_1 = 0;
    float norm_2 = 0;

    for (int i = 0; i < vector_1.size(); i++)
    {
        inner_product += vector_1[i] * vector_2[i];
        norm_1 += vector_1[i] * vector_1[i];
        norm_2 += vector_2[i] * vector_2[i];
    }

    norm_1 = sqrt(norm_1);
    norm_2 = sqrt(norm_2);

    return inner_product / (norm_1 * norm_2);    

}