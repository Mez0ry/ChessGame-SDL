#ifndef __MENU_HPP__
#define __MENU_HPP__
#include "GameScene.hpp"

#include "Text.hpp"
#include "KeyFrame.hpp"
#include "Memento.hpp"

#include "Clickable.hpp"
#include "Hoverable.hpp"

#include <deque>

class Window;
class Renderer;
class SceneManager;

class Navigation{
private:
  Core::Ref<Renderer> m_Renderer;
  const Core::Ref<Window> m_Window;
  public:
  Navigation(const std::string &border_path,const Core::Ref<Renderer> renderer, const Core::Ref<Window> window, std::vector<Clickable<Hoverable<Text>>>& text_vec);
  ~Navigation() = default;

  void LoadBorderTexture(const std::string &path);

  void OnCreate();
  void OnDestroy();

  void OnResize();
  void HandleInput(const Core::Ref<EventHandler> event_handler);
  void Update(float dt);
  void Render(const Core::Ref<Renderer> renderer);
  private:
  void SetupTargetBorderPos();
  void SetTextIdx(size_t idx);

  private:
  Texture m_BorderTexture;
  std::vector<Clickable<Hoverable<Text>>>& m_TextVec;
  uint8_t m_CurrTextIdx;
  Memento<Vec2i> m_BorderPosMemento;
  Vec2i m_TargetBorderPos;
  Vec2i m_InitialBorderPos;
  bool m_TargetChanged;
  Stellar::KeyFrame m_BorderKeyFrame;
};
class Menu : public GameScene {
private:
  Core::Ref<Renderer> m_Renderer;
  Core::Ref<Window> m_Window;
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
  std::vector<Clickable<Hoverable<Text>>> m_TextVec;
  std::vector<Memento<Vec2i>> m_TextPosMementos; // used as target pos in lerp

  Stellar::KeyFrame m_PlayKf,m_SettingsKf,m_ExitKf;

  std::array<std::string,3> m_MenuOptionsText = {
    "Play","Settings","Exit"
  };
  Navigation m_Navigation;
};
#endif // __MENU_HPP__