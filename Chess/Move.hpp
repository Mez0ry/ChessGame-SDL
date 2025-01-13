#ifndef __MOVE_HPP__
#define __MOVE_HPP__
class Piece;

struct Move{
    Vec2i from,to;
    Core::Ref<Piece> piece_to_move;
    Core::Ref<Piece> piece_to_kill;
};
#endif //!__MOVE_HPP__