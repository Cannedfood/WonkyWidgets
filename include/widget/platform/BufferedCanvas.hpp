#pragma once

#include "../Canvas.hpp"
#include "../Bitmap.hpp"

#include <memory>
#include <deque>

namespace widget {

struct DrawBuffer {
	struct Point { float x, y; };
	enum Mode {
		LINES,
		POLYGON,
		TRIANGLES
	};

	Mode                    mode;
	std::shared_ptr<Bitmap> bitmap;
	std::vector<Point>      positions;
	std::vector<Point>      texcoords;
	std::vector<Color>      colors;
};

class BufferedCanvas : public Canvas {
	std::vector<DrawBuffer::Point> mOffsets;
	std::deque<DrawBuffer>         mBuffers;
	DrawBuffer mBuffer;

	void pushBuffer(std::shared_ptr<Bitmap> const& b, DrawBuffer::Mode mode) {
		if(mBuffer.bitmap != b || mBuffer.mode != mode || mode == DrawBuffer::POLYGON) {
			mBuffers.emplace_back(std::move(mBuffer));
			mBuffer.bitmap = b;
		}
	}
public:
	BufferedCanvas() : mOffsets(1, {0, 0}) {}
	~BufferedCanvas() {}


	void pushArea(float x, float y, float w, float h) override {
		mOffsets.push_back({ mOffsets.back().x + x, mOffsets.back().y + y });
	}
	void popArea() override {
		mOffsets.pop_back();
	}

	void outlinePoly(float* points, size_t number, uint32_t color) override {
		pushBuffer(nullptr, DrawBuffer::LINES);
		mBuffer.positions.reserve(number);
		mBuffer.colors.reserve(number);
		Color c = color;
		for(size_t i = 0; i < number; i++) {
			mBuffer.positions.push_back({
				points[i * 2 + 0] + mOffsets.back().x,
				points[i * 2 + 1] + mOffsets.back().y
			});
			mBuffer.colors.emplace_back(color);
		}
	}
	void fillPoly(float* points, size_t number, uint32_t color) override {
		Color c = color;
		pushBuffer(nullptr, DrawBuffer::POLYGON);
		mBuffer.positions.reserve(number);
		mBuffer.colors.reserve(number);
		for(size_t i = 0; i < number; i++) {
			mBuffer.positions.push_back({
				points[i * 2 + 0] + mOffsets.back().x,
				points[i * 2 + 1] + mOffsets.back().y
			});
			mBuffer.colors.emplace_back(c);
		}
	}
	void fillPoly(float* points, float* texcoords, size_t number, std::shared_ptr<Bitmap> const& bitmap, uint32_t tint) override {
		Color c = tint;
		pushBuffer(bitmap, DrawBuffer::POLYGON);
		mBuffer.positions.reserve(number);
		mBuffer.positions.reserve(number);
		mBuffer.colors.reserve(number);
		for(size_t i = 0; i < number; i++) {
			mBuffer.positions.push_back({
				points[i * 2 + 0] + mOffsets.back().x,
				points[i * 2 + 1] + mOffsets.back().y
			});
			mBuffer.texcoords.push_back({
				texcoords[i * 2 + 0],
				texcoords[i * 2 + 1]
			});
			mBuffer.colors.emplace_back(c);
		}
	}
	void fillRects(float* coords, float* texcoords, size_t number, std::shared_ptr<Bitmap> const& bm, uint32_t tint) override {
		pushBuffer(bm, DrawBuffer::TRIANGLES);
		Color c = tint;
		mBuffer.colors.reserve(number * 6);
		mBuffer.positions.reserve(number * 6);
		mBuffer.texcoords.reserve(number * 6);
		for(size_t i = 0; i < number * 4; i += 4) {
			auto vertex = [&](size_t idx) {
				mBuffer.positions.push_back({
					coords[i * 2 + 0 + idx] + mOffsets.back().x,
					coords[i * 2 + 1 + idx] + mOffsets.back().y
				});
				mBuffer.texcoords.push_back({
					texcoords[i * 2 + 0 + idx],
					texcoords[i * 2 + 1 + idx]
				});
				mBuffer.colors.emplace_back(c);
			};
			vertex(0);
			vertex(1);
			vertex(2);
			vertex(2);
			vertex(3);
			vertex(0);
		}
	}

	bool popBuffer(DrawBuffer& bf) {
		if(mBuffers.empty()) return false;
		bf = std::move(mBuffers.front());
		mBuffers.pop_front();
		return true;
	}

	std::shared_ptr<Bitmap> loadTextureNow(uint8_t const* data, unsigned w, unsigned h, unsigned components) override = 0;
};

} // namespace widget
