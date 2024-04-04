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

class Coord
{
    public:
        Coord() {
            std::vector<GLfloat> vertices({
                0.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f,
                -1.0f, 0.0f, 0.0f,
                0.0f, -1.0f, 0.0f,
                0.0f, 0.0f, -1.0f
            });
            std::vector<uint32_t> indices({
                0, 1,
                0, 2,
                0, 3,
                0, 4,
                0, 5,
                0, 6,
            });
            mVertexArray = VertexArray::create();
            mVertexBuffer = Buffer::create(
                GL_ARRAY_BUFFER, GL_STATIC_DRAW,
                vertices.data(), sizeof(GLfloat) * 3,
                vertices.size());
            mIndexBuffer = Buffer::create(
                GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
                indices.data(), sizeof(uint32_t),
                indices.size());
            mVertexArray->setAttrib(0, 3, GL_FLOAT, false, sizeof(GLfloat) * 3, 0);
        }
        ~Coord() {};

        void draw() const {
            mVertexArray->bind();
            glDrawElements(GL_LINES, mIndexBuffer->getCount(), GL_UNSIGNED_INT, 0);
        }

    private:
        std::unique_ptr<VertexArray>    mVertexArray;
        std::shared_ptr<Buffer>         mVertexBuffer;
        std::shared_ptr<Buffer>         mIndexBuffer;

};

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

        Coord                           mCoord;
        std::unique_ptr<Program>        mCoordProgram           { nullptr };
        bool                            mIsCoord                { true };


        struct Light                    mLight;
        std::unique_ptr<Program>        mSimpleProgram          { nullptr };

        std::unique_ptr<Program>        mProgram                { nullptr };
        std::unique_ptr<Mesh>           mBox                    { nullptr };
        std::unique_ptr<Mesh>           mSphere                 { nullptr };
        std::unique_ptr<Model>          mModel                  { nullptr };

        struct Camera                   mCamera;
        glm::vec2                       mPrevMousePos           { 0.0f };
        bool                            mCameraDirectionControl { false };

        glm::vec4                       mClearColor             { 0.3f, 0.3f, 0.3f, 1.0f };
};

#endif
