#include "Grabber.h"
#include "App.h"
#include "Grid.h"


void Grabber::move(){
    if(state() != GRABBING) return;
    app->grid.moveTo(origin + (pDown(Qt::LeftButton) - p())/app->grid.zoom());

}
void Grabber::downL(){
    origin = app->grid.topLeft();
    setState(GRABBING);
}
void Grabber::upL(){ setState(GRABBER); }

void Grabber::setState(const ToolState state){
    _state = state;
    switch(state){
    case GRABBER:
        app->setCursor(grabberCursor);
        break;
    case GRABBING:
        app->setCursor(grabbingCursor);
        break;
    default:
        break;
    }
}
