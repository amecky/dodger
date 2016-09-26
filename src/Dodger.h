#pragma comment(lib, "D11.lib")
#pragma comment(lib, "DieselCore.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxerr.lib")
#pragma warning(disable : 4995)

#pragma once
#include "base\BaseApp.h"
#include <renderer\render_types.h>

struct GameContext;

class Dodger : public ds::BaseApp {


public:	
	Dodger();
	virtual ~Dodger();
	bool initialize() {
		return true;
	}
	bool loadContent();
	const char* getTitle() {
		return "Dodger";
	}
	void init();
	void update(float dt);
	void render();
	//void draw();
	//void onGUIButton(ds::DialogID dlgID, int button);
protected:
	void prepare(ds::Settings* settings);
private:
	GameContext* _context;
};