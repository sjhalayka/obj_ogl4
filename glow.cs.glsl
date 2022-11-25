// OpenGL 4.3 introduces compute shaders
#version 430

layout(local_size_x = 1, local_size_y = 1) in;

layout(binding = 0, rgba32f) writeonly uniform image2D output_image;
layout(binding = 1, rgba32f) readonly uniform image2D inputa_image;
layout(binding = 2, rgba32f) readonly uniform image2D inputb_image;


void main()
{
	// Get global coordinates
	const ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	const vec3 output_pixel = imageLoad(inputa_image, pixel_coords).rgb + 0.75*imageLoad(inputb_image, pixel_coords).rgb;

	imageStore(output_image, pixel_coords, vec4(output_pixel, 1.0));
}
