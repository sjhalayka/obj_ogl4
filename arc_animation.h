#pragma once


#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtx/string_cast.hpp"
using namespace glm;

#include <chrono>
using namespace std;

class arc_animation
{
public:

	size_t player_index;

	vec3 start_location, end_location;
	size_t end_cell_x, end_cell_y;

	vec3 curr_pos(void)
	{
		std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float, std::milli> elapsed = end_time - start_time;
		
		float e = elapsed.count() / 1000.0f;

		float t = 0;

		if (e >= duration)
			t = 1;
		else
			t = glm::clamp(e/duration, 0.0f, 1.0f);

		vec3 m = glm::mix(start_location, end_location, t);

		float y_offset = sin(t * glm::pi<float>());
		m.y += y_offset;

		return m;
	}

	std::chrono::high_resolution_clock::time_point start_time;
	float duration; // in seconds
};