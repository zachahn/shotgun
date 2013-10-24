/* This file is part of the Zenipex Library.
* Copyleft (C) 2011 Mitchell Keith Bloch a.k.a. bazald
*
* This source file is simply under the public domain.
*/

#include <zenilib.h>

#include "Tutorial_State.h"

using namespace std;
using namespace Zeni;

Tutorial_State::Tutorial_State()
: m_time_passed(0.0f),
m_max_time_step(1.0f / 20.0f), // make the largest physics step 1/20 of a second
m_max_time_steps(10.0f) // allow no more than 10 physics steps per frame
{
	set_pausable(true);
}

Tutorial_State::~Tutorial_State() {
}

void Tutorial_State::on_push() {
	get_Window().set_mouse_state(Window::MOUSE_RELATIVE);
}

void Tutorial_State::on_pop() {
	get_Controllers().reset_vibration_all();
}

void Tutorial_State::on_cover() {
	get_Controllers().reset_vibration_all();
}

void Tutorial_State::on_controller_axis(const SDL_ControllerAxisEvent &event) {
	switch(event.axis) {
	case SDL_CONTROLLER_AXIS_LEFTX:
		m_controls.lright = (event.value + 0.5f) / 32767.5f;
		break;

	case SDL_CONTROLLER_AXIS_LEFTY:
		m_controls.lup = (event.value + 0.5f) / 32767.5f;
		break;

	case SDL_CONTROLLER_AXIS_RIGHTX:
		m_controls.rright = (event.value + 0.5f) / 32767.5f;
		break;

	case SDL_CONTROLLER_AXIS_RIGHTY:
		m_controls.rrup = (event.value + 0.5f) / 32767.5f;
		break;

	default:
		break;
	}
}

void Tutorial_State::on_controller_button(const SDL_ControllerButtonEvent &event) {
	if(event.button == SDL_CONTROLLER_BUTTON_BACK && event.state == SDL_PRESSED)
		get_Game().push_Popup_Menu_State();
}

void Tutorial_State::on_key(const SDL_KeyboardEvent &event) {
	switch(event.keysym.sym) {
		case SDLK_w:
			m_controls.up = event.type == SDL_KEYDOWN;
			break;

		case SDLK_a:
			m_controls.left = event.type == SDL_KEYDOWN;
			break;

		case SDLK_s:
			m_controls.down = event.type == SDL_KEYDOWN;
			break;

		case SDLK_d:
			m_controls.right = event.type == SDL_KEYDOWN;
			break;

		default:
			Gamestate_Base::on_key(event);
	}
}

void Tutorial_State::on_mouse_motion(const SDL_MouseMotionEvent &event) {
	turn_camera(Point2f(-event.xrel / m_player.mouse_scale.i, event.yrel / m_player.mouse_scale.j));
}

void Tutorial_State::perform_logic() {
	{// Update time_passed
		const Time temp_time = get_Timer().get_time();
		m_time_passed += temp_time.get_seconds_since(m_current_time);
		m_current_time = temp_time;
	}

	/* Shrink time passed to an upper bound
	 *
	 * If your program is ever paused by your computer for 10 
	 * minutes and then allowed to continue running, you don't want 
	 * it to pause at the physics loop for another 10 minutes.
	 */
	if(m_time_passed / m_max_time_step > m_max_time_steps)
		m_time_passed = m_max_time_steps * m_max_time_step;

	/* Constant time-step physics update
	 *
	 * This type of update increases the stability of your program 
	 * in scenarios where the time step could allow things to move 
	 * further than you expect, potentially jumping though walls or 
	 * falling through floors... that sort of thing.  Keep the 
	 * max_time_step small and you are safe.
	 */
	while(m_time_passed > m_max_time_step) {
		m_time_passed -= m_max_time_step;
		step(m_max_time_step);
	}

	/* Simple physics update
	 *
	 * This type of update could be the only update you do if you 
	 * don't have concerns with respect to jumping though walls, 
	 * etc...
	 *
	 * Otherwise, if you have the other update, this serves to 
	 * improve the smoothness of your animations.  With the other 
	 * version alone, things can jerk a bit from frame to frame.
	 *
	 * To really see the improvement, turn up multisampling and try 
	 * it with this section (A) commented out, and then (B) not
	 * commented out.
	 */
	{
		step(m_time_passed);
		m_time_passed = 0.0f;
	}
}

void Tutorial_State::turn_camera(const Zeni::Point2f &amount) {
	Camera &camera = m_player.camera;

	camera.turn_left_xy(amount.x);
	
	// Back up a couple vectors
	const Quaternion prev_orientation = camera.orientation;
	const Vector3f prev_up = camera.get_up();

	camera.adjust_pitch(amount.y);

	/* Restore the backup vectors if flipped over
	 * (if the up vector is pointing down)
	 *
	 * Note that we want to be sure not to freeze a player 
	 * who is already flipped (for whatever reason).
	 */
	if(camera.get_up().k < 0.0f && prev_up.k >= 0.0f)
		camera.orientation = prev_orientation;
}

void Tutorial_State::step(const float &time_step) {
	if(m_controls.left - m_controls.right)
		m_player.camera.move_left_xy(m_player.max_velocity.i * time_step * (m_controls.left - m_controls.right));
	else
		m_player.camera.move_left_xy(m_player.max_velocity.i * time_step * -m_controls.lright);

	if(m_controls.up - m_controls.down)
		m_player.camera.move_forward_xy(m_player.max_velocity.j * time_step * (m_controls.up - m_controls.down));
	else
		m_player.camera.move_forward_xy(m_player.max_velocity.j * time_step * -m_controls.lup);

	turn_camera(Point2f(-m_controls.rright / m_player.controller_scale.i, m_controls.rrup / m_player.controller_scale.j));
}

void Tutorial_State::render() {
	Video &vr = get_Video();

	vr.set_3d(m_player.camera);

	Vertex3f_Texture p0(m_rect.position,                             Point2f(0.0f, 0.0f));
	Vertex3f_Texture p1(m_rect.position               + m_rect.vec1, Point2f(0.0f, 1.0f));
	Vertex3f_Texture p2(m_rect.position + m_rect.vec0 + m_rect.vec1, Point2f(1.0f, 1.0f)); 
	Vertex3f_Texture p3(m_rect.position + m_rect.vec0,               Point2f(1.0f, 0.0f));
	Material material("logo");

	Quadrilateral<Vertex3f_Texture> quad(p0, p1, p2, p3);
	quad.fax_Material(&material);

	vr.render(quad);
}
