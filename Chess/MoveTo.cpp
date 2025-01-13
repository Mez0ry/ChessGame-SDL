#include "MoveTo.hpp"
#include "Lerp.hpp"

MoveTo::MoveTo(Board &board, Core::Ref<Piece> piece, const SmoothMove &smooth_move) : m_Board(board),m_Piece(piece), m_SmoothMove(smooth_move)
{
    //board.GeneratePossibleMoves(piece);
}

CommandStatus MoveTo::Execute(std::optional<float> dt)
{
    m_Board.ForEachPiece([&](const Core::Ref<Piece> piece){
        m_Board.GenerateLegalMoves(piece);
    });

    if(!m_Board.IsMakeableMove(m_Piece,m_SmoothMove.board_move_to)){
        m_Board.SetTextureEntityPosition(m_Piece);
        return CommandStatus::FAILED;
    }
    
    auto& keyframe = m_SmoothMove.keyframe;

    if(keyframe.IsActionEmpty()){
        keyframe.Setup(m_SmoothMove.frame_duration,[&](float t){
            int dx = 0,dy = 0;
            auto& piece_texture = m_Piece->GetTexture();

            dx = Stellar::Lerp(m_SmoothMove.move_from.x,m_SmoothMove.move_to.x,m_SmoothMove.easing_type(t));
            dy = Stellar::Lerp(m_SmoothMove.move_from.y,m_SmoothMove.move_to.y,m_SmoothMove.easing_type(t));
            piece_texture.SetPosition({dx,dy});
        });
    }

    if(dt){
        keyframe.Update(dt.value());
    }
    
    if(keyframe.IsFinished()){
        m_Board.MakeMove(m_Piece,m_SmoothMove.board_move_to);

        m_Board.SetTextureEntityPosition(m_Piece);
        m_Board.GenerateLegalMoves(m_Piece);
        return CommandStatus::SUCCESS;
    }
    return CommandStatus::WAITING;
}
