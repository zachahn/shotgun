#include <zenilib.h>

#include "Crate_State.h"

using namespace Zeni;
using namespace Zeni::Collision;

namespace Crate {

  Crate_State::Crate_State()
    : m_crate(Point3f(12.0f, 12.0f, 0.0f),
              Vector3f(30.0f, 30.0f, 30.0f)),
    m_player(Camera(Point3f(0.0f, 0.0f, 50.0f),
             Quaternion(),
             1.0f, 10000.0f),
             Vector3f(0.0f, 0.0f, -39.0f),
             11.0f)
  {
    set_pausable(true);
  }

  void Crate_State::on_push() {
    get_Window().set_mouse_state(Window::MOUSE_RELATIVE);
  }

  void Crate_State::on_key(const SDL_KeyboardEvent &event) {
    switch(event.keysym.sym) {
      case SDLK_w:
        m_controls.forward = event.type == SDL_KEYDOWN;
        break;

      case SDLK_a:
        m_controls.left = event.type == SDL_KEYDOWN;
        break;

      case SDLK_s:
        m_controls.back = event.type == SDL_KEYDOWN;
        break;

      case SDLK_d:
        m_controls.right = event.type == SDL_KEYDOWN;
        break;

      case SDLK_SPACE:
        if(event.type == SDL_KEYDOWN) {
          m_player.jump();
          m_moved = true;
        }
        break;

      default:
        Gamestate_Base::on_key(event);
        break;
    }
  }

  void Crate_State::on_mouse_motion(const SDL_MouseMotionEvent &event) {
    m_player.adjust_pitch(event.yrel / 500.0f);
    m_player.turn_left_xy(-event.xrel / 500.0f);
  }

  void Crate_State::perform_logic() {
    const Time_HQ current_time = get_Timer_HQ().get_time();
    float processing_time = float(current_time.get_seconds_since(time_passed));
    time_passed = current_time;

    /** Get forward and left vectors in the XY-plane **/
    const Vector3f forward = m_player.get_camera().get_forward().get_ij().normalized();
    const Vector3f left = m_player.get_camera().get_left().get_ij().normalized();

    /** Get velocity vector split into a number of axes **/
    const Vector3f velocity = (m_controls.forward - m_controls.back) * 50.0f * forward
                            + (m_controls.left - m_controls.right) * 50.0f * left;
    const Vector3f x_vel = velocity.get_i();
    const Vector3f y_vel = velocity.get_j();
    Vector3f z_vel = m_player.get_velocity().get_k();

    /** Bookkeeping for sound effects **/
    if(velocity.magnitude() != 0.0f)
      m_moved = true;

    /** Keep delays under control (if the program hangs for some time, we don't want to lose responsiveness) **/
    if(processing_time > 0.1f)
      processing_time = 0.1f;

    /** Physics processing loop**/
    for(float time_step = 0.05f;
        processing_time > 0.0f;
        processing_time -= time_step)
    {
      if(time_step > processing_time)
        time_step = processing_time;

      /** Gravity has its effect **/
      z_vel -= Vector3f(0.0f, 0.0f, 50.0f * time_step);

      /** Try to move on each axis **/
      partial_step(time_step, x_vel);
      partial_step(time_step, y_vel);
      partial_step(time_step, z_vel);

      /** Keep player above ground; Bookkeeping for jumping controls **/
      const Point3f &position = m_player.get_camera().position;
      if(position.z < 50.0f) {
        m_player.set_position(Point3f(position.x, position.y, 50.0f));
        m_player.set_on_ground(true);
      }
    }
  }

  void Crate_State::render() {
    Video &vr = get_Video();

    vr.set_3d(m_player.get_camera());

    m_crate.render();
  }

  void Crate_State::partial_step(const float &time_step, const Vector3f &velocity) {
    m_player.set_velocity(velocity);
    const Point3f backup_position = m_player.get_camera().position;

    m_player.step(time_step);

    /** If collision with the crate has occurred, roll things back **/
    if(m_crate.get_body().intersects(m_player.get_body())) {
      if(m_moved)
      {
        /** Play a sound if possible **/
        m_crate.collide();
        m_moved = false;
      }

      m_player.set_position(backup_position);

      /** Bookkeeping for jumping controls **/
      if(velocity.k < 0.0f)
        m_player.set_on_ground(true);
    }
  }

}
