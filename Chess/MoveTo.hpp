#ifndef __MOVE_TO_COMMAND_HPP__
#define __MOVE_TO_COMMAND_HPP__
#include "IEntityCommand.hpp"
#include "Board.hpp"
#include "Easing.hpp"
#include "Piece.hpp"

struct SmoothMove{
    SmoothMove() : easing_type(Stellar::Easing::EaseInSine), frame_duration(1){}
    ~SmoothMove() = default;

    Vec2i move_from, move_to, board_move_to;
    Stellar::easing_type_t easing_type;
    float frame_duration;
    Stellar::KeyFrame keyframe;
};

/**
 * @brief makes move relative to board
 */
class MoveTo : public IEntityCommand
{
private:
    Board &m_Board;
    Core::Ref<Piece> m_Piece;
    SmoothMove m_SmoothMove;
public:
    MoveTo(Board &board, Core::Ref<Piece> piece,const SmoothMove& smooth_move);
    CommandStatus Execute(std::optional<float> dt) override;
};
#endif //!__MOVE_TO_COMMAND_HPP__