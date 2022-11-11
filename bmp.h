#ifndef BMP_H
#define BMP_H

#include <atomic>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <string>
#include <iostream>
#include <chrono>
#include <sstream>
using namespace std;

class RGB_uchar
{
public:
	unsigned char r, g, b;
};

class BMP
{
public:
	std::uint32_t width, height;
	std::uint16_t BitsPerPixel;
	std::vector<unsigned char> Pixels;

public:
	BMP(void)
	{

	}


	bool load(const char* FilePath, bool swap_red_and_blue = false)
	{
		std::fstream hFile(FilePath, std::ios::in | std::ios::binary);
		if (!hFile.is_open()) return false;

		hFile.seekg(0, std::ios::end);
		std::size_t Length = hFile.tellg();
		hFile.seekg(0, std::ios::beg);
		std::vector<std::uint8_t> FileInfo(Length);
		hFile.read(reinterpret_cast<char*>(FileInfo.data()), 54);

		if (FileInfo[0] != 'B' && FileInfo[1] != 'M')
		{
			hFile.close();
			return false;
		}

		// if (FileInfo[28] != 24 && FileInfo[28] != 32)
		if (FileInfo[28] != 32)
		{
			hFile.close();
			return false;
		}

		BitsPerPixel = FileInfo[28];
		width = FileInfo[18] + (FileInfo[19] << 8);
		height = FileInfo[22] + (FileInfo[23] << 8);
		std::uint32_t PixelsOffset = FileInfo[10] + (FileInfo[11] << 8);
		std::uint32_t size = ((width * BitsPerPixel + 31) / 32) * 4 * height;
		Pixels.resize(size);

		hFile.seekg(PixelsOffset, std::ios::beg);
		hFile.read(reinterpret_cast<char*>(Pixels.data()), size);
		hFile.close();


		// Reverse row order
		short unsigned int num_rows_to_swap = height;
		vector<unsigned char> buffer(static_cast<size_t>(width) * 4);

		if (0 != height % 2)
			num_rows_to_swap--;

		num_rows_to_swap /= 2;

		for (size_t i = 0; i < num_rows_to_swap; i++)
		{
			size_t y_first = i * static_cast<size_t>(width) * 4;
			size_t y_last = (static_cast<size_t>(height) - 1 - i) * static_cast<size_t>(width) * 4;

			memcpy(&buffer[0], &Pixels[y_first], static_cast<size_t>(width) * 4);
			memcpy(&Pixels[y_first], &Pixels[y_last], static_cast<size_t>(width) * 4);
			memcpy(&Pixels[y_last], &buffer[0], static_cast<size_t>(width) * 4);
		}

		if (swap_red_and_blue)
		{
			for (size_t i = 0; i < width; i++)
			{
				for (size_t j = 0; j < height; j++)
				{
					size_t index = 4 * (j * width + i);

					unsigned char temp_char;
					temp_char = Pixels[index + 0];
					Pixels[index + 0] = Pixels[index + 2];
					Pixels[index + 2] = temp_char;
				}
			}
		}



		return true;
	}


	std::vector<std::uint8_t> GetPixels() const { return this->Pixels; }
	std::uint32_t GetWidth() const { return this->width; }
	std::uint32_t GetHeight() const { return this->height; }
	bool HasAlphaChannel() { return BitsPerPixel == 32; }
};




void slice_tiles(const BMP& src_bmp, vector< vector<GLubyte> >& tile_data, size_t num_tiles, size_t image_width, size_t image_height, size_t tile_width, size_t tile_height)
{
	tile_data.clear();

	const size_t num_tiles_wide = image_width / tile_width;
	const size_t num_tiles_high = image_height / tile_height;

	size_t tile_index = 0;

	vector<unsigned char> tile_template(4 * tile_width * tile_height);
	tile_data.resize(num_tiles, tile_template);

	for (size_t i = 0; i < num_tiles_wide; i++)
	{
		for (size_t j = 0; j < num_tiles_high; j++)
		{
			size_t left = i * tile_width;
			size_t right = left + tile_width - 1;
			size_t top = j * tile_height;
			size_t bottom = top + tile_height - 1;

			for (size_t k = left, x = 0; k <= right; k++, x++)
			{
				for (size_t l = top, y = 0; l <= bottom; l++, y++)
				{
					size_t img_pos = 4 * (k * image_height + l);
					size_t sub_pos = 4 * (x * tile_height + y);

					tile_data[tile_index][sub_pos + 0] = src_bmp.Pixels[img_pos + 0];
					tile_data[tile_index][sub_pos + 1] = src_bmp.Pixels[img_pos + 1];
					tile_data[tile_index][sub_pos + 2] = src_bmp.Pixels[img_pos + 2];
					tile_data[tile_index][sub_pos + 3] = src_bmp.Pixels[img_pos + 3];
				}
			}

			tile_index++;
		}
	}
}




#endif