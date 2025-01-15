#include "MoveTo.hpp"
#include "Lerp.hpp"
#include "Move.hpp"

MoveTo::MoveTo(Board &board, Core::Ref<Piece> piece, const Move &smooth_move) : m_Board(board),m_Piece(piece), m_Move(smooth_move)
{
    
}

CommandStatus MoveTo::Execute(std::optional<float> dt)
{
    m_Board.ForEachAlivePiece([&](const Core::Ref<Piece> piece){
        m_Board.GenerateLegalMoves(piece);
    });

    if(!m_Board.IsMakeableMove(m_Piece,m_Move.board_move_to)){
        m_Board.SetTextureEntityPosition(m_Piece);
        return CommandStatus::FAILED;
    }

    auto& keyframe = m_Move.keyframe;

    if(keyframe.IsActionEmpty()){
        keyframe.Setup(m_Move.frame_duration,[&](float t){
            int dx = 0,dy = 0;
            auto& piece_texture = m_Piece->GetTexture();

            dx = Stellar::Lerp(m_Move.move_from.x,m_Move.move_to.x,m_Move.easing_type(t));
            dy = Stellar::Lerp(m_Move.move_from.y,m_Move.move_to.y,m_Move.easing_type(t));
            piece_texture.SetPosition({dx,dy});
        });
    }

    if(dt){
        keyframe.Update(dt.value());
    }

    if(keyframe.GetElapsedFrames() > m_Move.frame_duration * 0.9f){
        if(m_Move.piece_to_kill){
            m_Move.piece_to_kill->Kill();
        }
    }
    
    if(keyframe.IsFinished()){
        m_Board.MakeMove(m_Piece,m_Move);

        m_Board.SetTextureEntityPosition(m_Piece);
        m_Board.GenerateLegalMoves(m_Piece);
        return CommandStatus::SUCCESS;
    }
    return CommandStatus::WAITING;
}
