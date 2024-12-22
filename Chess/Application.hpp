#ifndef __CHESS_APPLICATION_HPP__
#define __CHESS_APPLICATION_HPP__
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
#endif //!__CHESS_APPLICATION_HPP__