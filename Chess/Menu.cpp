#include "Menu.hpp"
#include "SceneManager.hpp"

#include "Exit.hpp"

#include "Renderer.hpp"
#include "Window.hpp"
#include "TextureGuard.hpp"
#include "KeyboardInput.hpp"
#include "Lerp.hpp"
#include "AABB.hpp"


Menu::Menu(const Core::Ref<Renderer> renderer, const Core::Ref<Window> window, SceneManager &scene_manager) : m_Renderer(renderer), m_SceneManager(scene_manager), m_Navigation("resources/UI/kenney_fantasy-ui/Double/Border/panel-border-023.png", renderer, window, m_TextVec)
{
  auto [win_w, win_h] = window->GetWindowSize();

  Font text_font = Font::StaticLoadFont("resources/fonts/UrbanJungle/UrbanJungleDEMO.otf", win_w * 0.095);

  for (uint8_t idx = 0; idx < m_MenuOptionsText.size(); idx++)
  {
    m_TextVec.push_back(Clickable<Hoverable<Text>>());

    auto &curr = m_TextVec[idx];
    curr.LoadText(text_font, m_MenuOptionsText[idx].c_str(), Color(81, 82, 92, 255));
  }

  uint8_t play_idx = 0, settings_idx = 1, exit_idx = 2;

  m_TextVec[exit_idx].OnClick([&](){
    m_SceneManager.TransitionTo<Menu,Exit>();
  });
  
  m_Renderer->SetRenderDrawColor(Color(41, 44, 92, 255));
  OnResize(window);
}

void Menu::OnCreate()
{
  m_Renderer->SetRenderDrawColor(Color(41, 44, 92, 255));
  m_Navigation.OnCreate();
}

void Menu::OnResize([[maybe_unused]] const Core::Ref<Window> window)
{
  auto [win_w, win_h] = window->GetWindowSize();

  Rect rect({0, 0}, ObjectSize(win_w, win_h));
  ObjectSize text_size(win_w * 0.120, win_h * 0.064);

  uint16_t dy_offset = 0;
  for (auto &text : m_TextVec)
  {

    Vec2i new_pos;
    new_pos.x = (rect.Center().x - (text_size.GetWidth() / 2));
    new_pos.y = (rect.Center().y - (text_size.GetHeight() * 2)) + dy_offset;

    new_pos.y += 35;

    text->SetRect(new_pos, text_size);

    dy_offset += text_size.GetHeight() * 1.5f;
  }

  m_Navigation.OnResize();
}

void Menu::OnDestroy()
{
  m_Navigation.OnDestroy();
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
  m_Navigation.Update(dt);
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
    if(m_BorderPosMemento.IsEmpty()) 
      return;
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
  auto &current_text = m_TextVec[m_CurrTextIdx];

  auto border_size = m_BorderTexture.GetSize();

  auto center = current_text->GetRect().Center();
  auto text_size = current_text->GetSize();

  m_TargetBorderPos.x = center.x - (border_size.GetWidth() / 2);
  m_TargetBorderPos.y = center.y - (border_size.GetHeight() / 2) * (1.25f);
  
  m_BorderTexture.SetRect(m_TargetBorderPos,ObjectSize(text_size.GetWidth() * 2.5f, text_size.GetHeight() * 1.5f));
}

void Navigation::HandleInput(const Core::Ref<EventHandler> event_handler)
{
  for (auto i = 0; i < m_TextVec.size(); i++)
  {
    auto &loaded_text = m_TextVec[i].GetLoadedText();
    if (AABB::isColliding(m_BorderTexture.GetRect(), m_TextVec[i]->GetRect()) && i == m_CurrTextIdx)
    {
      m_TextVec[i].ChangeColor(Color(124, 79, 193, 255));
    }
    else
    {
      m_TextVec[i].ChangeColor(Color(81, 82, 92, 255));
    }

    if (m_TextVec[i].IsHovered() && !m_BorderKeyFrame.IsFinished() && !m_TargetChanged)
    {
      SetTextIdx(i);
    }
  }
}

void Navigation::Update(float dt)
{
  if (m_TargetChanged)
  {
    OnResize(); // used to update the position of the text border and its size
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

void Navigation::SetTextIdx(size_t idx)
{
  if (m_CurrTextIdx == idx)
    return;

  auto clamped = std::clamp(idx, size_t(0), m_TextVec.size());
  auto prev_curr_text_idx = std::clamp(static_cast<uint8_t>(m_CurrTextIdx), static_cast<uint8_t>(0), static_cast<uint8_t>(m_TextVec.size()));
  
  const auto& pos = m_TextVec[prev_curr_text_idx]->GetPosition();
  m_BorderPosMemento.Save(pos);

  this->m_CurrTextIdx = clamped;
  m_TargetChanged = true;
}