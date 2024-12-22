#ifndef __CHESS_Application_HPP__
#define __CHESS_Application_HPP__
#include <cstdint>
#include "SceneManager.hpp"

class Application
{
public:
    Application();
    ~Application();

    bool Run();
private:
    void HandleInput();
    void Update(float dt);
    void Render();

private:
    bool m_bIsRunning;
    SceneManager m_SceneManager;
};
#endif //!__CHESS_Application_HPP__