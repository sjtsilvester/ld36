#include "stdafx.h"
#include "EntityManager.h"
#include "Entity.h"
#include "SFLD.h"
#include "ResourceManager.h"
#include "TrapInferface.h"
#include "GameState.h"
#include "Trap.h"

EntityManager::EntityManager(ResourceManager<sf::Texture, std::string>* resourceManager, std::vector<PlayerInfo>* player_infos, GameState* game_state, int room_size):
	resourceManager_(resourceManager), player_infos_(player_infos),red_trap_(false),game_state_(game_state),room_size_(room_size){
	view_ = SFLD::window_->getDefaultView();
	red_tex.create(1024, 768);
	red_tex.clear(sf::Color::Red);
	font_.loadFromFile("media/Futura_ICG.ttf");
	lit_tex_.create(room_size, room_size);
	unlit_tex_.create(room_size, room_size);
	lit_tex_.clear(sf::Color(102, 59, 23));
	unlit_tex_.clear(sf::Color(46, 27, 11));
}

EntityManager::~EntityManager() = default;

sf::RenderTexture* EntityManager::getUnlitTexture() {
	return &unlit_tex_;
}

sf::RenderTexture* EntityManager::getLitTexture() {
	return &lit_tex_;
}

void EntityManager::renderText(sf::Text text) {
	texts_.push_back(text);
}

sf::Font* EntityManager::getFont() {
	return &font_;
}

void EntityManager::addTrap(Trap* trap) {
	trap_interface_->addTrap(trap);
}

void EntityManager::setTrapInterface(TrapInterface* trap_interface) {
	trap_interface_ = trap_interface;
}

void EntityManager::setViewFocus(Entity* entity) {
	view_.setCenter(entity->getPosition());
}

void EntityManager::addEntity(Entity* entity){
	push_queue_.push_back(std::unique_ptr<Entity>(entity));
}

EntityList* EntityManager::getEntities(){
	return &entities_;
}

void EntityManager::clear(){
	entities_.clear();
	push_queue_.clear();
}

void EntityManager::doRedTrap() {
	red_trap_ = true;
	red_timer_ = 0;
}

void EntityManager::sendTrap(MessageType type, int room_no) {
	game_state_->sendTrap(type, room_no);
}
void EntityManager::update(int frameTime){
	texts_.clear();
	if (red_trap_) {
		red_timer_ += frameTime;
		if (red_timer_ >= 2000) {
			red_trap_ = false;
		}
	}

	for (auto& it : entities_){
		if (it->isActive()) {
			it->update(frameTime);
		}
	}
	for (auto& it = entities_.begin(); it != entities_.end();){
		if ((*it)->isDestroyed()){
			it = entities_.erase(it);
		}
		else{
			it++;
		}
	}

	for (auto& it : push_queue_){
		entities_.push_back(std::move(it));
	}
	push_queue_.clear();
}

void EntityManager::renderTrapInterface(TrapInterface* trap_interface,sf::RenderTarget* target) {
	SFLD::window_->setView(SFLD::window_->getDefaultView());
	trap_interface->render(target);
	SFLD::window_->setView(view_);
}

void EntityManager::render(sf::RenderTarget* target){
	SFLD::window_->setView(view_);
	for (auto& it : entities_){
		if (it->isActive()) {
			it->render(target);
		}
	}
	if (red_trap_) {
		SFLD::window_->setView(SFLD::window_->getDefaultView());
		sf::Sprite red(red_tex.getTexture());
		target->draw(red);
		SFLD::window_->setView(view_);
	}
	for (auto& it : texts_) {
		target->draw(it);
	}
}
