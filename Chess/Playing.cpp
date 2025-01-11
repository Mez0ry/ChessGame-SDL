#include "Playing.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

Playing::Playing(const Core::Ref<Renderer> renderer, const Core::Ref<Window> window, SceneManager& scene_manager) : m_SceneManager(scene_manager), m_Renderer(renderer), m_Window(window), m_Board(ObjectSize(8,8))
{
    auto[win_w,win_h] = window->GetWindowSize();

    m_Board.Setup(renderer,window,"resources/game/boards/chess_board2.jpg",ObjectSize(128,128));

    const int pawn_amount = 8;
    const int knight_amount = 2;
    const int rook_amount = 2;
    const int bishop_amount = 2;

    const int teams_amount = 2;

    for(auto i = 0;i < teams_amount;i++){
        Team team;
        if((i & 1) == 0){
            team = Team::WHITE;
        }else{
            team = Team::BLACK;
        }

        const std::string prefix = (team == Team::BLACK) ? "b/b_"  : "w/w_";

        const std::string resource_pawn_name = "piece1.png";
        const std::string resources_knight_name = "piece2.png";
        const std::string resources_bishop_name = "piece3.png";
        const std::string resources_queen_name = "piece4.png";
        const std::string resources_king_name = "piece5.png";
        const std::string resources_rook_name = "piece6.png";
        
        const std::string pieces_path = "resources/game/pieces/" + prefix;

        const std::string pawn_texture_path = pieces_path + resource_pawn_name;
        const std::string rook_texture_path = pieces_path + resources_rook_name;
        const std::string bishop_texture_path = pieces_path + resources_bishop_name;
        const std::string knight_texture_path = pieces_path + resources_knight_name;
        const std::string queen_texture_path = pieces_path + resources_queen_name;
        const std::string king_texture_path = pieces_path + resources_king_name;

        Texture pawn_texture(renderer,pawn_texture_path),
                rook_texture(renderer,rook_texture_path),
                bishop_texture(renderer,bishop_texture_path),
                knight_texture(renderer,knight_texture_path),
                queen_texture(renderer,queen_texture_path),
                king_texture(renderer,king_texture_path);
        
        SDL_SetTextureAlphaMod(pawn_texture,0);
        SDL_SetTextureAlphaMod(rook_texture,0);
        SDL_SetTextureAlphaMod(bishop_texture,0);
        SDL_SetTextureAlphaMod(knight_texture,0);
        SDL_SetTextureAlphaMod(queen_texture,0);
        SDL_SetTextureAlphaMod(king_texture,0);

        for(int pawn_cnt = 0; pawn_cnt < pawn_amount;pawn_cnt++){
            m_Board.AddPiece(Core::CreateRef<Piece>(pawn_texture,PieceType::PAWN,team));
        }

        for(int knight_amount_cnt = 0; knight_amount_cnt < knight_amount;knight_amount_cnt++){
            m_Board.AddPiece(Core::CreateRef<Piece>(knight_texture,PieceType::KNIGHT,team));
        }

        for(int rook_amount_cnt = 0; rook_amount_cnt < rook_amount;rook_amount_cnt++){
            m_Board.AddPiece(Core::CreateRef<Piece>(rook_texture,PieceType::ROOK,team));
        }

        for(int bishop_amount_cnt = 0; bishop_amount_cnt < bishop_amount;bishop_amount_cnt++){
            m_Board.AddPiece(Core::CreateRef<Piece>(bishop_texture,PieceType::BISHOP,team));
        }

        m_Board.AddPiece(Core::CreateRef<Piece>(queen_texture,PieceType::QUEEN,team));
        m_Board.AddPiece(Core::CreateRef<Piece>(king_texture,PieceType::KING,team));

    }

    m_Board.LoadBoardFromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Playing::~Playing()
{
    
}

void Playing::OnCreate()
{
    m_Renderer->SetRenderDrawColor(Color(41, 44, 92, 255));
    m_Board.OnCreate();
}

void Playing::OnResize([[maybe_unused]] const Core::Ref<Window> window)
{
    m_Board.OnResize(window);
}

void Playing::OnDestroy()
{
    m_Board.OnDestroy();
}

void Playing::HandleInput([[maybe_unused]] const Core::Ref<EventHandler> event_handler)
{
    m_Board.HandleInput(event_handler);
}

void Playing::Update(float dt)
{
    m_Board.Update(dt);
}

void Playing::Render(const Core::Ref<Renderer> renderer)
{
    m_Board.Render(renderer);
}
