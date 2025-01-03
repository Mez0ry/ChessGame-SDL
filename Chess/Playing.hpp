#ifndef __PLAYING_SCENE_HPP__
#define __PLAYING_SCENE_HPP__
#include "GameScene.hpp"

class Renderer;
class Window;

class Playing : public GameScene{
private:
    SceneManager& m_SceneManager;
    Core::Ref<Renderer> m_Renderer;
    Core::Ref<Window> m_Window;
public:
    Playing(const Core::Ref<Renderer> renderer, const Core::Ref<Window> window, SceneManager& scene_manager);
    ~Playing();
    
    void OnCreate() override;
    void OnResize([[maybe_unused]] const Core::Ref<Window> window) override;
    void OnDestroy() override;

    void HandleInput([[maybe_unused]] const Core::Ref<EventHandler> event_handler) override;
    void Update(float dt) override;
    void Render(const Core::Ref<Renderer> renderer) override;
private:
};

#endif //!__PLAYING_SCENE_HPP__