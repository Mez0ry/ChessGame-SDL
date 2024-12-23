#include "Menu.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "TextureGuard.hpp"

 Menu::Menu(const Core::Ref<Renderer> renderer, const Core::Ref<Window> window, SceneManager& scene_manager) : m_Renderer(renderer), m_SceneManager(scene_manager)
 {
    auto[win_w,win_h] = window->GetWindowSize();

    Texture button_texture(renderer,"resources/UI/kenney_fantasy-ui/Default/Panel/panel-000.png");
    Font text_font = Font::StaticLoadFont("resources/fonts/Wombyland/OpenType-TT/WombyLand.ttf",win_w * 0.095);
    
    for(uint8_t idx = 0; idx < m_MenuOptionsText.size(); idx++){
        m_HoverableTexts.push_back(Hoverable<Text>());

        auto& curr = m_HoverableTexts[idx];
        curr.LoadText(renderer,text_font,m_MenuOptionsText[idx].c_str(),Color(81,82,92,255));
    }
     
    OnResize(window);
    m_Renderer->SetRenderDrawColor(Color(41, 44, 92,255));
 }
 
 void Menu::OnCreate()
 {
    
 }
 
 void Menu::OnResize([[maybe_unused]] const Core::Ref<Window> window)
 {
    auto[win_w,win_h] = window->GetWindowSize();
    
    Rect rect({0,0},ObjectSize(win_w,win_h));
    ObjectSize text_size(win_w * 0.100,win_h * 0.064);

    uint16_t dy_offset = 0;
    for(auto& text : m_HoverableTexts){

        Vec2i new_pos;
        new_pos.x = (rect.Center().x - (text_size.GetWidth() / 2));
        new_pos.y = (rect.Center().y - (text_size.GetHeight() * 2)) + dy_offset;

        new_pos.y += 35;

        text->SetRect(new_pos,text_size);

        dy_offset += text_size.GetHeight() * 1.3f;
    }
 }
 
 void Menu::OnDestroy()
 {
     
 }
 
 void Menu::HandleInput(const Core::Ref<EventHandler> event_handler)
 {
     
 }
 
 void Menu::Update(float dt)
 {
    
 }
 
 void Menu::Render(const Core::Ref<Renderer> renderer)
 {
    for(auto& text : m_HoverableTexts){
        renderer->Render(text);
    }
 }
 