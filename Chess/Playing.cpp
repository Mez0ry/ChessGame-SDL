#include "Playing.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

Playing::Playing(const Core::Ref<Renderer> renderer, const Core::Ref<Window> window, SceneManager& scene_manager) : m_SceneManager(scene_manager), m_Renderer(renderer), m_Window(window), m_Board(renderer,window)
{
    auto[win_w,win_h] = window->GetWindowSize();

    m_Board.Setup(ObjectSize(8,8),Core::CreateRef<RapidMode>(),"resources/game/boards/chess_board2.jpg",ObjectSize(128,128));
}

Playing::~Playing()
{
    
}

void Playing::OnCreate()
{
    m_Renderer->SetRenderDrawColor(Color(41, 44, 92, 255));
    m_Board.OnCreate();
}

void Playing::OnResize([[maybe_unused]] const Core::Ref<Window> window)
{
    m_Board.OnResize(window);
}

void Playing::OnDestroy()
{
    m_Board.OnDestroy();
}

void Playing::HandleInput([[maybe_unused]] const Core::Ref<EventHandler> event_handler)
{
    m_Board.HandleInput(event_handler);
}

void Playing::Update(float dt)
{
    m_Board.Update(dt);
}

void Playing::Render(const Core::Ref<Renderer> renderer)
{
    m_Board.Render(renderer);
}
