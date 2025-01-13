#include "Piece.hpp"
#include "AABB.hpp"

#include "MouseInput.hpp"
#include "EventHandler.hpp"

#include "Renderer.hpp"
#include "Lerp.hpp"

#include "Engine.hpp"

Piece::Piece(PieceType piece_type, Team team) : m_IsKilled(true), m_ToMove(false), m_Position(-1,-1),m_PrevPos(-1,-1), m_PieceType(piece_type), m_Team(team)
{
    m_LegalMoves.reserve(8);
}

Piece::Piece(const Core::Ref<Renderer> renderer, const std::string& texture_path, PieceType piece_type, Team team) : m_IsKilled(true), m_ToMove(false), m_Position(-1,-1),m_PrevPos(-1,-1), m_PieceType(piece_type), m_Team(team)
{
    m_LegalMoves.reserve(8);
    LoadTexture(renderer,texture_path.c_str());
}

Piece::Piece(Texture& texture, PieceType piece_type,Team team) : m_IsKilled(true),m_ToMove(false), m_Position(-1,-1),m_PrevPos(-1,-1), m_PieceType(piece_type), m_Team(team)
{
    m_LegalMoves.reserve(8);
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

void Piece::Render(const Core::Ref<Renderer> renderer)
{
    renderer->Render(m_Texture);
}

void Piece::SetPosition(const Vec2i& pos)
{
    if(m_Position == pos) 
        return;
    
    m_PrevPos = m_Position;
    m_Position = pos;
}

Vec2i Piece::GetPosition() const 
{
    return m_Position;
}

Vec2i Piece::GetPrevPosition() const
{
    return m_PrevPos;
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

std::vector<Vec2i> &Piece::GetLegalMoves()
{
    return m_LegalMoves;
}

bool Piece::IsLegalMove(const Vec2i& square) const
{
    return (std::find_if(m_LegalMoves.begin(),m_LegalMoves.end(),[&](const Vec2i& possible_move){
        return (possible_move == square);
    }) != m_LegalMoves.end());
}
