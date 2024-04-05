#ifndef INCLUDED_CONTEXT_HPP
#define INCLUDED_CONTEXT_HPP

#include "common.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Shader.hpp"
#include "Program.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Material.hpp"
#include "Framebuffer.hpp"

class Context
{
    public:
        ~Context();
        static std::unique_ptr<Context> create();
        void                            render();
        void                            renderImGui();
        void                            processKeyboardInput(GLFWwindow* window);
        void                            processMouseMove(double x, double y);
        void                            processMouseButton(int button, int action, double x, double y);
        void                            processMouseScroll(double xoffset, double yoffset);
        void                            reshapeViewport(int width, int height);

    private:
        Context();
        bool init();

        glm::mat4   getPerspectiveMatrix() const;

        uint32_t                        mClearBit               { 0 };
        int                             mWidth                  { WINDOW_WIDTH };
        int                             mHeight                 { WINDOW_HEIGHT };

        struct Light                    mLight;

        std::unique_ptr<Program>        mSimpleProgram          { nullptr };
        std::unique_ptr<Program>        mLightingProgram        { nullptr };
        std::unique_ptr<Program>        mPlaneProgram           { nullptr };
        std::unique_ptr<Program>        mEnvMapProgram          { nullptr };
        std::unique_ptr<Program>        mCubeProgram            { nullptr };
        std::unique_ptr<Program>        mTestProgram            { nullptr };

        std::unique_ptr<CubeTexture>    mCubeTexture            { nullptr };
        std::unique_ptr<Texture>        mPlaneTexture           { nullptr };

        std::unique_ptr<Mesh>           mBox                    { nullptr };
        std::unique_ptr<Mesh>           mSphere                 { nullptr };
        std::unique_ptr<Mesh>           mPlane                  { nullptr };
        std::unique_ptr<Model>          mModel                  { nullptr };

        struct Camera                   mCamera;
        glm::vec2                       mPrevMousePos           { 0.0f };
        bool                            mCameraDirectionControl { false };
        float                           mNear                   { 0.1f };
        float                           mFar                    { 100.0f };

        std::unique_ptr<Framebuffer>    mFramebuffer            { nullptr };
        std::unique_ptr<Program>        mPostProgram            { nullptr };
        float                           mGamma                  { 1.0f };

        glm::vec4                       mClearColor             { 0.3f, 0.3f, 0.3f, 1.0f };

        bool                            mIsWireframeActive      { false };
        bool                            mIsAnimationActive      { true };
        int                             mLightType              { 0 };
        int                             mImGuiImageSize         { 300 };
};

#endif
