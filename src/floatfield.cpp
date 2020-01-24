#include "floatfield.h"

namespace mapmaker {
FloatField::FloatField() : _width(0), _height(0) {}

float &FloatField::operator()(int x, int y) {
  return _data[x + _width * y];
}

const float &FloatField::operator()(int x, int y) const {
  return _data[x + _width * y];
}

size_t FloatField::width() const {
  return _width;
}

size_t FloatField::height() const {
  return _height;
}

void FloatField::clear(float value) {
  for (size_t i = 0; i < _data.size(); ++i) {
    _data[i] = value;
  }
}

void FloatField::resize(size_t width, size_t height) {
  _data.resize(width * height);
  _width = width;
  _height = height;
}


}  // namespace mapmaker
