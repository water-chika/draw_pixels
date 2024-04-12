#include "draw_pixels.hpp"
#include <vector>

int main() {
	std::vector<uint32_t> pixels(800 * 600);
	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			pixels[i * 800 + j] = 0xffff00;
		}
	}
	water::draw_pixels(800, 600, pixels.data());
	return 0;
}