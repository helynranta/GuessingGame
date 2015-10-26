#include "GUI/GUIElement.hpp"

void GUIElement::drawBackground () {
    drawBackground(m_width, m_height);
}
void GUIElement::drawBackground (int width) {
    drawBackground(width, m_height);
}

void GUIElement::drawBackground (int width, int height) {
    if(!m_hidden)
    {
        m_bg_x = m_x;
        m_bg_y = m_y;
        if(m_align == TEXT_ALIGN::CENTER) {
            m_bg_x = m_x - m_bg_width;
        } else if(m_align == TEXT_ALIGN::CENTER_XY) {
            m_bg_x -= m_bg_width/2*m_scale;
            m_bg_y -= m_bg_height/2*m_scale;
        } else if(m_align == TEXT_ALIGN::RIGHT) {
            m_bg_x -= m_bg_width*m_scale;
        } else if(m_align == TEXT_ALIGN::OVER) {
            m_bg_y -= m_bg_height*m_scale;
        } else if(m_align == TEXT_ALIGN::OVER_RIGHT) {
            m_bg_y -= m_bg_height*m_scale;
            m_bg_x -= m_bg_width*m_scale;
        }
        SDL_SetRenderDrawColor( m_renderer, 40, 40, 40, 255 );
        const SDL_Rect rect =  {m_bg_x, m_bg_y, int(width*m_scale), int(height*m_scale)};
        SDL_RenderFillRect(m_renderer, &rect);
        SDL_SetRenderDrawColor(m_renderer, 50, 50, 50, 255);
    }
}
