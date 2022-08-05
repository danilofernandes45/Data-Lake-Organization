#ifndef MODIFICATION_HPP
#define MODIFICATION_HPP

#include "State.hpp"

class Modification
{
    public:
        int operation; // 0 - add_parent / 1 - delete_parent
        int level;
        int level_id;
        int min_level;
        vector<State*> deleted_states;
};

#endif