#ifndef INCLUDED_CONTEXT_HPP
#define INCLUDED_CONTEXT_HPP

#include "Common.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Program.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"

class Context
{
    public:
        ~Context();
        static std::unique_ptr<Context> create();
        void                            render();
        void                            processKeyboardInput(GLFWwindow* window);
        void                            processMouseMove(double x, double y);
        void                            processMouseButton(int button, int action, double x, double y);
        void                            reshape(int width, int height);

    private:
        Context();
        bool init();

        int                             mWidth, mHeight;
        int                             mFragType;
        bool                            mIsEnableDepthBuffer;
        bool                            mIsActiveWireFrame;
        std::unique_ptr<Program>        mProgram;
        std::unique_ptr<Buffer>         mVBO, mEBO;
        std::unique_ptr<VertexArray>    mVAO;
        std::unique_ptr<Texture>        mTexture;
        std::unique_ptr<Texture>        mTexture2;

        struct Camera                   mCamera;
        glm::vec2                       mPrevMousePos;
        bool                            mCameraControl;

        glm::vec4                       mClearColor;
};

#endif
