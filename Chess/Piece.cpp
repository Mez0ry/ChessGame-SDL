#include "Piece.hpp"
#include "AABB.hpp"

#include "MouseInput.hpp"
#include "EventHandler.hpp"

#include "Renderer.hpp"
#include "Lerp.hpp"

#include "Engine.hpp"

Piece::Piece(PieceType piece_type, Team team) : m_IsKilled(true), m_ToMove(false), m_Position(-1,-1), m_PieceType(piece_type), m_Team(team)
{
    
}

Piece::Piece(const Core::Ref<Renderer> renderer, const char* texture_path, PieceType piece_type) : m_IsKilled(true), m_ToMove(false), m_Position(-1,-1), m_PieceType(piece_type)
{
     
}

Piece::Piece(Texture& texture, PieceType piece_type,Team team) : m_IsKilled(true),m_ToMove(false), m_Position(-1,-1), m_PieceType(piece_type), m_Team(team)
{
    m_Texture.ShareSDLTexture(texture);
}

Piece::~Piece()
{
    
}

void Piece::LoadTexture(const Core::Ref<Renderer> renderer, const char* texture_path)
{
    m_Texture.LoadTexture(renderer,texture_path);
}

PieceType Piece::GetPieceType() const
{
    return m_PieceType;
}

Team Piece::GetTeam() const
{
    return m_Team;
}

void Piece::Kill()
{
    m_IsKilled = true;
}

bool Piece::IsKilled() const
{
    return m_IsKilled;
}

void Piece::Revive()
{
    m_IsKilled = false;
}

void Piece::Update(float dt)
{
  if(!m_SmoothMoveStackKF.empty()){
    auto& stack_top = m_SmoothMoveStackKF.top();
    auto& keyframe = stack_top.keyframe;

    if(keyframe.IsActionEmpty()){
        keyframe.Setup(stack_top.frame_duration,[&](float t){
            int dx = 0,dy = 0;
            auto& piece_texture = this->GetTexture();

            dx = Stellar::Lerp(stack_top.move_from.x,stack_top.move_to.x,stack_top.easing_type(t));
            dy = Stellar::Lerp(stack_top.move_from.y,stack_top.move_to.y,stack_top.easing_type(t));
            STELLAR_CORE_INFO("delta: {0} : {1} ",dx,dy);
            piece_texture.SetPosition({dx,dy});
            //Render(Engine::GetModule<Renderer>());
        });
    }
    
    keyframe.Update(dt);
    
    if(keyframe.IsFinished()){
        m_SmoothMoveStackKF.pop();
        SetPosition(stack_top.board_move_to);
        if(m_SmoothMoveStackKF.empty()){
            m_IsMovingSmoothly = false;
        }

    }
  }
}

void Piece::Render(const Core::Ref<Renderer> renderer)
{
    renderer->Render(m_Texture);
}

void Piece::SetPosition(const Vec2i& pos)
{
    if(m_Position == pos) 
        return;
    m_Position = pos;
}

Vec2i Piece::GetPosition() const 
{
    return m_Position;
}

void Piece::SetSize(ObjectSize size)
{
    m_Texture.SetSize(size);
}

ObjectSize Piece::GetSize() const 
{
    return m_Texture.GetSize();
}

Piece::drag_texture_t& Piece::GetTexture()
{
    return m_Texture;
}

void Piece::Drag() {
    m_Texture.Drag();
}

void Piece::StopDragging() {
    m_Texture.StopDragging();
}

bool Piece::IsDragging() const
{
    return m_Texture.IsDragging();
}

bool Piece::IsSmoothMoving() const
{
    return m_IsMovingSmoothly;
}

void Piece::ApplySmoothMove(const Vec2i from, const Vec2i to, const Vec2i board_pos_move_to,int frame_duration,const Stellar::easing_type_t& easing)
{
    m_SmoothMoveStackKF.push(SmoothMove());
    auto& smooth_move = m_SmoothMoveStackKF.top();

    smooth_move.move_from = from;
    smooth_move.move_to = to;
    smooth_move.board_move_to = board_pos_move_to;
    smooth_move.easing_type = easing;
    smooth_move.frame_duration = frame_duration;

    m_IsMovingSmoothly = true;
}
