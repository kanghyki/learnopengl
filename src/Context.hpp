#ifndef INCLUDED_CONTEXT_HPP
#define INCLUDED_CONTEXT_HPP

#include "common.hpp"
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
        void                            processMouseScroll(double xoffset, double yoffset);
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
        bool                            mCameraDirectionControl;
        
        glm::vec3 m_lightColor { glm::vec3(1.0f, 1.0f, 1.0f) };
        glm::vec3 m_objectColor { glm::vec3(1.0f, 0.5f, 0.0f) };
        float m_ambientStrength { 0.5f };

        glm::vec4                       mClearColor;
};

#endif
