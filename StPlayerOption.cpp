#include "StPlayerOption.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Timer.h"
#include "Library/3D/Camera.h"

#include "Collision.h"

StPlayerOption::StPlayerOption(std::shared_ptr<SkinnedMesh> model) {
	this->model = model;

}

StPlayerOption::~StPlayerOption() {}

void StPlayerOption::Update() {
	
}

void StPlayerOption::Render() {}

void StPlayerOption::DrawDebugGui() {}
