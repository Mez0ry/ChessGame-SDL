#include "Menu.hpp"
#include "SceneManager.hpp"

#include "Exit.hpp"
#include "Playing.hpp"

#include "Renderer.hpp"
#include "Window.hpp"
#include "TextureGuard.hpp"
#include "KeyboardInput.hpp"
#include "Lerp.hpp"
#include "AABB.hpp"

Menu::Menu(const Core::Ref<Renderer> renderer, const Core::Ref<Window> window, SceneManager &scene_manager) : m_Renderer(renderer),m_Window(window), m_SceneManager(scene_manager), m_Navigation("resources/UI/kenney_fantasy-ui/Double/Border/panel-border-023.png", renderer, window, m_TextVec)
{
  auto [win_w, win_h] = window->GetWindowSize();

  Font text_font = Font::StaticLoadFont("resources/fonts/UrbanJungle/UrbanJungleDEMO.otf", win_w * 0.095);

  for (uint8_t idx = 0; idx < m_MenuOptionsText.size(); idx++)
  {
    m_TextVec.push_back(Clickable<Hoverable<Text>>());

    auto &curr = m_TextVec[idx];
    curr.LoadText(text_font, m_MenuOptionsText[idx].c_str(), Color(81, 82, 92, 255));
  }

  m_PlayKf.Setup(5,[&](float t){
    const uint8_t play_idx = 0;

    auto& target_text = m_TextVec[play_idx];
    const auto& target_pos = m_TextPosMementos[play_idx].GetState();
    int dx = Stellar::Lerp(target_text->GetPosition().x,target_pos.x,t);

    target_text->SetPosition({dx,target_text->GetPosition().y});
  });
  
  m_SettingsKf.Setup(5,[&](float t){
    const uint8_t settings_idx = 1;
    
    auto& target_text = m_TextVec[settings_idx];
    const auto& target_pos = m_TextPosMementos[settings_idx].GetState();
    int dx = Stellar::Lerp(target_text->GetPosition().x,target_pos.x,t);

    target_text->SetPosition({dx,target_text->GetPosition().y});
  });

  m_ExitKf.Setup(5,[&](float t){
    const uint8_t exit_idx = 2;

    auto& target_text = m_TextVec[exit_idx];
    const auto& target_pos = m_TextPosMementos[exit_idx].GetState();
    int dx = Stellar::Lerp(target_text->GetPosition().x,target_pos.x,t);

    target_text->SetPosition({dx,target_text->GetPosition().y});
  });

   m_TextVec[0].OnClick([&](){
    m_SceneManager.TransitionTo<Menu,Playing>();
  });

  m_TextVec[2].OnClick([&](){
    m_SceneManager.TransitionTo<Menu,Exit>();
  });
  
  m_Renderer->SetRenderDrawColor(Color(41, 44, 92, 255));
  OnResize(window);

  auto cnt = 0;
  for(auto& text : m_TextVec){
    auto text_size = text->GetSize();

    Vec2i initial_pos = {0,m_TextPosMementos[cnt].GetState().y};
    if((cnt & 1) == 0){
      initial_pos.x = win_w + text_size.GetWidth();
    }else{
      initial_pos.x = -text_size.GetWidth();
    }

    text->SetPosition(initial_pos);
    cnt++;
  }
  m_Navigation.OnResize();
}

void Menu::OnCreate()
{
  m_Renderer->SetRenderDrawColor(Color(41, 44, 92, 255));
  m_Navigation.OnCreate();
}

void Menu::OnResize([[maybe_unused]] const Core::Ref<Window> window)
{
  if(!m_TextPosMementos.empty()){
    m_TextPosMementos.clear();
  }

  auto [win_w, win_h] = window->GetWindowSize();

  Rect rect({0, 0}, ObjectSize(win_w, win_h));
  const ObjectSize text_size(win_w * 0.120, win_h * 0.064);

  for (size_t index = {0},dy_offset = {0}; index < m_TextVec.size(); index++,dy_offset += (text_size.GetHeight() * 1.5f))
  {
    auto& curr_text = m_TextVec[index];
    
    Vec2i target_pos;
    target_pos.x = (rect.Center().x - (text_size.GetWidth() / 2));
    target_pos.y = (rect.Center().y - (text_size.GetHeight() * 2)) + dy_offset;

    target_pos.y += 35;

    curr_text->SetSize(text_size);
    
    m_TextPosMementos.push_back(Memento<Vec2i>(target_pos));
  }

  m_Navigation.OnResize();
}

void Menu::OnDestroy()
{
  m_Navigation.OnDestroy();
  m_ExitKf.Restart();
}

void Menu::HandleInput(const Core::Ref<EventHandler> event_handler)
{
  for(auto& text : m_TextVec){
    if(text.IsClicked()){
      text.Execute();
    }
  }

  m_Navigation.HandleInput(event_handler);
}

void Menu::Update(float dt)
{
  if(m_Window->GetOpacity() > 0.5f && m_Window->GetOpacity() < 1.f){
    m_PlayKf.Update(dt);
    m_SettingsKf.Update(dt);
    m_ExitKf.Update(dt);
      
  }

  bool one_of_finished = (m_PlayKf.IsFinished() || m_SettingsKf.IsFinished() ||m_ExitKf.IsFinished() );
  if(one_of_finished){
    m_Navigation.Update(dt);
  }
   
}

void Menu::Render(const Core::Ref<Renderer> renderer)
{
  for (auto &text : m_TextVec)
  {
    renderer->Render(text,TextRenderType::BLENDED);
  }
  m_Navigation.Render(renderer);
}

Navigation::Navigation(const std::string &border_path, const Core::Ref<Renderer> renderer, const Core::Ref<Window> window, std::vector<Clickable<Hoverable<Text>>> &text_vec) : m_Renderer(renderer), m_Window(window), m_TextVec(text_vec), m_CurrTextIdx(0), m_TargetChanged(true)
{   
  LoadBorderTexture(border_path);
  m_BorderKeyFrame.Setup(0.100f, [&](float t){
     
    int dy = Stellar::Lerp(m_BorderPosMemento.GetState().y,m_TargetBorderPos.y,t);

    m_BorderTexture.SetPosition({m_TargetBorderPos.x,dy}); 
  });
  
}

void Navigation::LoadBorderTexture(const std::string &path)
{
  m_BorderTexture.LoadTexture(m_Renderer, path);
}

void Navigation::OnCreate()
{
  SetTextIdx(0);
}

void Navigation::OnDestroy()
{
  m_BorderPosMemento.ClearSnapshots();
}

void Navigation::OnResize()
{
  auto[win_w,win_h] = m_Window->GetWindowSize();

  auto &current_text = m_TextVec[m_CurrTextIdx];
  auto text_size = current_text->GetSize();

  SetupTargetBorderPos(); 

  m_InitialBorderPos = {m_TargetBorderPos.x,m_TargetBorderPos.y};
  SetTextIdx(0);
  m_BorderTexture.SetRect(m_InitialBorderPos,ObjectSize(text_size.GetWidth() * 2.5f, text_size.GetHeight() * 1.5f));
}

void Navigation::HandleInput([[maybe_unused]] const Core::Ref<EventHandler> event_handler)
{
  for (size_t i = 0; i < m_TextVec.size(); i++)
  {
    auto &curr_text = m_TextVec[i];

    if (AABB::isColliding(m_BorderTexture.GetRect(), curr_text->GetRect()) && i == m_CurrTextIdx)
    {
      curr_text.ChangeColor(Color(124, 79, 193, 255));
    }
    else
    {
      curr_text.ChangeColor(Color(81, 82, 92, 255));
    }

    if (curr_text.IsHovered() && !m_BorderKeyFrame.IsFinished() && !m_TargetChanged)
    {
      SetTextIdx(i);
    }
  }

  size_t tmp_idx = m_CurrTextIdx;

  if(KeyboardInput::IsKeyUp(SDLK_DOWN) && !m_BorderKeyFrame.IsFinished() && !m_TargetChanged){
    tmp_idx += 1;
    if(tmp_idx >= m_TextVec.size()){
      tmp_idx = 0;
    }
    SetTextIdx(tmp_idx);
  }else if(KeyboardInput::IsKeyUp(SDLK_UP) && !m_BorderKeyFrame.IsFinished() && !m_TargetChanged){
    tmp_idx -= 1;
    tmp_idx = std::clamp(tmp_idx, size_t(0), m_TextVec.size() - 1);
    SetTextIdx(tmp_idx);
  }
}

void Navigation::Update(float dt)
{
  if (m_TargetChanged)
  {
    SetupTargetBorderPos();
    if (m_BorderKeyFrame.Update(dt))
    {
      m_TargetChanged = false;
      m_BorderKeyFrame.Restart();
    }
  }
}

void Navigation::Render(const Core::Ref<Renderer> renderer)
{
  renderer->Render(m_BorderTexture);
}

void Navigation::SetupTargetBorderPos()
{
  auto &current_text = m_TextVec[m_CurrTextIdx];

  auto border_size = m_BorderTexture.GetSize();

  auto center = current_text->GetRect().Center();
  auto text_size = current_text->GetSize();

  m_TargetBorderPos.x = center.x - (border_size.GetWidth() / 2);
  m_TargetBorderPos.y = center.y - (border_size.GetHeight() / 2) * (1.25f);
}

void Navigation::SetTextIdx(size_t idx)
{
  if (m_CurrTextIdx == idx)
    return;

  auto clamped = std::clamp(idx, size_t(0), m_TextVec.size() - 1);
  auto prev_curr_text_idx = std::clamp(static_cast<uint8_t>(m_CurrTextIdx), static_cast<uint8_t>(0), static_cast<uint8_t>(m_TextVec.size() - 1));
  
  const auto& pos = m_TextVec[prev_curr_text_idx]->GetPosition();
  m_BorderPosMemento.Save(pos);

  this->m_CurrTextIdx = clamped;
  m_TargetChanged = true;
}