#pragma once
#include <string>

//--------------------------------------------------------------
//  Scene
//--------------------------------------------------------------
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}


	// ‰Šú‰»
	virtual void Initialize() = 0;

	// I—¹‰»
	virtual void Finalize() = 0;

	// XVˆ—
	virtual void Update() = 0;

	// •`‰æˆ—
	virtual void Render() = 0;

	// €”õŠ®—¹‚µ‚Ä‚¢‚é‚©
	bool IsReady() const { return ready; }
	// €”õŠ®—¹İ’è
	void SetReady() { ready = true; }

private:
	bool ready = false;
};