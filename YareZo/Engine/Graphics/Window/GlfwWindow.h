#ifndef YAREZO_GLFWWINDOW_H
#define YAREZO_GLFWWINDOW_H

#include "Core/Glfw.h"
#include "Graphics/Window/YzWindow.h"

namespace Yarezo::Graphics {
    class GlfwWindow : public YzWindow {
    public:
        GlfwWindow(WindowProperties& properties);
        virtual ~GlfwWindow();

        virtual void onUpdate() override;
        virtual bool shouldClose() override;
        virtual void close() override;
        virtual void releaseInputHandling() override;

    private:
        virtual void setIcon(const std::string& filePath);

        inline virtual void* getNativeWindow() const override { return m_Window; };

        void init();
        GLFWwindow* m_Window;
    };
}

#endif //YAREZO_GLFWWINDOW_H
