#include "EnemyBehavior.h"
#include "Constants.h"
#include <core\world\actions\ScaleByPathAction.h>
#include <core\world\actions\MoveByAction.h>
#include <core\world\actions\LookAtAction.h>
#include <core\world\actions\WiggleAction.h>
#include <core\world\actions\AlignToForceAction.h>
#include <core\world\actions\CollisionAction.h>
#include <core\world\actions\RotateByAction.h>
#include <core\world\actions\ScalingAction.h>
#include <core\world\actions\SeparateAction.h>
#include <core\world\actions\SeekAction.h>

namespace behaviors {

	// -----------------------------------------------------------
	// basic beahvior
	// -----------------------------------------------------------
	void createBasicBehaviors(ds::World* world, ds::V3Path *path) {		
		ds::ActionDefinition startups[] = {
			{ new ds::ScaleByPathActionSettings(path, 1.0f, 0.2f) },
			{ new ds::LookAtSettings(SID("Player"), 0.0f) }
		};
		world->createBehavior("start_up", startups, 2);

		ID attach = world->createBehavior("attach_collider");
		world->addSettings(attach, new ds::CollisionActionSettings(ds::PST_CIRCLE));
	}

	// -----------------------------------------------------------
	// wanderer
	// -----------------------------------------------------------
	void createWanderer(ds::World* world) {
		
		ds::ActionDefinition wsd[] = {
			{ new ds::CollisionActionSettings(ds::PST_CIRCLE) },
			{ new ds::RotateBySettings(DEGTORAD(180.0f),2.0f) }
		};
		world->createBehavior("wanderer_start", wsd, 2);

		ds::ActionDefinition wmv[] = {
			{ new ds::MoveBySettings(100.0f, 5.0f, true) },
			{ new ds::ScaleSettings(v3(1.0f), v3(0.8f), 0.4f) }
		};
		world->createBehavior("wanderer_move", wmv, 2);


		ds::ActionDefinition wrt[] = {
			{ new ds::RotateBySettings(DEGTORAD(180.0f), 2.0f) },
			{ new ds::ScaleSettings(v3(0.8f), v3(1.0f), 0.4f) }
		};
		world->createBehavior("wanderer_rotate", wrt, 2);

		ds::ConnectionDefinition wandererConnections[] = {
			{ SID("start_up"), ds::AT_SCALE_BY_PATH, SID("wanderer_start") },
			{ SID("wanderer_start"), ds::AT_ROTATE_BY, SID("wanderer_move") },
			{ SID("wanderer_move"), ds::AT_MOVE_BY, SID("wanderer_rotate") },
			{ SID("wanderer_rotate"), ds::AT_ROTATE_BY, SID("wanderer_move") }
		};
		world->connectBehaviors(wandererConnections, 4, OT_WANDERER);
	}

	// -----------------------------------------------------------
	// bullet behavior
	// -----------------------------------------------------------
	void createBulletBehavior(ds::World* world, float velocity) {
		ID bullets = world->createBehavior("bullets");
		world->addSettings(bullets, new ds::MoveBySettings(velocity, -1.0f, false));
	}

	// -----------------------------------------------------------
	// spotter behavior
	// -----------------------------------------------------------
	void createSpotterBehavior(ds::World* world) {
		ID spotterLook = world->createBehavior("spotter_look");
		world->addSettings(spotterLook, new ds::LookAtSettings(SID("Player"), 1.0f));

		ds::ActionDefinition sm[] = {
			{ new ds::MoveBySettings(150.0f, 5.0f, true) },
			{ new ds::WiggleSettings(50.0f, 8.0f, 5.0f) },
			{ new ds::AlignToForceSettings(5.0f) }
		};
		world->createBehavior("spotter_move", sm, 3);

		ds::ConnectionDefinition cd[] = {
			{ SID("start_up"), ds::AT_SCALE_BY_PATH, SID("attach_collider") },
			{ SID("attach_collider"), ds::AT_COLLIDER_ATTACHED, SID("spotter_look") },
			{ SID("spotter_look"), ds::AT_LOOK_AT, SID("spotter_move") },
			{ SID("spotter_move"), ds::AT_MOVE_BY, SID("spotter_look") }
		};
		world->connectBehaviors(cd, 4, OT_SPOTTER);
	}

	// -----------------------------------------------------------
	// follower behavior
	// -----------------------------------------------------------
	void createFollowerBehavior(ds::World* world, ds::V3Path *path, GameSettings* settings) {
		ds::ActionDefinition startups[] = {
			{ new ds::ScaleByPathActionSettings(path, 1.0f, 0.2f) },
			{ new ds::LookAtSettings(SID("Player"), 0.0f) },
			{ new ds::MoveBySettings(settings->follower.initialVelocity,0.2f) }
		};
		world->createBehavior("follower_start_up", startups, 3);

		ds::ActionDefinition follow[] = {
			{ new ds::SeparateSettings(OT_FOLLOWER, settings->follower.separationDistance, settings->follower.relaxation) },
			{ new ds::SeekSettings(SID("Player"), settings->follower.seekVelocity) },
		};
		world->createBehavior("follower_follow", follow, 2);

		ds::ConnectionDefinition cd[] = {
			{ SID("follower_start_up"), ds::AT_SCALE_BY_PATH, SID("attach_collider") },
			{ SID("attach_collider"), ds::AT_COLLIDER_ATTACHED, SID("follower_follow") }
		};
		world->connectBehaviors(cd, 2, OT_FOLLOWER);
	}

	// -----------------------------------------------------------
	// follower behavior
	// -----------------------------------------------------------
	void createWiggleFollowerBehavior(ds::World* world, ds::V3Path *path, GameSettings* settings) {
		ds::ActionDefinition startups[] = {
			{ new ds::ScaleByPathActionSettings(path, 1.0f, 0.2f) },
			{ new ds::LookAtSettings(SID("Player"), 0.0f) },
			{ new ds::MoveBySettings(20.0f, 0.2f) }
		};
		world->createBehavior("wiggle_follower_start_up", startups, 3);

		ds::ActionDefinition follow[] = {
			{ new ds::SeparateSettings(OT_WIGGLE_FOLLOWER, settings->follower.separationDistance, settings->follower.relaxation) },
			{ new ds::SeekSettings(SID("Player"), settings->follower.seekVelocity) },
			{ new ds::WiggleSettings(50.0f, 8.0f, 5.0f) },
		};
		world->createBehavior("wiggle_follower_follow", follow, 3);

		ds::ConnectionDefinition cd[] = {
			{ SID("wiggle_follower_start_up"), ds::AT_SCALE_BY_PATH, SID("attach_collider") },
			{ SID("attach_collider"), ds::AT_COLLIDER_ATTACHED, SID("wiggle_follower_follow") }
		};
		world->connectBehaviors(cd, 2, OT_WIGGLE_FOLLOWER);
	}
}

