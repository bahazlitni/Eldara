#include "Grabber.h"
#include "Scene.h"
#include "Grid.h"


void Grabber::move(){
    if(state() != GRABBING) return;
    scene->grid.moveTo(origin + (pDown(Qt::LeftButton) - p())/scene->grid.zoom());

}
void Grabber::downL(){
    origin = scene->grid.topLeft();
    setState(GRABBING);
}
void Grabber::upL(){ setState(GRABBER); }

void Grabber::setState(const ToolState state){
    _state = state;
    switch(state){
    case GRABBER:
        scene->setCursor(grabberCursor);
        break;
    case GRABBING:
        scene->setCursor(grabbingCursor);
        break;
    default:
        break;
    }
}
