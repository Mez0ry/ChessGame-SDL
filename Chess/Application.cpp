#include "Application.hpp"
#include "Timer.hpp"
#include "Engine.hpp"
#include "EventHandler.hpp"
#include "AssetManager.hpp"

#include "Easing.hpp"
#include "Lerp.hpp"

#include "Menu.hpp"
#include "Exit.hpp"
#include "Playing.hpp"

Application::Application() : m_bIsRunning(true) {
  if(!Engine::Initialize(SDL_INIT_EVERYTHING,IMG_INIT_PNG | IMG_INIT_JPG,true)){
    STELLAR_CRITICAL_THROW(std::runtime_error,"","Failed to initialize engine");
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

  Engine::RegisterModule<Window>("Chess Game SDL",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,700, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  SDL_SetWindowOpacity(*Engine::GetModule<Window>(),0);
  
  Engine::RegisterModule<Renderer>(Engine::GetModule<Window>(),-1,SDL_RENDERER_ACCELERATED);
  Engine::RegisterModule<EventHandler>();

  Engine::GetModule<Window>()->SetIcon("resources/logo/logo.png");
  
  Engine::RegisterModule<AssetManager>("resources/game");
  Engine::GetModule<AssetManager>()->LoadAssets(Engine::GetModule<Renderer>());
  
  m_SceneManager.AddScene<Menu>(Engine::GetModule<Renderer>(),Engine::GetModule<Window>(),m_SceneManager);
  m_SceneManager.AddChildScene<Menu,Exit>(Engine::GetModule<Renderer>(),Engine::GetModule<Window>(),m_bIsRunning);
  m_SceneManager.AddChildScene<Menu,Playing>(Engine::GetModule<Renderer>(),Engine::GetModule<Window>(),m_SceneManager);

  m_WindowSmoothShow.Setup(2,[&](float t){
    auto final_opacity = Stellar::Lerp(0,1,t);
    SDL_SetWindowOpacity(Engine::GetModule<Window>()->GetWindow(),Stellar::Easing::EaseInOutSine(final_opacity));
  });
}

Application::~Application() {}

bool Application::Run() {
  Timer cap_timer;
  
  Timer step_timer;
  
  while (m_bIsRunning)
  {
    cap_timer.Start();
    Engine::GetModule<Renderer>()->RenderClear();

    HandleInput();

    double dt = step_timer.GetTicks() / 1000.f;
    
    Update(dt);

    step_timer.Start();

    Render();
    
    Engine::GetModule<Renderer>()->RenderPresent();

    auto elapsed = cap_timer.GetTicks();
    if(elapsed <= 10)
    {
      SDL_Delay(10 - elapsed);
    }

  }

  return true;
}

void Application::HandleInput() {
 auto event_handler = Engine::GetModule<EventHandler>();
 event_handler->PollEvents();

 if(event_handler->IsQuit() || KeyboardInput::IsPressed(SDLK_ESCAPE)){
  m_bIsRunning = false;
 }
 
 if(WindowInput::OnEvent(SDL_WINDOWEVENT_RESIZED)){
  m_SceneManager.OnResize(Engine::GetModule<Window>());
 }

 m_SceneManager.HandleInput(event_handler);
}

void Application::Update(float dt) {
  m_WindowSmoothShow.Update(dt);
  m_SceneManager.Update(dt);
}

void Application::Render() {
 m_SceneManager.Render(Engine::GetModule<Renderer>());
}