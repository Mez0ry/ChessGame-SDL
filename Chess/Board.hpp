#ifndef __BOARD_HPP__
#define __BOARD_HPP__
#include "Matrix.hpp"
#include "ObjectSize.hpp"

#include "Window.hpp"
#include "Renderer.hpp"

#include "Square.hpp"

#include "Memento.hpp"
#include "KeyFrame.hpp"

class EventHandler;

class Board{
private:
    Core::Ref<Renderer> m_Renderer;
    Core::Ref<Window> m_Window;
public:
    Board(ObjectSize board_size);
    Board(const Core::Ref<Renderer> renderer,const Core::Ref<Window> window,const std::string& path,ObjectSize one_square_src_size, ObjectSize board_size);
    ~Board();

    void Setup(const Core::Ref<Renderer> renderer,const Core::Ref<Window> window,const std::string& path,ObjectSize one_square_src_size);
    
    void LoadSquareTexture(const std::string& path, ObjectSize square_src_size);

    void OnCreate();
    void OnResize([[maybe_unused]] const Core::Ref<Window> window);
    void OnDestroy();

    void HandleInput([[maybe_unused]] const Core::Ref<EventHandler> event_handler);
    void Update(float dt);
    void Render(const Core::Ref<Renderer> renderer);
private:
    Texture m_WhiteSquareTexture, m_BlackSquareTexture;
    ObjectSize m_BoardSize; // matrix size
    ObjectSize m_OneSquareSize,m_OneSquareSizeSrc;
    ObjectSize m_FullBoardSize;
    Vec2i m_BoardTopLeft;
    Stellar::KeyFrame m_BoardTopLeftKF;
    Memento<Vec2i> m_BoardTopLeftMemento;
};

#endif //!__BOARD_HPP__