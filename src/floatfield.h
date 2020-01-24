#ifndef FLOATFIELD_H
#define FLOATFIELD_H

#include <vector>

namespace mapmaker {
class FloatField {
 public:
  FloatField();
  float &operator()(int x, int y);
  const float &operator()(int x, int y) const;

  size_t width() const;
  size_t height() const;

  void clear(float value = 0);
  void resize(size_t width, size_t height);

 private:
  std::vector<float> _data;
  size_t _width;
  size_t _height;
};
}  // namespace mapmaker

#endif  // FLOATFIELD_H
