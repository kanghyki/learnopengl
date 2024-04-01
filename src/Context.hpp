#ifndef INCLUDED_CONTEXT_HPP
#define INCLUDED_CONTEXT_HPP

#include "common.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Shader.hpp"
#include "Program.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include "Model.hpp"

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

        int                             mWidth                  { WINDOW_WIDTH };
        int                             mHeight                 { WINDOW_HEIGHT };
        int                             mGUIx                   { 0 };
        int                             mGUIy                   { 0 };
        int                             mGUIwidth               { 0 };
        int                             mGUIheight              { 0 };
        bool                            mIsWindowUpdated        { false };
        double                          mAnimationTime          { 0.0f };

        bool                            mIsEnableDepthBuffer    { true };
        bool                            mIsWireframeActive      { false };
        bool                            mIsAnimationActive      { true };
        int                             mLightType              { 0 };

        struct Light                    mLight;
        std::unique_ptr<Program>        mProgram                { nullptr };
        std::unique_ptr<Program>        mSimpleProgram          { nullptr };
        std::unique_ptr<Mesh>           mMesh                   { nullptr };
        struct Material                 mMaterial;
        std::unique_ptr<Model>          mModel                  { nullptr };

        struct Camera                   mCamera;
        glm::vec2                       mPrevMousePos           { 0.0f };
        bool                            mCameraDirectionControl { false };

        glm::vec4                       mClearColor             { 0.0f, 0.0f, 0.0f, 1.0f };
};

#endif
