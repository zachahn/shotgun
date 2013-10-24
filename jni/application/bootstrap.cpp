/* This file is part of the Zenipex Library (zenilib).
 * Copyleft (C) 2011 Mitchell Keith Bloch (bazald).
 *
 * This source file is simply under the public domain.
 */

#include <zenilib.h>

#include "Crate_State.h"

#if defined(_DEBUG) && defined(_WINDOWS)
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

using namespace std;
using namespace Zeni;

class Instructions_State : public Gamestate_Base {
  Text_Box tb;

public:
  Instructions_State()
    : tb(Point2f(), Point2f(800.0f, 600.0f), "system_36_800x600",
    "This demonstrates a simple way of doing collision detection and response in 3D. "
    "Unfortunately, it is inadequate for more sophisticated scenarios.", Color())
  {
    tb.give_BG_Renderer(new Widget_Renderer_Color(get_Colors()["black"]));
  }

private:
  void on_key(const SDL_KeyboardEvent &event) {
    if(event.keysym.sym == SDLK_ESCAPE) {
      if(event.state == SDL_PRESSED)
        get_Game().pop_state();
    }
  }

  void render() {
    get_Video().set_2d(make_pair(Point2f(), Point2f(800.0f, 600.0f)), true);

    tb.render();
  }
};

class Bootstrap {
  class Gamestate_One_Initializer : public Gamestate_Zero_Initializer {
    virtual Gamestate_Base * operator()() {
      Window::set_title("zenilib Crate Example");

      get_Controllers();
      get_Video();
      get_Textures();
      get_Fonts();
      get_Sounds();

      return new Title_State<Crate::Crate_State, Instructions_State>("zenilib Example:\nCrate");
    }
  } m_goi;

public:
  Bootstrap() {
    g_gzi = &m_goi;
  }
} g_bootstrap;

int main(int argc, char **argv) {
  return zenilib_main(argc, argv);
}
