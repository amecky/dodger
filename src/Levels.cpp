#include "Levels.h"
#include <core\io\json.h>

const StaticHash NAMES[] = { SID("Follower"), SID("Wanderer"), SID("Spotter") };

Levels::Levels(ds::World* world, GameSettings* settings) : _world(world), _active(false) , _timer(0.0f) {
	
	_scale_path.add(0.0f, v3(0.1f, 0.1f, 0.0f));
	_scale_path.add(0.5f, v3(1.2f, 1.2f, 0.0f));
	_scale_path.add(0.75f, v3(0.75f, 0.75f, 0.0f));
	_scale_path.add(1.0f, v3(1.0f, 1.0f, 0.0f));

	_behaviors.push_back(new FollowerBehavior(_world, settings));
	_behaviors.push_back(new SpotterBehavior(_world, settings));
	_behaviors.push_back(new WandererBehavior(_world, settings));
}


Levels::~Levels() {
	_behaviors.destroy_all();
}

CubeEmitter* Levels::getEmitter(int type, int total) {
	switch (type) {
		case 0: return new TestCubeEmitter(); break;
		case 1: return new RandomCubeEmitter(); break;
		case 2: return new CircleCubeEmitter(20.0f); break;
		case 3: return new LineCubeEmitter(50.0f); break;
		case 4: return new CornerCircleCubeEmitter(50.0f); break;
		default: return new RandomCubeEmitter();
	}
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
// kill all
// -------------------------------------------------------
void Levels::killAll() {
	ID ids[256];
	for (uint32_t i = 0; i < _actors.size(); ++i) {
		StageActor* actor = _actors[i];
		int num = _world->find_by_type(actor->type, ids, 256);
		for (int j = 0; j < num; ++j) {
			_world->remove(ids[j]);
		}
	}
}

// -------------------------------------------------------
// tick
// -------------------------------------------------------
void Levels::tick(ID target, float dt) {
	ZoneTracker z("Levels::tick");
	_timer += dt;
	int all = 0;
	for (uint32_t i = 0; i < _actors.size(); ++i) {
		StageActor* actor = _actors[i];
		all += actor->emitted;
		actor->timer += dt;
		if (actor->mode == SAM_STARTED) {
			if (actor->timer > actor->initialDelay) {
				actor->mode = SAM_RUNNING;
				actor->timer = 1000.0f;
			}
		}
		else {
			if (_emitting && (actor->timer > actor->delay)) {
				actor->timer = 0.0f;
				int num = actor->num;
				if (actor->emitted < actor->total) {
					actor->emitter->prepare(num);
					for (int j = 0; j < num; ++j) {
						const EmitterData ed = actor->emitter->next(j);
						ID id = _world->create(ed.pos, actor->templateName);
						_world->setRotation(id, ed.rotation);
						actor->behavior->create(id);
						actor->type = _world->getType(id);
						//rotateTo(id, target);
						CubeData* data = (CubeData*)_world->attach_data(id, sizeof(CubeData), actor->type);
						data->energy = actor->energy;
					}					
					actor->emitted += num;
					//LOG << "starting enemies - idx: " << i << " num: " << num << " emitted: " << actor->emitted;
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
	if (all == _total && _emitting) {
		LOG << "ALL EMITTED!!!!!!!";
		_emitting = false;
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
	_emitting = true;
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
	actor->emitter = getEmitter(li.emitter,li.emitt);
	actor->energy = li.energy;
	actor->behavior = _behaviors[li.behavior];
	return actor;
}