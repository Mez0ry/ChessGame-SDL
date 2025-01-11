#ifndef __PIECE_HPP__
#define __PIECE_HPP__
#include "IEntity.hpp"
#include "KeyFrame.hpp"
#include "Easing.hpp"
#include <stack>

enum class PieceType : char{
    PAWN = 'p',ROOK = 'r',KNIGHT = 'n',BISHOP = 'b',QUEEN = 'q',KING = 'k' // for white team letters should be in uppercase
};

enum class Team : char{
 WHITE = 'w', BLACK = 'b'
};

class Piece : public IEntity{
private:
    using drag_texture_t = Draggable<Texture>;
public:
    Piece(PieceType piece_type, Team team);

    Piece(const Core::Ref<Renderer> renderer, const char* texture_path, PieceType piece_type);
    Piece(Texture& texture, PieceType piece_type,Team team);
    ~Piece();
    

    void Update(float dt) override;
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

    void SetSize(ObjectSize size) override;
    ObjectSize GetSize() const override;

    drag_texture_t& GetTexture();

    void Drag();
    void StopDragging();

    bool IsDragging() const;

    bool IsSmoothMoving() const;

    void ApplySmoothMove(const Vec2i from,const Vec2i to, const Vec2i board_pos_move_to, int frame_duration = 1, const Stellar::easing_type_t& easing = Stellar::Easing::EaseInSine);
private:
    struct SmoothMove{
        Vec2i move_from, move_to, board_move_to;
        Stellar::easing_type_t easing_type;
        float frame_duration;
        Stellar::KeyFrame keyframe;
    };

private:
    drag_texture_t m_Texture;
    Vec2i m_Position; // Pos on the board

    PieceType m_PieceType;
    Team m_Team;
    bool m_IsKilled,m_ToMove;
private: //Smooth stuff
    std::stack<SmoothMove> m_SmoothMoveStackKF;

    
    bool m_IsMovingSmoothly;
};

#endif //!__PIECE_HPP__