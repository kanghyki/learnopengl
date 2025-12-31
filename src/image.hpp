#ifndef INCLUDED_IMAGE_HPP
#define INCLUDED_IMAGE_HPP

#include "common.hpp"

class Image {
  public:
    static std::unique_ptr<Image> Load(const std::string& filepath, bool flip_vertical = true);
    static std::unique_ptr<Image> Create(int width, int height, int channel_count = 4);
    static std::unique_ptr<Image> CreateSingleColorImage(int width, int height,
                                                         const glm::vec4& color);
    ~Image();

    inline const uint8_t* data() const { return data_; };
    inline int width() const { return width_; };
    inline int height() const { return height_; };
    inline int channel_count() const { return channel_count_; };

  private:
    Image();
    bool LoadFile(const std::string& filepath, bool flip_vertical);
    bool Allocate(int width, int height, int channel_count);

    int width_{0};
    int height_{0};
    int channel_count_{0};
    uint8_t* data_{nullptr};
};

#endif
