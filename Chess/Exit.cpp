#include "Exit.hpp"
#include "Lerp.hpp"

Exit::Exit(const Core::Ref<Renderer> renderer,const Core::Ref<Window> window, bool& is_running) : m_Renderer(renderer), m_IsRunning(is_running){
    m_ThanksText.LoadFont("resources/fonts/road-rage/Road_Rage.otf",100);
    
    m_ThanksText.LoadText("Thanks for playing",m_ThanksColor);

    OnResize(window);

    m_ExitTextKF.Setup(10,[&](float t){
        Color start_value{84,219,236,255},
                target_value{41, 44, 92, 255};
            
        m_ThanksColor = Stellar::Lerp(start_value,target_value,t);
        m_ThanksText.ChangeColor(m_ThanksColor);
    });

}

Exit::~Exit(){

}

void Exit::OnResize(const Core::Ref<Window> window){
    auto[win_w,win_h] = window->GetWindowSize();

    m_ThanksText->SetSize(ObjectSize(win_w * 0.4f, win_h * 0.2f));
    auto size = m_ThanksText->GetSize();
    m_ThanksText->SetPosition({(win_w / 2) - (size.GetWidth() / 2),static_cast<int>(((win_h * 0.15f) - (size.GetHeight() / 2)))});
}

void Exit::OnCreate(){
    
}

void Exit::HandleInput([[maybe_unused]] const Core::Ref<EventHandler> event_handler){

}

void Exit::Update(float dt){
   if(m_ExitTextKF.Update(dt)){
    m_IsRunning = false;
   }
}

void Exit::Render(const Core::Ref<Renderer> renderer){
    renderer->Render(m_ThanksText,TextRenderType::BLENDED);
}