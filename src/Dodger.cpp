#include "Dodger.h"
#include <core\log\Log.h>
#include "Constants.h"
#include <renderer\sprites.h>
#include <gamestates\GameStateMachine.h>
#include "gamestates\MainGameState.h"
#include "gamestates\GameOverState.h"
#include "gamestates\MainMenuState.h"
#include <particles\ParticlesTestState.h>
#include "asteroids\AsteroidState.h"
//#include "gamestates\HighscoreState.h"
#include "GameContext.h"
#include "GameSettings.h"
#include "asteroids\WarpingGrid.h"
#include "objects\ElasticBorder.h"

ds::BaseApp *app = new Dodger();

Dodger::Dodger() : ds::BaseApp() {
	//_server = new ds::GameServer('127', '0', '0', '1', 9000);
}

Dodger::~Dodger() {
	//_server->close();
	//delete _server;
	delete _context->elasticBorder;
	delete _context->settings;
	delete _context->world;
	delete _context->grid;
	delete _context;
}

// -------------------------------------------------------
// prepare
// -------------------------------------------------------
void Dodger::prepare(ds::Settings* settings) {
	settings->screenWidth = 1280;
	settings->screenHeight = 720;
	settings->clearColor = ds::Color(0, 0, 0, 255);
	settings->fullScreen = false;
	settings->reportingDirectory = "reports";
	settings->synched = true;
	settings->reloading = true;
}
// -------------------------------------------------------
// Load content and prepare game
// -------------------------------------------------------
bool Dodger::loadContent() {

	RID material = ds::res::find("SpriteMaterial", ds::ResourceType::MATERIAL);	
	ds::Material* m = ds::res::getMaterial(material);
	m->texture = ds::res::find("TextureArray", ds::ResourceType::TEXTURE);
	_border_color = ds::Color(107, 72, 72, 255);

	_context = new GameContext;
	_context->settings = new GameSettings;
	_context->settings->load();
	_context->grid = new WarpingGrid(_context->settings);
	_context->grid->createGrid();

	_context->world = new ds::World;
	_context->world->setBoundingRect(ds::Rect(25, 20, 1250, 680));
	_context->world->useTemplates(ds::res::getWorldEntityTemplates(SID("game_objects")));
	_context->world->ignoreCollisions(OT_FOLLOWER, OT_FOLLOWER);
	_context->world->ignoreCollisions(OT_PLAYER, OT_BULLET);
	_context->world->ignoreCollisions(OT_BULLET, OT_BULLET);
	_context->world->ignoreCollisions(OT_HUGE_ASTEROID, OT_BIG_ASTEROID);
	_context->world->ignoreCollisions(OT_HUGE_ASTEROID, OT_MEDIUM_ASTEROID);
	_context->world->ignoreCollisions(OT_HUGE_ASTEROID, OT_SMALL_ASTEROID);

	_context->particles = ds::res::getParticleManager();
	_context->additiveBlendState = ds::res::findBlendState("AdditiveBlendState");

	_context->elasticBorder = new ElasticBorder();

	ds::ParticlesTestSettings pts;
	pts.render = false;
	pts.blendState = _context->additiveBlendState;
	pts.screenSize = v2(1280, 720);
	addGameState(new MainGameState(_context));
	addGameState(new GameOverState(_context));
	addGameState(new AsteroidState(_context));
	addGameState(new ds::ParticlesTestState(pts));
	addGameState(new MainMenuState(_context));
	// connect game states
	connectGameStates("MainGame", 1, "GameOver");
	connectGameStates("AsteroidState", 1, "GameOver");
	connectGameStates("MainGame", 2, "ParticlesTestState");
	connectGameStates("GameOver", 1, "MainGame");
	connectGameStates("GameOver", 2, "MainMenuState");
	connectGameStates("MainMenuState", 3, "MainGame");

	addShortcut("Save world", '0', 100);

	//_server->connect(this);

	return true;
}

void Dodger::init() {
	activate("MainGame");
}


// -------------------------------------------------------
// Update
// -------------------------------------------------------
void Dodger::update(float dt) {
	_context->grid->tick(dt);
	_context->elasticBorder->tick(dt);
	//_server->poll();
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void Dodger::render() {
	_context->grid->render();
	ds::SpriteBuffer* sprites = graphics::getSpriteBuffer();
	ds::ChannelArray* array = _context->world->getChannelArray();
	v3* positions = (v3*)array->get_ptr(ds::WEC_POSITION);
	ds::Texture* textures = (ds::Texture*)array->get_ptr(ds::WEC_TEXTURE);
	v3* rotations = (v3*)array->get_ptr(ds::WEC_ROTATION);
	v3* scales = (v3*)array->get_ptr(ds::WEC_SCALE);
	ds::Color* colors = (ds::Color*)array->get_ptr(ds::WEC_COLOR);
	for (uint32_t i = 0; i < array->size; ++i) {
		v2 p = positions[i].xy();
		float r = rotations[i].x;
		ds::Texture t = textures[i];
		sprites->draw(positions[i].xy(), textures[i], rotations[i].x, scales[i].xy(), colors[i]);
	}
	sprites->end();
	graphics::selectBlendState(_context->additiveBlendState);
	_context->particles->render();
	_context->elasticBorder->render();
	graphics::selectViewport(0);
	graphics::selectBlendState(0);
}

void Dodger::get(const ds::HTTPRequest& request, ds::HTTPResponse* response) {
	LOG << "requesting: " << request.path;
	//response->data.append("callback(");
	_context->world->generateJSON(response->data);
	//response->data.append(");");
	response->size = response->data.size();
}