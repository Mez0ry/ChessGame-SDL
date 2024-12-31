#ifndef __EXIT_HPP__
#define __EXIT_HPP__
#include "GameScene.hpp"
#include "Text.hpp"

#include "KeyFrame.hpp"
class Exit : public GameScene {
private:
  Core::Ref<Renderer> m_Renderer;
public:
  Exit(const Core::Ref<Renderer> renderer,const Core::Ref<Window> window,bool& is_running);
  ~Exit();

  void OnResize(const Core::Ref<Window> window) override;
  void OnCreate() override;
  
  void HandleInput(const Core::Ref<EventHandler> event_handler) override;
  void Update(float dt) override;
  void Render(const Core::Ref<Renderer> renderer) override;

private:
  bool& m_IsRunning;
  Text m_ThanksText;
  Color m_ThanksColor = {84,219,236,255};
  Stellar::KeyFrame m_ExitTextKF;
  
};

#endif //! __EXIT_HPP__