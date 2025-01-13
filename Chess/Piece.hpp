#ifndef __PIECE_HPP__
#define __PIECE_HPP__
#include <vector>

#include "IEntity.hpp"
#include "KeyFrame.hpp"
#include "Easing.hpp"
#include "Move.hpp"

#include "PieceType.hpp"

enum class Team : char{
 WHITE = 'w', BLACK = 'b'
};

class Piece : public IEntity{
private:
    using drag_texture_t = Draggable<Texture>;
public:
    Piece(PieceType piece_type, Team team);

    Piece(const Core::Ref<Renderer> renderer, const std::string& texture_path,PieceType piece_type, Team team);
    Piece(Texture& texture, PieceType piece_type,Team team);
    ~Piece();
    
    void Render(const Core::Ref<Renderer> renderer) override;
    
    void LoadTexture(const Core::Ref<Renderer> renderer, const char* texture_path);
    
    PieceType GetPieceType() const;
    Team GetTeam() const;

    void Kill() override;
    bool IsKilled() const override;
    void Revive() override;
    
    /**
     * @brief sets position on the board
     */
    void SetPosition(const Vec2i& pos) override;
    /**
     * @brief gets position on the board
     */
    Vec2i GetPosition() const override;

    Vec2i GetPrevPosition() const;

    void SetSize(ObjectSize size) override;
    ObjectSize GetSize() const override;

    drag_texture_t& GetTexture();

    void Drag();
    void StopDragging();

    bool IsDragging() const;

    std::vector<Vec2i>& GetLegalMoves();
    bool IsLegalMove(const Vec2i& square) const;
    
    bool operator==(const Piece& other) const{
        return (m_Team == other.m_Team && m_PieceType == other.m_PieceType);
    }
    
private:
    drag_texture_t m_Texture;
    Vec2i m_Position, m_PrevPos; // Pos on the board

    PieceType m_PieceType;
    Team m_Team;
    bool m_IsKilled,m_ToMove;
private:
    std::vector<Vec2i> m_LegalMoves;
};

#endif //!__PIECE_HPP__