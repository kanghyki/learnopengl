#ifndef INCLUDED_CONTEXT_HPP
#define INCLUDED_CONTEXT_HPP

#include "common.hpp"
#include "Camera.hpp"
#include "Light.hpp"
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
        void                            updateImGui();
        void                            processKeyboardInput(GLFWwindow* window);
        void                            processMouseMove(double x, double y);
        void                            processMouseButton(int button, int action, double x, double y);
        void                            processMouseScroll(double xoffset, double yoffset);
        void                            updateWindowSize(int width, int height);
        void                            updateGUIwindow(int x, int y, int width, int height);

    private:
        Context();
        bool init();

        // window
        int                             mWidth                  { WINDOW_WIDTH };
        int                             mHeight                 { WINDOW_HEIGHT };
        int                             mGUIx                   { 0 };
        int                             mGUIy                   { 0 };
        int                             mGUIwidth               { 0 };
        int                             mGUIheight              { 0 };
        bool                            mIsWindowUpdated        { false };
        double                          mAnimationTime          { 0.0f };

        // ImGui
        int                             mFragType               { 0 };
        bool                            mIsEnableDepthBuffer    { true };
        bool                            mIsWireframeActive      { false };
        bool                            mIsAnimationActive      { true };

        // render
        std::unique_ptr<Program>        mProgram                { nullptr };
        std::unique_ptr<Program>        mSimpleProgram          { nullptr };
        std::unique_ptr<Buffer>         mVBO, mEBO              { nullptr };
        std::unique_ptr<VertexArray>    mVAO                    { nullptr };
        std::unique_ptr<Texture>        mTexture                { nullptr };
        std::unique_ptr<Texture>        mTexture2               { nullptr };

        // camera
        struct Camera                   mCamera;
        glm::vec2                       mPrevMousePos           { 0.0f };
        bool                            mCameraDirectionControl { false };
        
        // lighting
        struct Light                    mLight;
        struct Material                 mMaterial;

        glm::vec4                       mClearColor             { 0.2f, 0.3f, 0.3f, 1.0f };
};

#endif
