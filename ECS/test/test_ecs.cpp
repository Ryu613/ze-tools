#include "ze/ecs/ecs.hpp"

using namespace ze;

struct ComponentA {
    int a = 0;
    std::string b{"default"};
};

int main() {
    ecs::Entity e = ecs::make_entity<ComponentA>(ComponentA{3,"test"});
    ecs::add_component_data<ComponentA>(e, ComponentA{2,"test2"});

    return EXIT_SUCCESS;
}