#ifndef __MENU_HPP__
#define __MENU_HPP__
#include "GameScene.hpp"
#include "GUI/Button.hpp"
#include "Text.hpp"
#include "KeyFrame.hpp"
#include "Memento.hpp"

class Window;
class Renderer;
class Navigation{
private:
  Core::Ref<Renderer> m_Renderer;
  const Core::Ref<Window> m_Window;
  public:
  Navigation(const std::string &border_path,const Core::Ref<Renderer> renderer, const Core::Ref<Window> window, std::vector<Hoverable<Text>>& hoverable_text);
  ~Navigation() = default;

  void LoadBorderTexture(const std::string &path);

  void OnCreate();
  void OnResize();
  void HandleInput(const Core::Ref<EventHandler> event_handler);
  void Update(float dt);
  void Render(const Core::Ref<Renderer> renderer);
  private:
  void SetTextIdx(size_t idx);

  private:
  Texture m_BorderTexture;
  std::vector<Hoverable<Text>>& m_HoverableTexts;
  uint8_t m_CurrTextIdx;
  Memento<Vec2i> m_BorderPosMemento;
  Vec2i m_TargetBorderPos;
  bool m_TargetChanged;
  Stellar::KeyFrame m_BorderKeyFrame;
};

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
  Navigation m_Navigation;
};
#endif // __MENU_HPP__