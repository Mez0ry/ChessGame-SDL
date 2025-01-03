#include "Playing.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

Playing::Playing(const Core::Ref<Renderer> renderer, const Core::Ref<Window> window, SceneManager& scene_manager) : m_SceneManager(scene_manager), m_Renderer(renderer), m_Window(window)
{
    
}

Playing::~Playing()
{
    
}

void Playing::OnCreate()
{
    m_Renderer->SetRenderDrawColor(Color(41, 44, 92, 255));
}

void Playing::OnResize([[maybe_unused]] const Core::Ref<Window> window)
{
    
}

void Playing::OnDestroy()
{
    
}

void Playing::HandleInput([[maybe_unused]] const Core::Ref<EventHandler> event_handler)
{
    
}

void Playing::Update(float dt)
{
    
}

void Playing::Render(const Core::Ref<Renderer> renderer)
{
    
}
