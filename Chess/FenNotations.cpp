#include "FenNotations.hpp"
#include "Piece.hpp"
#include "Board.hpp"

void ForsythEdwards::SetupCustomBoardPosition(const char* fen,Board& board)
{
    Vec2i new_pos;
    new_pos.x = 0;
    new_pos.y = board.GetBoardSize().GetHeight() - 1;
    auto& pieces = board.GetPieces();

    size_t space_cnt = 0;

    auto is_piece_symbol = [&](char symbol) -> bool{
        auto it = std::find_if(m_PiecesCharArr.begin(),m_PiecesCharArr.end(),[&](char img_symbol){
            return (img_symbol == symbol);
        });

        return (it != m_PiecesCharArr.end());
    };
    
    for(int i = 0; i < strlen(fen);++i){
        if(fen[i] == ' '){
            ++space_cnt;
        }

        auto create_piece = [&](char piece_symbol) -> Core::Ref<Piece>{
                Team team;
                PieceType piece_type;
                
                if(isupper(piece_symbol)){
                    team = Team::WHITE;
                    piece_type = static_cast<PieceType>(tolower(piece_symbol));
                }else if(islower(piece_symbol)){
                    team = Team::BLACK;
                    piece_type = static_cast<PieceType>(piece_symbol);
                }

                return Core::CreateRef<Piece>(piece_type,team);
        };

        if(std::isdigit(fen[i])){
            int digit = std::atoi(&fen[i]);
            new_pos.x += digit;
        }else if(fen[i] == '/'){
            new_pos.x = 0;
            --new_pos.y;
        }else if(is_piece_symbol(fen[i]) && space_cnt < 1){
            auto curr_piece = create_piece(fen[i]);

            curr_piece->SetPosition(new_pos);
            ++new_pos.x;
            
            curr_piece->Revive();

            pieces.push_back(curr_piece);
        }

    }
 
}

void ForsythEdwards::SetupStartingBoardPos(Board& board)
{
    this->SetupCustomBoardPosition(m_StartingBoardPosFen,board);
}
