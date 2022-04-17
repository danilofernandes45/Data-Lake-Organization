#include <stdio.h>
#include <bits/stdc++.h>
#include <vector>
#include <iostream>     // std::cout
// #include <algorithm>    // std::shuffle
// #include <random>       // std::default_random_engine
// #include <chrono>       // std::chrono::system_clock
using namespace std;

//PROJECT FILES
#include "Instance.hpp"
#include "Organization.hpp"

int main()
{
    Instance * instance = Instance::read_instance();
    float gamma = 1.0;

    Organization *org = Organization::generate_organization_by_clustering(instance, gamma);
    org->copy(instance->total_num_columns, instance->embedding_dim);

    return 0;
}