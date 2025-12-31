#ifndef INCLUDED_CAMERA_HPP
#define INCLUDED_CAMERA_HPP

#include "common.hpp"

enum CameraMove {
    kNone = 1 << 0,
    kFront = 1 << 1,
    kBack = 1 << 2,
    kLeft = 1 << 3,
    kRight = 1 << 4,
    kUp = 1 << 5,
    kDown = 1 << 6,
};

struct Camera {
    glm::mat4 GetViewMatrix() const {
        front_ = glm::rotate(glm::mat4(1.0f), glm::radians(yaw_), glm::vec3(0.0f, 1.0f, 0.0f)) *
                 glm::rotate(glm::mat4(1.0f), glm::radians(pitch_), glm::vec3(1.0f, 0.0f, 0.0f)) *
                 glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

        return glm::lookAt(position_, position_ + front_, up_);
    }

    glm::mat4 GetPerspectiveProjectionMatrix() const {
        return glm::perspective(glm::radians(fov_y_), aspect_, near_plane_, far_plane_);
    }

    inline void SetMove(CameraMove type) { move_status_ |= type; }
    inline void UnsetMove(CameraMove type) { move_status_ &= ~type; }
    void Move() {
        if (move_status_ == kNone)
            return;
        if (move_status_ & kFront)
            position_ += move_speed_ * front_;
        if (move_status_ & kBack)
            position_ -= move_speed_ * front_;
        glm::vec3 cameraRight = glm::normalize(glm::cross(up_, -front_));
        if (move_status_ & kLeft)
            position_ -= move_speed_ * cameraRight;
        if (move_status_ & kRight)
            position_ += move_speed_ * cameraRight;
        glm::vec3 cameraUp = glm::cross(-front_, cameraRight);
        if (move_status_ & kUp)
            position_ += move_speed_ * cameraUp;
        if (move_status_ & kDown)
            position_ -= move_speed_ * cameraUp;
    }

    void Rotate(glm::vec2 delta) {
        yaw_ -= delta.x * rot_speed_;
        pitch_ -= delta.y * rot_speed_;

        if (yaw_ < 0.0f)
            yaw_ += 360.0f;
        if (yaw_ > 360.0f)
            yaw_ -= 360.0f;

        if (pitch_ > 89.0f)
            pitch_ = 89.0f;
        if (pitch_ < -89.0f)
            pitch_ = -89.0f;
    }

    void ChangeAspect(int width, int height) { aspect_ = (float)width / (float)height; }

    void Reset() {
        pitch_ = {0.0f};
        yaw_ = {0.0f};
        position_ = {0.0f, 0.0f, 3.0f};
        front_ = {0.0f, 0.0f, -1.0f};
        up_ = {0.0f, 1.0f, 0.0f};
    }

    float pitch_{0.0f};
    float yaw_{0.0f};
    float fov_y_{45.0f};
    float aspect_{16.0f / 9.0f};
    float near_plane_{0.1f};
    float far_plane_{200.0f};
    unsigned char move_status_{0};
    float move_speed_{0.1f};
    float rot_speed_{0.15f};
    glm::vec3 position_{0.0f, 1.5f, 5.0f};
    mutable glm::vec3 front_{0.0f, 0.0f, -1.0f};
    glm::vec3 up_{0.0f, 1.0f, 0.0f};
};

#endif
