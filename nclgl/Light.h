#pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light {
public:

	Light() {
		this->position = Vector3(0, 0, 0);
		this->colour = Vector4(0, 0, 0, 0);
		this->radius = radius;


	}
	Light(Vector3 position, Vector4 colour, float radius) {
		this->position = position;
		this->colour = colour;
		this->radius = radius;

	}

	~Light(void) {};

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 val) { colour = val; }

protected:
	Vector3 position;
	Vector4 colour;
	float radius;
