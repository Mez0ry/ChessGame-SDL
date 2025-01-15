#ifndef __MOVE_TO_COMMAND_HPP__
#define __MOVE_TO_COMMAND_HPP__
#include "IEntityCommand.hpp"
#include "Board.hpp"
#include "Easing.hpp"
#include "Piece.hpp"

struct Move;

/**
 * @brief makes move relative to board
 */
class MoveTo : public IEntityCommand
{
private:
    Board &m_Board;
    Core::Ref<Piece> m_Piece;
    Move m_SmoothMove;
public:
    MoveTo(Board &board, Core::Ref<Piece> piece,const Move& smooth_move);
    CommandStatus Execute(std::optional<float> dt) override;
};
#endif //!__MOVE_TO_COMMAND_HPP__