#include "ze/ecs/ecs.hpp"

using namespace ze;


struct Position {
    int x, y, z;
};

struct Attr {
    // not trivial! not support now
    // std::string name;
    int hp;
    int mp;
};

class MoveSystem : ecs::System<MoveSystem> {
    //void OnUpdate() {
    //    for (auto& each : ecs::query<Position, Attr>()) {
    //        each.x += 2;
    //    }
    //}
};

struct Extra {

};

struct Another {
    float layer;
};

int main() {
    ecs::init();
    ecs::Entity e = ecs::make_entity<Position, Attr>(
        Position{ 50,24,12 },
        Attr{ 120, 100 }
    );
    // add Position & Attr component data to e
    ecs::add_component<Position, Attr>(e, Position{ 20,10,20 }, Attr{ 20, 105 });
    // add Position component data to e
    ecs::add_component<Position>(e, Position{ 20, 10, 20 });
    // add new component Extra to e
    ecs::add_component<Extra>(e, Extra{});
    // add new component Another and add these data to component
    ecs::add_component<Position, Another>(e, Position{ 20,40,20 }, Another{ 1.135656888 });
    // true
    ecs::has_components<Attr>(e);
    // remove Another component from e
    ecs::remove_components<Another>(e);
    // destroy entity e
    ecs::destroy_entity(e);
    // register new system and enable it
    // can add stage & priority in the future
    ecs::register_system<MoveSystem>();
    // trigger specific system's OnUpdate();
    ecs::system_update<MoveSystem>();
    // trigger registered systems' OnUpdate() in order
    ecs::system_update_all();
    // disable system not unregister
    ecs::disable_system<MoveSystem>();
    // re-enable system
    ecs::enable_system<MoveSystem>();
    // unregister system
    //ecs::unregister_system<MoveSystem>();
    return EXIT_SUCCESS;
}