#ifndef __MOVE_HPP__
#define __MOVE_HPP__
#include <memory>
class Piece;

struct Move{
    Move() : easing_type(Stellar::Easing::EaseInSine), frame_duration(1), piece_to_move(nullptr) ,piece_to_kill(nullptr){}
    ~Move() = default;

    Vec2i move_from, move_to, board_move_to;
    Core::Ref<Piece> piece_to_move,piece_to_kill;
    Stellar::easing_type_t easing_type;
    float frame_duration;
    Stellar::KeyFrame keyframe;
};

struct MoveInfo{
    bool is_castle,is_enpassant;
    Core::Ref<Piece> piece_killed,piece_moved;
};
#endif //!__MOVE_HPP__