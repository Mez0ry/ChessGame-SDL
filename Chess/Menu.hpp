#ifndef __MENU_HPP__
#define __MENU_HPP__
#include "GameScene.hpp"
#include "GUI/Button.hpp"
#include "Text.hpp"

class Window;
class Renderer;

class Menu : public GameScene {
private:
  Core::Ref<Renderer> m_Renderer;
  SceneManager &m_SceneManager;
public:
  Menu(const Core::Ref<Renderer> renderer, const Core::Ref<Window> window,SceneManager &scene_manager);
  ~Menu() = default;

  void OnCreate() override;
  void OnResize([[maybe_unused]] const Core::Ref<Window> window) override;
  void OnDestroy() override;

  void HandleInput(const Core::Ref<EventHandler> event_handler) override;
  void Update(float dt) override;
  void Render(const Core::Ref<Renderer> renderer) override;

private:
  Texture m_TitleTexture,m_TitlePanelTexture;
  std::vector<Hoverable<Text>> m_HoverableTexts;
  std::array<std::string,3> m_MenuOptionsText = {
    "Play","Settings","Exit"
  };

};
#endif // __MENU_HPP__