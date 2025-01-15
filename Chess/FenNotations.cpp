#include "FenNotations.hpp"
#include "Piece.hpp"
#include "Board.hpp"

void ForsythEdwards::SetupCustomBoardPosition(const std::string& fen,Board& board)
{
    Vec2i new_pos;
    new_pos.x = 0;
    new_pos.y = board.GetBoardSize().GetHeight() - 1;
    auto& pieces = board.GetPieces();

    char empty_space = ' ';

    auto pieces_end_pos = fen.find_first_of(empty_space);
    std::string pieces_fen = fen.substr(0,pieces_end_pos);

    ParsePiecePositions(pieces_fen,pieces,new_pos);

    std::string parsed_team_str = fen.substr(pieces_end_pos + 1,1);
    
    Team team_to_move = (parsed_team_str == "w") ? Team::WHITE : Team::BLACK;
    board.SetCurrentTurn(team_to_move);
}

void ForsythEdwards::SetupStartingBoardPos(Board& board)
{
    this->SetupCustomBoardPosition(m_StartingBoardPosFen,board);
}

void ForsythEdwards::ParsePiecePositions(std::string_view pieces_fen,std::vector<Core::Ref<Piece>>& pieces, Vec2i& new_pos)
{
    auto is_piece_symbol = [&](char symbol) -> bool{
        auto it = std::find_if(m_PiecesCharArr.begin(),m_PiecesCharArr.end(),[&](char img_symbol){
            return (img_symbol == symbol);
    });

        return (it != m_PiecesCharArr.end());
    };

   for(int i = 0; i < pieces_fen.size();++i){
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

        if(std::isdigit(pieces_fen[i])){
            int digit = std::atoi(&pieces_fen[i]);
            new_pos.x += digit;
        }else if(pieces_fen[i] == '/'){
            new_pos.x = 0;
            --new_pos.y;
        }else if(is_piece_symbol(pieces_fen[i])){
            auto curr_piece = create_piece(pieces_fen[i]);
            curr_piece->Revive();

            curr_piece->SetPosition(new_pos);
            ++new_pos.x;
            
            pieces.push_back(curr_piece);
        }
    }
}
