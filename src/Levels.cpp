#include "Levels.h"
#include <core\io\json.h>

const StaticHash NAMES[] = { SID("Follower"), SID("Wanderer"), SID("Spotter") };

Levels::Levels(ds::World* world, GameSettings* settings) : _world(world), _active(false) , _timer(0.0f) {

	_emitters.push_back(new TestCubeEmitter());
	_emitters.push_back(new RandomCubeEmitter());
	_emitters.push_back(new CircleCubeEmitter(40.0f));
	_emitters.push_back(new LineCubeEmitter(50.0f));

	_scale_path.add(0.0f, v3(0.1f, 0.1f, 0.0f));
	_scale_path.add(0.5f, v3(1.2f, 1.2f, 0.0f));
	_scale_path.add(0.75f, v3(0.75f, 0.75f, 0.0f));
	_scale_path.add(1.0f, v3(1.0f, 1.0f, 0.0f));

	_behaviors.push_back(new FollowerBehavior(_world, settings));
	_behaviors.push_back(new SpotterBehavior(_world, settings));
	_behaviors.push_back(new WandererBehavior(_world, settings));
}


Levels::~Levels() {
	_emitters.destroy_all();
	_behaviors.destroy_all();
}

// -------------------------------------------------------
// load
// -------------------------------------------------------
void Levels::load() {
	ds::JSONReader reader;
	if (reader.parse(SID("content\\resources\\levels.json"))) {
		int stages[64];
		int num = reader.get_categories(stages, 64);
		for (int i = 0; i < num; ++i) {
			int id = -1;
			if (reader.get_int(stages[i], "id", &id)) {
				int cubes[32];
				int nc = reader.get_categories(cubes, 32, stages[i]);
				Level& level = _levels[id];
				level.num = 0;
				for (int j = 0; j < nc; ++j) {
					LevelItem& item = level.items[j];
					reader.get_int(cubes[j], "id", &item.cubeType);
					reader.get_int(cubes[j], "emitt", &item.emitt);
					reader.get_int(cubes[j], "emitter", &item.emitter);
					reader.get_int(cubes[j], "max", &item.num);
					reader.get(cubes[j], "delay", &item.delay);
					reader.get(cubes[j], "initial_delay", &item.initialDelay);
					reader.get_int(cubes[j], "energy", &item.energy);
					reader.get_int(cubes[j], "behavior", &item.behavior);
					++level.num;
				}
			}
		}
	}
}

// -------------------------------------------------------
// get level by index
// -------------------------------------------------------
const Level& Levels::get(int index) const {
	return _levels[index];
}

// -------------------------------------------------------
// get number of level items
// -------------------------------------------------------
int Levels::getNumberOfItems(int index) const {
	const Level& l = _levels[index];
	return l.num;
}

// -------------------------------------------------------
// tick
// -------------------------------------------------------
void Levels::tick(ID target, float dt) {
	_timer += dt;
	for (uint32_t i = 0; i < _actors.size(); ++i) {
		StageActor* actor = _actors[i];
		actor->timer += dt;
		if (actor->mode == SAM_STARTED) {
			if (actor->timer > actor->initialDelay) {
				actor->mode = SAM_RUNNING;
				actor->timer = 1000.0f;
			}
		}
		else {
			if (actor->timer > actor->delay) {
				actor->timer = 0.0f;
				int num = actor->num;
				if (actor->emitted < actor->total) {
					actor->emitter->next();
					for (int i = 0; i < num; ++i) {
						ID id = _world->create(actor->emitter->get(i, num), actor->templateName);
						actor->behavior->create(id);
						actor->type = _world->getType(id);
						//float ttl = math::random(0.5f, 0.8f);
						//_world->scaleByPath(id, &_scale_path, ttl);
						//rotateTo(id, target);
						CubeData* data = (CubeData*)_world->attach_data(id, sizeof(CubeData), actor->type);
						data->energy = actor->energy;
					}
					LOG << "starting enemies - idx: " << i << " num: " << num << " emitted: " << actor->emitted;
					actor->emitted += num;
				}
			}
			if (_world->hasEvents()) {
				uint32_t n = _world->numEvents();
				for (uint32_t i = 0; i < n; ++i) {
					const ds::ActionEvent& event = _world->getEvent(i);
					if (_world->contains(event.id)) {
						int type = _world->getType(event.id);
						if (type == actor->type) {
							actor->behavior->tick(event, target, actor->type);
						}
					}
				}
			}
		}
	}
}

// -------------------------------------------------------
// start level by index
// -------------------------------------------------------
void Levels::start(int index) {
	_actors.destroy_all();
	const Level& level = _levels[index];
	_total = 0;
	for (int i = 0; i < level.num; ++i) {
		StageActor* actor = create(index, i);
		_total += actor->total;
		_actors.push_back(actor);
	}
	LOG << "enemies to kill: " << _total;
	_timer = 0.0f;
	_active = true;
	
}

// -------------------------------------------------------
// create StageActor
// -------------------------------------------------------
StageActor* Levels::create(int level, int offset) {
	StageActor* actor = new StageActor();
	const Level& l = _levels[level];
	const LevelItem& li = l.items[offset];
	actor->mode = SAM_STARTED;
	actor->delay = li.delay;
	actor->emitted = 0;
	actor->num = li.emitt;
	actor->total = li.num;
	actor->templateName = NAMES[li.cubeType];
	actor->timer = 0.0f;
	actor->initialDelay = li.initialDelay;
	actor->emitter = _emitters[li.emitter];
	actor->energy = li.energy;
	actor->behavior = _behaviors[li.behavior];
	return actor;
}