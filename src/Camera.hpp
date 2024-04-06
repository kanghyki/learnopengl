#ifndef INCLUDED_CAMERA_HPP
#define INCLUDED_CAMERA_HPP

#include "common.hpp"

enum eCameraMove {
  NONE = 1 << 0,
  FRONT = 1 << 1,
  BACK = 1 << 2,
  LEFT = 1 << 3,
  RIGHT = 1 << 4,
  UP = 1 << 5,
  DOWN = 1 << 6,
};

struct Camera {
  glm::mat4 getViewMatrix() {
    mFront = glm::rotate(glm::mat4(1.0f), glm::radians(mYaw),
                         glm::vec3(0.0f, 1.0f, 0.0f)) *
             glm::rotate(glm::mat4(1.0f), glm::radians(mPitch),
                         glm::vec3(1.0f, 0.0f, 0.0f)) *
             glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    return glm::lookAt(mPos, mPos + mFront, mUp);
  }

  glm::mat4 getPerspectiveProjectionMatrix() const {
    return glm::perspective(glm::radians(mFovY), mAspect, mNearPlane,
                            mFarPlane);
  }

  void move() {
    if (mMoveStatus == NONE) return;
    if (mMoveStatus & FRONT) mPos += mMoveSpeed * mFront;
    if (mMoveStatus & BACK) mPos -= mMoveSpeed * mFront;
    glm::vec3 cameraRight = glm::normalize(glm::cross(mUp, -mFront));
    if (mMoveStatus & LEFT) mPos -= mMoveSpeed * cameraRight;
    if (mMoveStatus & RIGHT) mPos += mMoveSpeed * cameraRight;
    glm::vec3 cameraUp = glm::cross(-mFront, cameraRight);
    if (mMoveStatus & UP) mPos += mMoveSpeed * cameraUp;
    if (mMoveStatus & DOWN) mPos -= mMoveSpeed * cameraUp;
  }

  void setMove(eCameraMove type) { mMoveStatus |= type; }
  void unsetMove(eCameraMove type) { mMoveStatus &= ~type; }

  void rotate(glm::vec2 delta) {
    mYaw -= delta.x * mRotSpeed;
    mPitch -= delta.y * mRotSpeed;

    if (mYaw < 0.0f) mYaw += 360.0f;
    if (mYaw > 360.0f) mYaw -= 360.0f;

    if (mPitch > 89.0f) mPitch = 89.0f;
    if (mPitch < -89.0f) mPitch = -89.0f;
  }

  void changeAspect(int width, int height) {
    mAspect = (float)width / (float)height;
  }

  void reset() {
    mPitch = {0.0f};
    mYaw = {0.0f};
    mPos = {0.0f, 0.0f, 3.0f};
    mFront = {0.0f, 0.0f, -1.0f};
    mUp = {0.0f, 1.0f, 0.0f};
  }

  void setMoveSpeed(float speed) { mMoveSpeed = speed; }

  float mPitch{0.0f};
  float mYaw{0.0f};
  float mFovY{45.0f};
  float mAspect{16.0f / 9.0f};
  float mNearPlane{0.1f};
  float mFarPlane{100.0f};
  unsigned char mMoveStatus{0};
  float mMoveSpeed{0.05f};
  float mRotSpeed{0.15f};

  glm::vec3 mPos{0.0f, 1.5f, 5.0f};
  glm::vec3 mFront{0.0f, 0.0f, -1.0f};
  glm::vec3 mUp{0.0f, 1.0f, 0.0f};
};

#endif
